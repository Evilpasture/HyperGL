# Troubleshooting & Common Bugs

Graphics programming is often "All or Nothing." If you make a small mistake, you usually get a black screen instead of an error message.

This guide covers the most common HyperGL pitfalls.

---

## 1. The "Black Screen of Death"

**Symptom:** The window opens, but everything is black (or the clear color).

**Checklist:**
1.  **Is the Camera inside the object?** Check your Near/Far planes. `near=0.1, far=1000.0` is standard.
2.  **Is Face Culling on?** If `cull_face='back'`, ensure your vertices are winding Counter-Clockwise (CCW). Try `cull_face='none'` to debug.
3.  **Is the Depth Test on?** Ensure `depth={'write': True, 'func': 'less'}` is set in the pipeline.
4.  **Are you calling `ctx.new_frame()`?** If you forget this, the screen won't clear.
5.  **Are your shaders compiling?** Check the console for `ValueError: Fragment Shader Error`.

---

## 2. GLSL: "Invalid Binding" or "Missing Resource"

**Symptom:** `ValueError: Missing resource for "u_tex" with binding 0`

**Cause:**
HyperGL validates that every uniform defined in your Python `layout` actually exists in the GLSL code. If the GLSL compiler optimized away an unused variable, HyperGL throws an error.

**Fix:**
*   **Remove the unused uniform** from your Python `layout` list.
*   OR force usage in the shader: `color += texture(u_tex, uv) * 0.0001;`

---

## 3. Buffer Updates are "Flickering"

**Symptom:** Objects flicker or disappear randomly.

**Cause:** Race Condition. You are writing to a Buffer (via `map()` or `write()`) while the GPU is still reading from it for the previous frame.

**Fix:**
*   Use **Double Buffering** for dynamic data (ping-pong buffers).
*   Use `ctx.fence()` to ensure the GPU is finished before writing.
*   Ensure your `stride` and `offset` in `vertex_buffers` matches your data layout exactly.

---

## 4. Bindless Textures crashing

**Symptom:** `Segmentation Fault` or Driver Crash (TDR).

**Cause:** You Garbage Collected the Python `Image` object while it was Resident on the GPU.

**Fix:**
You must keep a reference to the image object in Python as long as the GPU is using it.
```python
# BAD
def setup():
    tex = ctx.image(...)
    tex.make_resident(True)
    # 'tex' dies here, handle becomes invalid!

# GOOD
self.textures = [] # Keep alive
```

---

## 5. Context Loss

**Symptom:** `RuntimeError: [HyperGL] the context is lost`

**Cause:**
1.  You closed the window.
2.  You switched threads without migrating (Free-Threaded mode).
3.  The GPU driver crashed/reset (Windows TDR).

**Fix:**
*   Check `ctx.lost` before rendering.
*   If threading: ensure you call `ctx.migrate()` on the new thread.

---

## 6. Windows: "GDI Generic" Renderer

**Symptom:** `ctx.info['renderer']` says "GDI Generic" or performance is terrible (Software Rendering).

**Cause:**
Windows failed to load the hardware OpenGL driver (ICD). This often happens if you create a window, close it, and try to open a new one in the same process without cleaning up properly.

**Fix:**
*   Ensure you only call `hypergl.init()` **once**.
*   HyperGL's internal loader uses a "Dummy Window" trick to jump-start the ICD. If that fails, update your GPU drivers.

---

## 7. Python 3.13t: "Fatal Python Error: PyThreadState_Get: no current thread"

**Symptom:** Crash when calling HyperGL from a background thread. If you're lucky, HyperGL will throw an exception(could be any exception), but that's not guaranteed.

**Cause:**
You launched a `threading.Thread` but didn't ensure the Python GIL state was initialized for it (rare, but happens in embedded scenarios).

**Fix:**
Ensure you call `ctx.migrate()` inside the thread before doing any OpenGL work.
```python
def worker():
    ctx.migrate() # <--- REQUIRED
    ctx.new_frame()
```

## 8. "Invalid vertex attribute location X"

**Symptom:** `ValueError: Invalid vertex attribute location 0` (or 1, 2, etc.)

**Cause:**
You are trying to bind a vertex buffer to a location that the GLSL compiler has removed or never saw.
1.  **Missing Input:** You forgot to write `layout(location=X) in vec3 my_var;` in the Vertex Shader.
2.  **Unused Input:** You defined the input, but didn't use it to calculate `gl_Position` or an `out` variable. The GLSL compiler deleted it to save registers, and HyperGL's validation now sees that slot as "Empty."

**The Fix:**
*   Ensure the attribute is defined in GLSL with the correct `location`.
*   Ensure the attribute is "Active" by using it in a calculation.
*   *Debug Tip:* If you just want to keep an attribute for later but don't need it yet, add `gl_Position.xyz += my_attr * 0.00001;` to force it to stay active.