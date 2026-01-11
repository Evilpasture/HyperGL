# HyperGL

[Built upon ZenGL](https://github.com/szabolcsdombi/zengl)

**HyperGL** is a high-performance, Modern OpenGL wrapper for Python, written in C.

Unlike traditional wrappers (like PyOpenGL) that expose raw C-style OpenGL calls, HyperGL provides a **stateless, object-oriented abstraction layer**. It is designed to bridge the gap between Python's ease of use and the performance required for real-time rendering, mimicking "ModernGL" and Vulkan-style concepts (Pipelines, Descriptor Sets) while running on an OpenGL backend.

HyperGL assumes some existing knowledge of OpenGL. It is not for beginners, but it is definitely better than Vulkan's endless boilerplate.

## Key Features

*   **C-Extension Performance:** Written entirely in C using the CPython API for minimal overhead.
*   **Object-Oriented Abstraction:** No more global state machine. HyperGL encapsulates state into `Context`, `Pipeline`, `Buffer`, and `Image` objects.
*   **Pipeline Architecture:** Rendering state (shaders, blending, depth, stencil, culling) is baked into immutable `Pipeline` objects, reducing driver overhead during draw calls. If you try to do the following: 

*   `pipeline.__dict__` -> `AttributeError: 'hypergl.Pipeline' object has no attribute '__dict__'.`
*   **Smart Caching:** Internally caches Framebuffers, VAOs, Shader Programs, and Descriptor Sets to prevent redundant object creation and state changes.
*   **Compute Shader Support:** First-class support for Compute Shaders and SSBOs (Shader Storage Buffer Objects).
*   **Thread Safety:** Includes internal mutexes and a shared "trash bin" system to safely handle OpenGL resource destruction from Python's Garbage Collector, even across threads.
*   **Persistent Buffer Mapping:** Supports `GL_MAP_PERSISTENT_BIT` for zero-copy memory access between Python `memoryview`s and GPU buffers.
*   **Uniform Reflection:** Automatically parses shader code to determine uniform layouts, allowing direct data updates via Python buffer protocols.

---

## Core Concepts

### 1. Context
The entry point of the library. It manages the connection to the OpenGL driver, tracks global limits, and acts as a factory for creating resources. It requires a `loader` function (like `glfw.get_proc_address`) to initialize.

### 2. Pipeline
HyperGL moves away from `glUseProgram`, `glEnable`, and `glBlendFunc`. Instead, you define a **Pipeline**. A Pipeline contains:
*   Vertex and Fragment shaders.
*   Buffer layouts (Vertex attributes).
*   Resource bindings (Uniform buffers, Textures).
*   Fixed-function state (Blending, Depth/Stencil, Culling).
*   The target Framebuffer format.

### 3. Buffers
Wraps VBOs, IBOs, UBOs, and SSBOs.
*   **Unified:** One class (`Buffer`) handles vertex data, indices, uniforms, and storage.
*   **Mapped:** Can be mapped directly to a Python `memoryview` for high-performance data updates without creating temporary bytes objects.

### 4. Images
Wraps Textures and Renderbuffers.
*   Supports 2D, 3D (Array), and Cubemaps.
*   Handles Multisample Anti-Aliasing (MSAA) automatically.
*   Supports binding as Texture or Image (for Compute Read/Write).

---

## Usage Examples

### 1. Initialization
HyperGL is window-provider agnostic. You can use it with `glfw`, `pygame`, or `sdl2`.

```python
import hypergl
import glfw

# 1. Setup Window (Standard GLFW code)
glfw.init()
window = glfw.create_window(800, 600, "HyperGL", None, None)
glfw.make_context_current(window)

# 2. Initialize HyperGL
# Pass the raw get_proc_address function to the loader
hypergl.init(loader=glfw.get_proc_address)

# 3. Create Context
ctx = hypergl.context()
```

### 2. Creating Buffers and Data
```python
import struct

# Create a Vertex Buffer (x, y, u, v)
vertices = struct.pack('4f', -0.5, -0.5, 0.0, 0.0) + \
           struct.pack('4f',  0.5, -0.5, 1.0, 0.0) + \
           struct.pack('4f',  0.0,  0.5, 0.5, 1.0)

vbo = ctx.buffer(data=vertices, access='static_draw')

# Create a Uniform Buffer
ubo_data = ctx.buffer(size=64, uniform=True, access='dynamic_draw')
```

### 3. The Rendering Pipeline
```python
# Define Shader Source
vertex_code = """
#version 330 core
in vec2 in_pos;
in vec2 in_uv;
out vec2 v_uv;
void main() {
    gl_Position = vec4(in_pos, 0.0, 1.0);
    v_uv = in_uv;
}
"""

fragment_code = """
#version 330 core
in vec2 v_uv;
out vec4 out_color;
uniform vec3 color;
void main() {
    out_color = vec4(color, 1.0);
}
"""

# Create Pipeline
pipeline = ctx.pipeline(
    vertex_shader=vertex_code,
    fragment_shader=fragment_code,
    # Define Vertex Layout
    vertex_buffers=hypergl.bind(vbo, '2f 2f', 0, 1), # Location 0: Pos, Location 1: UV
    # Define Uniforms
    uniforms={'color': (1.0, 0.5, 0.2)},
    # Define Render State
    topology='triangles',
    cull_face='back',
    # Target Screen (default framebuffer)
    framebuffer=None, 
    vertex_count=3
)
```

### 4. Render Loop
```python
while not glfw.window_should_close(window):
    # Clear screen and reset state
    ctx.new_frame()
    
    # Update Uniforms (if needed) via dictionary access
    # pipeline.uniforms['color'][:] = struct.pack('3f', 0.0, 1.0, 0.0)

    # Draw
    pipeline.render()

    # Flush commands and unbind state
    ctx.end_frame()
    
    glfw.swap_buffers(window)
    glfw.poll_events()
```

### 5. Compute Shaders (SSBOs)
```python
# Create storage buffer
ssbo = ctx.buffer(size=1024, storage=True)

# Create Compute Object
compute = ctx.compute(
    compute_shader=shader_source_bytes,
    resources=[
        {'type': 'storage_buffer', 'binding': 0, 'buffer': ssbo, 'offset': 0, 'size': 1024}
    ]
)

# Dispatch
compute.run(x=16, y=1, z=1)

# Read results
result = ssbo.read()
```

---

## Advanced API Reference

### Math Helper: `hypergl.camera`
A highly optimized C-function to generate camera matrices (View/Projection) as bytes, ready to be written into a Uniform Buffer.

```python
# Returns 64 bytes (Mat4)
camera_matrix = hypergl.camera(
    eye=(0, 0, 5),
    target=(0, 0, 0),
    up=(0, 1, 0),
    fov=60.0,
    aspect=1.77,
    near=0.1,
    far=100.0
)
ubo.write(camera_matrix)
```

### Memory Mapping
For maximum performance with dynamic data (like particle systems), use `map()` to get a `memoryview` of the GPU buffer.

```python
# 1. Map buffer (Persistent mapping if supported)
ptr = my_buffer.map()

# 2. Write directly to memory (Zero copy)
ptr[0:12] = b'\x00\x00\x00...' 

# 3. Unmap (optional, usually done at cleanup)
# my_buffer.unmap() 
```

### Resource Inspection
You can inspect the internal GL state of objects for debugging:
```python
info = hypergl.inspect(pipeline)
print(info) # Output: {'type': 'pipeline', 'program': 12, 'vao': 5, ...}
```

---

## Building from Source

HyperGL is a C-extension. To build it, you need a C compiler installed.

```bash
# Standard Python build
python setup.py build_ext --inplace
```

*Note: On Windows, it links against `opengl32.lib`. On Linux, standard GL linking applies.*

## License

MIT License.