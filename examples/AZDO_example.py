import glfw
import struct
import random
import math
import hypergl
import time


class GLFWLoader:
    def load_opengl_function(self, name):
        return glfw.get_proc_address(name)


def main():
    if not glfw.init():
        raise RuntimeError("Failed to initialize GLFW")

    # --- FIX: Correct constants for OpenGL 4.6 ---
    glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 4)
    glfw.window_hint(glfw.CONTEXT_VERSION_MINOR, 6)
    glfw.window_hint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE)
    glfw.window_hint(glfw.RESIZABLE, False)

    WIDTH, HEIGHT = 1280, 720
    window = glfw.create_window(WIDTH, HEIGHT, "HyperGL AZDO - 100,000 Sprites", None, None)
    if not window:
        glfw.terminate()
        raise RuntimeError("Failed to create window. Do you have an OpenGL 4.6 capable GPU?")

    glfw.make_context_current(window)
    glfw.swap_interval(0)  # Unlock FPS

    hypergl.init(GLFWLoader())
    ctx = hypergl.context()

    print(f"GL Version: {ctx.info['version']}")
    print(f"Renderer:   {ctx.info['renderer']}")

    # --- SETUP TEXTURES ---
    NUM_UNIQUE_TEXTURES = 1000
    NUM_INSTANCES = 100000

    print(f"Creating {NUM_UNIQUE_TEXTURES} unique textures...")
    handles = []
    textures = []
    for i in range(NUM_UNIQUE_TEXTURES):
        tex = ctx.image((2, 2), format='rgba8unorm', texture=True)
        # Random colors
        c = [random.randint(50, 255) for _ in range(3)]
        pixels = struct.pack('=BBBB', *c, 255) * 4
        tex.write(pixels)

        h = tex.get_handle()
        tex.make_resident(True)
        handles.append(h)
        textures.append(tex)

    # Upload Handles to SSBO (Binding 0)
    handles_data = b''.join(struct.pack('=Q', h) for h in handles)
    ssbo_textures = ctx.buffer(data=handles_data, storage=True)

    # --- SETUP INDIRECT COMMANDS ---
    print(f"Generating commands for {NUM_INSTANCES} instances...")

    # struct DrawArraysIndirectCommand {
    #    uint count;         // 6 vertices per quad
    #    uint instanceCount; // 100,000 instances
    #    uint first;         // 0
    #    uint baseInstance;  // 0
    # }
    cmd_data = struct.pack('=IIII', 6, NUM_INSTANCES, 0, 0)
    indirect_buffer = ctx.buffer(data=cmd_data)

    # --- SHADERS ---
    vertex_shader = """
    #version 460
    #extension GL_ARB_bindless_texture : require

    layout(location=0) in vec2 in_pos;

    // Using uniform block for Time/Aspect to ensure std140 layout alignment
    layout(std140, binding = 1) uniform Globals {
        float u_time;
        float u_aspect;
    };

    out flat int texture_id;
    out vec2 uv;
    out vec3 v_color;

    float hash(float n) { return fract(sin(n) * 43758.5453123); }

    void main() {
        int id = gl_InstanceID;
        texture_id = id % 1000;

        float rnd = hash(float(id));
        float speed = 0.1 + rnd * 0.5;
        float size = 0.01 + hash(float(id) * 1.1) * 0.03;

        float angle = u_time * speed + rnd * 6.28;
        float radius = 0.2 + hash(float(id) * 2.2) * 1.5; 

        float x = cos(angle) * radius;
        float y = sin(angle) * radius;

        float z = sin(u_time * 0.5 + rnd * 10.0);
        float scale = size * (1.5 + z * 0.5);

        vec2 pos = vec2(x, y);
        pos.x /= u_aspect;

        gl_Position = vec4(pos + in_pos * scale, 0.0, 1.0);
        uv = in_pos * 0.5 + 0.5;
        v_color = vec3(1.0) * (0.6 + 0.4 * z);
    }
    """

    fragment_shader = """
    #version 460
    #extension GL_ARB_bindless_texture : require

    layout(location=0) out vec4 color;

    layout(std430, binding = 0) readonly buffer TextureManifest {
        sampler2D all_textures[]; 
    };

    in flat int texture_id;
    in vec2 uv;
    in vec3 v_color;

    void main() {
        vec4 texColor = texture(all_textures[texture_id], uv);
        color = texColor * vec4(v_color, 1.0);
    }
    """

    quad_data = struct.pack('=12f', -1., -1., 1., -1., -1., 1., -1., 1., 1., -1., 1., 1.)
    vbo = ctx.buffer(data=quad_data)

    # Create UBO for Globals (Time, Aspect)
    # 2 floats = 8 bytes, but std140 pads to vec4 (16 bytes) often.
    # Safe bet: allocate 16 bytes.
    # Create UBO for Globals
    ubo_globals = ctx.buffer(size=16, uniform=True, access='dynamic_draw')

    pipeline = ctx.pipeline(
        vertex_shader=vertex_shader,
        fragment_shader=fragment_shader,

        # --- Explicitly map the UBO name 'Globals' to Binding 1 ---
        layout=[
            {'name': 'Globals', 'binding': 1}
        ],

        vertex_buffers=hypergl.bind(vbo, '2f', 0),
        resources=[
            # Binding 0: SSBO (TextureManifest) - Validator ignores SSBOs usually
            {'type': 'storage_buffer', 'binding': 0, 'buffer': ssbo_textures},

            # Binding 1: UBO (Globals) - Must match the layout above
            {'type': 'uniform_buffer', 'binding': 1, 'buffer': ubo_globals}
        ],
        framebuffer=None,
        viewport=(0, 0, WIDTH, HEIGHT),
        topology='triangles',
        blend={'enable': True, 'src_color': 'src_alpha', 'dst_color': 'one', 'src_alpha': 'one', 'dst_alpha': 'one'}
    )

    print("Rendering...")

    start_time = time.time()
    frame_count = 0
    last_print = start_time

    while not glfw.window_should_close(window):
        glfw.poll_events()

        t = time.time() - start_time
        aspect = WIDTH / HEIGHT

        # Update UBO manually
        # Pack 2 floats. Since they are the first members of std140 block,
        # they pack as float, float, padding...
        ubo_data = struct.pack('=ff', t, aspect)
        ubo_globals.write(ubo_data)

        ctx.new_frame(clear=True)
        pipeline.render_indirect(buffer=indirect_buffer, count=1)
        ctx.end_frame()
        glfw.swap_buffers(window)

        frame_count += 1
        if time.time() - last_print > 1.0:
            print(f"FPS: {frame_count}")
            frame_count = 0
            last_print = time.time()

    ctx.release('all')
    glfw.destroy_window(window)
    glfw.terminate()


if __name__ == '__main__':
    main()