import sys
import time
import struct
import threading
import numpy as np
import glfw
import hypergl
from numba import njit, prange

# --- 1. CONFIGURATION ---
WIDTH, HEIGHT = 800, 600
CELL_COUNT = WIDTH * HEIGHT


# We use float32 to stay compatible with your existing vec4 SSBO if needed,
# but here we'll use a single float32 per cell (0.0=Empty, 1.0=Sand)
# To keep the shader simple, we'll store color data in the float.

# --- 2. THE SAND KERNEL ---
@njit(parallel=True, fastmath=True, nogil=True)
def update_sand_kernel(grid, next_grid, w, h, flip_flop):
    for i in prange(h - 1):
        y = (h - 2) - i
        for x in range(1, w - 1):
            idx = y * w + x
            val = grid[idx]

            if val > 0.1:
                # --- NEW: FLOOR CHECK ---
                if y >= h - 2:
                    next_grid[idx] = val
                    continue

                # 1. Try STRAIGHT DOWN
                below = (y + 1) * w + x
                if grid[below] < 0.1:
                    next_grid[below] = val
                    continue

                    # 2. Try DIAGONALS
                dir = 1 if flip_flop == 0 else -1
                side_a = (y + 1) * w + (x + dir)
                side_b = (y + 1) * w + (x - dir)

                if grid[side_a] < 0.1:
                    next_grid[side_a] = val
                elif grid[side_b] < 0.1:
                    next_grid[side_b] = val
                else:
                    # 3. All blocked, stay in place
                    next_grid[idx] = val


# --- 3. WORKER THREAD ---
def worker_loop(gpu_view, w, h):
    next_state = np.zeros_like(gpu_view)
    flip_flop = 0
    while True:
        # Clear the scratchpad every frame
        next_state.fill(0.0)

        # Run physics
        update_sand_kernel(gpu_view, next_state, w, h, flip_flop)

        # Commit back to the GPU (PCIe 4.0 handles this easily)
        gpu_view[:] = next_state[:]

        flip_flop = 1 - flip_flop


# --- 4. ENGINE SETUP ---
if not glfw.init(): sys.exit()
window = glfw.create_window(WIDTH, HEIGHT, "HyperGL | Persistent Sand", None, None)
glfw.make_context_current(window)
glfw.swap_interval(0)
ctx = hypergl.context()

# --- 5. DATA ALLOCATION (0x3C2 CONTRACT) ---
# Each cell is a float32. 0.0 = air, >0.0 = sand color
grid_data = np.zeros(CELL_COUNT, dtype='f4')
sand_buffer = ctx.buffer(data=grid_data.tobytes(), storage=True)

# Map for the worker
m = sand_buffer.map()
gpu_view = np.frombuffer(m, dtype='f4')

# --- 6. SHADERS ---
render_vs = '''
#version 460 core
layout(std430, binding = 0) buffer SandBuffer { float cells[]; };
uniform int Width;
uniform int Height;
out float v_Val;

void main() {
    float val = cells[gl_VertexID];
    v_Val = val;

    int y = gl_VertexID / Width;
    int x = gl_VertexID % Width;

    // Map grid to -1.0 -> 1.0
    float x_pos = (float(x) / float(Width)) * 2.0 - 1.0;
    float y_pos = 1.0 - (float(y) / float(Height)) * 2.0;

    gl_Position = vec4(x_pos, y_pos, 0.0, 1.0);
    gl_PointSize = 1.0;
}
'''

render_fs = '''
#version 460 core
in float v_Val;
out vec4 f_color;
void main() {
    if (v_Val < 0.1) discard;
    // Simple sand color based on the value
    f_color = vec4(0.9, 0.7, 0.3, 1.0) * v_Val;
}
'''

# --- 7. PIPELINES ---
image = ctx.image((WIDTH, HEIGHT), format='rgba8unorm')
render_pipeline = ctx.pipeline(
    vertex_shader=render_vs,
    fragment_shader=render_fs,
    resources=[{'type': 'storage_buffer', 'binding': 0, 'buffer': sand_buffer}],
    framebuffer=[image],
    topology='points',
    vertex_count=CELL_COUNT,
    uniforms={'Width': WIDTH, 'Height': HEIGHT}
)

# --- 8. START SIMULATION ---
sim_thread = threading.Thread(target=worker_loop, args=(gpu_view, WIDTH, HEIGHT), daemon=True)
sim_thread.start()

# --- 9. MAIN LOOP ---
while not glfw.window_should_close(window):
    # Mouse Interaction: Draw a circular blob of sand
    if glfw.get_mouse_button(window, glfw.MOUSE_BUTTON_LEFT) == glfw.PRESS:
        x, y = glfw.get_cursor_pos(window)
        ix, iy = int(x), int(y)

        brush_radius = 10  # Increase this for "The Firehose" effect
        for dy in range(-brush_radius, brush_radius):
            for dx in range(-brush_radius, brush_radius):
                # Circle formula: x^2 + y^2 <= r^2
                if dx * dx + dy * dy <= brush_radius * brush_radius:
                    target_x, target_y = ix + dx, iy + dy
                    if 0 <= target_x < WIDTH and 0 <= target_y < HEIGHT:
                        # Randomize the value slightly (0.8 - 1.0) for visual texture
                        gpu_view[target_y * WIDTH + target_x] = 0.8 + (np.random.random() * 0.2)
    if glfw.get_mouse_button(window, glfw.MOUSE_BUTTON_RIGHT) == glfw.PRESS:
        x, y = glfw.get_cursor_pos(window)
        ix, iy = int(x), int(y)
        erase_radius = 20
        for dy in range(-erase_radius, erase_radius):
            for dx in range(-erase_radius, erase_radius):
                if dx * dx + dy * dy <= erase_radius * erase_radius:
                    tx, ty = ix + dx, iy + dy
                    if 0 <= tx < WIDTH and 0 <= ty < HEIGHT:
                        gpu_view[ty * WIDTH + tx] = 0.0

    image.clear()
    render_pipeline.render()
    image.blit()

    glfw.swap_buffers(window)
    glfw.poll_events()

glfw.terminate()