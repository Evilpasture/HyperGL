import hypergl
import numpy as np
import time

# Optional: import torch
# import torch

def main():
    # 1. Initialize Headless (No Window Manager needed)
    hypergl.init(headless=True)
    ctx = hypergl.context()

    width, height = 84, 84 # Standard Atari/RL resolution
    
    # 2. Setup Resources
    # We use a Renderbuffer because we don't need to sample it, just read it.
    # It's faster for writing.
    target = ctx.image((width, height), format='rgba8unorm', texture=False)
    
    # Create a pipeline (simple triangle for demo)
    # In a real RL scenario, this would be your game environment render
    pipeline = ctx.pipeline(
        vertex_shader="""
            #version 330 core
            void main() {
                float x = float((gl_VertexID & 1) << 2);
                float y = float((gl_VertexID & 2) << 1);
                gl_Position = vec4(x - 1.0, y - 1.0, 0.0, 1.0);
            }
        """,
        fragment_shader="""
            #version 330 core
            layout(location=0) out vec4 color;
            uniform float u_time;
            void main() {
                color = vec4(sin(u_time), cos(u_time), 0.5, 1.0);
            }
        """,
        framebuffer=[target],
        vertex_count=3,
        uniforms={'u_time': 0.0}
    )

    # 3. Allocate Host Memory (NumPy)
    # We allocate this ONCE. We do not re-allocate per frame.
    # 4 channels * 1 byte (uint8)
    observation_buffer = np.zeros((height, width, 4), dtype=np.uint8)

    # If using PyTorch, you can wrap the numpy array or use a byte tensor
    # tensor_view = torch.from_numpy(observation_buffer)

    print(f"Starting Training Loop ({width}x{height})...")
    
    start = time.time()
    frames = 0
    
    # 4. The Loop
    for i in range(10000):
        # Update State (Uniforms)
        pipeline.uniforms['u_time'][:] = struct.pack('f', i * 0.01)

        # Render
        ctx.new_frame(clear=True) # Clears bound framebuffer
        pipeline.render()
        ctx.end_frame()

        # 5. Fast Readback
        # We pass the numpy array directly to .read()
        # HyperGL detects the buffer interface and writes directly into it.
        target.read(into=observation_buffer)

        # At this point, 'observation_buffer' (and 'tensor_view') 
        # contains the frame data. No return value, no extra copies.
        
        # RL Agent Logic would go here...
        # action = agent.predict(observation_buffer)
        
        frames += 1

    end = time.time()
    fps = frames / (end - start)
    print(f"Finished. Average FPS: {fps:.2f} (Render + CPU Readback)")
    
    # Verify data exists
    print(f"Sample pixel: {observation_buffer[42, 42]}")

    ctx.release('all')

# Small helper for the uniform packing in this specific example
import struct

if __name__ == "__main__":
    main()