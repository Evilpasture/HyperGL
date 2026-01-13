import glfw
import struct
import math
import time
import hypergl
import array

# --- CONFIGURATION ---
WIDTH, HEIGHT = 1600, 900
GRID_W, GRID_H = 256, 256  # 65k Vertices (Safe/Stable)
TOTAL_VERTS = GRID_W * GRID_H
WORKGROUP_SIZE = 16


class GLFWLoader:
    def load_opengl_function(self, name):
        return glfw.get_proc_address(name)


def main():
    if not glfw.init(): raise RuntimeError("GLFW Init Failed")

    glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 4)
    glfw.window_hint(glfw.CONTEXT_VERSION_MINOR, 6)
    glfw.window_hint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE)
    glfw.window_hint(glfw.RESIZABLE, False)
    glfw.window_hint(glfw.DEPTH_BITS, 24)

    window = glfw.create_window(WIDTH, HEIGHT, f"HyperGL Cloth - {TOTAL_VERTS:,} Vertices", None, None)
    if not window: glfw.terminate(); raise RuntimeError("Window Creation Failed")

    glfw.make_context_current(window)
    glfw.swap_interval(0)

    hypergl.init(GLFWLoader())
    ctx = hypergl.context()
    print(f"Renderer: {ctx.info['renderer']}")

    # =========================================================================
    # 1. RESOURCES
    # =========================================================================

    ssbo_pos_curr = ctx.buffer(size=TOTAL_VERTS * 16, storage=True)
    ssbo_pos_prev = ctx.buffer(size=TOTAL_VERTS * 16, storage=True)
    ssbo_normals = ctx.buffer(size=TOTAL_VERTS * 16, storage=True)

    print("Generating Mesh Indices...")
    indices = array.array('I')
    # Generate triangle strip indices
    # (Grid-1)^2 quads * 6 indices per quad
    for y in range(GRID_H - 1):
        row1 = y * GRID_W
        row2 = (y + 1) * GRID_W
        for x in range(GRID_W - 1):
            p1 = row1 + x
            p2 = p1 + 1
            p3 = row2 + x
            p4 = p3 + 1
            indices.extend((p1, p3, p2, p2, p3, p4))

    ibo = ctx.buffer(data=indices, index=True)
    index_count = len(indices)

    # Globals
    ubo_globals = ctx.buffer(size=128, uniform=True, access='dynamic_draw')

    # =========================================================================
    # 2. COMPUTE SHADERS
    # =========================================================================

    # Init: Create flat grid
    CS_INIT = f"""
    #version 460
    layout(local_size_x={WORKGROUP_SIZE}, local_size_y={WORKGROUP_SIZE}) in;

    struct Vertex {{ vec3 pos; float pinned; }};
    layout(std430, binding=0) writeonly buffer P1 {{ Vertex curr[]; }};
    layout(std430, binding=1) writeonly buffer P2 {{ Vertex prev[]; }};

    void main() {{
        uvec2 id = gl_GlobalInvocationID.xy;
        if (id.x >= {GRID_W} || id.y >= {GRID_H}) return;
        uint idx = id.y * {GRID_W} + id.x;

        float x = (float(id.x) / {GRID_W - 1}) * 10.0 - 5.0;
        float y = 5.0; 
        float z = (float(id.y) / {GRID_H - 1}) * 10.0 - 5.0;

        vec3 p = vec3(x, y, z);

        float pinned = 0.0;
        // Pin top edge every 32 vertices to create "curtains"
        if (id.y == 0 && (id.x % 32 == 0)) pinned = 1.0; 

        curr[idx] = Vertex(p, pinned);
        prev[idx] = Vertex(p, pinned);
    }}
    """

    # Physics: Gravity + Verlet
    CS_PHYSICS = f"""
    #version 460
    layout(local_size_x={WORKGROUP_SIZE}, local_size_y={WORKGROUP_SIZE}) in;

    struct Vertex {{ vec3 pos; float pinned; }};
    layout(std430, binding=0) buffer P1 {{ Vertex curr[]; }};
    layout(std430, binding=1) buffer P2 {{ Vertex prev[]; }};

    // ALIGNMENT FIX: Use vec4 for mouse_pos to align to 16 bytes
    layout(std140, binding=2) uniform Globals {{
        mat4 vp;
        vec4 mouse_pos; // xyz=pos, w=padding
        float click_mode;
        float dt;
    }};

    void main() {{
        uvec2 id = gl_GlobalInvocationID.xy;
        if (id.x >= {GRID_W} || id.y >= {GRID_H}) return;
        uint idx = id.y * {GRID_W} + id.x;

        Vertex v = curr[idx];
        if (v.pinned != 0.0) return; 

        Vertex old_v = prev[idx];

        // Verlet Integration
        vec3 vel = (v.pos - old_v.pos) * 0.99; // 0.99 Damping
        vec3 next_pos = v.pos + vel;
        next_pos.y -= 9.8 * dt * dt; // Gravity

        // Sphere Collision
        vec3 m_pos = mouse_pos.xyz;
        vec3 to_mouse = next_pos - m_pos;
        if (length(to_mouse) < 1.5) {{
            if (click_mode > 1.5) {{ v.pinned = -1.0; }} // Cut
            else {{ next_pos = m_pos + normalize(to_mouse) * 1.5; }} // Push
        }}

        // Floor Collision
        if (next_pos.y < -5.0) next_pos.y = -5.0;

        prev[idx].pos = v.pos; 
        curr[idx] = Vertex(next_pos, v.pinned);
    }}
    """

    # Constraints: Spring Solver
    CS_CONSTRAINTS = f"""
    #version 460
    layout(local_size_x={WORKGROUP_SIZE}, local_size_y={WORKGROUP_SIZE}) in;
    struct Vertex {{ vec3 pos; float pinned; }};
    layout(std430, binding=0) buffer P1 {{ Vertex curr[]; }};

    void solve(uint idx1, uint idx2, float rest) {{
        Vertex v1 = curr[idx1]; Vertex v2 = curr[idx2];
        if (v1.pinned < 0.0 || v2.pinned < 0.0) return;

        vec3 delta = v2.pos - v1.pos;
        float dist = length(delta);
        if (dist < 0.0001) return; 

        float diff = (dist - rest) / dist;

        // STABILITY FIX: Low stiffness (0.2) + More Iterations
        // prevents explosion in parallel solver
        vec3 corr = delta * 0.2 * diff; 

        if (v1.pinned == 0.0) curr[idx1].pos += corr;
        if (v2.pinned == 0.0) curr[idx2].pos -= corr;
    }}

    void main() {{
        uvec2 id = gl_GlobalInvocationID.xy;
        if (id.x >= {GRID_W} || id.y >= {GRID_H}) return;
        uint idx = id.y * {GRID_W} + id.x;

        float rest_x = 10.0 / {GRID_W - 1};
        float rest_y = 10.0 / {GRID_H - 1};

        if (id.x < {GRID_W}-1) solve(idx, idx + 1, rest_x);         
        if (id.y < {GRID_H}-1) solve(idx, idx + {GRID_W}, rest_y); 
    }}
    """

    # Normals: Recalculate for lighting
    CS_NORMALS = f"""
    #version 460
    layout(local_size_x={WORKGROUP_SIZE}, local_size_y={WORKGROUP_SIZE}) in;
    struct Vertex {{ vec3 pos; float pinned; }};
    layout(std430, binding=0) readonly buffer P1 {{ Vertex curr[]; }};
    layout(std430, binding=3) writeonly buffer N {{ vec4 norms[]; }}; 

    void main() {{
        uvec2 id = gl_GlobalInvocationID.xy;
        if (id.x >= {GRID_W} || id.y >= {GRID_H}) return;
        uint idx = id.y * {GRID_W} + id.x;

        vec3 p = curr[idx].pos;
        vec3 n = vec3(0,1,0);

        if (id.x > 0 && id.y > 0) {{
            vec3 l = curr[idx - 1].pos;
            vec3 u = curr[idx - {GRID_W}].pos;
            n = normalize(cross(u - p, l - p));
        }}
        norms[idx] = vec4(n, 0.0);
    }}
    """

    gx, gy = math.ceil(GRID_W / 16), math.ceil(GRID_H / 16)

    pipe_init = ctx.compute(CS_INIT, resources=[
        {'type': 'storage_buffer', 'binding': 0, 'buffer': ssbo_pos_curr},
        {'type': 'storage_buffer', 'binding': 1, 'buffer': ssbo_pos_prev},
    ])
    pipe_physics = ctx.compute(CS_PHYSICS, resources=[
        {'type': 'storage_buffer', 'binding': 0, 'buffer': ssbo_pos_curr},
        {'type': 'storage_buffer', 'binding': 1, 'buffer': ssbo_pos_prev},
        {'type': 'uniform_buffer', 'binding': 2, 'buffer': ubo_globals},
    ])
    pipe_constraints = ctx.compute(CS_CONSTRAINTS, resources=[
        {'type': 'storage_buffer', 'binding': 0, 'buffer': ssbo_pos_curr},
    ])
    pipe_normals = ctx.compute(CS_NORMALS, resources=[
        {'type': 'storage_buffer', 'binding': 0, 'buffer': ssbo_pos_curr},
        {'type': 'storage_buffer', 'binding': 3, 'buffer': ssbo_normals},
    ])

    # Run Initial Generation
    pipe_init.run(gx, gy, 1)

    # =========================================================================
    # 4. RENDER PIPELINE
    # =========================================================================

    VS = """
    #version 460
    struct Vertex { vec3 pos; float pinned; };
    layout(std430, binding=0) readonly buffer P1 { Vertex verts[]; };
    layout(std430, binding=3) readonly buffer N { vec4 norms[]; };

    // ALIGNMENT MATCH
    layout(std140, binding=2) uniform Globals {
        mat4 vp;
        vec4 mouse_pos;
        float click_mode;
        float dt;
    };

    out vec3 v_normal;
    out vec3 v_pos;
    out float v_del;

    void main() {
        int id = gl_VertexID;
        Vertex v = verts[id];
        gl_Position = vp * vec4(v.pos, 1.0);
        v_normal = norms[id].xyz;
        v_pos = v.pos;
        v_del = v.pinned;
    }
    """

    FS = """
    #version 460
    layout(location=0) out vec4 color;

    in vec3 v_normal;
    in vec3 v_pos;
    in float v_del;

    layout(std140, binding=2) uniform Globals {
        mat4 vp;
        vec4 mouse_pos;
        float click_mode;
        float dt;
    };

    void main() {
        if (v_del < -0.5) discard; 

        vec3 N = normalize(v_normal);
        vec3 L = normalize(vec3(0.5, 1.0, 0.3)); 

        float diff = max(dot(N, L), 0.0);
        vec3 col = vec3(0.8, 0.2, 0.3); 

        if (!gl_FrontFacing) {
            col = vec3(0.6, 0.1, 0.2);
            diff = max(dot(-N, L), 0.0);
        }

        if (length(v_pos - mouse_pos.xyz) < 1.5) col += vec3(0.3, 0.3, 0.8);

        color = vec4(col * (0.2 + 0.8 * diff), 1.0);
    }
    """

    pipe_draw = ctx.pipeline(
        vertex_shader=VS, fragment_shader=FS,
        layout=[{'name': 'Globals', 'binding': 2}],
        resources=[
            {'type': 'storage_buffer', 'binding': 0, 'buffer': ssbo_pos_curr},
            {'type': 'storage_buffer', 'binding': 3, 'buffer': ssbo_normals},
            {'type': 'uniform_buffer', 'binding': 2, 'buffer': ubo_globals},
        ],
        topology='triangles',
        viewport=(0, 0, WIDTH, HEIGHT),
        index_buffer=ibo,
        framebuffer=None,
        depth={'write': True, 'func': 'less'},
        cull_face='none'
    )

    # =========================================================================
    # 5. LOOP
    # =========================================================================

    cam_pos = [0.0, 2.0, 12.0]
    mouse_sphere_z = 0.0

    print("Cloth Sim Running.")

    def scroll_callback(window, xoffset, yoffset):
        nonlocal mouse_sphere_z
        mouse_sphere_z += yoffset * 0.5

    glfw.set_scroll_callback(window, scroll_callback)

    cmd_draw = ctx.buffer(data=struct.pack('=IIIII', index_count, 1, 0, 0, 0))

    while not glfw.window_should_close(window):
        glfw.poll_events()

        # Hardcoded DT for physics stability (60 FPS assumption)
        dt = 0.016

        mx, my = glfw.get_cursor_pos(window)
        wx = (mx / WIDTH - 0.5) * 16.0
        wy = -(my / HEIGHT - 0.5) * 9.0 + 2.0
        mouse_world = [wx, wy, mouse_sphere_z]

        click_mode = 0.0
        if glfw.get_mouse_button(window, glfw.MOUSE_BUTTON_LEFT) == glfw.PRESS: click_mode = 1.0
        if glfw.get_mouse_button(window, glfw.MOUSE_BUTTON_RIGHT) == glfw.PRESS: click_mode = 2.0

        view_proj = hypergl.camera(tuple(cam_pos), (0, 0, 0), (0, 1, 0), near=0.1, far=100.0)

        # FIXED PACKING:
        # mat4 (64) + vec4 (16) + float (4) + float (4)
        # Python: 4f (mouse xyz + pad) + f (click) + f (dt)
        ubo_data = bytearray(view_proj)
        ubo_data.extend(struct.pack('=4f f f', *mouse_world, 0.0, click_mode, dt))
        ubo_globals.write(ubo_data)

        ctx.new_frame(clear=True)

        # 1. Physics Step
        pipe_physics.run(gx, gy, 1)

        # 2. Constraints (8 Iterations for reasonable stiffness)
        for _ in range(8):
            pipe_constraints.run(gx, gy, 1)

        # 3. Normals
        pipe_normals.run(gx, gy, 1)

        # 4. Draw
        pipe_draw.render_indirect(buffer=cmd_draw, count=1)

        ctx.end_frame()
        glfw.swap_buffers(window)

        if glfw.get_key(window, glfw.KEY_SPACE) == glfw.PRESS:
            pipe_init.run(gx, gy, 1)

    ctx.release('all')
    glfw.terminate()


if __name__ == '__main__':
    main()