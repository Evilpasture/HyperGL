# The HyperGL Virtual Machine (HVM)

HyperGL includes a fully-featured, Turing-complete **Graphics Virtual Machine**. It is designed to execute rendering logic, compute dispatch, and memory management at native C speeds, completely bypassing the Python Global Interpreter Lock (GIL).

## 1. Architecture

### The Register File (`i0` - `i7`)
The VM provides **8 general-purpose 32-bit registers**.
*   **Storage:** They hold raw 32-bit bit patterns.
*   **Polymorphism:** They can be interpreted as `int32`, `uint32`, or `float32` depending on the opcode used (`add` vs `fadd`).

### The Stack
The VM features a **hardware-backed call stack** with a fixed depth of **16 levels** for subroutines (`CALL`) and a **value stack** (128 entries) for register preservation (`PUSH`/`POP`).

## 2. Instruction Set Architecture (ISA)

### Arithmetic & Logic (ALU)
*   `alu(a, b, op)`:
    *   **Integer:** `add`, `sub`, `mul`, `div`.
    *   **Float:** `fadd`, `fsub`, `fmul`, `fdiv` (IEEE-754).
    *   **Bits:** `and`, `or`, `xor`, `lsh` (<<), `rsh` (>>), `not` (~).
*   `sin_cos(in, s_dest, c_dest)`: Calculates `sin(in)` and `cos(in)` (treating inputs/outputs as `float32`).
*   `gen_rand(dest)`: Generates a deterministic pseudo-random `uint32` (Xorshift).

### Comparison & Branching
*   `cmp(dest, a, b, op)`: **Integer** comparison. `i[dest] = (int(a) op int(b))`.
*   `fcmp(dest, a, b, op)`: **Float** comparison. `i[dest] = (float(a) op float(b))`.
*   `jump_table(reg, [targets])`: O(1) Switch/Case structure.

### Memory & State
*   `load_reg(reg, buf, offset)` / `store_reg(...)`: Direct mapped memory access.
*   `load_reg_indirect(...)`: Array-style access `base + (index * stride)`.
*   `set_buffer_offset(buf, slot, reg, size)`: Dynamic UBO/SSBO sliding window.
*   `set_uniform(loc, reg, type)`: Updates shader constants. `type` can be `0` (float), `1` (int), or `2` (uint).

### Rendering
*   `draw(vc, ic, first)`: Standard draw.
*   `draw_indirect(...)`: Buffer-driven draw.
*   `draw_indirect_count(...)`: **GPU-Autonomy.** The GPU decides the draw count by reading a buffer.

## 3. The Python Compiler (`SceneCompiler`)

The `SceneCompiler` helper translates Python context managers into assembly labels and jumps.

```python
sc = hypergl.SceneCompiler(cmd)

# 1. FOR LOOPS
with sc.loop(reg=0, count=100):
    cmd.draw()

# 2. IF (Register Logic)
# Uses 'i7' as the result register by default
with sc.if_cond(reg_a=0, reg_b=1, op='<'):
    cmd.draw()
```

## 4. The Optimizer ("Super Opcodes")

When `cmd.end()` is called, the C-side peephole optimizer merges instructions to reduce driver overhead.

| Original Sequence | Optimized Opcode | Benefit |
| :--- | :--- | :--- |
| `BIND_SET` + `DRAW` | **`SUPER_BIND_DRAW`** | 50% less dispatch overhead |
| `BIND_SET` + `DRAW_INDIRECT` | **`BIND_SET_DRAW_INDIRECT`** | Zero-latency material application |

## 5. Debugging

### Disassembler
View the compiled bytecode in human-readable format.
```python
print("\n".join(cmd.disassemble()))
```
**Output:**
```text
OFFSET  OPCODE              OPERANDS                HUMAN NOTES
-----------------------------------------------------------------------
0x0000: GET_TIME            i0                      ; Size: 12 bytes
0x000C: SIN_COS             i0 -> i1, i2            ; sin(i0)->i1
0x0018: FCMP                i7, i1, i6, >           ; Float Compare
0x0024: SKIP_REG_ZERO       i7                      ; Branch if False
0x002C: DRAW                vc:-1, ic:-1            ; Draw
```

### Execution Tracing
Pass `trace=True` to `submit()` to dump registers to stderr during execution.
```python
cmd.submit(trace=True)
# [HGL VM] D:0 | Off:  24 | FCMP | i0:1.57  i1:1.0  i7:1
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