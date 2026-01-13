# HyperGL

**Zero-Overhead, Object-Oriented OpenGL 4.6 Engine for Python.**

[Built upon ZenGL](https://github.com/szabolcsdombi/zengl) | [License: MIT](LICENSE)

> **"Trains Reinforcement Learning agents at 12,000+ FPS with zero-copy NumPy readback. No window manager required."**

HyperGL is a high-performance rendering engine written in C that bridges the gap between Python's ease of use and the raw power of modern GPU features. It is designed for **Machine Learning**, **Game Engine Development**, and **High-Performance Visualization**.

Unlike traditional wrappers (PyOpenGL) that incur heavy CPU overhead per-call and block the GIL, HyperGL uses an immutable **Pipeline** architecture and supports **AZDO (Approaching Zero Driver Overhead)** features like Multi-Draw Indirect and Bindless Textures.

---

## 🚀 Key Features

*   ✅ **Headless Rendering:** Runs on Linux servers, Docker containers, and CI/CD pipelines without X11 or a window manager.
*   ✅ **Zero-Copy Readback:** Read render targets directly into **NumPy** arrays or **PyTorch** tensors at 12k+ FPS.
*   ✅ **Bindless Textures (`GL_ARB_bindless_texture`):** Pass 64-bit texture handles to shaders via SSBOs. Access thousands of unique textures in a single draw call.
*   ✅ **Multi-Draw Indirect (MDI):** Generate draw commands on the GPU or CPU and render millions of instances with a single Python call.
*   ✅ **Compute Shaders:** First-class support for Compute and SSBOs for GPU-side pre-processing.
*   ✅ **Free-Threading Ready:** Fully compatible with Python 3.13+ free-threading (No-GIL), allowing physics and ML logic to run in parallel with the render loop.

---

## 📦 Installation

```bash
pip install git+https://github.com/Evilpasture/HyperGL.git
```

*Requirements: OpenGL 3.3+ (4.6 recommended for AZDO features).*

---

## ⚡ Examples

### 1. Headless RL / ML Pipeline (12,000 FPS)

Render a scene and get the pixel data into NumPy instantly. Perfect for OpenAI Gym environments.

```python
import hypergl
import numpy as np
import struct

# 1. Initialize Headless (No Window required)
hypergl.init(headless=True)
ctx = hypergl.context()

# 2. Setup Resources
# 84x84 is standard for Atari/RL
target = ctx.image((84, 84), format='rgba8unorm', texture=False) 

pipeline = ctx.pipeline(
    vertex_shader="...",   # Standard GLSL
    fragment_shader="...",
    framebuffer=[target],
    vertex_count=3
)

# 3. Zero-Copy Memory
# Allocate ONCE. We write directly into this buffer.
observation = np.zeros((84, 84, 4), dtype=np.uint8)

# 4. Training Loop
while training:
    # Update State
    pipeline.uniforms['u_time'][:] = struct.pack('f', time)

    # Render (Releases GIL)
    ctx.new_frame(clear=True)
    pipeline.render()
    ctx.end_frame()

    # Readback (Zero Allocation)
    target.read(into=observation)
    
    # 'observation' now contains the frame. No return value, no garbage.
```

### 2. AZDO: 100,000 Sprites (Bindless + Indirect)

Render massive scenes with almost zero CPU overhead using modern OpenGL 4.6 features.

```python
# 1. Create Bindless Texture
texture = ctx.image((512, 512), texture=True)
texture.make_resident(True) # Make accessible to shaders

# 2. Upload 64-bit Handle to GPU Buffer (SSBO)
# The helper method automatically fetches the handle and writes it.
ssbo = ctx.buffer(size=1024, storage=True)
ssbo.write_texture_handle(offset=0, image=texture)

# 3. Pack Indirect Commands
# Draw 6 vertices, 100,000 instances
cmd_data = ctx.pack_indirect([(6, 100000, 0, 0)])
indirect_buf = ctx.buffer(data=cmd_data)

# 4. Render
ctx.new_frame()
# Single C-call triggers the GPU to execute the command buffer
pipeline.render_indirect(buffer=indirect_buf, count=1)
ctx.end_frame()
```

---

## 🛠️ Architecture

HyperGL is built on three core pillars:

1.  **Context**: The entry point. Manages the connection to the driver (Headless or Windowed).
2.  **Pipeline**: An immutable state object. You define shaders, blending, depth state, and layouts **once**. At render time, it's just a pointer swap.
3.  **Resources**: `Buffer` (VBO/SSBO/UBO) and `Image` (Texture/FBO) wrap the raw OpenGL handles and manage lifetime safely.

### Why is it so fast?
*   **C-Extension**: The render loop happens in C.
*   **State Caching**: HyperGL caches internal GL state to avoid redundant driver calls.
*   **GIL Release**: Long-running GL operations release the Global Interpreter Lock.
*   **Direct Memory Access**: `memoryview` support allows Python to write directly to mapped GPU memory.

---

## ⚠️ Advanced Usage Notes

*   **Bindless Safety**: When using `make_resident(True)`, you **must** keep the Python `Image` object alive. If it is garbage collected while resident, the GPU handle becomes invalid, potentially crashing the driver.
*   **Context Loss**: If running in a windowed environment (GLFW/SDL), handle context loss by checking `ctx.lost`.

---

## Building from Source

HyperGL has **no Python dependencies**. You only need a C compiler.

```bash
# Windows (MSVC) / Linux (GCC/Clang)
python setup.py build_ext --inplace
```

---

[License: MIT](LICENSE)