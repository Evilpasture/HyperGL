import glfw
import struct
import math
import time
import hypergl
import random

# --- CONFIGURATION ---
WIDTH, HEIGHT = 1280, 720
AGENT_COUNT = 1_000_000
WORKGROUP_SIZE = 256
NUM_GROUPS = math.ceil(AGENT_COUNT / WORKGROUP_SIZE)

# Simulation Parameters
SENSOR_ANGLE = 45.0 * (3.14159 / 180.0)
SENSOR_DIST = 9.0
TURN_SPEED = 0.2
DECAY_RATE = 0.98
DIFFUSE_RATE = 1.0


class GLFWLoader:
    def load_opengl_function(self, name):
        return glfw.get_proc_address(name)


def main():
    if not glfw.init(): raise RuntimeError("GLFW Init Failed")

    glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 4)
    glfw.window_hint(glfw.CONTEXT_VERSION_MINOR, 6)
    glfw.window_hint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE)
    glfw.window_hint(glfw.RESIZABLE, False)

    window = glfw.create_window(WIDTH, HEIGHT, f"HyperGL Slime Mold - {AGENT_COUNT:,} Agents", None, None)
    if not window: glfw.terminate(); raise RuntimeError("Window Creation Failed")

    glfw.make_context_current(window)
    glfw.swap_interval(0)

    hypergl.init(GLFWLoader())
    ctx = hypergl.context()
    print(f"Renderer: {ctx.info['renderer']}")

    # =========================================================================
    # 1. RESOURCES
    # =========================================================================

    # A. Agent Data
    agent_data = bytearray(AGENT_COUNT * 16)
    print("Seeding Agents...")

    ptr = 0
    for i in range(AGENT_COUNT):
        r = math.sqrt(random.random()) * (HEIGHT * 0.4)
        theta = random.random() * 6.28

        x = WIDTH / 2 + math.cos(theta) * r
        y = HEIGHT / 2 + math.sin(theta) * r
        angle = theta + 3.14

        struct.pack_into('=fffi', agent_data, ptr, x, y, angle, 0)
        ptr += 16

    ssbo_agents = ctx.buffer(data=agent_data, storage=True)

    # B. Trail Maps
    trail_map_a = ctx.image((WIDTH, HEIGHT), format='rgba16float', texture=True)
    trail_map_b = ctx.image((WIDTH, HEIGHT), format='rgba16float', texture=True)

    trail_map_a.get_handle();
    trail_map_a.make_resident(True)
    trail_map_b.get_handle();
    trail_map_b.make_resident(True)

    # C. Globals
    ubo_globals = ctx.buffer(size=64, uniform=True, access='dynamic_draw')

    # D. Dummy Draw Command (for Indirect Drawing of Points)
    cmd_draw = ctx.buffer(data=struct.pack('=IIII', AGENT_COUNT, 1, 0, 0))

    # =========================================================================
    # 2. PIPELINE: AGENT LOGIC (COMPUTE)
    # =========================================================================

    CS_UPDATE = f"""
    #version 460
    layout(local_size_x={WORKGROUP_SIZE}) in;

    struct Agent {{ vec2 pos; float angle; float pad; }};

    layout(std430, binding=0) buffer Agents {{ Agent agents[]; }};
    layout(binding=1) uniform sampler2D trailMap;

    layout(std140, binding=2) uniform Globals {{ 
        float time; float dt; vec2 res; 
        float sensorAngle; float sensorDist; float turnSpeed;
    }};

    float hash(float n) {{ return fract(sin(n) * 43758.5453); }}

    float sense(Agent a, float angleOffset) {{
        float angle = a.angle + angleOffset;
        vec2 dir = vec2(cos(angle), sin(angle));
        vec2 pos = a.pos + dir * {SENSOR_DIST:.1f};
        if (pos.x < 0 || pos.x > res.x || pos.y < 0 || pos.y > res.y) return 0.0;
        return texture(trailMap, pos / res).r;
    }}

    void main() {{
        uint id = gl_GlobalInvocationID.x;
        if (id >= {AGENT_COUNT}) return;

        Agent a = agents[id];
        float rnd = hash(float(id) * time);

        float left   = sense(a, {SENSOR_ANGLE:.3f});
        float center = sense(a, 0.0);
        float right  = sense(a, -{SENSOR_ANGLE:.3f});

        float turn = {TURN_SPEED:.3f} * dt * 60.0; 

        if (center > left && center > right) {{ }}
        else if (center < left && center < right) {{ a.angle += (rnd - 0.5) * 2.0 * turn; }}
        else if (right > left) {{ a.angle -= turn; }}
        else if (left > right) {{ a.angle += turn; }}

        vec2 dir = vec2(cos(a.angle), sin(a.angle));
        a.pos += dir * 60.0 * dt; 

        if (a.pos.x < 0.0) a.pos.x = res.x;
        if (a.pos.x > res.x) a.pos.x = 0.0;
        if (a.pos.y < 0.0) a.pos.y = res.y;
        if (a.pos.y > res.y) a.pos.y = 0.0;

        agents[id] = a;
    }}
    """

    # =========================================================================
    # 3. PIPELINE: DEPOSIT (RENDER POINTS)
    # =========================================================================

    VS_DEPOSIT = """
        #version 460
        struct Agent { vec2 pos; float angle; float pad; };
        layout(std430, binding=0) readonly buffer Agents { Agent agents[]; };

        layout(std140, binding=2) uniform Globals { 
            float time; float dt; vec2 res; 
        };

        out vec3 v_color;

        // HSL to RGB helper
        vec3 hsv2rgb(vec3 c) {
            vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
            vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
            return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
        }

        void main() {
            Agent a = agents[gl_VertexID];
            vec2 ndc = (a.pos / res) * 2.0 - 1.0;
            gl_Position = vec4(ndc, 0.0, 1.0);
            gl_PointSize = 1.0;

            // Color based on Angle
            v_color = hsv2rgb(vec3(a.angle / 6.28, 1.0, 1.0));
        }
        """

    FS_DEPOSIT = """
        #version 460
        layout(location=0) out vec4 color;
        in vec3 v_color;
        void main() {
            // Draw colored pheromones
            color = vec4(v_color, 0.1); 
        }
        """

    pipe_deposit = ctx.pipeline(
        vertex_shader=VS_DEPOSIT, fragment_shader=FS_DEPOSIT,
        layout=[{'name': 'Globals', 'binding': 2}],
        resources=[
            {'type': 'storage_buffer', 'binding': 0, 'buffer': ssbo_agents},
            {'type': 'uniform_buffer', 'binding': 2, 'buffer': ubo_globals}
        ],
        topology='points',
        viewport=(0, 0, WIDTH, HEIGHT),
        blend={'enable': True, 'src_color': 'one', 'dst_color': 'one', 'src_alpha': 'one', 'dst_alpha': 'one'},
        framebuffer=None
    )

    # =========================================================================
    # 4. PIPELINE: DIFFUSE & DECAY
    # =========================================================================

    VS_QUAD = """
    #version 460
    layout(location=0) in vec2 in_pos;
    out vec2 v_uv;
    void main() {
        v_uv = in_pos * 0.5 + 0.5;
        gl_Position = vec4(in_pos, 0.0, 1.0);
    }
    """

    FS_DIFFUSE = f"""
    #version 460
    layout(location=0) out vec4 color;
    in vec2 v_uv;

    layout(binding=0) uniform sampler2D inputTex;

    void main() {{
        vec2 texel = 1.0 / textureSize(inputTex, 0);
        vec4 sum = vec4(0.0);
        for(int x=-1; x<=1; x++) {{
            for(int y=-1; y<=1; y++) {{
                sum += texture(inputTex, v_uv + vec2(x, y) * texel);
            }}
        }}
        color = (sum / 9.0) * {DECAY_RATE};
    }}
    """

    quad_vbo = ctx.buffer(data=struct.pack('=8f', -1, -1, 1, -1, -1, 1, 1, 1))

    pipe_diffuse = ctx.pipeline(
        vertex_shader=VS_QUAD, fragment_shader=FS_DIFFUSE,
        vertex_buffers=hypergl.bind(quad_vbo, '2f', 0),
        layout=[{'name': 'inputTex', 'binding': 0}],
        resources=[{'type': 'sampler', 'binding': 0, 'image': trail_map_a}],  # Placeholder validation fix
        viewport=(0, 0, WIDTH, HEIGHT),
        framebuffer=None
    )

    # =========================================================================
    # 5. MAIN LOOP
    # =========================================================================

    last_time = time.time()
    idx = 0
    fbos = [[trail_map_a], [trail_map_b]]
    images = [trail_map_a, trail_map_b]

    # Pre-create pipeline variants
    pipes_deposit = [
        ctx.pipeline(template=pipe_deposit, framebuffer=fbos[0]),
        ctx.pipeline(template=pipe_deposit, framebuffer=fbos[1])
    ]

    pipes_diffuse = [
        ctx.pipeline(template=pipe_diffuse, framebuffer=fbos[0],
                     resources=[{'type': 'sampler', 'binding': 0, 'image': images[1]}]),
        ctx.pipeline(template=pipe_diffuse, framebuffer=fbos[1],
                     resources=[{'type': 'sampler', 'binding': 0, 'image': images[0]}]),
    ]

    pipes_logic = [
        ctx.compute(CS_UPDATE, resources=[
            {'type': 'storage_buffer', 'binding': 0, 'buffer': ssbo_agents},
            {'type': 'uniform_buffer', 'binding': 2, 'buffer': ubo_globals},
            {'type': 'sampler', 'binding': 1, 'image': images[1]}
        ]),
        ctx.compute(CS_UPDATE, resources=[
            {'type': 'storage_buffer', 'binding': 0, 'buffer': ssbo_agents},
            {'type': 'uniform_buffer', 'binding': 2, 'buffer': ubo_globals},
            {'type': 'sampler', 'binding': 1, 'image': images[0]}
        ])
    ]

    print("Slime Mold Simulation Running...")

    while not glfw.window_should_close(window):
        glfw.poll_events()
        now = time.time()
        dt = min(now - last_time, 0.05)
        last_time = now

        ubo_globals.write(struct.pack('=4f', now, dt, WIDTH, HEIGHT))

        curr = idx % 2
        prev = (idx + 1) % 2

        ctx.new_frame(clear=False)

        # 1. Diffuse (Read Prev -> Write Curr)
        pipes_diffuse[curr].render()

        # 2. Logic (Read Prev)
        pipes_logic[curr].run(NUM_GROUPS, 1, 1)

        # 3. Deposit (Write Curr)
        pipes_deposit[curr].render_indirect(buffer=cmd_draw, count=1)

        # 4. Blit to Screen
        images[curr].blit()

        ctx.end_frame()
        glfw.swap_buffers(window)
        idx += 1

    ctx.release('all')
    glfw.terminate()


if __name__ == '__main__':
    main()