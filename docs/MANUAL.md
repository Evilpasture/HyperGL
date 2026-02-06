# HyperGL User Manual

HyperGL is not a standard OpenGL wrapper. It enforces a **Pipeline-based** architecture similar to Vulkan or Metal, but runs on OpenGL 4.5+. 

This approach minimizes driver overhead (AZDO) and allows Python to feed the GPU at 12,000+ FPS.

## 1. The Context
Everything starts with the Context. Unlike PyOpenGL, HyperGL manages the window and context lifecycle internally (or via a loader).

```python
import hypergl

# Headless (Server/Docker)
hypergl.init(headless=True)

# Windowed (Requires a loader like glut/glfw/sdl, or HyperGL's internal windows loader)
hypergl.init() 

ctx = hypergl.context()
```

### The Frame Loop
HyperGL requires explicit frame delineation.
1.  `ctx.new_frame()`: Clears the screen and resets internal state cache.
2.  `ctx.end_frame()`: Flushes commands to the GPU.

## 2. Resources (Buffers & Images)

### Buffers (VBO, UBO, SSBO)
Buffers are raw GPU memory.

```python
# Upload data immediately
vbo = ctx.buffer(data=b'...')

# Allocate empty memory (dynamic)
ubo = ctx.buffer(size=1024, access='dynamic_draw')

# Shader Storage Buffer (SSBO) for Compute/Bindless
ssbo = ctx.buffer(size=1024, storage=True)
```

**Zero-Copy Write:**
You can write to a buffer without creating a new bytes object using `offset`.
```python
# Write to the second half of the buffer
vbo.write(data=my_numpy_array, offset=512)
```

### Images (Textures)
Images represent Textures or Renderbuffers.

```python
# Standard 2D Texture
img = ctx.image((512, 512), format='rgba8unorm')

# Renderbuffer (Optimized for FBO attachment, cannot be sampled)
rbo = ctx.image((512, 512), texture=False)
```

**Reading Pixels (The Fast Way):**
Do not use `img.read()` directly if performance matters. Use `into`.
```python
import numpy as np
raw_buffer = np.empty((512, 512, 4), dtype='u1')

# This performs a C-level memcpy directly into the NumPy array
img.read(into=raw_buffer)
```

## 3. The Pipeline (Render State)

In HyperGL, you do not call `glUseProgram`, `glEnable`, or `glBindBuffer`. You create a **Pipeline** object that encapsulates all state.

```python
pipeline = ctx.pipeline(
    vertex_shader='...',
    fragment_shader='...',
    # 1. Layout: Maps GLSL names to Binding Points
    layout=[
        {'name': 'u_texture', 'binding': 0},
        {'name': 'CameraBlock', 'binding': 0}
    ],
    # 2. Resources: Maps Binding Points to Python Objects
    resources=[
        {'type': 'sampler', 'binding': 0, 'image': my_texture},
        {'type': 'uniform_buffer', 'binding': 0, 'buffer': my_ubo}
    ],
    # 3. Vertex Attributes
    vertex_buffers=hypergl.bind(vbo, '3f 2f', 0, 1),
    framebuffer=[screen_image],
    topology='triangles',
    vertex_count=3
)
```

### Rendering
Rendering is just a method call on the pipeline object.
```python
ctx.new_frame()
pipeline.render() # Applies ALL state defined above
ctx.end_frame()
```

### Updating Uniforms
Uniforms are exposed as a dictionary of `memoryview` objects. Writing to them updates the GPU memory.

```python
# Update float uniform
pipeline.uniforms['u_time'][:] = struct.pack('f', time.time())

# Update matrix (bytes)
pipeline.uniforms['u_mvp'][:] = my_matrix_bytes
```

## 4. Helper: Vertex Layouts
HyperGL uses a shorthand syntax for vertex attributes in `hypergl.bind`.

| Code | Format | Size (Bytes) | GLSL Type |
| :--- | :--- | :--- | :--- |
| `1f` | float | 4 | `float` |
| `2f` | float | 8 | `vec2` |
| `3f` | float | 12 | `vec3` |
| `4f` | float | 16 | `vec4` |
| `1i` | int | 4 | `int` |
| `1u` | uint | 4 | `uint` |
| `4u1` | uint8 (norm) | 4 | `vec4` (from bytes) |

**Example:**
Position (`vec3`) and UV (`vec2`) packed in one buffer.
```python
# Layout string: '3f 2f'
# Attribute Locations: 0 (Position), 1 (UV)
ctx.bind(vbo, '3f 2f', 0, 1)
```