# MIT License
# Copyright (c) 2024 Szabolcs Dombi

from typing import Any, Dict, Iterable, List, Literal, Protocol, Tuple, TypedDict

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

# --- Helper Classes & Types ---

class BufferView:
    """
    A lightweight view into a subsection of a Buffer.
    Created via Buffer.view().
    """
    pass

Vec3 = Tuple[float, float, float]
Viewport = Tuple[int, int, int, int]
Data = bytes | bytearray | memoryview | BufferView | Any

class LayoutBinding(TypedDict, total=False):
    """Defines a binding index for a named resource in a shader."""
    name: str
    binding: int

class BufferResource(TypedDict, total=False):
    """Descriptor for binding a Uniform Buffer."""
    type: Literal['uniform_buffer']
    binding: int
    buffer: Buffer
    offset: int
    size: int

class StorageBufferResource(TypedDict, total=False):
    """Descriptor for binding a Shader Storage Buffer."""
    type: Literal['storage_buffer']
    binding: int
    buffer: Buffer
    offset: int
    size: int

class SamplerResource(TypedDict, total=False):
    """Descriptor for binding a Texture/Sampler pair."""
    type: Literal['sampler']
    binding: int
    image: Image
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

class VertexBufferBinding(TypedDict, total=False):
    """Configuration for a vertex buffer attribute."""
    buffer: Buffer
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

class ContextLoader(Protocol):
    """Callback protocol for loading OpenGL function pointers."""
    def load_opengl_function(name: str) -> int: ...

# --- Main Objects ---

class ImageFace:
    """
    Represents a specific layer or mip-level of an Image (Texture).
    This object is used to attach specific faces to Framebuffers.
    """
    image: Image
    size: Tuple[int, int]
    samples: int
    color: bool

    def clear(self) -> None:
        """Clear the content of this specific face/level based on the Image's clear_value."""
        ...

    def blit(
        self,
        target: ImageFace,
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

class Buffer:
    """
    Represents an OpenGL Buffer Object (VBO, IBO, UBO, SSBO).
    """
    size: int

    def read(self, size: int | None = None, offset: int = 0, into=None) -> bytes:
        """
        Download data from the GPU buffer.
        
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
    def write_texture_handle(self, offset: int, image: Image) -> None:
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

    def read(self, size: Tuple[int, int] | None = None, offset: Tuple[int, int] | None = None, into=None) -> bytes:
        """
        Read pixel data from the image.
        
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
        target: Image | None = None,
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
        vertex_shader: str = ...,
        fragment_shader: str = ...,
        layout: Iterable[LayoutBinding] = (),
        resources: Iterable[BufferResource | SamplerResource] = (),
        uniforms: Dict[str, Any] | None = None,
        depth: DepthSettings | None = None,
        stencil: StencilSettings | None = None,
        blend: BlendSettings | None = None,
        framebuffer: Iterable[Image | ImageFace] | None = ...,
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
        template: Pipeline = ...,
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
        resources: Iterable[BufferResource | SamplerResource | StorageBufferResource] = (),
        uniforms: Dict[str, Any] | None = None,
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

    def release(self, obj: Buffer | Image | Pipeline | Compute | Literal['shader_cache'] | Literal['all']) -> None:
        """
        Manually release an OpenGL object or clear internal caches.
        This bypasses the garbage collector for immediate resource cleanup.
        """
        ...

    def migrate(self) -> None:
        """
        Migrate the Context to the current thread.
        Call this once at the start of your Render Thread loop if using multiple threads.
        """
        ...

    def gc(self) -> List[Buffer | Image | Pipeline | Compute]:
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

def inspect(self, obj: Buffer | Image | Pipeline | Compute):
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