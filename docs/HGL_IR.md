# The HGL-IR Programming Language

**HGL-IR (HyperGL Intermediate Representation)** is a high-performance scripting language for GPU orchestration. It compiles directly to HyperGL VM Bytecode.

Unlike Python, which incurs interpreter overhead, HGL-IR executes at the speed of C. It is designed for:
*   **Procedural Animation:** Calculating positions/rotations on the fly.
*   **GPU-Driven Rendering:** Logic that decides what to draw based on GPU memory.
*   **Zero-Overhead Loops:** Issuing 10,000 draw calls without a Python loop.

---

## 1. Basic Syntax

HGL-IR is an assembly-like language with high-level control structures.

### Registers
The VM has **8 General Purpose Registers** (`i0` through `i7`).
*   They hold 32-bit values (Integer, Float, or Bitmask).
*   They persist across function calls.

```hgl
mov i0, 100        // Integer
mov i1, 3.14159    // Float (Automatically bit-cast)
mov i2, 0xFF00FF   // Hex
```

### The Environment (`@`)
HGL-IR cannot create resources (Buffers/Pipelines). It references objects created in Python using the `@` symbol. These references are resolved via the `env` dictionary passed to the compiler.

```hgl
bind @my_pipeline  // Looks up 'my_pipeline' in the Python env dict
store i0, @my_buf, 0
```

### Comments
```hgl
// This is a comment
# This is also a comment
```

---

## 2. Preprocessor & Aliases

HGL-IR supports a safe, two-pass preprocessor for modularity.

### Directives
*   `#define KEY VALUE`: Constant replacement.
*   `#include "key"`: Inserts the content of `env["key"]` into the script.

### Register Aliasing
Give registers meaningful names to keep your code readable.

```hgl
#include "constants.hgl"

alias counter = i0
alias offset  = i1

mov counter, MAX_ITEMS  // MAX_ITEMS defined in constants.hgl
```

---

## 3. Control Flow

Unlike raw assembly, HGL-IR provides structured blocks to handle jumps and labels automatically.

### Loops
The `loop` construct repeats a block `N` times. The counter counts **down**.

```hgl
// i0 will go: 10, 9, 8 ... 1
loop 10 using i0 {
    print "Iteration"
    print i0
}
```

### Conditionals (`if`)
Executes the block if the register comparison is true.
Operators: `==`, `!=`, `<`, `>`, `<=`, `>=`.

```hgl
if i0 < i1 {
    draw 3, 1
}
```

### Subroutines (`call` / `ret`)
You can call other CommandBuffers as subroutines.

```hgl
call @sub_buffer
```

---

## 4. Instruction Set Reference

### 📦 Data Movement
| Instruction | Syntax | Description |
| :--- | :--- | :--- |
| **mov** | `mov REG, VAL_OR_REG` | Sets register to a value (int/float) or copies another register. |
| **alias** | `alias NAME = REG` | Assigns a name to a register. |

### 📐 Arithmetic (ALU)
All math operations work in-place: `DEST = DEST op SRC`.

| Opcode | Description |
| :--- | :--- |
| `add`, `sub`, `mul`, `div` | **Integer** arithmetic. |
| `fadd`, `fsub`, `fmul`, `fdiv` | **Floating Point** arithmetic (IEEE-754). |
| `and`, `or`, `xor` | Bitwise logic. |
| `lsh`, `rsh` | Bitwise Shift Left / Right. |
| `not` | Bitwise NOT (Unary: `not i0`). |

### 🔮 Procedural Math
| Instruction | Syntax | Description |
| :--- | :--- | :--- |
| **sincos** | `sincos IN, SIN, COS` | Calculates sin/cos of `IN` (radians). Output is float. |
| **rand** | `rand DEST` | Generates a random `uint32` using Xorshift32. |

### 💾 Memory Access
| Instruction | Syntax | Description |
| :--- | :--- | :--- |
| **store** | `store REG, @BUF, OFFSET` | Writes register to `@BUF` at literal byte offset. |
| **load** | `load REG, @BUF, OFFSET` | Reads from `@BUF` into register. |
| **store_indirect** | `store_indirect VAL, @BUF, IDX, STRIDE` | Writes `VAL` to `BUF[IDX * STRIDE]`. |
| **load_indirect** | `load_indirect DEST, @BUF, IDX, STRIDE` | Reads `DEST` from `BUF[IDX * STRIDE]`. |
| **copy** | `copy @SRC, @DST, S_OFF, D_OFF, SIZE` | GPU-side `memcpy` between buffers (Register offsets). |
| **slide** | `slide @BUF, SLOT, REG, SIZE, "TYPE"` | Slides a UBO/SSBO window based on register offset. |

### 🎨 Rendering & Compute
| Instruction | Syntax | Description |
| :--- | :--- | :--- |
| **bind** | `bind @OBJ` | Binds Pipeline, Compute, or DescriptorSet. |
| **draw** | `draw COUNT, INSTANCE` | Draw call. Omit args to use Pipeline defaults. |
| **dispatch** | `dispatch X, Y, Z` | Compute dispatch. |
| **draw_mdi_count** | `draw_mdi_count @DRAW, @CNT, MAX` | GPU-Driven Indirect Draw (GL 4.6). |
| **uniform** | `uniform @PIPE, "NAME", REG, "TYPE"` | Injects register value into a named shader uniform. |
| **barrier** | `barrier` | Memory barrier for Compute->Draw sync. |
| **clear** | `clear` | Clears framebuffer. |

### ⚙️ System & Debug
| Instruction | Syntax | Description |
| :--- | :--- | :--- |
| **time** | `time REG` | Stores app time (sec) as float in register. |
| **delta** | `delta REG` | Stores frame delta (sec) as float in register. |
| **print** | `print "Msg"` (opt: `REG`) | Prints message (and optional register) to stderr. |
| **assert** | `assert REG, VAL, "OP"` | Halts execution if condition fails. |

---

## 5. Example Program

This script implements a **GPU-Driven Particle Jitter**. It iterates through an array of objects, calculates a random offset for each, updates their position in memory, and draws them.

```python
import hypergl
from hypergl import HGLCompiler

# 1. Setup Python Resources
ctx = hypergl.context()
pipe = ctx.pipeline(...)
ubo = ctx.buffer(size=4096, uniform=True)

# 2. The HGL-IR Script
script = """
    # Registers
    alias loop_idx = i0
    alias rand_val = i1
    alias offset   = i2
    
    bind @my_pipe

    # Loop 100 times (draw 100 objects)
    loop 100 using loop_idx {
        
        # 1. Generate Random Jitter (Float 0.0 - 1.0)
        # Note: 'rand' gives uint bits. To use as float, we mask or use logic.
        # Simple version: just use raw bits for visual noise.
        rand rand_val
        
        # 2. Update Uniform Buffer at correct slot
        # Calculate offset: (100 - loop_idx) * 16 (std140 alignment)
        mov offset, 100
        sub offset, loop_idx
        mov i3, 16
        mul offset, i3
        
        # Write random pos to UBO
        store_indirect rand_val, @my_ubo, offset, 1
        
        # 3. Slide the UBO window for the shader
        slide @my_ubo, 0, offset, 16, "uniform"
        
        # 4. Draw
        draw 6, 1
    }
"""

# 3. Compile and Run
cmd = ctx.command_buffer()
comp = HGLCompiler(cmd, env={'my_pipe': pipe, 'my_ubo': ubo})
comp.compile(script)

cmd.submit()
```