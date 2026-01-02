import sys
import time
import struct
import glfw
import hypergl
import numpy as np
from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from hypergl import StorageBufferResource, BlendSettings

if not getattr(sys, "_is_freethreaded", lambda: False)():
    print("WARNING: Running with GIL. Ensure modules are built for PEP 703.")

PARTICLE_COUNT = 5000000
WORKGROUP_SIZE = 256

if not glfw.init():
    sys.exit()

glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 4)
glfw.window_hint(glfw.CONTEXT_VERSION_MINOR, 5)
glfw.window_hint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE)
glfw.window_hint(glfw.RESIZABLE, False)

glfw.window_hint(glfw.DOUBLEBUFFER, True)
window = glfw.create_window(1280, 720, "HyperGL Compute Particles", None, None)
glfw.make_context_current(window)
glfw.swap_interval(0)

ctx = hypergl.context()

image = ctx.image((1280, 720), 'rgba8unorm')

print(f"Allocating {PARTICLE_COUNT * 16 / 1024 / 1024:.2f} MB for particles...")

pos_data = np.random.uniform(-600, 600, (PARTICLE_COUNT, 2)).astype('f4')
vel_data = np.random.uniform(-1, 1, (PARTICLE_COUNT, 2)).astype('f4')

particle_data = np.column_stack((pos_data, vel_data)).flatten().tobytes()

inv_mass_data = (1.0 / np.random.uniform(0.8, 1.2, PARTICLE_COUNT)).astype('f4').tobytes()

particle_buffer = ctx.buffer(data=particle_data)
props_buffer = ctx.buffer(data=inv_mass_data)

compute_src = '''
    #version 460 core

    // Set workgroup size (must match Python dispatch calculation)
    layout(local_size_x = 256) in;

    // Binding 0: Particle Data (Read/Write)
    // std430 layout is standard for SSBOs
    layout(std430, binding = 0) buffer ParticleBuffer {
        vec4 particles[]; // x, y, vx, vy
    };

    // Binding 1: Properties (Read Only)
    layout(std430, binding = 1) buffer PropsBuffer {
        float inv_mass[];
    };

    uniform vec2 Target;
    uniform vec2 MouseVel;
    uniform int IsRepel; // Use int for bool in uniforms
    uniform float Dt;
    uniform int Count;

    void main() {
        // Global ID is the particle index
        uint id = gl_GlobalInvocationID.x;

        // Boundary check (in case particle count isn't a multiple of 256)
        if (id >= Count) return;

        // Load data
        vec4 data = particles[id];
        float ip = inv_mass[id];

        vec2 p = data.xy;
        vec2 v = data.zw;

        // --- Physics Logic ---
        vec2 d = Target - p;
        float dist_sq = dot(d, d) + 60.0;
        float inv_dist = 1.0 / sqrt(dist_sq);

        float force;
        if (IsRepel == 1) {
            force = -1200.0 * (inv_dist * inv_dist * inv_dist) * 12000.0;
        } else {
            force = 100.0 * inv_dist;
        }

        vec2 acc = (d * force + MouseVel * 5.0) * ip;
        vec2 dir = normalize(d);
        vec2 orbit = vec2(-dir.y, dir.x) * (inv_dist * 2000.0);

        // High-speed Damping
        float damping = pow(0.9, Dt);
        v = v + acc * Dt + orbit * Dt;

        // Speed Cap
        float max_speed = 500.0; 
        float speed = length(v);
        if (speed > max_speed) {
            v = (v / speed) * max_speed;
        }

        float damping_factor_per_second = 0.9;
        v *= pow(damping_factor_per_second, Dt);

        // Move
        p = p + v * Dt;

        // Boundary Wrap
        // mod in GLSL behaves differently for negatives, so we add the wrap range
        vec2 wrap = vec2(1280.0, 720.0);
        vec2 center_offset = vec2(640.0, 360.0);
        p = mod(p + center_offset, wrap) - center_offset;

        // Store data back to buffer
        particles[id] = vec4(p, v);
    }
'''

comp_storage_buffer_res: list[StorageBufferResource] = [
        {'type': 'storage_buffer', 'binding': 0, 'buffer': particle_buffer},
        {'type': 'storage_buffer', 'binding': 1, 'buffer': props_buffer},
    ]

compute_pass = ctx.compute(
    compute_shader=compute_src,
    resources=comp_storage_buffer_res,
    uniforms={
        'Dt': 0.016,
        'Target': [0.0, 0.0],
        'MouseVel': [0.0, 0.0],
        'IsRepel': 0,
        'Count': PARTICLE_COUNT,
    }
)

render_vs = '''
    #version 460 core

    // Define the buffer structure again to read it
    layout(std430, binding = 0) buffer ParticleBuffer {
        vec4 particles[];
    };

    out vec3 v_Color;

    void main() {
        // Read directly from buffer using Vertex ID
        vec4 data = particles[gl_VertexID];
        vec2 pos = data.xy;
        vec2 vel = data.zw;

        // Calculate speed for coloring
        float speed = length(vel);
        float t = clamp(speed * 0.05, 0.0, 1.0);
        v_Color = mix(vec3(0.1, 0.4, 1.0), vec3(1.0, 0.8, 0.6), t);

        gl_PointSize = 1.0;
        gl_Position = vec4(pos / vec2(640.0, 360.0), 0.0, 1.0);
    }
'''

render_fs = '''
    #version 460 core
    in vec3 v_Color;
    layout (location = 0) out vec4 out_color;
    void main() {
        out_color = vec4(v_Color, 0.8);
    }
'''

quad_vs = '''
    #version 460 core
    vec2 positions[4] = vec2[](
        vec2(-1.0, -1.0), vec2(1.0, -1.0),
        vec2(-1.0,  1.0), vec2(1.0,  1.0)
    );
    void main() {
        gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
    }
'''

render_storage_buffer_res: StorageBufferResource = [{'type': 'storage_buffer', 'binding': 0, 'buffer': particle_buffer}]
render_blend_settings: BlendSettings = {
    'enable': True,
    'src_color': 'src_alpha',
    'dst_color': 'one_minus_src_alpha'
}

render_pipeline = ctx.pipeline(
    vertex_shader=render_vs,
    fragment_shader=render_fs,
    resources=render_storage_buffer_res,
    framebuffer=[image],
    topology='points',
    vertex_count=PARTICLE_COUNT,
    blend=render_blend_settings,
)

fade_blend_settings: BlendSettings = {
    'enable': True,
    'src_color': 'src_alpha',
    'dst_color': 'one_minus_src_alpha'
}

fade_pipe = ctx.pipeline(
    vertex_shader=quad_vs,
    fragment_shader='''
        #version 450 core
        out vec4 c;
        void main() { c = vec4(0, 0, 0, 0.15); }
    ''',
    framebuffer=[image],
    topology='triangle_strip',
    vertex_count=4,
    blend=fade_blend_settings,
)

frame = 0
t_prev = time.perf_counter()
m_prev_x, m_prev_y = glfw.get_cursor_pos(window)

group_count = (PARTICLE_COUNT + WORKGROUP_SIZE - 1) // WORKGROUP_SIZE

print("Starting Loop...")
print("Available uniforms:", list(compute_pass.uniforms.keys()))
while not glfw.window_should_close(window):
    mx, my = glfw.get_cursor_pos(window)
    m_vel_x = mx - m_prev_x
    m_vel_y = (360 - my) - (360 - m_prev_y)
    m_prev_x, m_prev_y = mx, my

    is_repelling = glfw.get_mouse_button(window, glfw.MOUSE_BUTTON_LEFT) == glfw.PRESS
    target_x = mx - 640
    target_y = 360 - my

    ctx.new_frame()
    fade_pipe.render()

    compute_pass.uniforms['Target'][:] = struct.pack('2f', target_x, target_y)
    compute_pass.uniforms['MouseVel'][:] = struct.pack('2f', m_vel_x, m_vel_y)
    compute_pass.uniforms['IsRepel'][:] = struct.pack('i', int(is_repelling))

    compute_pass.run(group_count, 1, 1)

    render_pipeline.render()

    image.blit()
    ctx.end_frame()
    glfw.swap_buffers(window)
    glfw.poll_events()

    frame += 1
    if frame % 60 == 0:
        t_now = time.perf_counter()
        fps = 60 / (t_now - t_prev)
        t_prev = t_now
        glfw.set_window_title(window, f"HyperGL Compute | FPS: {int(fps)} | Particles: {PARTICLE_COUNT}")

glfw.terminate()