# HyperGL

**High-Performance, Object-Oriented Modern OpenGL for Python.**

[Built upon ZenGL](https://github.com/szabolcsdombi/zengl) | [License: MIT](LICENSE)

**HyperGL** is a stateless, object-oriented abstraction layer for OpenGL 3.3 up to 4.6, written almost entirely in C. It bridges the gap between Python's ease of use and the raw performance required for real-time rendering engine development.

Unlike traditional wrappers (like PyOpenGL) that expose raw C-style API calls, HyperGL encapsulates state into immutable **Pipelines** and resource objects. It mimics concepts found in Vulkan and ModernGL but runs on the OpenGL backend, abstracting away the global state machine hell that was ubiquitous to every language.

---

## v1.1: AZDO Update

HyperGL v1.1 introduces **AZDO (Approaching Zero Driver Overhead)** features, effectively removing the CPU bottleneck for Python rendering.

*   **Bindless Textures (`GL_ARB_bindless_texture`):** Pass 64-bit texture handles directly to shaders via SSBOs. Access thousands of unique textures in a single draw call.
*   **Indirect Drawing (`GL_ARB_multi_draw_indirect`):** Generate draw commands on the GPU or store them in buffers. Issue a single Python call to render millions of instances.
*   **Free-Threading Support (Python 3.14t):** Fully compatible with Python's experimental free-threaded (No-GIL) build, allowing render loops and game logic to run in true parallel.

**Performance Benchmark (RTX 2060 / Python 3.14t):**
*   **1,000,000 Sprites:** ~44 FPS (GPU Fill-rate bound)
*   **CPU Usage:** ~15% (Idle)

---

## Key Features

*   **C-Extension Performance:** Written in C using the CPython API for minimal overhead.
*   **Pipeline Architecture:** Rendering state (shaders, blending, depth, layout) is baked into immutable `Pipeline` objects. No more `glEnable`/`glDisable`, looks clean.
*   **Smart Caching:** Internally caches Framebuffers, VAOs, Programs, and Descriptor Sets to prevent redundant driver calls.
*   **Compute Support:** First-class support for Compute Shaders and SSBOs (Shader Storage Buffer Objects).
*   **Thread Safety:** Includes internal mutexes and a shared "trash bin" system to safely handle OpenGL resource destruction from Python's Garbage Collector across threads.
*   **Persistent Mapping:** Supports `GL_MAP_PERSISTENT_BIT` for zero-copy memory access between Python `memoryview`s and GPU buffers.

---

## Installation

No dependencies.
```bash
pip install git+https://github.com/Evilpasture/HyperGL.git
```

*Note: For AZDO features, an OpenGL 4.5+ capable GPU is recommended.*

---

## Core Concepts

1.  **Context:** The entry point. Manages the connection to the driver and resource creation.
2.  **Buffer:** Wraps VBOs, IBOs, UBOs, and SSBOs into a unified, mappable class.
3.  **Image:** Wraps Textures and Renderbuffers (2D, Array, Cubemap, MSAA).
4.  **Pipeline:** An immutable state object defining *how* to draw (Shaders + State + Layout + Resources).

---

## Usage Examples

### 1. Basic Initialization (GLFW)

HyperGL is window-provider agnostic.

```python
import hypergl
import glfw
import struct

# 1. Setup Window
if not glfw.init(): raise RuntimeError()
glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 3)
glfw.window_hint(glfw.CONTEXT_VERSION_MINOR, 3)
glfw.window_hint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE)
window = glfw.create_window(800, 600, "HyperGL", None, None)
glfw.make_context_current(window)

# 2. Initialize HyperGL
hypergl.init(loader=glfw.get_proc_address)
ctx = hypergl.context()
```

### 2. The Rendering Pipeline

HyperGL pipelines define everything required for a draw call upfront.

```python
# Geometry
vertices = struct.pack('6f', -0.5, -0.5, 0.5, -0.5, 0.0, 0.5)
vbo = ctx.buffer(data=vertices)

# Shaders
vertex_shader = """
    #version 330 core
    in vec2 in_pos;
    void main() { gl_Position = vec4(in_pos, 0.0, 1.0); }
"""
fragment_shader = """
    #version 330 core
    out vec4 out_color;
    uniform vec3 color;
    void main() { out_color = vec4(color, 1.0); }
"""

# Pipeline
pipeline = ctx.pipeline(
    vertex_shader=vertex_shader,
    fragment_shader=fragment_shader,
    layout=[], # Used for UBO bindings
    vertex_buffers=hypergl.bind(vbo, '2f', 0), # Bind '2f' format to location 0
    uniforms={'color': (1.0, 0.5, 0.2)},
    framebuffer=None, # Render to screen
    topology='triangles',
    vertex_count=3
)

# Render Loop
while not glfw.window_should_close(window):
    ctx.new_frame()
    pipeline.render()
    ctx.end_frame()
    glfw.swap_buffers(window)
    glfw.poll_events()
```

### 3. AZDO: Bindless Textures & Indirect Drawing

Render thousands of objects with unique textures in a single draw call.

```python
# 1. Create Bindless Texture
texture = ctx.image((512, 512), texture=True)
handle = texture.get_handle() # Get 64-bit GPU Handle
texture.make_resident(True)   # Make accessible to shaders

# 2. Upload Handle to SSBO
ctx.buffer(data=struct.pack('=Q', handle), storage=True)

# 3. Create Indirect Command Buffer
# Struct: { count, instanceCount, first, baseInstance }
# Draw 6 vertices, 100,000 instances
cmd_data = struct.pack('=IIII', 6, 100000, 0, 0)
indirect_buf = ctx.buffer(data=cmd_data)

# 4. Render
# The shader uses #extension GL_ARB_bindless_texture to read the handle from the SSBO
ctx.new_frame()
pipeline.render_indirect(buffer=indirect_buf, count=1)
ctx.end_frame()
```

### 4. Compute Shaders

```python
# Create storage buffer
ssbo = ctx.buffer(size=1024, storage=True)

# Create Compute Object
compute = ctx.compute(
    compute_shader=shader_source_bytes,
    resources=[
        {'type': 'storage_buffer', 'binding': 0, 'buffer': ssbo}
    ]
)

# Dispatch & Read
compute.run(x=16, y=1, z=1)
result = ssbo.read()
```

---

## Utilities (Optional helpers/debug tools)

*   **`hypergl.camera(...)`**: Fast C-based camera matrix generation (View/Projection).
*   **`hypergl.inspect(obj)`**: Debug tool to view internal GL handles and state.
*   **`buffer.map()`**: Get a `memoryview` of a buffer for zero-copy updates.

## Building from Source

```bash
# Requires a C compiler (preferably MSVC)
python setup.py build_ext --inplace
```

## License

MIT License