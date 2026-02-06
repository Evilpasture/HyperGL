# Pipeline State Objects (PSO)

In HyperGL, you do not issue commands like `glUseProgram`, `glEnable`, or `glBindFramebuffer`. Instead, you create a **Pipeline**.

A Pipeline is an **immutable** object that represents the *entire* state of the GPU required to issue a draw call.

## 1. The Anatomy of a Pipeline

When you create a pipeline, HyperGL validates and bakes the following state:

1.  **Shaders:** Vertex, Fragment (and Geometry/Tessellation if supported).
2.  **Resource Bindings:** Which UBOs/SSBOs go to which binding points.
3.  **Vertex Layout:** VBOs, Attributes, Strides, and Offsets.
4.  **Raster State:** Blending, Depth Test, Stencil, Culling, Topology.
5.  **Targets:** The Framebuffer (or screen).

```python
pipeline = ctx.pipeline(
    vertex_shader=...,
    fragment_shader=...,
    layout=[...],       # Shader Resource Interface
    resources=[...],    # Actual Python Objects
    depth={'write': True, 'func': 'less'},
    cull_face='back',
    blend={'enable': True, 'src_color': 'src_alpha', ...},
    vertex_buffers=[...],
    framebuffer=[image],
    viewport=(0, 0, 800, 600)
)
```

## 2. Why Immutable?

In traditional OpenGL, changing state (`glEnable`) is expensive because the driver has to re-validate the entire pipeline every time you draw.

By making the pipeline immutable:
1.  **Validation happens once:** Errors are caught at initialization, not during the render loop.
2.  **State Deduplication:** HyperGL caches internal GL objects. If two pipelines share the same Shader Program or Blend State, the driver overhead is zero.
3.  **Thread Safety:** Pipelines are read-only during rendering, making them safe to use in multi-threaded command generation (future roadmap).

## 3. Managing "State Explosion"

Since you need a new Pipeline object for every combination of states (e.g., one for Opaque, one for Transparent, one for Wireframe), you might end up with too many objects.

### Solution: Templates
Use the `template` argument to clone an existing pipeline and change only specific properties.

```python
# Base Pipeline (Opaque)
base_pipe = ctx.pipeline(
    vertex_shader=vs, fragment_shader=fs,
    depth={'write': True, 'func': 'less'},
    framebuffer=[fbo]
)

# Variant (Transparent)
# Reuses compiled shaders, layouts, and framebuffer targets!
trans_pipe = ctx.pipeline(
    template=base_pipe,
    blend={'enable': True, 'src_color': 'src_alpha', 'dst_color': 'one_minus_src_alpha'},
    depth={'write': False, 'func': 'less'} # Read-only depth
)
```

## 4. Dynamic State

Some state *must* change frequently. HyperGL exposes these as dynamic properties or Uniforms.

### Viewport
You can change the viewport without recreating the pipeline.
```python
pipeline.viewport = (0, 0, 1920, 1080)
```

### Uniforms
Uniforms are not "baked" into the PSO. They are mutable memory.
```python
# Fast Update
pipeline.uniforms['u_time'][:] = struct.pack('f', time.time())
```

### Indirect Draw Parameters
When using `render_indirect`, the vertex count and instance count are read from a GPU buffer, effectively making the draw call dynamic.

## 5. Vertex Layouts

Mapping buffer data to shader attributes is done via `vertex_buffers`.

```python
ctx.pipeline(
    ...,
    vertex_buffers=[
        # Binding 0: Positions (vec3)
        {'buffer': vbo_pos, 'location': 0, 'format': '3f', 'offset': 0, 'stride': 12},
        
        # Binding 1: Instance Matrix (mat4 = 4 x vec4)
        # Note: Matrices take 4 locations!
        {'buffer': vbo_inst, 'location': 1, 'format': '4f', 'offset': 0,  'stride': 64, 'step': 'instance'},
        {'buffer': vbo_inst, 'location': 2, 'format': '4f', 'offset': 16, 'stride': 64, 'step': 'instance'},
        {'buffer': vbo_inst, 'location': 3, 'format': '4f', 'offset': 32, 'stride': 64, 'step': 'instance'},
        {'buffer': vbo_inst, 'location': 4, 'format': '4f', 'offset': 48, 'stride': 64, 'step': 'instance'},
    ]
)
```

### `hypergl.bind` Helper
Manually calculating strides is tedious. Use the helper:
```python
# Automatically calculates strides and offsets for interleaved data
bindings = hypergl.bind(vbo, '3f 3f 2f', 0, 1, 2)
```

## 6. Rendering

Calling `pipeline.render()` performs the following sequence in C:

1.  **Bind Framebuffer** (if changed).
2.  **Bind Program** (if changed).
3.  **Bind VAO** (if changed).
4.  **Apply Global Settings** (Depth, Blend, Cull - only if changed).
5.  **Bind Resources** (Textures, UBOs).
6.  **Upload Dirty Uniforms**.
7.  **Issue `glDrawArrays` / `glDrawElements`**.

This tight loop minimizes driver calls.