# Safety Manual: How to Crash Your Driver

HyperGL is a low-level API. While it protects the Python Interpreter from Segfaults, it **cannot** protect your GPU Driver from valid-but-destructive commands.

If you bypass the safety rails, you will encounter the **TDR (Timeout Detection and Recovery)**, where the OS resets the GPU driver, freezing your screen for 3 seconds and killing your app.

Here are the most common ways to cause a hard crash via "Bad Python."

## 1. The Indirect Buffer Bomb

**The Scenario:**
You use `render_indirect`. You populate the buffer with commands. You accidentally set `vertex_count` to a massive number (e.g., `4,000,000,000`) or a negative integer that cast to unsigned huge int.

**The Code:**
```python
# BAD: Negative 1 becomes UINT_MAX (4.2 Billion)
cmd = struct.pack('=IIII', -1, 1, 0, 0) 
buffer.write(cmd)
pipeline.render_indirect(buffer)
```

**The Result:**
The GPU attempts to read 4 billion vertices. It reads past the VBO, past the texture memory, and into unmapped VRAM.
*   **Best Case:** Garbage geometry covers the screen.
*   **Worst Case:** **GPU Page Fault**. The Driver triggers a TDR reset. The context is lost.

## 2. The `std140` Misalignment

**The Scenario:**
You use a Uniform Buffer (UBO). GLSL expects `std140` layout (vec3 takes 16 bytes). Python `struct.pack` uses tight packing (vec3 takes 12 bytes).

**The Code:**
```python
# GLSL: layout(std140) uniform B { vec3 pos; float scale; };
# Expects: [x, y, z, pad, scale, pad, pad, pad] (32 bytes)

# PYTHON: Tight packing
# Sends:   [x, y, z, scale] (16 bytes)
data = struct.pack('=3f f', x, y, z, scale)
ubo.write(data)
```

**The Result:**
The GPU reads `scale` from the padding bytes of `pos`. It reads garbage for the actual scale. If `scale` is used as an array index or loop counter in the shader, the shader hangs (infinite loop) -> **TDR Crash**.

**The Fix:**
Always pad your structs to 16 bytes (vec4).
```python
# Correct: Add padding
data = struct.pack('=3f f', x, y, z, 0.0) + struct.pack('=f 3f', scale, 0, 0, 0)
```

## 3. The Bindless texture "Use-After-Free"

**The Scenario:**
You make a texture resident. You delete the Python object. The GC runs. You render.

**The Code:**
```python
def setup():
    tex = ctx.image(...)
    handle = tex.get_handle()
    tex.make_resident(True)
    return handle 
    # 'tex' is garbage collected HERE. 
    # Driver deletes texture. Handle is invalid.

h = setup()
# ... later ...
shader.uniforms['u_tex'] = h
pipeline.render()
```

**The Result:**
**Immediate Segmentation Fault** inside the NVIDIA/AMD Driver DLL. The driver tries to dereference a handle that points to freed memory.

**The Fix:**
You *must* keep the Python `Image` object alive as long as the handle is used.

## 4. The Mapped Memory Race

**The Scenario:**
You map a buffer (`storage=True`) for the CPU to write particles. You issue a draw call. You immediately write to the buffer again without waiting.

**The Code:**
```python
ptr = buffer.map()
while True:
    # CPU writes
    write_physics(ptr)
    
    # GPU reads (Draw)
    pipeline.render()
    
    # CPU writes again immediately!
    write_physics(ptr) 
```

**The Result:**
Visual tearing / flickering. On some strict drivers, this can cause a synchronization stall or a hard crash if the memory controller gets confused by simultaneous RW access to non-coherent memory.

**The Fix:**
Use **Triple Buffering** or `ctx.fence()`.

## 5. The "Zombie" Context

**The Scenario:**
You are using `multiprocessing` (not `threading`). You fork the process. You try to use `ctx` in the child process.

**The Result:**
The OS handles for the GPU Context (HGLRC/GLXContext) are not valid in the child process.
*   **Linux:** Hard Crash / X11 Error.
*   **Windows:** Silent failure or Access Violation.

**The Fix:**
Never share Context objects across Processes. Create a new Context in every Process.