import sys, time, threading, numpy as np, glfw, hypergl

WIDTH, HEIGHT = 800, 800
# [x, y, old_x, old_y] - Humanoid layout
points = np.array([
    [400, 150, 400, 150], # 0: Head
    [400, 250, 400, 250], # 1: Hips
    [330, 200, 330, 200], # 2: L-Shoulder
    [470, 200, 470, 200], # 3: R-Shoulder
    [300, 150, 300, 150], # 4: L-Hand
    [500, 150, 500, 150], # 5: R-Hand
    [370, 400, 370, 400], # 6: L-Foot
    [430, 400, 430, 400]  # 7: R-Foot
], dtype='f4')

# (A, B, Target Length)
bones = [
    # --- VISIBLE SKELETON ---
    (0, 1, 100),  # 0: Head to 1: Hips (Spine)
    (1, 2, 80), (1, 3, 80),  # 1: Hips to 2,3: Shoulders
    (2, 4, 70), (3, 5, 70),  # 2,3: Shoulders to 4,5: Hands
    (1, 6, 150), (1, 7, 150),  # 1: Hips to 6,7: Feet

    # --- THE "SPINE STIFFENER" (Invisible) ---
    (0, 2, 90), (0, 3, 90),  # Connect Head to both Shoulders (Prevents 180 neck snap)
    (2, 3, 140),  # Shoulder to Shoulder (Prevents chest collapse)
    (2, 6, 220), (3, 7, 220)  # Shoulder to opposite Hip/Leg (Prevents spine folding)
]

# We ONLY want to draw the original 7 bones, not the muscles!
# So we keep the index buffer for lines exactly as it was.
indices = np.array([0, 1, 1, 2, 1, 3, 2, 4, 3, 5, 1, 6, 1, 7], dtype='u4')

mouse_state = {'x': 0, 'y': 0, 'active': False}


def physics_worker(pts):
    gravity, ground = 0.35, 780
    # Different friction for different body parts?
    # For now, let's just tighten the global friction
    friction = 0.96

    while True:
        # 1. Verlet Movement
        for i, p in enumerate(pts):
            # Head (0) and Hips (1) have less friction (heavier)
            # Hands and feet have more friction (air resistance)
            current_friction = friction if i > 1 else 0.99

            vx = (p[0] - p[2]) * current_friction
            vy = (p[1] - p[3]) * current_friction
            p[2], p[3] = p[0], p[1]
            p[0], p[1] = p[0] + vx, p[1] + vy + gravity

        # 2. Mouse Drag (Pin the Head)
        if mouse_state['active']:
            pts[0][0], pts[0][1] = mouse_state['x'], mouse_state['y']
            pts[0][2], pts[0][3] = mouse_state['x'], mouse_state['y']

        # 3. Constraint Solver (The "Skeleton" Glue)
        for _ in range(50):  # Increased from 20 to 50
            for i1, i2, dist in bones:
                p1, p2 = pts[i1], pts[i2]
                dx, dy = p2[0] - p1[0], p2[1] - p1[1]
                current = np.sqrt(dx * dx + dy * dy) or 0.1
                diff = (dist - current) / current * 0.5
                p1[0] -= dx * diff;
                p1[1] -= dy * diff
                p2[0] += dx * diff;
                p2[1] += dy * diff

        # 4. Bound Collisions
        for p in pts:
            p[1] = min(max(p[1], 10), ground)
            p[0] = min(max(p[0], 10), WIDTH-10)
        time.sleep(0.008)

# --- ENGINE SETUP ---
if not glfw.init(): sys.exit()
window = glfw.create_window(WIDTH, HEIGHT, "Ragdoll Skeleton", None, None)
glfw.make_context_current(window)
ctx = hypergl.context()
image = ctx.image((WIDTH, HEIGHT), format='rgba8unorm')

pos_buf = ctx.buffer(data=points[:, :2].tobytes(), storage=True)
idx_buf = ctx.buffer(data=indices.tobytes(), storage=False)
gpu_pts = np.frombuffer(pos_buf.map(), dtype='f4').reshape(-1, 2)

vs = '''#version 460 core
layout(std430, binding = 0) buffer P { vec2 p[]; };
void main() {
    vec2 v = p[gl_VertexID];
    gl_Position = vec4((v.x/400.0)-1.0, 1.0-(v.y/400.0), 0.0, 1.0);
    gl_PointSize = 12.0; // MAKE JOINTS VISIBLE
}'''
fs = '''#version 460 core
out vec4 f; void main() { f = vec4(1.0, 0.5, 0.2, 1.0); }''' # Bright Orange

pipe_pts = ctx.pipeline(vertex_shader=vs, fragment_shader=fs, resources=[{'type': 'storage_buffer', 'binding': 0, 'buffer': pos_buf}],
                        framebuffer=[image], topology='points', vertex_count=len(points))

pipe_lines = ctx.pipeline(vertex_shader=vs, fragment_shader=fs, resources=[{'type': 'storage_buffer', 'binding': 0, 'buffer': pos_buf}],
                         framebuffer=[image], topology='lines', index_buffer=idx_buf, vertex_count=len(indices))

threading.Thread(target=physics_worker, args=(points,), daemon=True).start()

while not glfw.window_should_close(window):
    mx, my = glfw.get_cursor_pos(window)
    mouse_state['x'], mouse_state['y'] = mx, my
    mouse_state['active'] = glfw.get_mouse_button(window, glfw.MOUSE_BUTTON_LEFT) == glfw.PRESS

    gpu_pts[:] = points[:, :2]
    image.clear()
    pipe_lines.render()
    pipe_pts.render()
    image.blit()
    glfw.swap_buffers(window)
    glfw.poll_events()