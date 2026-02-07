# The HyperGL Virtual Machine (HVM)

HyperGL includes a fully-featured, Turing-complete **Graphics Virtual Machine**. It is designed to execute rendering logic, compute dispatch, and memory management at native C speeds, completely bypassing the Python Global Interpreter Lock (GIL).

It is not just a command list; it is a processor. It has registers, a stack, arithmetic logic, and control flow.


## 1. Architecture

### The Register File (`i0` - `i7`)
The VM provides **8 general-purpose 32-bit registers**.
*   **Storage:** They hold raw 32-bit bit patterns.
*   **Polymorphism:** They can be interpreted as `int32`, `uint32`, or `float32` depending on the instruction used.
*   **Persistence:** Registers persist across `subroutine` calls (like C `static` variables), but can be pushed/popped to the stack.

### The Stack
The VM features a **hardware-backed call stack** with a fixed depth of **16 levels** for subroutines (`CALL` instruction) and a **value stack** (128 entries) for register preservation (`PUSH`/`POP`).

### The Memory Model
The VM interacts with GPU memory (Buffers) directly.
*   **Direct Access:** `LOAD_REG` / `STORE_REG` (Read/Write mapped pointers).
*   **Indirect Access:** `LOAD_REG_INDIRECT` (Array-style access: `base + (index * stride)`).
*   **VRAM-to-VRAM:** `COPY_BUFFER` moves data between buffers without CPU intervention.


## 2. High-Level Python Compiler (`SceneCompiler`)

While the VM runs binary bytecode, you write logic using the `SceneCompiler` helper, which translates Python context managers into assembly labels and jumps.

### Control Flow
```python
sc = hypergl.SceneCompiler(cmd)

# 1. FOR LOOPS
# "Run 100 times using register i0 as the counter"
with sc.loop(reg=0, count=100):
    cmd.draw()

# 2. WHILE LOOPS (Memory)
# "Keep running while the value at buffer[offset] is not zero"
with sc.while_not_zero(buffer, offset=0):
    cmd.dispatch(1, 1, 1)

# 3. IF / ELSE (Conditionals)
# "If i0 < i1..."
with sc.if_cond(reg_a=0, reg_b=1, op='<'):
    cmd.draw()

# 4. SWITCH / CASE (Jump Tables)
# "Jump to label based on i0 in O(1) time"
with sc.switch(reg=0, cases=3) as labels:
    cmd.label(labels[0]) # Case 0
    # ... logic ...
    cmd.goto(labels[2])  # Manual break
```

### Type-Safe Injection
Since registers are typeless bits, the compiler provides helpers to inject Python values correctly:
```python
sc.set_f32(0, 3.14159)   # Bit-casts float to uint32 for i0
sc.set_u32(1, 0xFF00FF)  # Sets uint32 for i1
sc.inject_uniform(program, "u_Time", reg=0) # Uploads i0 to shader
```


## 3. Instruction Set Architecture (ISA) v2.0

### Arithmetic & Logic (ALU)
The VM includes a full ALU for procedural animation and logic.
*   `alu(a, b, op)`:
    *   **Math:** `add`, `sub`, `mul`, `div` (Integer).
    *   **Bits:** `and`, `or`, `xor`, `lsh` (<<), `rsh` (>>), `not` (~).
*   `sin_cos(in, s_dest, c_dest)`: Calculates `sin(in)` and `cos(in)` (treating inputs/outputs as `float32`).
*   `gen_rand(dest)`: Generates a deterministic pseudo-random `uint32` (Xorshift).
*   `cmp(dest, a, b, op)`: Sets `dest` to 1 if comparison (`==, !=, <, >`) is true, 0 otherwise.

### Memory Operations
*   `load_reg(reg, buf, offset)`: `i[reg] = buf[offset]`.
*   `store_reg(reg, buf, offset)`: `buf[offset] = i[reg]`.
*   `load_reg_indirect(reg, buf, idx_reg, base, stride)`: `i[reg] = buf[base + (i[idx] * stride)]`.
*   `copy_buffer(src, dst, src_reg, dst_reg, size_reg)`: GPU-side `memcpy`.

### Rendering & Compute
*   `bind_pipeline(pipe)`: Optimizes redundant binds automatically.
*   `bind_descriptor_set(set)`: Fast material switching.
*   `draw_indirect(buf, ...)`: Standard Indirect Draw.
*   `draw_indirect_count(buf, count_buf, ...)`: **GPU-Autonomy.** The GPU decides the draw count by reading `count_buf`.
*   `set_buffer_offset(buf, slot, reg, size)`: Dynamic UBO/SSBO sliding window.
*   `set_uniform(loc, reg, type)`: Updates shader constants from VM registers.

### System & Telemetry
*   `get_time(reg)`: Stores `float` seconds since app start.
*   `get_delta(reg)`: Stores `float` frame delta time.
*   `get_stat(reg, stat)`: Reads engine perf counters (`draw_calls`, `pipeline_swaps`) into a register for dynamic LOD logic.
*   `assert_reg(reg, val, op)`: Debug check. halts VM if false.


## 4. The Optimizer (The "Super Opcodes")

When you call `cmd.end()`, the C-side peephole optimizer scans the bytecode and merges common patterns into "Super Instructions" to reduce driver overhead.

| Original Pattern | Optimized Opcode | Benefit |
| :--- | :--- | :--- |
| `BIND_SET` + `DRAW` | **`SUPER_BIND_DRAW`** | 50% less dispatch overhead |
| `BIND_SET` + `DRAW_INDIRECT` | **`BIND_SET_DRAW_INDIRECT`** | Zero-latency material application |

*Note: The optimizer is safe. It will not merge instructions if a `GOTO` targets the second instruction.*


## 5. Debugging & Introspection

### Smart Disassembler
You can view the compiled assembly of any buffer. It resolves labels, variable names, and optimization results.

```python
print("\n".join(cmd.disassemble()))
```
**Output:**
```text
OFFSET  OPCODE              OPERANDS                HUMAN NOTES
-----------------------------------------------------------------------
0x0000: SET_ITER            i0, 10                  ; Size: 16 bytes
        LABEL               "TREE_LOOP"
0x0010: CALL                "DrawTreeMesh"          ; Execute subroutine
0x0020: JUMP_ITER           i0, "TREE_LOOP"         ; Decrement and loop
0x0030: RET                                         ; Return to parent caller
```

### Execution Tracing
Pass `trace=True` to `submit()` to see a live register dump in the console.
```python
cmd.submit(trace=True)
# [HGL VM] D:0 | Off:  32 | ALU   | i0:10   i1:20   i2:0    i3:0 
```

## 6. Binary Format (HGB 2.0)

The **HyperGL Binary (HGB)** format is relocatable. When saved, pointers to Python objects (Buffers, Pipelines) are "deflated" into indices. When loaded via `patch()`, they are "inflated" back into pointers using a provided asset list.

**Security:** The inflation process performs **Strict Type Checking**. You cannot trick the VM into treating a `Buffer` as a `Pipeline` to cause a crash.


## 7. Example: GPU-Driven Particle Simulation

This script simulates physics and renders particles without a single Python loop running during the frame.

```python
# 1. Setup
sc = hypergl.SceneCompiler(cmd)
cmd.begin()

# 2. Physics Step (Compute)
cmd.bind_compute(physics_pipe)
cmd.get_delta(0)            # i0 = dt
sc.inject_uniform(physics_pipe, "u_Dt", 0)
cmd.dispatch(GROUPS, 1, 1)

# 3. Memory Barrier (Auto-inserted by VM, but logic here for clarity)
# The VM ensures the Compute write is visible to the Draw read.

# 4. Rendering Step
cmd.bind_pipeline(render_pipe)
cmd.draw()

cmd.end()

# 5. Run (Zero GIL)
cmd.submit()
```