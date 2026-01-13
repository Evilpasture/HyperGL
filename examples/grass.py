import glfw
import struct
import math
import time
import hypergl
import random

# --- CONFIGURATION ---
WIDTH, HEIGHT = 1600, 900

# 2000x2000 grid = 4 Million Blades
GRID_DIM = 2000
BLADE_COUNT = GRID_DIM * GRID_DIM

# Compute Shader Settings
WORKGROUP_SIZE = 256
NUM_GROUPS = math.ceil(BLADE_COUNT / WORKGROUP_SIZE)


class GLFWLoader:
    def load_opengl_function(self, name):
        return glfw.get_proc_address(name)


def main():
    if not glfw.init(): raise RuntimeError("GLFW Init Failed")

    glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 4)
    glfw.window_hint(glfw.CONTEXT_VERSION_MINOR, 6)
    glfw.window_hint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE)
    glfw.window_hint(glfw.RESIZABLE, False)
    # High precision depth buffer to prevent Z-fighting on grass blades
    glfw.window_hint(glfw.DEPTH_BITS, 24)

    window = glfw.create_window(WIDTH, HEIGHT, f"HyperGL Grass - {BLADE_COUNT:,} Blades", None, None)
    if not window: glfw.terminate(); raise RuntimeError("Window Creation Failed")

    glfw.make_context_current(window)
    glfw.swap_interval(0)

    hypergl.init(GLFWLoader())
    ctx = hypergl.context()

    print(f"Renderer: {ctx.info['renderer']}")
    print(f"Generating {BLADE_COUNT:,} Blades on GPU...")

    # =========================================================================
    # 1. RESOURCES
    # =========================================================================

    # A. The Field (Static Data)
    # We allocate it here, but we will fill it using a Compute Shader (CS_GEN)
    # Stride 16 bytes: vec4(x, z, height, angle)
    # 4 Million * 16 bytes = ~64MB VRAM
    ssbo_field = ctx.buffer(size=BLADE_COUNT * 16, storage=True)

    # B. The Render Queue (Visible Data)
    # The Culling shader copies visible blades here every frame
    ssbo_visible = ctx.buffer(size=BLADE_COUNT * 16, storage=True)

    # C. Indirect Command Buffers (Double Buffered)
    # We draw 7 vertices per blade (Triangle Strip)
    # Initial: count=7, instanceCount=0, first=0, baseInstance=0
    cmd_initial = struct.pack('=IIII', 7, 0, 0, 0)

    cmd_buffers = [
        ctx.buffer(data=cmd_initial),
        ctx.buffer(data=cmd_initial)
    ]

    # D. Globals (Camera, Time, Wind)
    # Layout: mat4 vp, vec3 cam_pos, float time, vec2 mouse_pos, vec2 padding
    ubo_globals = ctx.buffer(size=128, uniform=True, access='dynamic_draw')

    # =========================================================================
    # 2. COMPUTE SHADER: GENERATOR (Runs Once)
    # =========================================================================
    # Generates 4 Million grass positions on the GPU instantly.

    CS_GEN = f"""
    #version 460
    layout(local_size_x={WORKGROUP_SIZE}) in;

    struct Blade {{ vec4 data; }}; // x, z, height, angle
    layout(std430, binding=0) writeonly buffer Field {{ Blade blades[]; }};

    float hash(float n) {{ return fract(sin(n) * 43758.5453); }}

    void main() {{
        uint id = gl_GlobalInvocationID.x;
        if (id >= {BLADE_COUNT}) return;

        // Map ID to Grid (x, z)
        int row = int(id) / {GRID_DIM};
        int col = int(id) % {GRID_DIM};

        float r1 = hash(float(id));
        float r2 = hash(float(id) * 3.14);
        float r3 = hash(float(id) * 6.28);

        // Position with jitter
        float x = (col - {GRID_DIM}/2.0) * 0.15 + (r1 * 0.1);
        float z = (row - {GRID_DIM}/2.0) * 0.15 + (r2 * 0.1);

        // Height variation (0.5 to 1.5)
        float h = 0.5 + r3 * 1.0;

        // Random rotation
        float angle = r1 * 6.28;

        blades[id].data = vec4(x, z, h, angle);
    }}
    """

    pipe_gen = ctx.compute(CS_GEN, resources=[
        {'type': 'storage_buffer', 'binding': 0, 'buffer': ssbo_field}
    ])

    # Run Generation
    pipe_gen.run(NUM_GROUPS, 1, 1)

    # =========================================================================
    # 3. COMPUTE SHADER: CULLER (Runs Every Frame)
    # =========================================================================
    # Checks frustum visibility and fills the draw buffer.

    CS_CULL = f"""
    #version 460
    layout(local_size_x={WORKGROUP_SIZE}) in;

    struct Blade {{ vec4 data; }};

    layout(std430, binding=0) readonly buffer Field {{ Blade all_blades[]; }};
    layout(std430, binding=1) writeonly buffer Visible {{ Blade visible_blades[]; }};
    layout(std430, binding=2) buffer Cmd {{ uint count; uint instanceCount; uint first; uint base; }};

    layout(std140, binding=3) uniform Globals {{
        mat4 vp;
        vec3 cam_pos;
        float time;
        vec2 mouse_pos; // World space x,z
    }};

    void main() {{
        uint id = gl_GlobalInvocationID.x;
        if (id >= {BLADE_COUNT}) return;

        Blade b = all_blades[id];
        vec3 pos = vec3(b.data.x, 0.0, b.data.y); // y holds z in packed struct

        // --- CULLING ---
        // 1. Distance Culling
        // Calculate squared distance to camera (faster than sqrt)
        vec3 dvec = pos - cam_pos;
        float dist_sq = dot(dvec, dvec);

        // Render distance: 150 units
        if (dist_sq > 22500.0) return; 

        // 2. Frustum Culling
        // Project point to clip space
        vec4 clip = vp * vec4(pos.x, 0.0, b.data.y, 1.0);

        // Check if inside normalized device coordinates (NDC)
        // We add a generous margin (+2.0) because wind makes grass move around
        if (clip.z < -clip.w || 
            clip.x < -clip.w * 2.0 || clip.x > clip.w * 2.0 ||
            clip.y < -clip.w * 2.0 || clip.y > clip.w * 2.0) return;

        // Visible! Append to list.
        uint idx = atomicAdd(instanceCount, 1);
        visible_blades[idx] = b;
    }}
    """

    # Double-buffered compute pipelines
    pipelines_cull = [
        ctx.compute(CS_CULL, resources=[
            {'type': 'storage_buffer', 'binding': 0, 'buffer': ssbo_field},
            {'type': 'storage_buffer', 'binding': 1, 'buffer': ssbo_visible},
            {'type': 'storage_buffer', 'binding': 2, 'buffer': cmd_buffers[0]},
            {'type': 'uniform_buffer', 'binding': 3, 'buffer': ubo_globals},
        ]),
        ctx.compute(CS_CULL, resources=[
            {'type': 'storage_buffer', 'binding': 0, 'buffer': ssbo_field},
            {'type': 'storage_buffer', 'binding': 1, 'buffer': ssbo_visible},
            {'type': 'storage_buffer', 'binding': 2, 'buffer': cmd_buffers[1]},
            {'type': 'uniform_buffer', 'binding': 3, 'buffer': ubo_globals},
        ])
    ]

    # =========================================================================
    # 4. RENDER PIPELINE: GEOMETRY GENERATION
    # =========================================================================

    VS = """
    #version 460

    // Read from the VISIBLE buffer populated by Compute Shader
    struct Blade { vec4 data; };
    layout(std430, binding=1) readonly buffer Visible { Blade blades[]; };

    layout(std140, binding=3) uniform Globals {
        mat4 vp;
        vec3 cam_pos;
        float time;
        vec2 mouse_pos;
    };

    out vec3 v_color;
    out float v_ao;

    // Simple Hash for random color variation
    float hash(float n) { return fract(sin(n) * 43758.5453); }

    void main() {
        // Get instance data
        Blade b = blades[gl_InstanceID];
        float bx = b.data.x;
        float bz = b.data.y; // packed in Y
        float h  = b.data.z; // packed in Z
        float ang = b.data.w; // packed in W

        // --- WIND MATH ---
        // Combine low freq (swell) and high freq (flutter) noise
        float wind = sin(bx * 0.5 + time) + cos(bz * 0.3 + time * 0.8) * 0.5;
        wind += sin(bx * 2.0 + time * 3.0) * 0.2; 

        // --- INTERACTION ---
        // Push grass away from mouse
        vec2 to_mouse = vec2(bx, bz) - mouse_pos;
        float dist = length(to_mouse);
        float push = smoothstep(3.0, 0.0, dist) * 2.0; // 3.0 radius

        vec2 push_dir = normalize(to_mouse) * push;

        // --- GEOMETRY GENERATION ---
        // We generate a shape from 7 vertices (Triangle Strip)
        // 0--1
        // | /|
        // 2--3
        // | /|
        // 4--5
        //   6 (Tip)

        // Vertex properties based on ID (0-6)
        int v_id = gl_VertexID;

        // Taper width: Bottom is wide, top is thin
        // t goes from 0.0 (bottom) to 1.0 (top)
        float t = float(v_id / 2) / 3.0; 
        if (v_id == 6) t = 1.0;

        float width = (1.0 - t) * 0.1; 

        // Left (-1) or Right (+1) side of blade
        float side = (v_id % 2 == 0) ? -1.0 : 1.0;
        if (v_id == 6) side = 0.0; // Tip is center

        // Rotation Matrix
        float c = cos(ang);
        float s = sin(ang);
        mat2 rot = mat2(c, -s, s, c);

        // Local position (flat)
        vec2 local = vec2(width * side, 0.0);
        local = rot * local;

        // Apply Curve (Bend)
        // Bending increases with height (t^2)
        float bend_amount = (wind * 0.5) + push;
        vec2 offset = vec2(bend_amount, 0.0) * (t * t);

        // Final World Position
        vec3 pos = vec3(bx, 0.0, bz);
        pos.x += local.x + offset.x;
        pos.z += local.y + offset.y; // Using offset.x for simplicity in wind dir
        pos.y += t * h;

        gl_Position = vp * vec4(pos, 1.0);

        // --- COLOR ---
        float rnd = hash(float(gl_InstanceID));
        vec3 base_col = mix(vec3(0.05, 0.2, 0.02), vec3(0.1, 0.4, 0.05), rnd);
        vec3 tip_col = mix(vec3(0.2, 0.6, 0.1), vec3(0.6, 0.7, 0.2), rnd);

        v_color = mix(base_col, tip_col, t);
        v_ao = t; // Darker at bottom? No, simpler to just use t for gradient
    }
    """

    FS = """
    #version 460
    layout(location=0) out vec4 color;

    in vec3 v_color;
    in float v_ao;

    void main() {
        // Simple fake ambient occlusion (darker at bottom)
        vec3 final = v_color * (0.3 + 0.7 * v_ao);
        color = vec4(final, 1.0);
    }
    """

    pipe_draw = ctx.pipeline(
        vertex_shader=VS, fragment_shader=FS,
        layout=[
            {'name': 'Globals', 'binding': 3}
        ],
        resources=[
            {'type': 'storage_buffer', 'binding': 1, 'buffer': ssbo_visible},
            {'type': 'uniform_buffer', 'binding': 3, 'buffer': ubo_globals},
        ],
        topology='triangle_strip',
        viewport=(0, 0, WIDTH, HEIGHT),
        framebuffer=None,
        depth={'write': True, 'func': 'less'},
        cull_face='none'  # Grass needs to be double-sided
    )

    # =========================================================================
    # 5. LOOP
    # =========================================================================

    # Camera setup
    cam_yaw = 0.0
    cam_pitch = -0.5
    cam_pos = [0.0, 5.0, 0.0]

    last_time = time.time()
    frame_idx = 0
    start_time = time.time()

    print("Use WASD to move. Mouse to look. The grass reacts to the camera position.")

    while not glfw.window_should_close(window):
        glfw.poll_events()

        now = time.time()
        dt = now - last_time
        last_time = now
        total_time = now - start_time

        # --- CAMERA INPUT ---
        speed = 10.0 * dt
        if glfw.get_key(window, glfw.KEY_LEFT_SHIFT) == glfw.PRESS: speed *= 3.0

        forward = [math.sin(cam_yaw), 0.0, -math.cos(cam_yaw)]
        right = [math.cos(cam_yaw), 0.0, math.sin(cam_yaw)]

        if glfw.get_key(window, glfw.KEY_W) == glfw.PRESS:
            cam_pos[0] += forward[0] * speed
            cam_pos[2] += forward[2] * speed
        if glfw.get_key(window, glfw.KEY_S) == glfw.PRESS:
            cam_pos[0] -= forward[0] * speed
            cam_pos[2] -= forward[2] * speed
        if glfw.get_key(window, glfw.KEY_A) == glfw.PRESS:
            cam_pos[0] -= right[0] * speed
            cam_pos[2] -= right[2] * speed
        if glfw.get_key(window, glfw.KEY_D) == glfw.PRESS:
            cam_pos[0] += right[0] * speed
            cam_pos[2] += right[2] * speed

        if glfw.get_key(window, glfw.KEY_Q) == glfw.PRESS: cam_yaw -= 2.0 * dt
        if glfw.get_key(window, glfw.KEY_E) == glfw.PRESS: cam_yaw += 2.0 * dt

        # Interaction: Mouse Pos mapped to world X,Z near camera
        # For simplicity in this demo, let's make the "Mower" follow the camera position
        mower_x = cam_pos[0] + forward[0] * 3.0
        mower_z = cam_pos[2] + forward[2] * 3.0

        # --- MATRIX SETUP ---
        cam_target = [
            cam_pos[0] + math.sin(cam_yaw) * math.cos(cam_pitch),
            cam_pos[1] + math.sin(cam_pitch),
            cam_pos[2] - math.cos(cam_yaw) * math.cos(cam_pitch)
        ]

        view_proj = hypergl.camera(
            tuple(cam_pos), tuple(cam_target), (0, 1, 0),
            near=0.1, far=300.0, fov=60
        )

        # --- UPDATE GLOBALS ---
        # mat4(64), pos(12), time(4), mouse(8), pad(8) -> 96 bytes
        # Alignment rule: vec3 is 16 bytes (xyz + pad). vec2 is 8 bytes.
        ubo_data = bytearray(view_proj)
        ubo_data.extend(struct.pack('=3f f 2f 2f',
                                    *cam_pos, total_time, mower_x, mower_z, 0.0, 0.0
                                    ))
        ubo_globals.write(ubo_data)

        # --- AZDO FRAME ---
        ctx.new_frame(clear=True)

        idx = frame_idx % 2
        active_cmd = cmd_buffers[idx]
        active_cull = pipelines_cull[idx]

        # 1. Reset
        active_cmd.write(cmd_initial)

        # 2. Cull
        active_cull.run(NUM_GROUPS, 1, 1)

        # 3. Draw
        pipe_draw.render_indirect(buffer=active_cmd, count=1)

        ctx.end_frame()
        glfw.swap_buffers(window)
        frame_idx += 1

    ctx.release('all')
    glfw.terminate()


if __name__ == '__main__':
    main()