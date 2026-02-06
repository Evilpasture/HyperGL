# ECS Philosophy: The Source of Truth

When integrating Physics (Culverin) and Graphics (HyperGL) into an Entity Component System (ECS), a common question arises:

> **"Who owns the Position?"**

*   Does the ECS `Transform` component own it?
*   Does the Physics Engine own it?
*   Does the Render Scene Graph own it?

If you try to sync all three every frame in Python, your performance will die.

## 1. The Hierarchy of Truth

In the HyperGL/Culverin stack, we define Truth based on **Authority**:

| Layer | Type of Truth | Authority Level | Role |
| :--- | :--- | :--- | :--- |
| **Physics (Culverin)** | **Absolute Truth** | 🥇 High | Determines where objects *actually* are. Handles collisions and constraints. |
| **ECS (Python)** | **Logical Truth** | 🥈 Medium | Stores "Intent" (e.g., "Player wants to move left"). Only overrides Physics on events (Teleport/Spawn). |
| **Graphics (HyperGL)** | **Perceived Truth** | 🥉 Low | Displays the *Interpolated* past state. Never writes back to Physics or ECS. |

---

## 2. The "Read-Only" Render Loop

The Render System should treat the Physics World as a **Read-Only Database**.

**❌ The Wrong Way (Two-Way Sync):**
```python
# SLOW & BUGGY
for entity in entities:
    # 1. Read Physics
    pos = physics.get_pos(entity.body)
    # 2. Write ECS
    entity.transform.pos = pos
    # 3. Read ECS
    render.draw(entity.transform.pos)
```
*Why it fails:* You iterate in Python O(N) times. You bloat the ECS with data the logic layer rarely checks.

**✅ The Right Way (Direct Pipe):**
```python
# FAST & CLEAN
# 1. Physics Engine updates internal arrays.
# 2. Render System grabs the entire array buffer.
# 3. GPU draws.
render_data = physics.get_render_state(alpha)
instance_buffer.write(render_data)
```
*Why it works:* The ECS `Transform` component is effectively bypassed for rendering. The GPU talks directly to the Physics memory.

---

## 3. When the ECS takes Control (The "Teleport" Event)

The ECS `Transform` component is usually **stale** (out of date). It does not know where the body is every frame. 

It only asserts authority when **Game Logic** dictates a discontinuity (Teleportation).

```python
class PlayerSystem:
    def on_respawn(self, entity, spawn_point):
        # 1. LOGIC: Assert new truth
        entity.transform.pos = spawn_point
        
        # 2. PHYSICS: Override simulation
        # "I am the Captain now."
        world.set_transform(entity.body, spawn_point, (0,0,0,1))
        
        # 3. PHYSICS: Reset velocities to prevent "pop"
        world.set_linear_velocity(entity.body, 0, 0, 0)
```

**Rule:** Logic writes to Physics *only* on events. Physics writes to Graphics *every* frame.

---

## 4. Interpolation vs. Simulation

**Visual Truth** (what the user sees) is distinct from **Simulation Truth** (where the object is).

*   **Simulation:** Runs at fixed 60Hz. Steps 100 to 101.
*   **Visual:** Runs at Monitor Hz (144Hz, etc.). Steps 100.2, 100.8...

HyperGL renders an **Interpolated State**.
*   It blends `Previous_Physics_State` and `Current_Physics_State`.
*   Therefore, **what you see on screen is slightly in the past (1 frame latency).**

This is standard in all major engines (Unreal, Unity, Godot). Do not try to fight it by forcing the render transform to match the current physics step exactly, or you will get micro-stutter.

## 5. Summary Diagram

```text
[ Game Logic / AI ]
       |
       | (Apply Forces / Teleport)
       v
[ Physics Simulation (Culverin) ] <--- THE TRUTH (Master Copy)
       |
       | (Bulk Copy via get_render_state)
       v
[ GPU Memory (HyperGL) ]
       |
       v
    [ Screen ]
```

The ECS `Transform` component is merely a **Cache** or a **staging area** for initial setup, not the runtime source of truth.