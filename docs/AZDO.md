# AZDO Guide: Approaching Zero Driver Overhead

Standard OpenGL is slow because the CPU must issue thousands of validation calls (`glUniform`, `glBindTexture`) per frame. 

**AZDO** is a set of modern OpenGL (4.5+) features designed to bypass this overhead. HyperGL exposes these features directly.

---

## 1. Bindless Textures (`GL_ARB_bindless_texture`)

Instead of binding textures to slots (0, 1, 2...), you pass a **64-bit integer handle** to the shader. This allows you to access thousands of textures in a single draw call.

### Python Setup
```python
# 1. Create Image
tex = ctx.image((512, 512), texture=True)

# 2. Get Handle & Make Resident
# "Resident" means the GPU guarantees the memory is available.
tex.get_handle()
tex.make_resident(True) 

# 3. Create a buffer to hold the handles
# Size = N * 8 bytes (uint64)
ssbo_textures = ctx.buffer(size=1024, storage=True)

# 4. Upload handle
ssbo_textures.write_texture_handle(offset=0, image=tex)
```

### GLSL Setup
You must enable the extension and use `sampler2D` inside a buffer or struct.

```glsl
#version 460
#extension GL_ARB_bindless_texture : require

// A buffer containing an array of texture handles
layout(std430, binding = 0) readonly buffer TextureHeap {
    sampler2D textures[];
};

in flat int v_tex_index; // Instance ID or Material ID
in vec2 v_uv;

void main() {
    // Direct indexing! No uniform binding required.
    vec4 color = texture(textures[v_tex_index], v_uv);
}
```

### ⚠️ Important: Safety
If the Python `tex` object is garbage collected while resident, the handle becomes invalid. If the GPU tries to access an invalid handle, **it will crash the driver.** Always keep a reference to your images.

---

## 2. Multi-Draw Indirect (MDI)

MDI allows the GPU (or CPU) to build a list of draw commands in a buffer, and then execute them all with a single Python call.

### The Command Structure
HyperGL expects the standard OpenGL indirect command format:

| Field | Type | Description |
| :--- | :--- | :--- |
| `count` | `uint` | Vertex count per instance |
| `instanceCount` | `uint` | Number of instances to draw |
| `first` | `uint` | Start vertex index |
| `baseInstance` | `uint` | Base instance ID (useful for indexing SSBOs) |

*(If using indexed drawing, there is an extra `baseVertex` field).*

### CPU-Driven MDI (Batching)
Used when you know what to draw on the Python side but want to avoid loop overhead.

```python
# Create 10,000 draw commands
# (count, instanceCount, first, baseInstance)
commands = [(6, 1, 0, i) for i in range(10000)]

# Pack into bytes
cmd_data = ctx.pack_indirect(commands)

# Upload
cmd_buffer = ctx.buffer(data=cmd_data)

# Draw
pipeline.render_indirect(buffer=cmd_buffer, count=10000)
```

### GPU-Driven MDI (Culling)
Used when the GPU decides what to draw (e.g., Frustum Culling).

1.  **Initialize Buffer:** Create a buffer with `count=0`.
2.  **Compute Shader:**
    ```glsl
    layout(std430, binding=0) buffer DrawCmd {
        uint count;
        uint instanceCount;
        uint first;
        uint baseInstance;
    };
    
    void main() {
        if (isVisible(obj_id)) {
            // Atomic increment is the key
            atomicAdd(instanceCount, 1);
        }
    }
    ```
3.  **Render:** Call `render_indirect` pointing to that buffer.

---

## 3. Persistent Mapping (`GL_MAP_PERSISTENT_BIT`)

Used for high-frequency CPU-to-GPU data transfer (e.g., streaming vertices or updating uniforms every frame).

### Setup
Only **Shader Storage Buffer Objects (SSBO)** support mapping in HyperGL.

```python
# Create buffer
# storage=True is required
# size must be 16-byte aligned
particle_buffer = ctx.buffer(size=1024*16, storage=True)

# Map it
# This returns a memoryview pointing to GPU-accessible memory
ptr = particle_buffer.map()

# Write data (NumPy)
import numpy as np
view = np.frombuffer(ptr, dtype='f4')
view[:] = my_new_positions[:]
```

### Synchronization
Because the CPU writes directly to GPU memory, you need to ensure the GPU isn't reading while you write (tearing).

1.  **Triple Buffering:** Use 3 separate buffers and cycle them.
2.  **Fences:** Use `ctx.fence()` to wait for the GPU to finish reading a region before writing to it again.

---

## 4. Sparse Binding (Bindless UBOs)

While HyperGL uses standard `glBindBufferRange` for UBOs, you can emulate "bindless" uniform data using SSBOs.

Instead of binding a UBO for every object (slow), put **all** object data into one giant SSBO.

**GLSL:**
```glsl
struct ObjectData {
    mat4 model;
    vec4 color;
};

layout(std430, binding=1) readonly buffer World {
    ObjectData objects[];
};

void main() {
    // gl_BaseInstance is passed via the Indirect Command
    ObjectData obj = objects[gl_BaseInstance]; 
    gl_Position = vp * obj.model * vec4(in_pos, 1.0);
}
```

This technique allows rendering millions of unique objects in a single draw call.

---

## 5. Summary Checklist

To achieve 10,000+ draws/frame:

1.  [ ] **Stop using `uniforms`:** Use SSBOs or UBOs for per-object data.
2.  [ ] **Stop binding textures:** Use Bindless Textures in an array.
3.  [ ] **Stop looping in Python:** Use `render_indirect`.
4.  [ ] **Stop creating objects per frame:** Pre-allocate buffers and reuse them.
5.  [ ] **Minimize Layout Changes:** Group objects by Shader/Blend Mode. Changing the `Pipeline` object is the most expensive operation.