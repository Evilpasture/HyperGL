# Command Buffers & The HyperGL VM

The **Command Buffer** is the most advanced feature of HyperGL. It allows you to record a sequence of rendering commands into a binary bytecode buffer and execute them in a tight C loop.

This architecture bypasses the Python interpreter entirely during the render pass, allowing for **Zero-GIL Rendering**, **GPU-Driven Control Flow**, and **Modular Subroutines**.

---

## 1. The Workflow: Compile once, Run forever

HyperGL Command Buffers use a **Bake-and-Play** model.

1.  **Begin:** `cmd.begin()` resets the buffer.
2.  **Record:** High-level Python calls are translated into 8-byte aligned C packets.
3.  **End:** `cmd.end()` finalizes the buffer and patches all jump offsets (Labels/Gotos).
4.  **Submit:** `cmd.submit()` replays the bytecode at the speed of the CPU cache.

```python
cmd = ctx.command_buffer()
cmd.begin()
cmd.clear()
cmd.bind_pipeline(my_pipeline)
cmd.draw()
cmd.end()

while running:
    # 0 Python overhead. The entire frame is replayed in C.
    cmd.submit() 
```

---

## 2. High-Level Python Compiler

While the C VM works with "Assembly-like" instructions, HyperGL provides high-level Python constructs to make recording complex logic intuitive.

### Loops and Conditions
The `SceneCompiler` translates Python `with` blocks into `GOTO`, `LABEL`, and `SKIP` instructions.

```python
sc = hypergl.SceneCompiler(cmd)

# VM-side Loop: repeats 100 times entirely in C
with sc.loop(reg=0, count=100):
    cmd.draw()

# GPU-Driven Branching: skips the block if vis_buffer[0] == 0
with sc.condition(vis_buffer, offset=0):
    cmd.print("Object is visible!")
    cmd.draw()
```

### Modular Subroutines
The `@subroutine` decorator allows you to define reusable chunks of rendering logic. Subroutines are recorded once and called via the `CALL` instruction, preserving hardware state inheritance.

```python
@hypergl.subroutine
def draw_skybox(cb):
    cb.bind_pipeline(sky_pipe)
    cb.draw()

# In main buffer:
main_cmd.begin()
draw_skybox(main_cmd) # Emits a single C-level CMD_CALL
main_cmd.end()
```

---

## 3. Instruction Set Reference (ISA)

### 🎨 Graphics & Compute
*   `clear(mask)`: Clears the FBO.
*   `bind_pipeline(pipe)`: Binds Shaders/VAO/State.
*   `bind_descriptor_set(set)`: Swaps textures/UBOs without changing shaders.
*   `draw(vc, ic, first)`: Standard draw. `-1` inherits from Pipeline.
*   `draw_indirect(buf, count...)`: AZDO indirect drawing.
*   `dispatch(x, y, z)` / `barrier(flags)`: Compute management.

### 🔀 Control Flow & Subroutines
*   `call(other_cb)`: Executes another buffer. Inherits parent state. (Max depth: 16).
*   `ret()`: Exits current buffer immediately.
*   `label(name)` / `goto(target)`: Named jumps.
*   `skip_if_zero(buf, offset)`: Skips the **next** instruction if memory is 0.

### 🔢 Registers & ALU
The VM has **8 internal registers (i0-i7)** that persist across subroutine calls.
*   `set_iter(reg, val)`: Set register value.
*   `load_reg(reg, buf, offset)`: Read `uint32` from GPU memory into register.
*   `store_reg(reg, buf, offset)`: Write register value to GPU memory.
*   `alu(reg_a, reg_b, op)`: Arithmetic (`add`, `sub`, `mul`, `div`, `and`, `or`).
*   `jump_iter(reg, target)`: Decrement register and jump if `> 0`.

### ⏱️ Synchronization (Fencing)
*   `signal(fence)`: Updates a Fence object with a new sync point.
*   `wait(fence)`: **GPU-side wait**. Pauses GPU queue until fence is reached.
*   `skip_if_not_ready(fence)`: **Non-blocking poll**. Skips the next instruction if the GPU hasn't reached the fence yet.

---

## 4. Concurrency & Safety

### The "Zero-GIL" submission
`cmd.submit()` releases the GIL. This is the "Holy Grail" for Python 3.13t:
1.  **Core A (Render Thread):** Enters `cmd.submit()`, locks the GL context, and starts spinning through bytecode.
2.  **Core B (Logic Thread):** Python continues running AI, Physics (Culverin), or Networking.
3.  **Result:** The rendering logic effectively consumes **zero** Python time.

### Instruction Integrity
*   **Volatile Reads:** `skip_if_zero` and `load_reg` use `volatile` C pointers, ensuring the VM always sees the latest data written by the GPU or other CPU threads.
*   **Yield Points:** The VM checks for Python signals (Ctrl+C) every 1024 instructions. Even an infinite `GOTO` loop in C can be interrupted.
*   **Memory Safety:** Bounds checks are performed on every pointer advancement. If the bytecode is corrupted, execution aborts safely.

---

## 5. Performance Tips

1.  **Use `bind_descriptor_set`:** Switching Pipelines (Shaders) is expensive. Switching Descriptor Sets (Textures) is cheap.
2.  **Use Subroutines:** They keep your main buffer clean and the `CMD_CALL` instruction is virtually free.
3.  **Register Math:** Perform counters and offsets in registers rather than re-recording.
4.  **Headless Performance:** In headless/ML environments, use a single Command Buffer to avoid all Python overhead during training steps.

## 6. The .hgb Binary Format

HyperGL Command Buffers can be serialized into a relocatable binary format. This allows for extremely fast scene loading and the creation of "Baked Scene" assets.

### File Header (16 bytes)
| Field | Offset | Size | Description |
| :--- | :--- | :--- | :--- |
| **Magic** | 0 | 4 | `HGLB` (0x424C4748) |
| **Major** | 4 | 2 | ISA Version (Breaking changes) |
| **Minor** | 6 | 2 | Feature Version (Backward compatible) |
| **Data Size** | 8 | 4 | Byte length of the recorded bytecode |
| **Reserved** | 12 | 4 | Alignment padding |

### Usage
```python
# Save to disk
bytecode, assets = cmd.serialize()
with open("world.hgb", "wb") as f:
    f.write(bytecode)
# Store 'assets' metadata separately (JSON/MsgPack)

# Load from disk
with open("world.hgb", "rb") as f:
    raw_bytes = f.read()
new_cmd = ctx.command_buffer()
new_cmd.patch(raw_bytes, reconstructed_assets)
```