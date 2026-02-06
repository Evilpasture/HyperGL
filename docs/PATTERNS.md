# HyperGL Design Patterns

HyperGL encourages specific architectural patterns to maximize performance.

## 1. The Pipeline Pattern
Unlike standard OpenGL (which is a state machine), HyperGL treats render state as immutable objects.

**Bad (PyOpenGL style):**
```python
glUseProgram(prog)
glUniform1f(...)
glBindBuffer(...)
glEnable(GL_DEPTH_TEST)
glDrawArrays(...)
```

**Good (HyperGL style):**
```python
# Define state ONCE
pipeline = ctx.pipeline(
    program=prog,
    depth={'write': True, 'func': 'less'},
    resources=[...]
)

# Apply state in one call
pipeline.render() 
```
*Why?* This allows the C-extension to filter redundant state changes and validate resources at creation time, not render time.

## 2. The "Struct-Pack" Uniform Pattern
Updating uniforms individually is slow in Python due to function call overhead. HyperGL encourages bulk updates using `struct.pack`.

**Pattern:**
```python
# GLSL
# layout(std140, binding=0) uniform Globals { float time; vec2 res; float pad; };

# Python
# Update 16 bytes at once
pipeline.uniforms['Globals'][:] = struct.pack('=fff f', time, width, height, 0.0)
```
*Tip:* Always align structs to 16 bytes (vec4 size) for compatibility with `std140` layouts.

## 3. The Bindless Texture Pattern
Binding textures to slots (`GL_TEXTURE0`, `GL_TEXTURE1`...) is a bottleneck.

**Pattern:**
1.  Get the 64-bit handle: `h = img.get_handle()`.
2.  Make resident: `img.make_resident(True)`.
3.  Upload `h` to a buffer (SSBO or UBO).
4.  Access in shader: `texture(sampler2D(handle), uv)`.

*Requirement:* Your hardware must support `GL_ARB_bindless_texture` (NVIDIA 600+, AMD GCN+).

## 4. The "Indirect Culling" Pattern
To render millions of objects, the CPU cannot loop through them.

**Steps:**
1.  **SSBO A (World Data):** Contains all object data (pos, scale).
2.  **SSBO B (Visible Data):** Empty at start of frame.
3.  **Command Buffer:** Contains `DrawArraysIndirectCommand`. Reset `instanceCount` to 0.
4.  **Compute Shader:**
    *   Reads Object form A.
    *   Checks View Frustum.
    *   If visible: `idx = atomicAdd(cmd.instanceCount, 1)`.
    *   Copies Object to SSBO B at `idx`.
5.  **Render Pass:** `pipeline.render_indirect(buffer=cmd_buffer)`.

## 5. The Mapped Memory Pattern (No-GIL)
For Python 3.13t (Free-Threaded), we want to utilize all CPU cores while the GPU renders.

**Pattern:**
1.  **Main Thread:** Handles Window, Input, and `ctx.end_frame()`.
2.  **Worker Thread:** Handles Physics/Logic.
3.  **Synchronization:**
    *   Create a `buffer(storage=True)`.
    *   Call `view = buffer.map()`.
    *   Pass `view` to the Worker Thread.
    *   Worker writes to `view` (using NumPy or Numba).
    *   Main Thread renders.

*Note:* `glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT)` may be required depending on how aggressive your driver buffers PCIe transfers, though `Coherent` mapping usually handles this.

## 6. Zero-Allocation Readback
Reading textures back to the CPU is the slowest operation in graphics.

**Pattern:**
1.  Pre-allocate the destination buffer (NumPy array) *once*.
2.  Use `image.read(into=numpy_array)`.

This bypasses Python object creation entirely, performing a raw `memcpy` from the driver's pixel buffer to the NumPy array's data pointer.