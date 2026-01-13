import glfw
import struct
import math
import time
import hypergl
import random

# --- CONFIGURATION ---
WIDTH, HEIGHT = 1600, 900
INSTANCE_COUNT = 250_000
WORKGROUP_SIZE = 256
NUM_GROUPS = math.ceil(INSTANCE_COUNT / WORKGROUP_SIZE)
TEXTURE_COUNT = 16


class GLFWLoader:
    def load_opengl_function(self, name):
        return glfw.get_proc_address(name)


def main():
    if not glfw.init(): raise RuntimeError("GLFW Init Failed")

    # Request OpenGL 4.6 Core
    glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 4)
    glfw.window_hint(glfw.CONTEXT_VERSION_MINOR, 6)
    glfw.window_hint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE)
    glfw.window_hint(glfw.RESIZABLE, False)
    glfw.window_hint(glfw.DEPTH_BITS, 24)

    window = glfw.create_window(WIDTH, HEIGHT, f"HyperGL Infinite City - {INSTANCE_COUNT:,} Objects", None, None)
    if not window: glfw.terminate(); raise RuntimeError("Window Creation Failed")

    glfw.make_context_current(window)
    glfw.swap_interval(0)  # Unlock FPS

    hypergl.init(GLFWLoader())
    ctx = hypergl.context()

    print(f"Renderer: {ctx.info['renderer']}")
    print("Generating Resources...")

    # =========================================================================
    # 1. RESOURCES: BINDLESS TEXTURES
    # =========================================================================
    ssbo_texture_handles = ctx.buffer(size=TEXTURE_COUNT * 8, storage=True)
    textures = []

    for i in range(TEXTURE_COUNT):
        size = 32
        tex_data = bytearray(size * size * 4)

        r = random.randint(80, 200)
        g = random.randint(50, 100)
        b = random.randint(50, 80)

        for y in range(size):
            for x in range(size):
                row = y // 8
                shift = (row % 2) * 8
                is_mortar = (y % 8 == 0) or ((x + shift) % 16 == 0)

                idx = (y * size + x) * 4
                if is_mortar:
                    tex_data[idx:idx + 4] = b'\x30\x30\x30\xFF'
                else:
                    noise = random.randint(-10, 10)
                    tex_data[idx] = max(0, min(255, r + noise))
                    tex_data[idx + 1] = max(0, min(255, g + noise))
                    tex_data[idx + 2] = max(0, min(255, b + noise))
                    tex_data[idx + 3] = 255

        img = ctx.image((size, size), format='rgba8unorm', data=tex_data, texture=True)
        img.get_handle()
        img.make_resident(True)

        ssbo_texture_handles.write_texture_handle(i * 8, img)
        textures.append(img)

    # =========================================================================
    # 2. RESOURCES: SCENE DATA
    # =========================================================================

    world_data = bytearray(INSTANCE_COUNT * 16)

    grid_size = int(math.sqrt(INSTANCE_COUNT))
    spacing = 2.0

    ptr = 0
    for i in range(INSTANCE_COUNT):
        x = (i % grid_size) * spacing - (grid_size * spacing / 2)
        z = (i // grid_size) * spacing - (grid_size * spacing / 2)

        # Add random.uniform(-0.01, 0.01) to Y
        # This ensures no two roofs are mathematically identical
        y = (math.sin(x * 0.05) + math.cos(z * 0.05)) * 10.0
        y += random.uniform(-0.01, 0.01)

        # y = (math.sin(x * 0.05) + math.cos(z * 0.05)) * 10.0
        # y += (math.sin(x * 0.2) * math.cos(z * 0.2)) * 2.0

        tex_id = random.randint(0, TEXTURE_COUNT - 1)

        struct.pack_into('=fffi', world_data, ptr, x, y, z, tex_id)
        ptr += 16

    ssbo_world = ctx.buffer(data=world_data, storage=True)
    ssbo_visible = ctx.buffer(size=INSTANCE_COUNT * 16, storage=True)

    cmd_initial = struct.pack('=IIII', 36, 0, 0, 0)

    # Double Buffered Indirect Commands
    cmd_buffers = [
        ctx.buffer(data=cmd_initial),
        ctx.buffer(data=cmd_initial)
    ]

    ubo_globals = ctx.buffer(size=128, uniform=True, access='dynamic_draw')

    # =========================================================================
    # 3. COMPUTE SHADERS (Culling)
    # =========================================================================

    CS_CULL = f"""
    #version 460
    layout(local_size_x={WORKGROUP_SIZE}) in;

    struct Instance {{ vec3 pos; uint tex_id; }};

    layout(std430, binding=0) readonly buffer World {{ Instance all_instances[]; }};
    layout(std430, binding=1) writeonly buffer Visible {{ Instance visible_instances[]; }};
    layout(std430, binding=2) buffer DrawCmd {{
        uint count;
        uint instanceCount; 
        uint first;
        uint baseInstance;
    }};

    layout(std140, binding=3) uniform Globals {{
        mat4 view_proj;
        vec3 cam_pos;
        float padding;
        vec3 cam_dir;
    }};

    void main() {{
        uint id = gl_GlobalInvocationID.x;
        if (id >= {INSTANCE_COUNT}) return;

        Instance obj = all_instances[id];

        // --- GPU CULLING LOGIC ---
        vec3 to_obj = obj.pos - cam_pos;
        float dist_sq = dot(to_obj, to_obj);

        bool visible = true;

        if (dist_sq > 25000.0) visible = false; 

        if (visible) {{
            vec3 dir = normalize(to_obj);
            float angle = dot(dir, cam_dir);
            if (angle < 0.5 && dist_sq > 100.0) visible = false;
        }}

        if (visible) {{
            uint index = atomicAdd(instanceCount, 1);
            visible_instances[index] = obj;
        }}
    }}
    """

    pipelines_cull = [
        ctx.compute(CS_CULL, resources=[
            {'type': 'storage_buffer', 'binding': 0, 'buffer': ssbo_world},
            {'type': 'storage_buffer', 'binding': 1, 'buffer': ssbo_visible},
            {'type': 'storage_buffer', 'binding': 2, 'buffer': cmd_buffers[0]},
            {'type': 'uniform_buffer', 'binding': 3, 'buffer': ubo_globals},
        ]),
        ctx.compute(CS_CULL, resources=[
            {'type': 'storage_buffer', 'binding': 0, 'buffer': ssbo_world},
            {'type': 'storage_buffer', 'binding': 1, 'buffer': ssbo_visible},
            {'type': 'storage_buffer', 'binding': 2, 'buffer': cmd_buffers[1]},
            {'type': 'uniform_buffer', 'binding': 3, 'buffer': ubo_globals},
        ])
    ]

    # =========================================================================
    # 4. RENDER PIPELINE
    # =========================================================================

    VS = """
    #version 460

    struct Instance { vec3 pos; uint tex_id; };
    layout(std430, binding=1) readonly buffer Visible { Instance instances[]; };

    layout(std140, binding=3) uniform Globals {
        mat4 view_proj;
    };

    out vec3 v_uv;
    out flat uint v_tex;

    void main() {
        Instance inst = instances[gl_InstanceID];

        int v = gl_VertexID % 36;
        vec3 box[8] = vec3[]( 
            vec3(-0.5,-0.5,0.5), vec3(0.5,-0.5,0.5), vec3(0.5,0.5,0.5), vec3(-0.5,0.5,0.5), 
            vec3(-0.5,-0.5,-0.5), vec3(0.5,-0.5,-0.5), vec3(0.5,0.5,-0.5), vec3(-0.5,0.5,-0.5) 
        );
        int idx[36] = int[]( 
            0,1,2, 2,3,0, 1,5,6, 6,2,1, 7,6,5, 5,4,7, 4,0,3, 3,7,4, 4,5,1, 1,0,4, 3,2,6, 6,7,3 
        );

        vec3 local_pos = box[idx[v]];

        gl_Position = view_proj * vec4(inst.pos + local_pos, 1.0);
        v_uv = local_pos + 0.5;
        v_tex = inst.tex_id;
    }
    """

    FS = """
    #version 460
    #extension GL_ARB_bindless_texture : require
    layout(location=0) out vec4 color;

    layout(std430, binding=4) readonly buffer TexHeap { sampler2D textures[]; };

    in vec3 v_uv;
    in flat uint v_tex;

    void main() {
        vec4 t = texture(textures[v_tex], v_uv.xy);
        color = t;
    }
    """

    pipe_draw = ctx.pipeline(
        vertex_shader=VS, fragment_shader=FS,
        layout=[{'name': 'Globals', 'binding': 3}],
        resources=[
            {'type': 'storage_buffer', 'binding': 1, 'buffer': ssbo_visible},
            {'type': 'uniform_buffer', 'binding': 3, 'buffer': ubo_globals},
            {'type': 'storage_buffer', 'binding': 4, 'buffer': ssbo_texture_handles},
        ],
        topology='triangles',
        viewport=(0, 0, WIDTH, HEIGHT),
        framebuffer=None,
        depth={'write': True, 'func': 'less'},
        cull_face='back'
    )

    # =========================================================================
    # 5. MAIN LOOP
    # =========================================================================

    camera_yaw = 0.0
    camera_pos = [0.0, 30.0, 0.0]
    last_time = time.time()
    frame_idx = 0

    print("Double Buffered GPU Culling Active.")

    while not glfw.window_should_close(window):
        glfw.poll_events()
        now = time.time()
        dt = now - last_time
        last_time = now

        # --- CAMERA ---
        speed = 20.0 * dt
        if glfw.get_key(window, glfw.KEY_W) == glfw.PRESS:
            camera_pos[0] += math.sin(camera_yaw) * speed
            camera_pos[2] -= math.cos(camera_yaw) * speed
        if glfw.get_key(window, glfw.KEY_S) == glfw.PRESS:
            camera_pos[0] -= math.sin(camera_yaw) * speed
            camera_pos[2] += math.cos(camera_yaw) * speed
        if glfw.get_key(window, glfw.KEY_A) == glfw.PRESS: camera_yaw -= 2.0 * dt
        if glfw.get_key(window, glfw.KEY_D) == glfw.PRESS: camera_yaw += 2.0 * dt

        cam_dir = [math.sin(camera_yaw), 0.0, -math.cos(camera_yaw)]
        cam_target = [camera_pos[0] + cam_dir[0], camera_pos[1], camera_pos[2] + cam_dir[2]]

        # Upload Camera Data
        view_proj = hypergl.camera(
            tuple(camera_pos),
            tuple(cam_target),
            (0, 1, 0),
            near=1.0,  # <--- CHANGED
            far=2000.0
        )
        ubo_data = bytearray(view_proj)
        ubo_data.extend(struct.pack('=3f f 3f f', *camera_pos, 0, *cam_dir, 0))
        ubo_globals.write(ubo_data)

        # --- AZDO FRAME START ---
        ctx.new_frame(clear=True)

        idx = frame_idx % 2
        active_cmd = cmd_buffers[idx]
        active_cull_pipe = pipelines_cull[idx]

        active_cmd.write(cmd_initial)
        active_cull_pipe.run(NUM_GROUPS, 1, 1)
        pipe_draw.render_indirect(buffer=active_cmd, count=1)

        ctx.end_frame()
        glfw.swap_buffers(window)
        frame_idx += 1

    ctx.release('all')
    glfw.terminate()


if __name__ == '__main__':
    main()