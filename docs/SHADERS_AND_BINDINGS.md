# Shaders, Bindings, and Dead Code Elimination

One of the most frustrating parts of modern OpenGL is **Active Resource Management**. HyperGL is strict about this validation to prevent silent failures.

## The "Inactive Uniform" Error

If you define a uniform in GLSL but **do not use it** to calculate the output, the GLSL compiler removes it. This is called **Dead Code Elimination**.

### The Scenario
**GLSL:**
```glsl
uniform float u_unused; // <--- Defined but never used
void main() {
    gl_Position = vec4(0.0);
}
```

**Python:**
```python
pipeline = ctx.pipeline(
    ...,
    layout=[{'name': 'u_unused', 'binding': 0}] # ERROR!
)
```

### The Fix
HyperGL throws a `ValueError` because you are trying to bind a resource to a variable that doesn't exist in the compiled program.

1.  **Delete the binding** from your Python code if the shader doesn't use it.
2.  **Force usage** in GLSL (e.g., multiply result by `u_unused * 0.0 + 1.0` - hacky).
3.  **Use explicit bindings in GLSL (OpenGL 4.2+)**.

## Explicit Bindings (Recommended)

Instead of mapping names strings in Python, hardcode the binding in GLSL. This is more robust.

**GLSL:**
```glsl
layout(binding = 0) uniform sampler2D u_texture;
layout(binding = 1) uniform CameraBlock { ... };
```

**Python:**
You still need to tell HyperGL about the resource list, but you might skip the `layout` list for UBOs/Samplers if the reflection data catches it correctly. However, providing the `layout` list is the safest way to ensure HyperGL knows which Python binding slot (0, 1, 2) maps to which Uniform Name.

## Uniform Blocks (UBO) vs Uniforms

### Standard Uniforms (`uniform float u_time`)
*   Updated via `pipeline.uniforms['u_time'][:] = ...`
*   Data lives in the Pipeline object.
*   Good for small, frequently changing data (Time, Model Matrix).

### Uniform Buffers (`layout(std140) uniform Common { ... }`)
*   Updated by writing to a `ctx.buffer()`.
*   Bound via `resources=[{'type': 'uniform_buffer', ...}]`.
*   **Good for:** Data shared between multiple pipelines (View Matrix, Lighting Config).

## Troubleshooting "Missing Resource"

If you get `ValueError: Missing resource for "u_tex" with binding 0`:

1.  **Check Layout:** Did you define `{'name': 'u_tex', 'binding': 0}` in `layout`?
2.  **Check Resources:** Did you define `{'type': 'sampler', 'binding': 0, 'image': img}` in `resources`?
3.  **Check Indices:** Do the binding numbers match? (0 -> 0).