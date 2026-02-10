# The HGL-IR Programming Language v2.0

**HGL-IR (HyperGL Intermediate Representation)** is a high-performance scripting language for GPU orchestration. It compiles directly to HyperGL VM Bytecode.

Unlike Python, which incurs interpreter overhead, HGL-IR executes at the speed of C (Zero-GIL). It is designed for:
*   **Procedural Animation:** Calculating positions/rotations on the fly.
*   **GPU-Driven Rendering:** Logic that decides what to draw based on GPU memory.
*   **Zero-Overhead Loops:** Issuing 10,000 draw calls without a Python loop.


## 1. Variables & Registers

HGL-IR offers two ways to manage data: **High-Level Variables** (Recommended) and **Raw Registers** (Assembly-style).

### Automatic Variables (`var`)
The compiler includes a register allocator. You can declare variables, and the compiler maps them to available hardware registers (`i0`-`i5`).

```hgl
var t = time()
var speed = 2.0
fmul t, speed
```

### Hardware Registers
The VM has **8 General Purpose 32-bit Registers** (`i0` through `i7`).
*   **i0 - i5:** Available for user logic.
*   **i6 - i7:** Reserved scratchpads for the compiler (used for immediate values and comparison results).

```hgl
mov i0, 100        // Integer
mov i1, 3.14159    // Float (Automatically bit-cast)
```

## 2. Smart Properties (Reflection)

Instead of manually binding uniforms using strings and types, HGL-IR uses **Compile-Time Reflection** to look up uniform locations and types from your Python objects.

**Syntax:** `@object.property = value`

```hgl
# Old Way (Legacy)
# uniform @pipe, "u_Time", t, "float"

# New Way (Recommended)
# 1. Compiler finds 'u_Time' location in @pipe
# 2. Compiler detects type is GL_FLOAT
# 3. Compiler emits optimized bytecode
@pipe.u_Time = t
```

## 3. Control Flow

### Conditionals (`if` vs `fif`)
Because the VM is typeless, you must explicitly choose between Integer and Floating-Point logic.

| Command | Usage | Description |
| :--- | :--- | :--- |
| **`if`** | `if i0 < 10 { ... }` | **Integer** comparison. Use for counters, loops, bools. |
| **`fif`** | `fif sin_val > 0.0 { ... }` | **Float** comparison. Use for positions, physics, time. |

### Loops
The `loop` construct repeats a block `N` times. The counter counts **down**.

```hgl
# i0 will go: 10, 9, 8 ... 1
loop 10 using i0 {
    draw 6, 1
}
```


## 4. Built-in Functions

HGL-IR supports functional syntax for common system operations.

| Syntax | Description |
| :--- | :--- |
| `var x = time()` | Stores app time (sec) in `x`. |
| `var dt = delta()` | Stores frame delta time in `dt`. |
| `var r = rand()` | Stores random `uint32` bits in `r`. |
| `var (s, c) = sincos(x)` | Calculates Sine and Cosine of `x`. Returns tuple. |


## 5. Instruction Set Reference

### Data Movement
| Instruction | Syntax | Description |
| :--- | :--- | :--- |
| **mov** | `mov REG, VAL` | Sets register to a value or copies another register. |
| **alias** | `alias NAME = REG` | Manual register naming. |
| **var** | `var NAME = VAL` | Automatic register allocation. |

### Arithmetic (ALU)
All math operations work in-place: `DEST = DEST op SRC`.

| Category | Opcodes | Description |
| :--- | :--- | :--- |
| **Integer** | `add`, `sub`, `mul`, `div` | Standard integer math. |
| **Float** | `fadd`, `fsub`, `fmul`, `fdiv` | IEEE-754 Floating Point math. |
| **Bitwise** | `and`, `or`, `xor`, `not` | Bitwise logic. |
| **Shift** | `lsh`, `rsh` | Left/Right bit shift. |

### Comparison
| Opcode | Syntax | Description |
| :--- | :--- | :--- |
| **cmp** | `cmp DST, A, B, "OP"` | **Integer** Compare. Stores 1 or 0 in DST. |
| **fcmp** | `fcmp DST, A, B, "OP"` | **Float** Compare. Stores 1 or 0 in DST. |

*Operators:* `==`, `!=`, `<`, `>`, `<=`, `>=`.

### Memory & Rendering
| Instruction | Syntax | Description |
| :--- | :--- | :--- |
| **bind** | `bind @OBJ` | Binds Pipeline, Compute, or DescriptorSet. |
| **draw** | `draw COUNT, INSTANCE` | Draw call. |
| **dispatch** | `dispatch X, Y, Z` | Compute dispatch. |
| **store** | `store REG, @BUF, OFFSET` | Writes register to `@BUF`. |
| **load** | `load REG, @BUF, OFFSET` | Reads from `@BUF` into register. |
| **slide** | `slide @BUF, SLOT, REG, SIZE` | Slides a UBO/SSBO window based on register offset. |
| **barrier** | `barrier` | Memory barrier for Compute->Draw sync. |


## 6. Example Program

This script implements a **GPU-Driven Strobe Light**. It uses floating-point math to calculate a sine wave and conditional logic to toggle rendering.

```python
import hypergl
from hypergl import HGLCompiler

# 1. Setup Python Resources
pipe = ctx.pipeline(...)

# 2. The HGL-IR Script
script = """
    # 1. Automatic Variable Declaration
    var t = time()
    var (s, c) = sincos(t)
    
    # 2. Setup State
    bind @my_pipe
    
    # 3. Smart Property Setting
    # Automatically finds 'u_Offset' location and type (float)
    @my_pipe.u_Offset = s
    
    # 4. Floating Point Logic
    # If sine is positive, draw. If negative, skip.
    fif s > 0.0 {
        draw 3, 1
    }
"""

# 3. Compile and Run
cmd = ctx.command_buffer()
comp = HGLCompiler(cmd, env={'my_pipe': pipe})
comp.compile(script)

cmd.submit()
```