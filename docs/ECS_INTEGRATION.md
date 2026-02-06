# HyperGL + Culverin: ECS Integration Guide

This guide explains how to build a high-performance Entity Component System (ECS) using **HyperGL** (Graphics) and **Culverin** (Physics).

The goal: **Render 10,000+ physics objects at 60Hz using Python 3.13t (Free-Threaded).**

You can make ECS work with HyperGL regardless of any physics engine, I'll be choosing Culverin for convenience and for double documentation.
---

## 1. The Data Flow

In a traditional Python engine (like PyGame or PyBullet), you do this:
1.  Loop over entities.
2.  Get Position (Python Tuple).
3.  Update Sprite (Python Attribute).
4.  Draw one by one.

**This is slow.**

In the HyperGL/Culverin ECS, the loop looks like this:
1.  **Physics Step:** Culverin updates internal C-arrays.
2.  **Sync:** You get a raw `memoryview` of positions.
3.  **Render:** You upload that `memoryview` directly to a HyperGL `Buffer`.
4.  **Draw:** HyperGL draws 10,000 instances in one call.

**Zero Python loops required.**

---

## 2. The Components

### Physics Component
Doesn't store data. Stores a **Handle**.
```python
class RigidBody:
    def __init__(self, world, shape, pos):
        self.handle = world.create_body(pos, shape=shape)
```

### Render Component
Doesn't store meshes. Stores an **ID** for the Instance Buffer.
```python
class MeshInstance:
    def __init__(self, mesh_id, material_id):
        self.mesh_id = mesh_id
        self.material_id = material_id
```

---

## 3. The System: "PhysicsToRender"

This is the critical piece of infrastructure. It maps the Physics World to the Graphics Instance Buffer.

```python
class RenderSystem:
    def __init__(self, ctx, world, max_instances=10000):
        self.ctx = ctx
        self.world = world
        
        # 1. Instance Buffer (GPU)
        # 28 bytes per instance: vec3 pos, vec4 rot (quat)
        self.instance_buffer = ctx.buffer(size=max_instances * 28, access='stream_draw')
        
        # 2. Pipeline
        self.pipeline = ctx.pipeline(
            ...,
            vertex_buffers=[
                {'buffer': mesh_vbo, 'format': '3f', 'location': 0},
                # Instanced Attributes (Step=1)
                {'buffer': self.instance_buffer, 'format': '3f 4f', 'location': 1, 'step': 'instance'}
            ]
        )

    def update(self, dt, alpha):
        # 1. Get Interpolated State from Physics
        # Returns a contiguous bytes object/memoryview
        # [x, y, z, qx, qy, qz, qw, x, y, z...]
        render_data = self.world.get_render_state(alpha)
        
        # 2. Upload to GPU
        # Zero-Copy: C-level memcpy from Physics -> GL Driver
        self.instance_buffer.write(render_data)
        
        # 3. Draw
        self.ctx.new_frame()
        self.pipeline.render(instance_count=self.world.count)
        self.ctx.end_frame()
```

---

## 4. The Vertex Shader

Your shader needs to handle the Quaternion rotation coming from Culverin.

```glsl
#version 460 core

layout(location=0) in vec3 in_pos;      // Mesh Vertex
layout(location=1) in vec3 inst_pos;    // Instance Position
layout(location=2) in vec4 inst_quat;   // Instance Rotation

uniform mat4 u_view_proj;

vec3 rotate_vector(vec4 q, vec3 v) {
    return v + 2.0 * cross(q.xyz, cross(q.xyz, v) + q.w * v);
}

void main() {
    vec3 world_pos = rotate_vector(inst_quat, in_pos) + inst_pos;
    gl_Position = u_view_proj * vec4(world_pos, 1.0);
}
```

---

## 5. Free-Threaded Game Loop (Python 3.13t)

To get max performance, run Physics and Rendering in parallel.

```python
import threading
import time

def physics_loop(world):
    while running:
        # GIL is RELEASED here
        world.step(1/60.0) 
        
        # Game Logic (AI, Scripts)
        run_scripts()

def render_loop(window, ctx, render_system):
    while not glfw.window_should_close(window):
        # Calculate Alpha for interpolation
        alpha = (time.time() - last_phys_time) / (1/60.0)
        
        # Render (GIL Released during draw)
        render_system.update(dt, alpha)
        
        glfw.swap_buffers(window)

# Start Physics on Worker
t = threading.Thread(target=physics_loop)
t.start()

# Run Render on Main
render_loop(...)
```

---

## 6. Handling "Broken" Transforms

Sometimes you need to teleport an object (e.g., respawn).

1.  **Physics Side:** `world.set_position(handle, (0, 10, 0))`
2.  **Render Side:** Do nothing.
    *   The `get_render_state()` call automatically reads the *new* position from the physics engine.
    *   The Instance Buffer is updated automatically next frame.

**You never need to manually sync positions in Python.** The physics engine is the "Source of Truth," and the GPU is the direct consumer.