# HyperGL

**Zero-Overhead, Object-Oriented OpenGL 4.6 Engine for Python.**

[Built upon ZenGL](https://github.com/szabolcsdombi/zengl) | [License: MIT](LICENSE)

> **"Trains Reinforcement Learning agents at 12,000+ FPS with zero-copy NumPy readback. No window manager required."**

HyperGL is a high-performance graphics runtime that treats OpenGL 4.6 as a target for its own **C-side Bytecode Virtual Machine**. It is designed for massive simulations, reinforcement learning, and game engines where the Python interpreter is usually the bottleneck.


## The HyperGL VM

Traditional wrappers (PyOpenGL, ModernGL) pay a **"Python Boundary Tax"** for every draw call. If you loop 10,000 times in Python to draw a forest, your CPU spends 90% of its time managing Python method lookups and 10% actually talking to the GPU.

**HyperGL solves this by moving the render loop into a C-VM:**
1.  **Record:** High-level Python logic (loops, branches, subroutines) is compiled once into a linear binary buffer.
2.  **Submit:** A single C-call replays the entire scene at the limit of the CPU cache.
3.  **No GIL:** The VM releases the GIL during execution, allowing your Python logic and Graphics to run on separate cores.

### Performance Benchmarks (10,000 Draw Calls)
*Tests conducted on Python 3.14.2t (Free-Threaded)*

| Method | Overhead | 10k Draws | Speedup |
| :--- | :--- | :--- | :--- |
| **Traditional Python Loop** | ~65ns / call | 0.65ms | 1x |
| **HyperGL Bytecode VM** | **~9ns / call** | **0.09ms** | **~7x faster** |

*Note: In complex scenes with texture/UBO swapping, the gap widens to **50x-100x** because the VM manages hardware state transitions entirely in C.*


## Key Features

*   ✅ **Turing-Complete Command VM:** Supports `GOTO`, `CALL/RET` (Subroutines), `ALU` math, and 8 internal registers.
*   ✅ **GPU-Driven Branching:** Instructions like `skip_if_zero` allow the C-loop to skip draw calls based on data in mapped GPU memory (Occlusion Culling).
*   ✅ **Binary Scene Format (.hgb):** Serialize recorded bytecode to disk and re-inflate it instantly, bypassing Python setup entirely.
*   ✅ **AZDO & Bindless:** Native support for Multi-Draw Indirect and `GL_ARB_bindless_texture` for "infinite" texture variety.
*   ✅ **Persistent Mapping:** Map GPU SSBOs directly to NumPy/Numba views for zero-copy CPU-GPU communication.


## The Compiler DSL

HyperGL provides a high-level compiler to make VM programming feel like standard Python.

```python
import hypergl
import struct

# 1. Define a Reusable Subroutine (Asset)
@hypergl.subroutine
def draw_forest_cluster(cb):
    sc = hypergl.SceneCompiler(cb)
    cb.bind_pipeline(tree_pipe)
    
    # A Loop that runs entirely in C
    with sc.loop(reg=0, count=1000):
        cb.draw()

# 2. Compile the Frame
cmd = ctx.command_buffer()
sc = hypergl.SceneCompiler(cmd)

cmd.begin()
cmd.clear()

# Logic-based branching in C (Occlusion Culling)
with sc.condition(visibility_ssbo, offset=0):
    draw_forest_cluster(cmd) # Emits 1 C instruction: CALL

cmd.end()

# 3. Submit
while True:
    # Releases GIL. Python can run Physics (Culverin) or AI in parallel.
    cmd.submit() 
```

## Zero-Copy Ecosystem

HyperGL is designed to work in synergy with **[Culverin](https://github.com/Evilpasture/Culverin)** (Jolt Physics for Python). It's another project of mine.

1.  **Physics (Culverin):** Calculates 10,000 ragdolls and writes to a C-Shadow Buffer.
2.  **Graphics (HyperGL):** Maps that same memory as an SSBO.
3.  **Submission:** The VM reads positions directly from the C-array.
4.  **Result:** High-fidelity simulation where **data never becomes a Python object** between the physics step and the draw call.

## Architecture

1.  **The Context:** Managing the hardware handshake (Headless, GLFW, SDL2).
2.  **Pipeline State Objects (PSO):** Immutable state blobs (Shaders, Blending, Depth) validated at startup.
3.  **The VM:** A RISC-inspired execution loop for draw submission.
4.  **The Compiler:** High-level Python tools that emit bytecode.

## Safety

HyperGL is a low-level tool. While it protects the Python interpreter from crashing, it gives you enough power to crash your GPU driver (TDR).
*   **Volatile Memory:** The VM peeks at live GPU memory; ensure your buffers are mapped.
*   **Infinite Loops:** You can create them with `GOTO`. We check for Python signals (Ctrl+C) every 1024 instructions to keep you safe.
*   **Manual Deletion:** Command Buffers hold references to their assets, but remember to call `.release('all')` when shutting down.

## Building from Source

HyperGL is a self-contained C-extension with zero Python dependencies.

```bash
# Clone with submodules if you are contributing
git clone --recursive https://github.com/Evilpasture/HyperGL.git
cd HyperGL
pip install .
```

or

```bash
pip install git+https://github.com/Evilpasture/HyperGL.git
```

---

[License: MIT](LICENSE)