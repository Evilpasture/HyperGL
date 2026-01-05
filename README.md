# HyperGL

[Original Author and Repository](https://github.com/szabolcsdombi/zengl)

HyperGL is a high-performance, thread-safe fork of ZenGL. It bridges the gap between the strict, Pythonic API of ZenGL and the raw power of modern OpenGL 4.5 features like Compute Shaders and Persistent Memory Mapping.

And no dependencies. Just one pip install away!

```bash
pip install git+https://github.com/Evilpasture/HyperGL.git
```

## 1. Key Differences from ZenGL

| Feature | ZenGL | HyperGL |
| :--- | :--- | :--- |
| **OpenGL Version** | 3.3 / ES 3.0 | **4.5 / 4.6 (Core Profile)** |
| **Threading** | Single-threaded | **Thread-Safe** (Internal Mutexes + Atomic Ref-counting) |
| **Compute Shaders** | No | **Yes** (`ctx.compute`) |
| **Storage Buffers (SSBO)** | No | **Yes** (Huge data arrays) |
| **Buffer Mapping** | No | **Yes** (Zero-copy `memoryview` via Persistent Mapping) |
| **Resource Safety** | Immediate Deletion | **Deferred Trash System** (Prevents segfaults in GC) |

---

## 2. Installation & Context Creation

Unlike ZenGL, HyperGL does not aggressively try to create a window for you. It expects a loader or an active context, though helper utilities are provided.

### Basic Setup (Windowed)

HyperGL works best with `glfw`, `moderngl-window`, or `pygame`.

```python
import glfw
import hypergl
import struct

# 1. Initialize Window
glfw.init()
glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 4)
glfw.window_hint(glfw.CONTEXT_VERSION_MINOR, 5) # Required for SSBO/Compute
glfw.window_hint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE)

window = glfw.create_window(800, 600, "HyperGL", None, None)
glfw.make_context_current(window)

# 2. Initialize HyperGL
# It automatically detects the active GLFW context
ctx = hypergl.context()

print(ctx.info['version'])
```

### Headless Setup

HyperGL provides a helper to spawn invisible windows/contexts for headless compute tasks.

```python
import hypergl

# loader(headless=True) handles OS-specific hidden window creation
ctx = hypergl.init(loader=hypergl.loader(headless=True))
```

---

## 3. The New Features

### A. Compute Shaders
HyperGL treats Compute Shaders as first-class citizens.

```python
# 1. Create a buffer to hold data (SSBO)
# 'storage=True' is crucial. It allocates the buffer for GL_SHADER_STORAGE_BUFFER
data = bytearray(1024)
buf = ctx.buffer(data, storage=True)

# 2. Define the Shader
compute_shader = """
#version 450
layout(local_size_x = 1, local_size_y = 1) in;

// Bind to binding point 0
layout(std430, binding = 0) buffer Output {
    float values[];
};

void main() {
    uint idx = gl_GlobalInvocationID.x;
    values[idx] = float(idx) * 2.0;
}
"""

# 3. Create the Compute Pipeline
# We bind the buffer to binding point 0
compute = ctx.compute(
    compute_shader,
    resources=[
        {
            'type': 'storage_buffer',
            'binding': 0,
            'buffer': buf,
        }
    ]
)

# 4. Run it
# Dispatch 256 workgroups
compute.run(x=256)
```

### B. Persistent Buffer Mapping
In ZenGL, you had to use `read()` and `write()`, which incur driver overhead. HyperGL allows you to map GPU memory directly into Python's address space.

```python
import struct

# Create a buffer (must be storage=True to allow persistent mapping in HyperGL)
buf = ctx.buffer(size=1024, storage=True)

# Map it. This returns a standard Python memoryview.
# The pointer is cached, so calling map() again is free.
view = buf.map()

# Write directly to GPU memory (Zero-copy)
# Because HyperGL uses GL_MAP_COHERENT_BIT, no glMemoryBarrier is needed for simple updates.
view[0:4] = struct.pack('f', 123.456)

# Read directly
print(struct.unpack('f', view[0:4])[0])

# Unmap is optional if you want to keep the pointer valid, 
# but good practice when destroying the object.
buf.unmap()
```

### C. Manual Binding
You can manually bind buffers to binding points without creating a pipeline. This is useful for sharing data between multiple compute passes globally.

```python
buf = ctx.buffer(size=1024, storage=True)

# Binds to layout(binding = 3) buffer ...
buf.bind(3) 
```

---

## 4. API Reference (New & Changed)

### `Context`

*   **`ctx.compute(compute_shader, resources, uniforms, ...)`**
    *   Creates a `Compute` object.
    *   `resources`: Accepts `'storage_buffer'`, `'uniform_buffer'`, and `'sampler'`.
*   **`ctx.buffer(..., storage=False)`**
    *   `storage=True`: Allocates the buffer using `glBufferStorage` with `GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT`. Required for mapping and SSBO usage.
*   **`ctx.gc()`**
    *   Returns a list of currently tracked OpenGL objects. Useful for debugging leaks.

### `Buffer`

*   **`buf.map() -> memoryview`**
    *   Maps the buffer into client memory. Requires the buffer to be created with `storage=True`.
*   **`buf.unmap()`**
    *   Unmaps the buffer. Invalidates the `memoryview`.
*   **`buf.bind(unit)`**
    *   Binds the buffer to `GL_SHADER_STORAGE_BUFFER` at the specified binding index.

### `Pipeline`

*   **`ctx.pipeline(..., template=other_pipeline)`**
    *   **New Optimization:** Creates a new pipeline sharing the **same** OpenGL Program object and Shader objects as the `template`.
    *   Allows you to create variants of a pipeline (different blending, topology, or framebuffer targets) without recompiling shaders.

---
