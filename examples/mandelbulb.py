import sys, time, threading, numpy as np, glfw, hypergl


# --- 1. THE PARAMETER WORKER (CPU SCULPTOR) ---
# We are sharing a small buffer of 16 floats (a 4x4 matrix)
def worker_loop(params_view):
    t = 0.0
    while True:
        # Calculate a 4D rotation matrix based on time
        # This "folds" the fractal differently every frame
        s, c = np.sin(t), np.cos(t)

        # We write directly to the GPU-mapped memory
        params_view[0] = c  # Rotation X
        params_view[1] = s
        params_view[2] = np.sin(t * 0.5)  # Morph factor
        params_view[3] = 4.0 + np.sin(t) * 2.0  # Fractal Power (Iteration weight)

        t += 0.01
        time.sleep(0.005)


# --- 2. ENGINE SETUP ---
if not glfw.init(): sys.exit()
window = glfw.create_window(800, 800, "HyperGL | Fractal Raymarcher", None, None)
glfw.make_context_current(window)
ctx = hypergl.context()

# We only need 16 floats for our parameters
param_data = np.zeros(16, dtype='f4')
ssbo = ctx.buffer(data=param_data.tobytes(), storage=True)
params_view = np.frombuffer(ssbo.map(), dtype='f4')

# --- 3. RAYMARCHING SHADER ---
# This shader renders a 3D scene by "marching" a ray into a distance field
render_vs = '''#version 460 core
out vec2 v_uv;
void main() {
    v_uv = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
    gl_Position = vec4(v_uv * 2.0 - 1.0, 0.0, 1.0);
}'''

render_fs = '''#version 460 core
layout(std430, binding = 0) buffer B { float p[]; };
in vec2 v_uv; out vec4 f;

// Fractal Distance Function
float map(vec3 pos) {
    vec3 z = pos;
    float dr = 1.0;
    float r = 0.0;
    float power = p[3]; // Controlled by CPU
    for (int i = 0; i < 8 ; i++) {
        r = length(z);
        if (r > 2.0) break;
        float theta = acos(z.z/r) * power + p[0]; // CPU Rotations
        float phi = atan(z.y, z.x) * power + p[1];
        dr =  pow(r, power - 1.0) * power * dr + 1.0;
        float zr = pow(r, power);
        z = zr * vec3(sin(theta)*cos(phi), sin(phi)*sin(theta), cos(theta));
        z += pos;
    }
    return 0.5 * log(r) * r / dr;
}

void main() {
    vec3 ro = vec3(0, 0, -3); // Camera
    vec3 rd = normalize(vec3(v_uv * 2.0 - 1.0, 1.5));
    float t = 0.0;
    for(int i=0; i<64; i++) {
        float d = map(ro + rd * t);
        if(d < 0.001 || t > 10.0) break;
        t += d;
    }
    vec3 col = vec3(1.0 - t * 0.2); // Simple fog
    col *= vec3(p[2], 0.5, 1.0 - p[2]); // CPU Color morphing
    f = vec4(col, 1.0);
}'''

# --- 4. PIPELINE ---
image = ctx.image((800, 800), format='rgba8unorm')
pipe = ctx.pipeline(
    vertex_shader=render_vs,
    fragment_shader=render_fs,
    resources=[{'type': 'storage_buffer', 'binding': 0, 'buffer': ssbo}],
    framebuffer=[image],
    topology='triangles',
    vertex_count=3
)

threading.Thread(target=worker_loop, args=(params_view,), daemon=True).start()

while not glfw.window_should_close(window):
    image.clear()
    pipe.render()
    image.blit()
    glfw.swap_buffers(window)
    glfw.poll_events()