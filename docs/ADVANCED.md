# Advanced Features: AZDO & Threading

## 1. Bindless Textures (`GL_ARB_bindless_texture`)

Bindless textures allow you to access practically unlimited textures in a shader without binding them to slots. You pass a 64-bit handle (uint64) to the GPU via a Uniform Buffer or SSBO.

### Usage
```python
# 1. Get Handle
# Requires the image to be created.
tex_handle = my_image.get_handle() 

# 2. Make Resident
# The GPU cannot access the texture until it is resident.
my_image.make_resident(True)

# 3. Pass to Shader
# Write the 64-bit int to a buffer mapped to an SSBO
ssbo.write_texture_handle(offset=0, image=my_image)
```

### ⚠️ Critical Safety Warning
**Garbage Collection:** If the Python `my_image` object is garbage collected while it is Resident, the GPU handle becomes invalid. If the GPU tries to access that handle, **the driver will crash (Segfault/TDR).**

**Rule:** You must keep a reference to `my_image` for as long as the GPU might use its handle.

## 2. Multi-Draw Indirect (MDI)

MDI allows you to issue thousands of draw calls from a single Python call. The draw commands are stored in a `Buffer`.

### Structure
The command buffer must contain structs of:
```c
struct DrawArraysIndirectCommand {
    uint count;
    uint instanceCount;
    uint first;
    uint baseInstance;
}; // 16 bytes
```

### Usage
```python
# 1. Pack Commands
# List of (count, instance_count, first, base_instance)
commands = [
    (6, 1, 0, 0),
    (6, 1, 6, 1),
    (36, 5, 12, 2)
]
cmd_bytes = ctx.pack_indirect(commands)

# 2. Upload to Buffer
cmd_buffer = ctx.buffer(data=cmd_bytes)

# 3. Render
# This issues 3 draw calls on the GPU side
pipeline.render_indirect(buffer=cmd_buffer, count=3)
```

## 3. Compute Shaders

HyperGL supports Compute Shaders for physics, culling, or image processing.

```python
compute = ctx.compute(
    compute_shader="""
    #version 450
    layout(local_size_x=16) in;
    layout(binding=0, rgba8) writeonly uniform image2D dest;
    void main() { ... }
    """,
    resources=[
        {'type': 'sampler', 'binding': 0, 'image': my_image}
    ]
)

# Dispatch (x, y, z groups)
compute.run(10, 10, 1)
```

## 4. Free-Threading (Python 3.13t)

HyperGL is built for the `cp313t` (Free-Threaded) build.

*   **GIL Release:** `ctx.new_frame()`, `ctx.end_frame()`, `pipeline.render()`, and `buffer.read()` release the GIL.
*   **Physics/Logic Thread:** You can run heavy Python logic (AI, Physics) on a secondary thread while HyperGL renders on the main thread.

### Context Migration
OpenGL Contexts are thread-local. To move a context to another thread:

```python
# Thread A (Current Owner)
ctx.release_thread() 

# ... Signal Thread B ...

# Thread B (New Owner)
ctx.migrate() 
# Now Thread B can call ctx.new_frame()
```

## 5. Dual-Path Execution: Immediate vs. Baked

HyperGL provides two ways to interact with the GPU. Choosing the right one is key to performance.

| Feature | Immediate Mode (`pipe.render()`) | Command Buffer (`cmd.submit()`) |
| :--- | :--- | :--- |
| **Complexity** | Simple, Pythonic | Advanced, Bytecode-based |
| **Overhead** | ~10-50μs per call | **~1-2μs per buffer** |
| **GIL** | GIL-bound (Mostly) | **GIL-Free** |
| **Logic** | Python `if/for` loops | **C-side `skip/goto` loops** |
| **Best For** | UI, Prototyping, One-offs | **World Rendering, Particles, RL** |

### The "Single-Instruction" Truth
Technically, calling `pipeline.render()` is exactly the same as creating a Command Buffer with a single `DRAW` instruction and submitting it. By keeping immediate methods, you are just providing a "Short-Circuit" for that common use case.

---

### Pro-Tip: "The UI Bridge"
One very cool trick you can do is **Subroutine Injection**:

```python
# Record the heavy world once
world_cmd = ctx.command_buffer()
world_cmd.begin()
world_cmd.draw_world()
world_cmd.end()

# In your render loop:
while True:
    ctx.new_frame()
    
    # 1. Path A: Baked (Ultra Fast)
    world_cmd.submit() 
    
    # 2. Path B: Immediate (Flexible)
    # Draw dynamic UI that changes every frame in Python
    for button in ui.active_buttons:
        ui_pipe.render() 
        
    ctx.end_frame()
```