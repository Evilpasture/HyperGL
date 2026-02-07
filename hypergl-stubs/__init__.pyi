# MIT License
# Copyright (c) 2024 Szabolcs Dombi

# Stub file for HyperGL with rich docstrings.

from typing import Any, Dict, Iterable, List, Literal, Protocol, Tuple, TypedDict, Optional, Union, Iterator, Callable, TypeVar, ParamSpec

# --- Enums and Literals ---

CullFace = Literal['front', 'back', 'front_and_back', 'none']
"""Face culling modes."""

Topology = Literal['points', 'lines', 'line_loop', 'line_strip', 'triangles', 'triangle_strip', 'triangle_fan']
"""Primitive rendering topology."""

MinFilter = Literal[
    'nearest',
    'linear',
    'nearest_mipmap_nearest',
    'linear_mipmap_nearest',
    'nearest_mipmap_linear',
    'linear_mipmap_linear',
]
"""Texture minification filters."""

MagFilter = Literal['nearest', 'linear']
"""Texture magnification filters."""

TextureWrap = Literal['repeat', 'clamp_to_edge', 'mirrored_repeat']
"""Texture wrapping modes."""

CompareMode = Literal['ref_to_texture', 'none']
"""Texture comparison mode for depth textures."""

CompareFunc = Literal['never', 'less', 'equal', 'lequal', 'greater', 'notequal', 'gequal', 'always']
"""Comparison functions for depth/stencil tests."""

StencilOp = Literal['zero', 'keep', 'replace', 'incr', 'decr', 'invert', 'incr_wrap', 'decr_wrap']
"""Stencil buffer update operations."""

Step = Literal['vertex', 'instance']
"""Vertex attribute step rate."""

BlendConstant = Literal[
    'zero',
    'one',
    'src_color',
    'one_minus_src_color',
    'src_alpha',
    'one_minus_src_alpha',
    'dst_alpha',
    'one_minus_dst_alpha',
    'dst_color',
    'one_minus_dst_color',
    'src_alpha_saturate',
    'constant_color',
    'one_minus_constant_color',
    'constant_alpha',
    'one_minus_constant_alpha',
    'src1_alpha',
    'src1_color',
    'one_minus_src1_color',
    'one_minus_src1_alpha',
]
"""Blending factors."""

BlendFunc = Literal[
    'add',
    'subtract',
    'reverse_subtract',
    'min',
    'max',
]
"""Blending equations."""

VertexFormatShort = Literal[
    '2u1', '4u1', '2i1', '4i1', '2nu1', '4nu1', '2ni1', '4ni1',
    '2u2', '4u2', '2i2', '4i2', '2nu2', '4nu2', '2ni2', '4ni2',
    '2h', '4h', '1f', '2f', '3f', '4f',
    '1u', '2u', '3u', '4u', '1i', '2i', '3i', '4i',
]
"""Shorthand notation for vertex attribute formats."""

VertexFormat = Literal[
    'uint8x2', 'uint8x4', 'sint8x2', 'sint8x4',
    'unorm8x2', 'unorm8x4', 'snorm8x2', 'snorm8x4',
    'uint16x2', 'uint16x4', 'sint16x2', 'sint16x4',
    'unorm16x2', 'unorm16x4', 'snorm16x2', 'snorm16x4',
    'float16x2', 'float16x4',
    'float32', 'float32x2', 'float32x3', 'float32x4',
    'uint32', 'uint32x2', 'uint32x3', 'uint32x4',
    'sint32', 'sint32x2', 'sint32x3', 'sint32x4',
]
"""Explicit vertex attribute formats."""

ImageFormat = Literal[
    'r8unorm', 'rg8unorm', 'rgba8unorm',
    'r8snorm', 'rg8snorm', 'rgba8snorm',
    'r8uint', 'rg8uint', 'rgba8uint',
    'r16uint', 'rg16uint', 'rgba16uint',
    'r32uint', 'rg32uint', 'rgba32uint',
    'r8sint', 'rg8sint', 'rgba8sint',
    'r16sint', 'rg16sint', 'rgba16sint',
    'r32sint', 'rg32sint', 'rgba32sint',
    'r16float', 'rg16float', 'rgba16float',
    'r32float', 'rg32float', 'rgba32float',
    'depth16unorm', 'depth24plus', 'depth24plus-stencil8', 'depth32float',
]
"""Pixel formats for Textures and Renderbuffers."""

BufferAccess = Literal[
    'stream_draw', 'stream_read', 'stream_copy',
    'static_draw', 'static_read', 'static_copy',
    'dynamic_draw', 'dynamic_read', 'dynamic_copy',
]
"""Usage hints for Buffer memory allocation."""

UniformType = Literal['float', 'int', 'uint']

SYNC_GPU_COMMANDS_COMPLETE: int
SYNC_FLUSH_COMMANDS_BIT: int
ALREADY_SIGNALED: int
TIMEOUT_EXPIRED: int
CONDITION_SATISFIED: int
WAIT_FAILED: int
TIMEOUT_IGNORED: int

# --- Helper Classes & Types ---

class BufferView:
    """
    A lightweight view into a subsection of a Buffer.
    Created via Buffer.view().
    """
    pass

# Common Aliases
Vec3 = Tuple[float, float, float]
Viewport = Tuple[int, int, int, int]
Data = bytes | bytearray | memoryview | BufferView | Any
UniformMap = Dict[str, Any]

class LayoutBinding(TypedDict, total=False):
    """Defines a binding index for a named resource in a shader."""
    name: str
    binding: int

class BufferResource(TypedDict, total=False):
    """Descriptor for binding a Uniform Buffer."""
    type: Literal['uniform_buffer']
    binding: int
    buffer: 'Buffer'
    offset: int
    size: int

class StorageBufferResource(TypedDict, total=False):
    """Descriptor for binding a Shader Storage Buffer."""
    type: Literal['storage_buffer']
    binding: int
    buffer: 'Buffer'
    offset: int
    size: int

class SamplerResource(TypedDict, total=False):
    """Descriptor for binding a Texture/Sampler pair."""
    type: Literal['sampler']
    binding: int
    image: 'Image'
    min_filter: MinFilter
    mag_filter: MagFilter
    min_lod: float
    max_lod: float
    lod_bias: float
    wrap_x: TextureWrap
    wrap_y: TextureWrap
    wrap_z: TextureWrap
    compare_mode: CompareMode
    compare_func: CompareFunc
    max_anisotropy: float

# Resource Aliases
PipelineResource = BufferResource | SamplerResource
ComputeResource = BufferResource | SamplerResource | StorageBufferResource
FramebufferAttachment = Union['Image', 'ImageFace']

# Object Group Aliases
Releasable = Union['Buffer', 'Image', 'Pipeline', 'Compute', 'Fence']
Inspectable = Union['Buffer', 'Image', 'Pipeline', 'Compute']

class VertexBufferBinding(TypedDict, total=False):
    """Configuration for a vertex buffer attribute."""
    buffer: 'Buffer'
    format: VertexFormat
    location: int
    offset: int
    stride: int
    step: Step

class DepthSettings(TypedDict, total=False):
    """Depth test configuration."""
    write: bool
    func: CompareFunc

class StencilFaceSettings(TypedDict, total=False):
    """Stencil operation configuration for a specific face."""
    fail_op: StencilOp
    pass_op: StencilOp
    depth_fail_op: StencilOp
    compare_op: CompareFunc
    compare_mask: int
    write_mask: int
    reference: int

class StencilSettings(TypedDict, total=False):
    """Stencil test configuration."""
    front: StencilFaceSettings
    back: StencilFaceSettings
    both: StencilFaceSettings

class BlendSettings(TypedDict, total=False):
    """Color blending configuration."""
    enable: bool | int
    src_color: BlendConstant
    dst_color: BlendConstant
    src_alpha: BlendConstant
    dst_alpha: BlendConstant
    op_color: BlendFunc
    op_alpha: BlendFunc

class Info(TypedDict):
    """Information about the OpenGL Context and driver limits."""
    vendor: str
    renderer: str
    version: str
    glsl: str
    max_uniform_buffer_bindings: int
    max_uniform_block_size: int
    max_combined_uniform_blocks: int
    max_combined_texture_image_units: int
    max_vertex_attribs: int
    max_draw_buffers: int
    max_samples: int
    max_shader_storage_buffer_bindings: int

class ContextLoader(Protocol):
    """Callback protocol for loading OpenGL function pointers."""
    def load_opengl_function(self, name: str) -> int: ...

# --- Exceptions ---

class Error(Exception):
    """
    Base exception class for all HyperGL-specific errors.
    Used for VM failures (stack overflow, invalid opcodes), context losses, 
    and resource allocation errors.
    """
    pass

# --- Internal State Objects ---

class DescriptorSet:
    """
    A baked set of resource bindings (Uniform Buffers, Storage Buffers, and Samplers).
    Internal object used by Pipelines and CommandBuffers to swap materials 
    without changing the underlying shader program.
    """
    pass

class GlobalSettings:
    """
    A baked set of global OpenGL states (Depth, Stencil, Blending, Culling).
    Internal object used to minimize redundant state changes in the driver.
    """
    pass

# --- Main Objects ---

class ImageFace:
    """
    Represents a specific layer or mip-level of an Image (Texture).
    This object is used to attach specific faces to Framebuffers.
    """
    image: 'Image'
    size: Tuple[int, int]
    samples: int
    color: bool

    def clear(self) -> None:
        """Clear the content of this specific face/level based on the Image's clear_value."""
        ...

    def blit(
        self,
        target: 'ImageFace',
        offset: Tuple[int, int] | None = None,
        size: Tuple[int, int] | None = None,
        crop: Viewport | None = None,
        filter: bool = False,
    ) -> None:
        """
        Copy a region of pixels from this face to another.
        
        Args:
            target: The destination ImageFace.
            offset: (x, y) destination offset.
            size: (width, height) destination size.
            crop: (x, y, w, h) source region to copy.
            filter: If True, uses GL_LINEAR interpolation; otherwise GL_NEAREST.
        """
        ...

class Fence:
    """
    Represents an OpenGL Sync Object (Fence).
    Used to synchronize CPU-GPU execution for AZDO (Persistent Mapping).
    """
    def wait(self, flags: int = 0, timeout: int = TIMEOUT_IGNORED) -> int:
        """
        Block the CPU until the GPU reaches this fence.
        Releases the GIL while waiting.
        
        Returns:
            One of: ALREADY_SIGNALED, TIMEOUT_EXPIRED, CONDITION_SATISFIED, WAIT_FAILED.
        """
        ...

    def wait_gpu(self) -> None:
        """
        Instructs the GPU to wait for this fence before executing subsequent 
        commands in the queue. Does not block the CPU.
        """
        ...

    @property
    def signaled(self) -> bool:
        """True if the fence has been signaled by the GPU."""
        ...


# --- High-Level Compiler Types ---

class SceneCompiler:
    """
    High-level compiler for the Command Buffer VM.
    Translates Python context managers into GOTO, LABEL, and JUMP bytecode.
    """
    def __init__(self, cb: 'CommandBuffer') -> None: ...

    def loop(self, reg: int, count: Optional[int] = None) -> Iterator[None]:
        """
        Bytecode Construct: FOR LOOP
        Repeats the block 'count' times using register i[reg] as the counter.
        
        Example:
            with sc.loop(reg=0, count=100):
                cb.draw()
        """
        ...

    def condition(self, buffer: 'Buffer', offset: int, invert: bool = False) -> Iterator[None]:
        """
        Bytecode Construct: IF (Memory-based)
        Skips the block if a uint32 value in GPU memory is 0 (or not 0).
        
        Args:
            buffer: A MAPPED SSBO buffer.
            offset: Byte offset to a uint32 value.
            invert: If True, skips if value is NOT zero.
        """
        ...

    def reg_condition(self, reg: int, invert: bool = False) -> Iterator[None]:
        """
        Bytecode Construct: IF (Register-based)
        Skips the block if internal VM register i[reg] is 0 (or not 0).
        Useful for branching after ALU operations.
        
        Example:
            cmd.alu(0, 1, 'sub')
            with sc.reg_condition(reg=0):
                cmd.print("Values are equal")
        """
        ...
    def scope(self, registers: List[int]) -> Iterator[None]:
        """
        High-level Construct: REGISTER SCOPE
        
        Automatically PUSHes the provided list of registers onto the stack 
        before entering the block, and POPs them in reverse order upon exiting.
        
        This is the standard way to ensure a subroutine doesn't accidentally 
        overwrite registers used by the caller.
        
        Example:
            with sc.scope([0, 1, 2]):
                # Use i0, i1, i2 freely here
                cmd.set_iter(0, 100)
            # i0, i1, i2 are now back to their original values
        """
        ...

    def while_not_zero(self, buffer: 'Buffer', offset: int) -> Iterator[None]:
        """
        Bytecode Construct: WHILE (Memory-based)
        Repeats the block as long as the uint32 value in GPU memory is not 0.
        """
        ...

    def while_reg_not_zero(self, reg: int) -> Iterator[None]:
        """
        Bytecode Construct: WHILE (Register-based)
        Repeats the block as long as the internal VM register i[reg] is not 0.
        """
        ...

    def switch(self, reg: int, cases: int) -> Iterator[List[str]]:
        """
        Bytecode Construct: SWITCH/CASE
        Emits an O(1) jump table based on i[reg]. 
        Yields a list of labels the user must define for each case.
        """
        ...

P = ParamSpec("P")
R = TypeVar("R")

def subroutine(func: Callable[P, R]) -> Callable[P, R]:
    """
    Decorator for Command Buffer Subroutines.
    
    Records the decorated function into a persistent child CommandBuffer on first use.
    Subsequent calls from a main buffer emit a high-speed C-level 'CMD_CALL'.
    
    Features:
        - Context Isolation: Automatically caches separate buffers per OpenGL context.
        - Hardware Inheritance: Child buffers inherit parent Pipeline/Compute state.
        - Register Persistence: i0-i7 registers are shared between caller and subroutine.
    
    Note:
        The decorated function must accept a CommandBuffer as its first argument.
    """
    ...

class CommandBuffer:
    """
    A high-performance C-side bytecode Virtual Machine for OpenGL commands.
    
    Command Buffers allow you to 'record' a sequence of rendering and compute instructions
    once and 'replay' them at C-speed. This bypasses Python interpreter overhead, 
    eliminates the "boundary tax" of calling C-functions from Python loops, and 
    enables Zero-GIL rendering.

    Lifecycle:
        1. begin(): Resets the buffer and starts recording.
        2. Record instructions (draw, dispatch, skip, etc.)
        3. end(): Finalizes the buffer and patches labels/jumps.
        4. submit(): Executes the sequence on the GPU thread.
    """

    def begin(self) -> None:
        """
        Resets the command buffer to an empty state and opens it for recording.
        All previously recorded instructions and labels are cleared.
        """
        ...

    def end(self) -> None:
        """
        Finalizes the recording. This phase resolves all 'label' offsets and 
        'goto' targets. Once closed, instructions cannot be added until begin() 
        is called again.
        """
        ...

    def nop(self, size: int = 8) -> None:
        """
        Bytecode Instruction: NOP
        A no-operation instruction. Can be used to reserve space in the buffer.
        """
        ...

    def erase(self, offset: int) -> None:
        """
        Overwrites the instruction at 'offset' with a NOP.
        This effectively 'mutes' the instruction without changing the buffer size 
        or breaking label offsets. Use with tell() to find offsets.
        """
        ...

    def submit(self, budget: int = 1_000_000, trace: bool = False) -> int:
        """
        Executes the recorded bytecode sequence in a tight C loop.
        
        This method releases the Global Interpreter Lock (GIL), allowing other 
        Python threads to run in parallel. It checks for Python signals (Ctrl+C) 
        every 1024 instructions to ensure infinite loops can be interrupted.

        Args:
            budget: Maximum number of VM instructions allowed per execution. 
                    Prevents runaway 'while' loops from hanging the thread.
                    Default is 1,000,000.
            trace: If True, prints execution state to stderr (slow, use for debug).

        Returns:
            The total number of instructions executed (including subroutines).

        Raises:
            Error: If a VM error occurs 
                   (e.g., stack overflow, recursion limit, budget exceeded).

        Note:
            Must be called on the thread that owns the OpenGL context.
        """
        ...

    def bind_pipeline(self, pipeline: 'Pipeline') -> None:
        """
        Bytecode Instruction: BIND_PIPELINE
        
        Binds a graphics pipeline state (Shaders, VAO, FBO, Blend/Depth state).
        Subsequent draw calls will inherit this state.
        """
        ...

    def bind_compute(self, compute: 'Compute') -> None:
        """
        Bytecode Instruction: BIND_COMPUTE
        
        Binds a compute pipeline. Subsequent dispatch calls will use this state.
        """
        ...

    def bind_descriptor_set(self, descriptor_set: DescriptorSet) -> None:
        """
        Bytecode Instruction: BIND_DESCRIPTOR_SET
        
        Fast-swaps resource bindings (Textures, UBOs, SSBOs) without changing 
        the active pipeline. This is the optimal path for material swapping.
        """
        ...

    def clear(self, mask: int = 0x4100 | 0x0400) -> None:
        """
        Bytecode Instruction: CLEAR
        
        Clears the currently bound framebuffer using the specified bitmask.
        Default is Color + Depth + Stencil.
        """
        ...

    def draw(self, vertex_count: int = -1, instance_count: int = -1, first: int = -1) -> None:
        """
        Bytecode Instruction: DRAW
        
        Issues a draw call (Arrays or Elements depending on pipeline). 
        Arguments set to -1 will inherit the default values defined in the Pipeline.
        """
        ...

    def draw_indirect(self, buffer: 'Buffer', count: int = 1, offset: int = 0, stride: int = 0) -> None:
        """
        Bytecode Instruction: DRAW_INDIRECT
        
        Issues a Multi-Draw Indirect call. Automatically issues a memory barrier 
        to ensure preceding Compute Shader writes to the indirect buffer are visible.
        """
        ...

    def dispatch(self, x: int, y: int, z: int) -> None:
        """
        Bytecode Instruction: DISPATCH
        
        Dispatches a compute shader with the given workgroup dimensions.
        """
        ...

    def barrier(self, flags: int = 0xFFFFFFFF) -> None:
        """
        Bytecode Instruction: BARRIER
        
        Issues a glMemoryBarrier. Essential for synchronizing memory between 
        Compute passes and Fragment/Vertex passes.
        """
        ...

    def skip_if_zero(self, buffer: 'Buffer', offset: int) -> None:
        """
        Bytecode Instruction: SKIP_IF_ZERO
        
        Peeks at a uint32 in a MAPPED buffer. If the value is 0, the VM jumps 
        over the instruction immediately following this one.
        
        Used for GPU-driven occlusion culling and LOD branching.
        """
        ...

    def skip_if_not_zero(self, buffer: 'Buffer', offset: int) -> None:
        """
        Bytecode Instruction: SKIP_IF_NOT_ZERO
        
        Peeks at a uint32 in a MAPPED buffer. If the value is not 0, the VM jumps 
        over the instruction immediately following this one.
        """
        ...

    def label(self, name: str) -> None:
        """
        Records a named marker at the current write position. 
        Labels are used as targets for 'goto' instructions.
        """
        ...

    def goto(self, target: Union[str, int]) -> None:
        """
        Bytecode Instruction: GOTO
        
        Unconditionally jumps the VM instruction pointer to a named label (str) 
        or an absolute byte offset (int).
        """
        ...

    def tell(self) -> int:
        """
        Returns the current write cursor position (byte offset) in the buffer.
        """
        return 0

    def print(self, message: str, buffer: Optional['Buffer'] = None, offset: int = 0) -> None:
        """
        Bytecode Instruction: PRINT
        
        Outputs a message to stderr from the C-loop during execution.
        If a mapped buffer is provided, it also peeks and prints the uint32 at 'offset'.
        """
        ...

    def dump(self, buffer: 'Buffer', offset: int = 0, count: int = 1, type: Literal['float', 'int', 'uint'] = 'float', message: str = "DUMP") -> None:
        """
        Bytecode Instruction: DUMP
        
        Formats and prints a range of memory from a mapped buffer to stderr.
        Interpretation of data is controlled by the 'type' parameter.
        """
        ...

    def call(self, other: 'CommandBuffer') -> None:
        """
        Bytecode Instruction: CALL
        
        Executes another CommandBuffer as a subroutine. The child buffer 
        inherits the currently bound Pipeline and state from the caller.
        
        Note:
            Maximum recursion depth is 16.
        """
        ...
    
    def signal(self, fence: Fence) -> None:
        """
        Bytecode Instruction: SIGNAL_FENCE
        
        Updates the provided Fence object with a new sync point when reached 
        in the execution sequence.
        """
        ...

    def wait(self, fence: Fence) -> None:
        """
        Bytecode Instruction: WAIT_FENCE
        Instructs the GPU to wait for the fence before proceeding. 
        Does not block the CPU. Used for cross-buffer sync.
        """
        ...

    def skip_if_not_ready(self, fence: Fence) -> None:
        """
        Bytecode Instruction: SKIP_IF_NOT_READY
        Polls the fence (non-blocking). If the GPU has not reached the 
        sync point, the instruction immediately following this one is skipped.
        """
        ...

    def set_iter(self, reg: int, value: int) -> None:
        """Sets internal VM register i[reg] to value. (reg: 0-7)"""
        ...

    def jump_iter(self, reg: int, target: Union[str, int]) -> None:
        """
        Decrements i[reg]. If the result is greater than 0, 
        jumps to the target label or offset.
        """
        ...

    def store_reg(self, reg: int, buffer: 'Buffer', offset: int) -> None:
        """
        Bytecode Instruction: STORE_REG
        Writes the current value of VM register i[reg] into the MAPPED buffer 
        at the specified byte offset.
        """
        ...
    def load_reg(self, reg: int, buffer: 'Buffer', offset: int) -> None:
        """Reads a uint32 from a mapped buffer into register i[reg]."""
        ...

    def alu(self, reg_a: int, reg_b: int, op: Literal['add', 'sub', 'mul', 'div', 'and', 'or', "xor", "lsh", "rsh", "not"]) -> None:
        """Performs i[reg_a] = i[reg_a] OP i[reg_b]."""
        ...

    def ret(self) -> None:
        """Bytecode Instruction: RET. Exits current buffer/subroutine."""
        ...

    def ret_if_zero(self, buffer: 'Buffer', offset: int) -> None:
        """Bytecode Instruction: RET_IF_ZERO. Exits current buffer if memory is 0."""
        ...

    def ret_if_not_zero(self, buffer: 'Buffer', offset: int) -> None:
        """Bytecode Instruction: RET_IF_NOT_ZERO. Exits current buffer if memory is not 0."""
        ...

    def skip_reg_zero(self, reg: int) -> None:
        """
        Bytecode Instruction: SKIP_REG_ZERO
        
        Peeks at internal VM register i[reg]. If the value is 0, the VM jumps 
        over the instruction immediately following this one.
        
        This allows for branching based on C-side ALU calculations.
        """
        ...

    def skip_reg_not_zero(self, reg: int) -> None:
        """
        Bytecode Instruction: SKIP_REG_NOT_ZERO
        
        Peeks at internal VM register i[reg]. If the value is not 0, the VM jumps 
        over the instruction immediately following this one.
        """
        ...

    def load_reg_indirect(self, reg: int, buffer: 'Buffer', index_reg: int, base_offset: int = 0, stride: int = 4) -> None:
        """
        Bytecode Instruction: LOAD_REG_INDIRECT
        
        Performs an array-style read from a MAPPED buffer into register i[reg].
        Calculation: i[reg] = buffer[base_offset + (i[index_reg] * stride)]
        
        Allows the VM to iterate through arrays of data (e.g. visibility bits) in a loop.
        """
        ...

    def store_reg_indirect(self, reg: int, buffer: 'Buffer', index_reg: int, base_offset: int = 0, stride: int = 4) -> None:
        """
        Bytecode Instruction: STORE_REG_INDIRECT
        
        Performs an array-style write from register i[reg] into a MAPPED buffer.
        Calculation: buffer[base_offset + (i[index_reg] * stride)] = i[reg]
        
        Allows the VM to write back results of calculations to GPU memory (e.g. patching MDI counts).
        """
        ...

    def serialize(self) -> Tuple[bytes, List[Any]]:
        """
        Serializes the bytecode and object references into a relocatable format.
        Returns: (bytecode_bytes, symbol_table_list)
        """
        ...

    def patch(self, bytecode: bytes, symbol_table: List[Any]) -> None:
        """
        Inflates a serialized bytecode block by mapping symbols back to live pointers.
        Automatically closes the buffer for submission.
        """
        ...

    def push(self, reg: int) -> None:
        """
        Bytecode Instruction: PUSH
        
        Saves the current value of VM register i[reg] onto the internal VM stack.
        The stack is shared across all subroutine calls (CALL).
        
        Note:
            The VM stack has a fixed capacity of 128 entries. Exceeding this 
            limit will result in undefined behavior or a VM abort.
            
        Example:
            cmd.push(0) # Backup register 0
        """
        ...

    def pop(self, reg: int) -> None:
        """
        Bytecode Instruction: POP
        
        Removes the top value from the VM stack and restores it into register i[reg].
        Registers should be popped in the exact reverse order they were pushed.
        
        Example:
            cmd.pop(0) # Restore register 0
        """
        ...

    def draw_indirect_count(self, buffer: 'Buffer', count_buffer: 'Buffer', offset: int = 0, count_offset: int = 0, max_count: int = 1, stride: int = 0) -> None:
        """
        Bytecode Instruction: DRAW_INDIRECT_COUNT
        Draws up to 'max_count' objects, reading the actual count from 'count_buffer'.
        Enables 100% GPU-driven rendering.
        """
        ...

    def set_uniform(self, location: int, reg: int, type: UniformType = 'float') -> None:
        """
        Bytecode Instruction: SET_UNIFORM
        Injects the current 32-bit value of internal VM register i[reg] directly 
        into a shader uniform location.
        """
        ...

    def jump_table(self, reg: int, targets: List[Union[str, int]]) -> None:
        """
        Bytecode Instruction: JUMP_TABLE
        Performs an O(1) jump based on the value in i[reg].
        If i[reg] == 0, jump to targets[0], etc.
        """
        ...

    def assert_reg(self, reg: int, value: int, op: Literal['==', '!=', '<', '>', '<=', '>='] = '==') -> None:
        """
        Bytecode Instruction: ASSERT_REG
        Halts VM execution and raises hypergl.Error if the condition i[reg] OP value is false.
        """
        ...

class Buffer:
    """
    Represents an OpenGL Buffer Object (VBO, IBO, UBO, SSBO).
    """
    size: int

    def read(self, size: int | None = None, offset: int = 0, into: Any | None = None) -> bytes | None:
        """
        Download data from the GPU buffer.
        Note: This method is implicitly synchronous and will wait for the GPU to finish 
        all pending work before reading.
        
        Args:
            size: Number of bytes to read. Defaults to remainder of buffer.
            offset: Byte offset to start reading from.
            into: Optional writable buffer (bytearray, memoryview) to read directly into.
            
        Returns:
            bytes object if `into` is None, otherwise None.
        """
        ...

    def write(self, data: Data, offset: int = 0) -> None:
        """
        Upload data to the GPU buffer.
        
        Args:
            data: The bytes-like object to upload.
            offset: Byte offset in the GPU buffer to write to.
        """
        ...

    def view(self, size: int | None = None, offset: int = 0) -> BufferView:
        """
        Create a lightweight view of a subsection of this buffer.
        Useful for binding specific ranges of UBOs/SSBOs.
        """
        ...

    def map(self) -> memoryview:
        """
        Map the buffer into client memory. 
        Only supported for Storage Buffers (SSBOs).
        Returns a writable memoryview backed by the mapped pointer.
        """
        ...

    def unmap(self) -> None:
        """
        Unmap the buffer. Invalidate any existing memoryviews.
        """
        ...

    def bind(self, unit: int) -> None:
        """
        Bind this buffer to a specific SSBO binding point index.
        Only valid for buffers created with storage=True.
        """
        ...
    def write_texture_handle(self, offset: int, image: 'Image') -> None:
        """
        Write a 64-bit bindless texture handle into the buffer at the specific offset.
        Automatically retrieves or creates the handle from the Image object.
        """
        ...

class Image:
    """
    Represents an OpenGL Texture or Renderbuffer.
    """
    size: Tuple[int, int]
    format: ImageFormat
    samples: int
    array: int
    renderbuffer: bool
    clear_value: Iterable[int | float] | int | float

    def face(self, layer: int = 0, level: int = 0) -> ImageFace:
        """
        Get a specific face/layer/level of the image.
        
        Args:
            layer: The array layer or cubemap face index (0-5).
            level: The mipmap level.
        """
        ...

    def clear(self) -> None:
        """
        Clear the entire image (or all attached faces) using the set clear_value.
        """
        ...

    def write(
        self,
        data: Data,
        size: Tuple[int, int] | None = None,
        offset: Tuple[int, int] | None = None,
        layer: int | None = None,
        level: int = 0,
    ) -> None:
        """
        Upload pixel data to the image.
        
        Args:
            data: Bytes-like object containing pixel data.
            size: (width, height) of the update region.
            offset: (x, y) offset of the update region.
            layer: Specific layer/face to update (if array/cubemap).
            level: Mipmap level to update.
        """
        ...

    def mipmaps(self) -> None:
        """Generate mipmaps for the image (glGenerateMipmap)."""
        ...

    def read(self, size: Tuple[int, int] | None = None, offset: Tuple[int, int] | None = None, into: Any | None = None) -> bytes | None:
        """
        Read pixel data from the image.
        Note: This method is implicitly synchronous and will wait for the GPU to finish 
        all pending work before reading. Handles MSAA resolve automatically.
        
        Args:
            size: (width, height) to read.
            offset: (x, y) offset to read from.
            into: Optional buffer to write pixels into.
            
        Returns:
            bytes object if `into` is None.
        """
        ...
    
    def get_handle(self) -> int:
        """
        Get the 64-bit bindless texture handle (GL_ARB_bindless_texture).
        Creates the handle if it doesn't exist.
        """
        ...

    def make_resident(self, resident: bool = True) -> None:
        """
        Make the bindless texture handle resident (accessible by shaders) or non-resident.
        """
        ...

    def blit(
        self,
        target: Optional['Image'] = None,
        offset: Tuple[int, int] | None = None,
        size: Tuple[int, int] | None = None,
        crop: Viewport | None = None,
        filter: bool = False,
    ) -> None:
        """
        Blit the first layer/face of this image to a target.
        If target is None, blits to the default framebuffer (screen).
        """
        ...

class Pipeline:
    """
    An immutable object representing the entire graphics pipeline state.
    Includes Shaders, Layouts, Render State (Blend/Depth), and Resource Bindings.
    """
    vertex_count: int
    instance_count: int
    first_vertex: int
    viewport: Viewport
    uniforms: Dict[str, memoryview] | None
    descriptor_set: DescriptorSet
    """
    A dictionary proxy to the uniform memory. 
    Modify the memoryviews in this dict to update shader uniforms.
    """

    def render(self) -> None:
        """
        Execute the draw call (glDrawArrays or glDrawElements) using the encapsulated state.
        """
        ...

    def render_indirect(self, buffer: Buffer, count: int, offset: int = 0, stride: int = 0) -> None:
        """
        Execute an indirect draw call (glMultiDrawArraysIndirect or glMultiDrawElementsIndirect).
        
        Args:
            buffer: A Buffer object containing the draw commands.
            count: The number of draw commands to execute.
            offset: Byte offset into the buffer where commands start.
            stride: Byte stride between commands. 0 means tightly packed.
        """
        ...

class Compute:
    """
    Represents a Compute Shader pipeline.
    """
    uniforms: Dict[str, memoryview] | None

    def run(self, x: int = 1, y: int = 1, z: int = 1) -> None:
        """
        Dispatch the compute shader (glDispatchCompute).
        
        Args:
            x, y, z: Number of local workgroups in each dimension.
        """
        ...

class Context:
    """
    The main entry point for managing OpenGL state and creating resources.
    Supports high-performance AZDO workflows and multi-threaded context handover.
    """
    info: Info
    includes: Dict[str, str]
    screen: int
    loader: ContextLoader
    lost: bool

    def buffer(
        self,
        data: Data | None = None,
        size: int | None = None,
        access: BufferAccess | None = None,
        index: bool = False,
        uniform: bool = False,
        storage: bool = False,
        external: int = 0,
    ) -> Buffer:
        """
        Create a new Buffer Object.
        
        Args:
            data: Initial data to upload. Mutually exclusive with `size`.
            size: Size of the buffer in bytes.
            access: Usage hint (e.g., 'static_draw').
            index: Set True if this is an Element Array Buffer (indices).
            uniform: Set True if this is a Uniform Buffer Object (UBO).
            storage: Set True if this is a Shader Storage Buffer Object (SSBO).
            external: Wrap an existing OpenGL buffer ID.
        """
        ...

    def image(
        self,
        size: Tuple[int, int],
        format: ImageFormat = 'rgba8unorm',
        data: Data | None = None,
        samples: int = 1,
        array: int = 0,
        levels: int = 1,
        texture: bool | None = None,
        cubemap: bool = False,
        external: int = 0,
    ) -> Image:
        """
        Create a new Image (Texture or Renderbuffer).
        
        Args:
            size: (width, height).
            format: Internal pixel format.
            data: Initial pixel data.
            samples: Number of MSAA samples (1 = no MSAA).
            array: Number of layers (0 = not an array).
            levels: Number of mipmap levels.
            texture: If False, creates a Renderbuffer (optimized for FBO attachment).
            cubemap: Set True to create a Cube Map.
            external: Wrap an existing OpenGL texture ID.
        """
        ...

    def pipeline(
        self,
        vertex_shader: str | None = None,
        fragment_shader: str | None = None,
        layout: Iterable[LayoutBinding] = (),
        resources: Iterable[PipelineResource] = (),
        uniforms: UniformMap | None = None,
        depth: DepthSettings | None = None,
        stencil: StencilSettings | None = None,
        blend: BlendSettings | None = None,
        framebuffer: Iterable[FramebufferAttachment] | None = None,
        vertex_buffers: Iterable[VertexBufferBinding] = (),
        index_buffer: Buffer | None = None,
        short_index: bool = False,
        cull_face: CullFace = 'none',
        topology: Topology = 'triangles',
        vertex_count: int = 0,
        instance_count: int = 0,
        first_vertex: int = 0,
        viewport: Viewport | None = None,
        uniform_data: memoryview | None = None,
        viewport_data: memoryview | None = None,
        render_data: memoryview | None = None,
        includes: Dict[str, str] | None = None,
        template: Pipeline | None = None,
    ) -> Pipeline:
        """
        Create a graphics Pipeline state object.
        
        Args:
            vertex_shader: Source code for vertex shader.
            fragment_shader: Source code for fragment shader.
            layout: Layout overrides for shader attributes.
            resources: List of UBOs and Textures to bind.
            uniforms: Initial values for uniforms (dict).
            depth: Depth test settings.
            stencil: Stencil test settings.
            blend: Blending settings.
            framebuffer: List of images (attachments) to render into.
            vertex_buffers: Definitions of vertex attributes and buffer bindings.
            index_buffer: Buffer containing vertex indices.
            short_index: True for 16-bit indices, False for 32-bit.
            cull_face: Face culling mode.
            topology: Primitive type (e.g., 'triangles').
            vertex_count: Number of vertices to draw (if no index buffer).
            instance_count: Number of instances to draw.
            first_vertex: Index of the first vertex.
            viewport: (x, y, w, h) override.
            includes: Dict of include strings for the shader preprocessor.
            template: Create a new pipeline inheriting state from an existing one.
        """
        ...

    def compute(
        self,
        compute_shader: str | bytes,
        resources: Iterable[ComputeResource] = (),
        uniforms: UniformMap | None = None,
        uniform_data: memoryview | None = None,
    ) -> Compute:
        """
        Create a Compute Shader pipeline.
        
        Args:
            compute_shader: Source code for the compute shader.
            resources: Buffers (SSBO/UBO) and Images to bind.
            uniforms: Initial uniform values.
        """
        ...

    def new_frame(self, reset: bool = True, clear: bool = True) -> None:
        """
        Prepare the context for a new frame.
        Clears the default framebuffer and optionally resets internal state trackers.
        """
        ...

    def end_frame(self, clean: bool = True, flush: bool = True) -> None:
        """
        Finish the frame.
        Unbinds resources (clean) and flushes the command queue to the GPU.
        """
        ...

    def fence(self) -> Fence:
        """
        Insert a new sync fence into the OpenGL command stream.
        """
        ...

    def command_buffer(self) -> CommandBuffer:
        """
        Creates a new Command Buffer for recording.
        Recording can happen on any thread, but submission must happen on the render thread.
        """
        ...

    def release(self, obj: Releasable | Literal['shader_cache', 'all']) -> None:
        """
        Add an OpenGL object to the Shared Trash bin for safe deletion.
        
        This method is thread-safe. Resources are queued in a mutex-protected 
        buffer and physically deleted by the Render Thread during the next 
        call to new_frame() or end_frame().
        """
        ...


    def migrate(self) -> None:
        """
        Bind (Handover) the OpenGL context to the calling OS thread.
        
        This performs the low-level OS handshake (e.g., wglMakeCurrent) 
        required to move GPU rendering capability to a new thread.
        Ensure the previous owner has called release_thread() first.
        """
        ...

    def release_thread(self) -> None:
        """
        Unbind the OpenGL context from the current OS thread.
        
        Call this on the 'Main Thread' after initialization to allow 
         a dedicated 'Render Thread' to claim the context via migrate().
        """
        ...

    def gc(self) -> List[Union[Buffer, Image, Pipeline, Compute]]:
        """Trigger garbage collection of released GL objects."""
        ...

    # --- Indirect Drawing Helper ---
    def pack_indirect(self, commands: Iterable[Iterable[int]], indexed: bool = False) -> bytes:
        """
        Pack a list of draw commands into a binary bytes object suitable for an Indirect Draw Buffer.
        
        Args:
            commands: A list of tuples/lists.
                      If indexed=False: (count, instanceCount, first, baseInstance)
                      If indexed=True:  (count, instanceCount, firstIndex, baseVertex, baseInstance)
            indexed: Set True if packing commands for glMultiDrawElementsIndirect.
            
        Returns:
            A bytes object containing the tightly packed C-structs.
        """
        ...

    @property
    def stats(self) -> Dict[str, int]:
        """
        Returns a dictionary of real-time performance counters.
        Includes: draw_calls, pipeline_swaps, set_swaps, and dispatch_calls.
        """
        ...

    def reset_stats(self) -> None:
        """Resets all performance counters in ctx.stats to zero."""
        ...

    def finish(self) -> None:
        """
        Hard synchronization. Blocks the CPU until every single GPU command 
        currently in the queue is finished. Useful for benchmarks and tests.
        """
        ...

    def invalidate_state(self) -> None:
        """
        Forces the engine to forget all cached hardware state.
        Call this if you are using external raw OpenGL calls alongside HyperGL.
        """
        ...

# --- Module Level Functions ---

def init(loader: ContextLoader | None = None, headless: bool = False):
    """
    Initialize the HyperGL module.
    
    Args:
        loader: A custom loader.
        headless: If True, creates a hidden window/context (useful for servers/tests).
                  Ignored if loader is provided.
    """
    ...

def cleanup() -> None:
    """Force cleanup of the default context and release all resources."""
    ...

def context() -> Context:
    """Retrieve the singleton HyperGL Context."""
    ...

def inspect(obj: Inspectable) -> Dict[str, Any]:
    """
    Return a dictionary containing internal GL state/IDs of the object.
    Useful for debugging.
    """
    ...

def camera(
    eye: Vec3,
    target: Vec3 = (0.0, 0.0, 0.0),
    up: Vec3 = (0.0, 0.0, 1.0),
    fov: float = 45.0,
    aspect: float = 1.0,
    near: float = 0.1,
    far: float = 1000.0,
    size: float = 1.0,
    clip: bool = False,
) -> bytes:
    """
    Fast C implementation to calculate a Camera Matrix (View * Projection).
    
    Returns:
        64 bytes representing a 4x4 float32 matrix, ready for UBO upload.
    """
    ...

def bind(
    buffer: Buffer | None,
    layout: str,
    *attributes: int,
    offset: int = 0,
    instance: bool = False,
) -> List[VertexBufferBinding]:
    """
    Helper to create a VertexBufferBinding list for a Pipeline.
    
    Args:
        buffer: The vertex buffer.
        layout: A format string (e.g. '3f 2f') describing attributes.
        attributes: Attribute locations (0, 1, 2...).
        offset: Byte offset in the buffer.
        instance: If True, attributes advance per-instance.
    """
    ...

def calcsize(layout: str) -> int:
    """Calculate the byte size of a vertex layout string."""
    ...

def loader(headless: bool = False) -> ContextLoader:
    """
    Get a default platform-specific loader if available.
    """
    ...