# Threading & Concurrency

HyperGL is designed for Python 3.13t (Free-Threaded). It allows you to run Physics, AI, and Logic on separate cores while the Main Thread handles rendering.

## 1. The Golden Rule of OpenGL

**An OpenGL Context can only be active on ONE thread at a time.**

If you try to call `pipeline.render()` from Thread B while Thread A owns the context, the driver will crash or ignore the call.

## 2. The Recommended Pattern: "Worker Logic, Main Render"

This is the safest and most performant pattern. The Main Thread owns the context forever. Worker threads never touch OpenGL directly; they write to **Mapped Memory**.

### Setup
1.  **Main Thread:** Initializes Window & Context.
2.  **Resources:** Create `Storage Buffers (SSBO)` for all dynamic data (Particles, Transforms, Matrices).
3.  **Mapping:** Call `ptr = buffer.map()`. This gives you a CPU pointer to GPU memory.
4.  **Worker Thread:** Receives the pointer. Writes to it using NumPy/Numba/Ctypes.

### Loop
*   **Worker:** Calculates physics -> Writes to `ptr`.
*   **Main:** Calls `ctx.new_frame()` -> `pipeline.render()` -> `ctx.end_frame()`.

**Why this works:** Writing to mapped memory does *not* require an active OpenGL context. It's just RAM access over PCIe.

## 3. The Migration Pattern (Context Handover)

If you *must* issue draw calls from a secondary thread, you must migrate the context.

```python
import hypergl
import threading

ctx = hypergl.context()

def render_worker():
    # 2. Claim Context
    ctx.migrate()
    
    # 3. Render
    ctx.new_frame()
    pipeline.render()
    ctx.end_frame()
    
    # 4. Release Context (Optional, if handing back)
    ctx.release_thread()

# 1. Release from Main Thread
ctx.release_thread()

t = threading.Thread(target=render_worker)
t.start()
t.join()

# 5. Reclaim on Main Thread
ctx.migrate()
```

**Overhead:** Migration involves an OS-level context switch (`wglMakeCurrent` / `glXMakeCurrent`). Do not do this every frame if possible.

## 4. The GIL (Global Interpreter Lock)

Even in standard Python (3.10-3.12), HyperGL releases the GIL during potentially slow operations:

*   `pipeline.render()`
*   `compute.run()`
*   `buffer.read()`
*   `ctx.end_frame()` (glFlush/glFinish)
*   `fence.wait()`

This means a background Python thread *can* run while the GPU is drawing, even without Python 3.13t.

## 5. Garbage Collection & Threads

**Danger:** If a background thread triggers Python's Garbage Collector, and the GC decides to delete a HyperGL object (like a Buffer), the `__del__` method might be called on the wrong thread.

**Solution:** HyperGL uses a **Shared Trash Queue**.
1.  If `Buffer.__del__` is called, it does *not* call `glDeleteBuffers`.
2.  It adds the ID to a thread-safe queue.
3.  The next time `ctx.new_frame()` is called (on the Render Thread), the queue is flushed and resources are safely deleted.

## 6. Safety Checklist

1.  [ ] **Never** call `ctx.image()` or `ctx.buffer()` from a worker thread unless you migrated the context.
2.  [ ] **Never** write to `pipeline.uniforms` from multiple threads simultaneously (Race condition).
3.  [ ] **Do** use `buffer.map()` for heavy data transfer.
4.  [ ] **Do** use `ctx.fence()` if you see tearing (GPU reading while CPU writes).

## 7. The "Render Worker" Pattern

Common in Game Engines and GUI apps. The **Main Thread** handles the OS Window (Events/Resize), while a dedicated **Render Thread** issues draw calls.

### Rules
1.  **Window Creation:** Must happen on Main Thread (OS limitation).
2.  **Context Creation:** Can happen on Main, but must be released immediately.
3.  **Context Ownership:** Render Thread calls `migrate()` once at startup and keeps it forever.

### This example will use GLFW.

```python
import hypergl
import threading
import glfw

def render_loop(window):
    # 3. CLAIM CONTEXT
    # The context was created on Main, but we steal it here.
    # Note: We must create the Python context object *inside* this thread
    # OR pass an existing one that has been released.
    
    # Option A: Create new wrapper for existing OS context
    # hypergl.init() must be called here or shared carefully.
    
    # Option B (Better): Migration
    # ctx.migrate()
    
    ctx = hypergl.context() 
    ctx.migrate() # Bind to this thread

    while not glfw.window_should_close(window):
        ctx.new_frame()
        # ... draw ...
        ctx.end_frame()
        
        # Swap Buffers (Must happen on the thread with the context)
        glfw.swap_buffers(window)

def main():
    glfw.init()
    # 1. CREATE WINDOW (Hidden or Visible)
    window = glfw.create_window(800, 600, "Threaded Render", None, None)
    
    # 2. PREPARE CONTEXT
    glfw.make_context_current(window) # Create GL context
    
    # Initialize HyperGL pointers on Main Thread
    hypergl.init(glfw_loader)
    ctx = hypergl.context()
    
    # CRITICAL: RELEASE CONTEXT
    # Unbinds it from Main Thread so Worker can grab it.
    ctx.release_thread() 
    glfw.make_context_current(None) 

    # 4. START WORKER
    t = threading.Thread(target=render_loop, args=(window,))
    t.start()

    # 5. EVENT LOOP (Main Thread)
    while not glfw.window_should_close(window):
        glfw.wait_events()
        # Handle Resize / Input here
    
    t.join()
```

### Caveats
*   **MacOS:** MacOS has strict threading rules for Cocoa. `glfw.poll_events()` **must** be on the Main Thread. Rendering on a secondary thread works, but context switching can be finicky on some macOS versions.
*   **Resize:** When the window resizes, the Main Thread gets the event. You must signal the Render Thread to update `pipeline.viewport`. Do not call `glViewport` from the Main Thread.