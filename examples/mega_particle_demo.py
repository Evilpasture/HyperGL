import sys
import time
import struct
import math
import threading
import numpy as np
import glfw
import hypergl
from numba import njit, prange

# --- 1. CONFIGURATION ---
PARTICLE_COUNT = 10000000  # The heavy load
WORKGROUP_SIZE = 256  # GPU Local size
DT = 0.005  # Fixed physics timestep


# --- 2. THE NUMBA KERNEL (CPU SIMD) ---
# nogil=True releases the lock (critical even in 3.14t for Numba internal threading)
# fastmath=True enables AVX2/AVX-512 instructions
# Pre-calculate once
sin_lut = np.sin(np.linspace(0, 2*np.pi, 4096)).astype(np.float32)


@njit(parallel=True, fastmath=True, nogil=True)
def cpu_physics_kernel(data, t, sin_lut):
    n = data.shape[0]
    freq = np.float32(0.005)
    phase = np.float32(t * 2.0)
    # Scaling factor to map (y * freq + phase) to 0-4095
    # (4096 / (2 * pi)) approx 651.8986
    lut_scale = np.float32(651.8986)

    for i in prange(n):
        # Calculate index using bitwise AND for speed (only works if LUT size is power of 2)
        angle = data[i, 1] * freq + phase
        idx = int(angle * lut_scale) & 4095

        data[i, 2] += sin_lut[idx] * np.float32(10.0)


# --- 3. THE WORKER THREAD (ZERO-GL VERSION) ---
# WE DON'T map() HERE.
def worker_loop(gpu_view, sin_lut): # Add sin_lut here
    print(f" [Worker] Physics Thread Active")
    while True:
        t = time.perf_counter()
        try:
            # Pass it into the Numba kernel
            cpu_physics_kernel(gpu_view, t, sin_lut)
        except Exception as e:
            print(f"Worker Error: {e}")
            break
        time.sleep(0.001)


# --- 4. ENGINE SETUP ---
if not glfw.init(): sys.exit()

glfw.window_hint(glfw.FLOATING, glfw.TRUE)
glfw.window_hint(glfw.RESIZABLE, glfw.FALSE)
# Disable VSync handled by swap_interval later
window = glfw.create_window(1280, 720, "HyperGL 3.14t | 5M Particles", None, None)
glfw.make_context_current(window)
glfw.swap_interval(0)

# Init Context
ctx = hypergl.context()

# --- 5. DATA ALLOCATION ---
print(f" [Alloc] Allocating {PARTICLE_COUNT * 16 / 1024 ** 2:.1f} MB VRAM...")

# Pos(2) + Vel(2)
# Initialize in a spiral
indices = np.arange(PARTICLE_COUNT, dtype='f4')
radius = np.sqrt(indices) * 0.4
angle = indices * 0.005
pos_vel = np.zeros((PARTICLE_COUNT, 4), dtype='f4')
pos_vel[:, 0] = np.cos(angle) * radius
pos_vel[:, 1] = np.sin(angle) * radius
# Initial velocity to create orbit
pos_vel[:, 2] = -np.sin(angle) * 100.0
pos_vel[:, 3] = np.cos(angle) * 100.0

# GPU Buffer 0: Position & Velocity
# Usage hint: 'stream_draw' (updated frequently by CPU)
# Force the target to be an SSBO so map() accepts it
particle_buffer = ctx.buffer(
    data=pos_vel.tobytes(),
    storage=True
)

m = particle_buffer.map()

# GPU Buffer 1: Mass (Static)
inv_mass = np.random.uniform(0.1, 1.0, PARTICLE_COUNT).astype('f4')
mass_buffer = ctx.buffer(data=inv_mass.tobytes(), access='static_read')

# CPU Side Mirror
# We use a bytearray for the buffer protocol, then view it as numpy
cpu_staging = bytearray(PARTICLE_COUNT * 16)
cpu_view = np.frombuffer(cpu_staging, dtype='f4')
gpu_view = np.frombuffer(m, dtype='f4').reshape(PARTICLE_COUNT, 4)

# --- 6. SHADERS (OPTIMIZED GLSL) ---

compute_src = '''
#version 460 core
layout(local_size_x = 256) in;

layout(std430, binding = 0) buffer ParticleBuffer { vec4 particles[]; };
layout(std430, binding = 1) buffer PropsBuffer { float inv_mass[]; };

uniform vec2 Target;
uniform vec2 MouseVel;
uniform float ForceMult; // + for attract, - for repel
uniform float Dt;
uniform float Damping;   // pow(0.9, dt) precalculated
uniform int Count;

void main() {
    uint id = gl_GlobalInvocationID.x;
    if (id >= Count) return;

    vec4 data = particles[id];
    float im = inv_mass[id];

    vec2 p = data.xy;
    vec2 v = data.zw;

    // Physics
    vec2 d = Target - p;
    // inversesqrt is usually 1 instruction on modern GPUs
    float dist_sq = dot(d, d) + 100.0; 
    float inv_dist = inversesqrt(dist_sq);

    // Branchless Force App
    vec2 dir = d * inv_dist;
    float force = ForceMult * inv_dist; // ForceMult handles sign

    vec2 acc = (dir * force + MouseVel * 5.0) * im;

    // Vortex/Curl Noise approximation
    vec2 orbit = vec2(-dir.y, dir.x) * (inv_dist * 2000.0);

    // Euler Integration
    v = (v + acc * Dt + orbit * Dt) * Damping;

    // Soft Speed Limit (Branchless)
    float speed_sq = dot(v, v);
    // mix(a, b, step(edge, x)) is faster than if(x > edge)
    // If speed_sq > 250000, scale it down.
    float is_fast = step(250000.0, speed_sq);
    v = mix(v, v * inversesqrt(speed_sq) * 500.0, is_fast);

    p += v * Dt;

    // Boundary Wrap (Mod)
    vec2 area = vec2(1280.0, 720.0);
    vec2 half_area = vec2(640.0, 360.0);
    // (p + half) % area - half
    p = mod(p + half_area, area) - half_area;

    particles[id] = vec4(p, v);
}
'''

render_vs = '''
#version 460 core
layout(std430, binding = 0) buffer ParticleBuffer { vec4 particles[]; };

out vec3 v_Color;

// Inside render_vs main()
void main() {
    vec4 data = particles[gl_VertexID];
    
    // Calculate distance from center (0 to ~640)
    float dist = length(data.xy);
    
    // Calculate angle
    float angle = atan(data.y, data.x);
    
    // Color Palette based on distance (Galaxy arms)
    // Core = White/Yellow, Middle = Purple/Blue, Edge = Dark
    
    vec3 core = vec3(1.0, 0.9, 0.5); // Warm center
    vec3 mid = vec3(0.5, 0.1, 1.0);  // Purple
    vec3 edge = vec3(0.0, 0.1, 0.3); // Dark Blue
    
    float t = clamp(dist / 450.0, 0.0, 1.0);
    vec3 base_color = mix(core, mid, t);
    base_color = mix(base_color, edge, smoothstep(0.4, 1.0, t));
    
    // Add "Twinkle" based on velocity
    float speed = length(data.zw);
    float energy = clamp(speed * 0.005, 0.0, 1.0);
    
    v_Color = base_color + vec3(energy); // Add white glow for fast particles
    
    gl_Position = vec4(data.xy * vec2(1.0/640.0, 1.0/360.0), 0.0, 1.0);
    gl_PointSize = 1.0;
}
'''

render_fs = '''
#version 460 core
in vec3 v_Color;
layout(location=0) out vec4 f_color;
void main() {
    f_color = vec4(v_Color, 0.6); // Alpha for blending
}
'''

# --- 7. PIPELINES ---

# Using rgba16f for High Dynamic Range (values > 1.0 accumulate glow)
image = ctx.image((1280, 720), format='rgba16float')

compute_pass = ctx.compute(
    compute_shader=compute_src,
    resources=[
        {'type': 'storage_buffer', 'binding': 0, 'buffer': particle_buffer},
        {'type': 'storage_buffer', 'binding': 1, 'buffer': mass_buffer},
    ],
    uniforms={
        'Dt': DT,
        'Count': PARTICLE_COUNT,
        'Target': (0.0, 0.0),
        'MouseVel': (0.0, 0.0),
        'ForceMult': 10000.0,
        'Damping': 0.95
    }
)

render_pipeline = ctx.pipeline(
    vertex_shader=render_vs,
    fragment_shader=render_fs,
    resources=[
        {'type': 'storage_buffer', 'binding': 0, 'buffer': particle_buffer}
    ],
    framebuffer=[image],
    topology='points',
    vertex_count=PARTICLE_COUNT,
    blend={
        'enable': True,
        'src_rgb': 'src_alpha',
        'dst_rgb': 'one',  # Additive blending
        'src_alpha': 'src_alpha',
        'dst_alpha': 'one',
        'op_color': 'add',
        'op_alpha': 'add'
    }
)

# --- 8. START THREADS ---
sim_thread = threading.Thread(
    target=worker_loop,
    # Pass both the mapped view and the LUT
    args=(gpu_view, sin_lut),
    daemon=True
)
sim_thread.start()

# --- 9. RENDER LOOP ---
frame = 0
t_prev = time.perf_counter()
mx, my = 0, 0
last_mx, last_my = 0, 0

print(" [Main] Starting Loop...")

while not glfw.window_should_close(window):
    # 1. Update State
    last_mx, last_my = mx, my
    x_raw, y_raw = glfw.get_cursor_pos(window)
    mx, my = x_raw - 640, 360 - y_raw  # Center 0,0

    mvx = mx - last_mx
    mvy = my - last_my

    is_repel = glfw.get_mouse_button(window, glfw.MOUSE_BUTTON_LEFT) == glfw.PRESS

    # 2. Update Uniforms (MUST USE STRUCT PACK + SLICE ASSIGNMENT)
    # The dictionary maps strings -> memoryviews. We write bytes into them.

    # Vec2 updates
    compute_pass.uniforms['Target'][:] = struct.pack('2f', mx, my)
    compute_pass.uniforms['MouseVel'][:] = struct.pack('2f', mvx, mvy)

    # Float updates (Fixing the Crash)
    force_val = -80000.0 if is_repel else 15000.0
    # Pack float ('f') and write to buffer [:]
    compute_pass.uniforms['ForceMult'][:] = struct.pack('f', force_val)

    # Damping update
    damping_val = math.pow(0.98, DT * 60.0)
    compute_pass.uniforms['Damping'][:] = struct.pack('f', damping_val)

    # 3. GPU Physics
    # Dispatch: N / 256, rounded up
    groups = (PARTICLE_COUNT + 255) // 256
    compute_pass.run(groups, 1, 1)

    # 4. Render
    image.clear()
    render_pipeline.render()
    image.blit()  # Blit to screen

    glfw.swap_buffers(window)
    glfw.poll_events()

    # 5. Metrics
    frame += 1
    if frame % 60 == 0:
        now = time.perf_counter()
        fps = 60 / (now - t_prev)
        t_prev = now
        title = f"HyperGL 3.14t | FPS: {int(fps)} | N: {PARTICLE_COUNT // 1000}k | {'REPEL' if is_repel else 'ATTRACT'}"
        glfw.set_window_title(window, title)

glfw.terminate()