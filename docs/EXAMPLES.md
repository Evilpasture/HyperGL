# HyperGL Example Breakdown

This guide breaks down the included examples into practical techniques. Whether you are building a game engine, a physics simulation, or an RL environment, start here.

## Machine Learning & RL

### Headless Rendering (The "Gym" Pattern)
**Source:** `examples/headless_example.py`

Designed for OpenAI Gym/PettingZoo environments where no window is required.

**Key Techniques:**
1.  **Headless Init:** `hypergl.init(headless=True)` avoids creating an X11/Win32 window.
2.  **Renderbuffers:** Use `ctx.image(..., texture=False)` for the framebuffer. It is faster than a Texture if you only need to read from it, not sample it later.
3.  **Zero-Allocation Readback:**
    ```python
    # Allocate once
    obs = np.zeros((H, W, 4), dtype='u1')
    # Read loop
    while training:
        ...
        target.read(into=obs) # C-level memcpy
    ```

---

## GPU Simulation (Compute Shaders)

### Particles & Gravity
**Source:** `examples/particle_demo.py`

Simulates 5,000,000 particles entirely on the GPU.

**Key Techniques:**
*   **SSBOs (Shader Storage Buffer Objects):** Store position/velocity in a `ctx.buffer(storage=True)`.
*   **Compute Dispatch:** Use `ctx.compute(...)` to run physics math.
*   **Vertex ID Rendering:** The Vertex Shader reads directly from the SSBO using `gl_VertexID`. No VBO binding required.

### Slime Mold (Physarum Polycephalum)
**Source:** `examples/mold.py`

A 1-million agent simulation mimicking biological transport networks.

**Key Techniques:**
*   **Ping-Pong Buffers:** Use two images (`trail_map_a`, `trail_map_b`). Read from A, write to B, then swap indices.
*   **Image Atomic Operations:** (Optional, though this example uses blending) Writing to images from Compute.
*   **Indirect Point Rendering:** Rendering agents as `GL_POINTS` using coordinates stored in the SSBO.

### Cloth Physics
**Source:** `examples/cloth.py`

A mass-spring system using Verlet integration.

**Key Techniques:**
*   **Fixed-Step Physics:** Hardcoded `dt` in the shader to prevent explosion.
*   **Workgroup Synchronization:** Using `barrier()` (implicit in separate dispatch calls) to ensure physics steps happen in order: `Integration -> Constraints -> Normals`.

---

## High-Performance Rendering (AZDO)

### Infinite City (Instancing & Culling)
**Source:** `examples/AZDO_cubes.py`

Renders 250,000 objects with occlusion culling and texture variation.

**Key Techniques:**
1.  **Bindless Textures:** Pass 64-bit handles to the shader via an SSBO. Allows accessing 16+ different textures in a single draw call without binding slots.
2.  **GPU Culling:** A Compute Shader checks if an object is visible. If yes, it uses `atomicAdd` to increment a counter in the **Indirect Command Buffer**.
3.  **Indirect Draw:** `pipeline.render_indirect(buffer=cmd_buffer)`. The CPU doesn't know how many objects are drawn; the GPU decides.

### Procedural Grass
**Source:** `examples/grass.py`

Renders 4,000,000 blades of grass.

**Key Techniques:**
*   **Procedural Generation:** A "Generator" compute shader runs *once* at startup to populate the field SSBO.
*   **Frustum Culling:** A "Culler" compute shader runs *every frame* to populate the visible list.
*   **Geometry Generation:** The Vertex Shader generates the blade shape on-the-fly from a single point (Triangle Strip expansion), saving VRAM.

---

## Hybrid CPU/GPU (Python 3.13t)

### Mega Particles (Numba + Mapped Memory)
**Source:** `examples/mega_particle_demo.py`

The "Holy Grail" of Python graphics: Running CPU physics on a separate thread without the GIL blocking the render loop.

**Key Techniques:**
1.  **Persistent Mapping:** `ptr = buffer.map()`. The GPU memory is mapped to a CPU pointer.
2.  **Numba `nogil`:** A CPU kernel compiled with `@njit(nogil=True, parallel=True)`.
3.  **Zero-Copy Update:** The Numba kernel writes directly to the mapped pointer. The GPU sees the changes instantly (over PCIe). No `glBufferSubData` calls required.

### Sand Simulation (Cellular Automata)
**Source:** `examples/sand_sim.py`

Similar to the particle demo, but for a dense grid. Demonstrates how to synchronize a CPU simulation thread with a GPU visualization thread using raw memory views.