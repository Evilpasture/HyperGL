# Command Buffers

**HyperGL Command Buffers** allow you to record a sequence of rendering commands once and replay them infinitely at C-speed.

This feature moves the "Render Loop" overhead from Python to C, allowing for **Zero-GIL rendering** and massive CPU savings.

## 1. The Workflow

Instead of calling `pipeline.render()` every frame, you record it.

```python
# 1. Create Buffer
cmd = ctx.command_buffer()

# 2. Record Commands
# (The context is NOT modified during recording)
cmd.clear()
cmd.bind_pipeline(my_pipeline)
cmd.bind_descriptor_set(material_a)
cmd.draw(vertex_count=3)

# 3. Submit (Inside the loop)
# Executes the C-bytecode. Extremely fast.
while running:
    ctx.new_frame(clear=False) # Buffer handles clear
    cmd.submit()
    ctx.end_frame()
```

## 2. Dynamic State (Uniforms)

Since the Command Buffer is immutable, how do you animate things?
**Mapped Memory.**

The `Pipeline` object exposes its uniforms as `memoryview`s. You can write to them *even while the command buffer is recorded*.

```python
# Record once
cmd.bind_pipeline(pipe)
cmd.draw()

# Loop
t = 0.0
while True:
    # Update the memory the pipeline looks at
    pipe.uniforms['u_time'][:] = struct.pack('f', t) 
    
    # Submit sees the new data instantly
    cmd.submit() 
    t += 0.01
```

## 3. Supported Commands

| Method | Description |
| :--- | :--- |
| `clear(mask)` | Clears the framebuffer/screen. |
| `bind_pipeline(pipe)` | Binds a Graphics Pipeline (Shader, VAO, Blend state). |
| `bind_compute(comp)` | Binds a Compute Pipeline. |
| `bind_descriptor_set(set)` | Fast-swaps resources (Textures/UBOs) without changing Shaders. |
| `draw(vertex_count, ...)` | Issues `glDrawArrays` or `glDrawElements`. |
| `draw_indirect(buffer, ...)` | Issues `glMultiDrawIndirect` (AZDO). |
| `dispatch(x, y, z)` | Runs a Compute Shader. |
| `barrier(flags)` | Issues `glMemoryBarrier`. |

## 4. Advanced: Material Swapping

The most powerful feature is mixing `bind_pipeline` (Expensive) and `bind_descriptor_set` (Cheap).

```python
# Bind the heavy state once (Shaders, Depth, Blending)
cmd.bind_pipeline(uber_pipeline)

for material in materials:
    # Cheap switch: Just Textures and Uniform Buffers
    cmd.bind_descriptor_set(material.descriptor_set)
    cmd.draw()
```

## 5. Threading & Safety

*   **Recording:** Can be done on **Any Thread** (Worker). It just writes to a malloc'd byte array.
*   **Submission:** Must be done on the **Render Thread** (Context Owner).
*   **Lifecycle:** The CommandBuffer holds strong references to every object it uses. It is safe to `del pipeline` in Python; the Command Buffer keeps it alive until the buffer itself is destroyed.
