#ifndef HYPERGL_DEFS_H
#define HYPERGL_DEFS_H

#include <Python.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define HGL_MAGIC 0x424C4748  // "HGLB" (HyperGL Binary) in Little Endian

// Current VM Specification version
#define HGL_ISA_MAJOR 1
#define HGL_ISA_MINOR 1

typedef struct HGLHeader {
    uint32_t magic;    // Must be HGL_MAGIC
    uint16_t major;    // Breaking changes
    uint16_t minor;    // New features (backward compatible)
    uint32_t data_size; // Size of bytecode following this header
    uint32_t _reserved; // Alignment padding
} HGLHeader;

// --- Compiler Hints ---
#if defined(__GNUC__) || defined(__clang__)
    #define LIKELY(x)   __builtin_expect(!!(x), 1)
    #define UNLIKELY(x) __builtin_expect(!!(x), 0)
    #define FORCE_INLINE __attribute__((always_inline)) inline
    #define NO_ALIAS    __restrict
    #define PURE_FUNC   __attribute__((pure))
    #define CONST_FUNC  __attribute__((const))
    #define UNUSED __attribute__((unused))
#else
    #define LIKELY(x)   (x)
    #define UNLIKELY(x) (x)
    #define FORCE_INLINE inline
    #define NO_ALIAS
    #define PURE_FUNC
    #define CONST_FUNC
    #define UNUSED
#endif

// --- Integer Limits ---
#ifdef UINT32_MAX
    #undef UINT32_MAX
    #define UINT32_MAX (4294967295U)
#endif

#ifdef INT32_MAX
    #undef INT32_MAX
    #define INT32_MAX 2147483647
#endif

#ifdef INT32_MIN
    #undef INT32_MIN
    #define INT32_MIN (-2147483647 - 1)
#endif

// --- Python 3.13+ Lock Shim ---
#ifndef Py_MOD_GIL_NOT_USED
    #define Py_MOD_GIL_NOT_USED 0
#endif

// Support for Python < 3.13 which doesn't have PyMutex
#if PY_VERSION_HEX < 0x030D0000
    typedef PyThread_type_lock PyMutex;
    // SHIM: Ensure this is called once during module initialization!
    static PyThread_type_lock global_init_lock = NULL; 
    
    static void PyMutex_Lock(PyMutex *m) {
        // If m is null, we need to create the lock. 
        // We assume global_init_lock was created in PyInit.
        if (*m == NULL) {
            PyThread_acquire_lock(global_init_lock, 1);
            if (*m == NULL) {
                *m = PyThread_allocate_lock();
            }
            PyThread_release_lock(global_init_lock);
        }
        PyThread_acquire_lock(*m, 1);
    }
    
    static void PyMutex_Unlock(PyMutex *m) {
        if (*m != NULL) {
            PyThread_release_lock(*m);
        }
    }
    
    // Shim for PyDict_GetItemRef (New in 3.13)
    static int PyDict_GetItemRef(PyObject *p, PyObject *key, PyObject **result) {
        PyObject *tmp = PyDict_GetItem(p, key); // Borrowed
        if (tmp) {
            Py_INCREF(tmp); // Now owned
            *result = tmp;
            return 1;
        }
        *result = NULL;
        return 0;
    }

    // Shim for PyDict_SetDefaultRef (New in 3.13)
    static int PyDict_SetDefaultRef(PyObject *d, PyObject *key, PyObject *default_value, PyObject **result) {
        PyObject *existing = PyDict_GetItem(d, key);
        if (existing) {
            *result = Py_NewRef(existing);
            return 1; // Key existed
        }
        if (PyDict_SetItem(d, key, default_value) < 0) {
            *result = NULL;
            return -1;
        }
        *result = Py_NewRef(default_value);
        return 0; // New key inserted
    }
#endif

// --- Version & Config ---
#ifndef HYPERGL_VERSION
#define HYPERGL_VERSION "0.0.0"
#endif
#define MAX_ATTACHMENTS 8
#define MIN_BUFFER_BINDINGS 8
#define MAX_BUFFER_BINDINGS 8
#define MAX_SAMPLER_BINDINGS 16

// --- Platform Specifics & Atomic Macros ---
#ifdef _WIN32
    #include <windows.h>
    #define GL_API __stdcall
    #define Atomic_Decrement(ptr) InterlockedDecrement((volatile LONG*)(ptr))
    #define Atomic_Increment(ptr) InterlockedIncrement((volatile LONG*)(ptr))
    #define Atomic_Load(ptr) InterlockedCompareExchange((volatile LONG*)(ptr), 0, 0)
#else
    #define GL_API
    #define Atomic_Decrement(ptr) __atomic_sub_fetch(ptr, 1, __ATOMIC_SEQ_CST)
    #define Atomic_Increment(ptr) __atomic_add_fetch(ptr, 1, __ATOMIC_SEQ_CST)
    #define Atomic_Load(ptr) __atomic_load_n(ptr, __ATOMIC_SEQ_CST)
#endif

// --- Macros ---
#ifdef DISABLE_LOCKS
    #undef PyMutex_Lock
    #undef PyMutex_Unlock
    #define PyMutex_Lock(m) ((void)0)
    #define PyMutex_Unlock(m) ((void)0)
#endif

#if ENABLE_VALIDATION
    #define VALIDATE(cond, exc, fmt, ...) \
        do { \
            if (!(cond)) { \
                PyErr_Format((exc), (fmt), ##__VA_ARGS__); \
                return NULL; \
            } \
        } while (0)

    #define VALIDATE_PTR(ptr, exc, fmt, ...) \
        do { \
            if (!(ptr)) { \
                if (!PyErr_Occurred()) PyErr_Format((exc), (fmt), ##__VA_ARGS__); \
                return NULL; \
            } \
        } while (0)

    #define VALIDATE_LOCKED(ctx, cond, exc, fmt, ...) \
        do { \
            if (!(cond)) { \
                PyMutex_Unlock(&(ctx)->state_lock); \
                PyErr_Format((exc), (fmt), ##__VA_ARGS__); \
                return NULL; \
            } \
        } while (0)
#else
    #define VALIDATE(cond, exc, fmt, ...) ((void)0)
    #define VALIDATE_PTR(ptr, exc, fmt, ...) ((void)0)
    #define VALIDATE_LOCKED(ctx, cond, exc, fmt, ...) ((void)0)
#endif

#define GL_STATE_UNKNOWN  (-1)
#define GL_STATE_DISABLED  0
#define GL_STATE_ENABLED   1

// OPTIMIZATION: Zero-cost state filtering. 
// NOTE: These macros MUST be called while self->state_lock is HELD.
#define LOCKED_GL_ENABLE_STATE(GL_ENUM, FIELD) \
    do { \
        if (self->gl_state.FIELD != GL_STATE_ENABLED) { \
            glEnable(GL_ENUM); \
            self->gl_state.FIELD = GL_STATE_ENABLED; \
        } \
    } while (0)

#define LOCKED_GL_DISABLE_STATE(GL_ENUM, FIELD) \
    do { \
        if (self->gl_state.FIELD != GL_STATE_DISABLED) { \
            glDisable(GL_ENUM); \
            self->gl_state.FIELD = GL_STATE_DISABLED; \
        } \
    } while (0)

// --- Struct Definitions ---

typedef unsigned long long GLuint64;
typedef struct HyperGLSync *GLsync;
typedef Py_ssize_t intptr;

typedef struct VertexFormat
{
    int type;
    int size;
    int normalize;
    int integer;
} VertexFormat;

typedef struct ImageFormat
{
    int internal_format;
    int format;
    int type;
    int components;
    int pixel_size;
    int buffer;
    int color;
    int clear_type;
    int flags;
} ImageFormat;

typedef struct UniformBinding
{
    int function;
    int location;
    int count;
    int offset;
} UniformBinding;

typedef struct UniformHeader
{
    int count;
    UniformBinding binding[1];
} UniformHeader;

typedef struct StencilSettings
{
    int fail_op;
    int pass_op;
    int depth_fail_op;
    int compare_op;
    int compare_mask;
    int write_mask;
    int reference;
} StencilSettings;

typedef struct Viewport
{
    int x;
    int y;
    int width;
    int height;
} Viewport;

typedef union ClearValue
{
    float clear_floats[4];
    int clear_ints[4];
    unsigned clear_uints[4];
} ClearValue;

typedef struct IntPair
{
    int x;
    int y;
} IntPair;

typedef struct Limits
{
    int max_uniform_buffer_bindings;
    int max_uniform_block_size;
    int max_combined_uniform_blocks;
    int max_combined_texture_image_units;
    int max_vertex_attribs;
    int max_draw_buffers;
    int max_samples;
    int max_shader_storage_buffer_bindings;
} Limits;

typedef struct ModuleState
{
    PyObject *helper;
    PyObject *empty_tuple;
    PyObject *str_none;
    PyObject *str_triangles;
    PyObject *str_static_draw;
    PyObject *str_dynamic_draw;
    PyObject *str_rgba8unorm;
    PyObject *str_TOPOLOGY;
    PyObject *str_BUFFER_ACCESS;
    PyObject *str_IMAGE_FORMAT;
    PyObject *str_VERTEX_FORMAT;
    PyObject *default_loader;
    PyObject *default_context;
    PyObject *HyperGLError;
    PyTypeObject *Context_type;
    PyTypeObject *Buffer_type;
    PyTypeObject *Image_type;
    PyTypeObject *Pipeline_type;
    PyTypeObject *Compute_type;
    PyTypeObject *ImageFace_type;
    PyTypeObject *BufferView_type;
    PyTypeObject *DescriptorSet_type;
    PyTypeObject *GlobalSettings_type;
    PyTypeObject *GLObject_type;
    PyTypeObject *Fence_type;
    PyTypeObject *CommandBuffer_type;
    Limits limits;
    void *opengl_handle;
    void *(*wglGetProcAddress)(const char *);
    int gl_initialized;
    PyMutex global_lock;
    PyMutex setup_lock;
} ModuleState;

typedef enum {
    TRASH_BUFFER        = 1,
    TRASH_TEXTURE       = 2,
    TRASH_RENDERBUFFER  = 3,
    TRASH_FRAMEBUFFER   = 4,
    TRASH_VERTEX_ARRAY  = 5,
    TRASH_PROGRAM       = 6,
    TRASH_SHADER        = 7,
    TRASH_SAMPLER       = 8,
    TRASH_QUERY         = 9,
    TRASH_FENCE         = 10
} TrashType;

typedef struct {
    uint64_t id;
    int type; 
} TrashItem;

typedef struct Fence {
    PyObject_HEAD
    struct Context *ctx;
    GLsync sync;
} Fence;

typedef struct SharedTrash {
    PyMutex lock;
    TrashItem *bin;
    size_t count;
    size_t capacity;
    volatile long ref_count;
} SharedTrash;

typedef struct GLObject
{
    PyObject_HEAD 
    volatile long uses; 
    int obj;
    int type;
    SharedTrash *trash;
    PyObject *extra; 
} GLObject;

typedef struct BufferBinding
{
    struct Buffer *buffer;
    int offset;
    int size;
} BufferBinding;

typedef struct SamplerBinding
{
    GLObject *sampler;
    struct Image *image;
} SamplerBinding;

typedef struct DescriptorSetBuffers
{
    int binding_count;
    BufferBinding binding[MAX_BUFFER_BINDINGS];
} DescriptorSetBuffers;

typedef struct DescriptorSetSamplers
{
    int binding_count;
    SamplerBinding binding[MAX_SAMPLER_BINDINGS];
} DescriptorSetSamplers;

typedef struct DescriptorSet
{
    PyObject_HEAD int uses;
    DescriptorSetBuffers uniform_buffers;
    DescriptorSetBuffers storage_buffers;
    DescriptorSetSamplers samplers;
} DescriptorSet;

typedef struct BlendState
{
    int op_color;
    int op_alpha;
    int src_color;
    int dst_color;
    int src_alpha;
    int dst_alpha;
} BlendState;

typedef struct GlobalSettings
{
    PyObject_HEAD int uses;
    int attachments;
    int cull_face;
    int depth_enabled;
    int depth_write;
    int depth_func;
    int stencil_enabled;
    int blend_enabled;
    StencilSettings stencil_front;
    StencilSettings stencil_back;
    BlendState blend;
} GlobalSettings;

typedef struct GLStateShadow {
    int8_t cull_face;
    int8_t depth_test;
    int8_t stencil_test;
    int8_t blend;
    int8_t primitive_restart;
    int8_t program_point_size;
    int8_t seamless_cube;
    int8_t _pad; // keep alignment
} GLStateShadow;

typedef struct Context
{
    PyObject_HEAD
    ModuleState *module_state;
    PyMutex state_lock;
    PyObject *descriptor_set_cache;
    PyObject *global_settings_cache;
    PyObject *sampler_cache;
    PyObject *vertex_array_cache;
    PyObject *framebuffer_cache;
    PyObject *program_cache;
    PyObject *shader_cache;
    PyObject *includes;
    GLObject *default_framebuffer;
    DescriptorSet *current_descriptor_set;
    GlobalSettings *current_global_settings;
    PyObject *info_dict;
    SharedTrash *trash_shared;
    int current_read_framebuffer;
    int current_draw_framebuffer;
    int current_program;
    int current_vertex_array;
    int current_depth_mask;
    int current_stencil_mask;
    int default_texture_unit;
    
    // UPDATED: 'char' to match Py_T_BOOL expectations
    char is_lost; 
    char is_gles;
    char is_webgl;
    
    // Bitfields are generally fine, but ensure careful access
    unsigned int is_mask_default      : 1;
    unsigned int is_stencil_default   : 1;
    unsigned int is_blend_default     : 1;
    unsigned int padding_bits         : 5;

    // Raw OS handles for migration
    void* hdc;   // The Device Context (Windows)
    void* hglrc; // The OpenGL Context (Windows)
    unsigned long thread_id; 
    
    GLStateShadow gl_state;
    Viewport current_viewport;
} Context;

typedef struct Buffer
{
    PyObject_HEAD      
    Context *ctx;
    int buffer;
    int target;
    int size;
    int access;
    int is_persistently_mapped;
    // int gpu_dirty; // TODO: implement this across every single GPU call
    void *mapped_ptr;
    PyObject *memoryview;
} Buffer;

typedef struct Image
{
    PyObject_HEAD
    Context *ctx;
    PyObject *size;
    PyObject *format;
    PyObject *faces;
    PyObject *layers;
    
    PyMutex state_lock;
    
    ImageFormat fmt;
    ClearValue clear_value;
    
    int image;
    int width;
    int height;
    int samples;
    int array;
    int cubemap;
    int target;
    
    // UPDATED: 'char' to match Py_T_BOOL
    char renderbuffer;
    
    int layer_count;
    int level_count;

    int external;

    GLuint64 bindless_handle;
    int is_resident;
} Image;

typedef struct RenderParameters
{
    int vertex_count;
    int instance_count;
    int first_vertex;
} RenderParameters;

typedef struct Pipeline
{
    PyObject_HEAD
    Context *ctx;
    PyObject *create_kwargs;
    DescriptorSet *descriptor_set;
    GlobalSettings *global_settings;
    GLObject *framebuffer;
    GLObject *vertex_array;
    GLObject *program;
    PyObject *uniforms;
    PyObject *uniform_layout;
    PyObject *uniform_data;
    PyObject *viewport_data;
    PyObject *render_data;
    Py_buffer uniform_layout_buffer;
    Py_buffer uniform_data_buffer;
    Py_buffer viewport_data_buffer;
    Py_buffer render_data_buffer;
    RenderParameters params;
    Viewport viewport;
    int topology;
    int index_type;
    int index_size;
} Pipeline;

typedef struct Compute
{
    PyObject_HEAD
    Context *ctx;
    PyObject *create_kwargs;
    DescriptorSet *descriptor_set;
    PyObject *global_settings; 
    GLObject *framebuffer;     
    GLObject *vertex_array;    
    GLObject *program;
    PyObject *uniforms;
    PyObject *uniform_layout;
    PyObject *uniform_data;
    PyObject *viewport_data;   
    PyObject *render_data;     
    Py_buffer uniform_layout_buffer;
    Py_buffer uniform_data_buffer;
    Py_buffer viewport_data_buffer;
    Py_buffer render_data_buffer;
    RenderParameters params;
    Viewport viewport;
    int topology;
    int index_type;
    int index_size;

    PyObject *workgroup_size;
} Compute;

typedef struct ImageFace
{
    PyObject_HEAD Context *ctx;
    Image *image;
    GLObject *framebuffer;
    PyObject *size;
    int width;
    int height;
    int layer;
    int level;
    int samples;
    int flags;
} ImageFace;

typedef struct BufferView
{
    PyObject_HEAD Buffer *buffer;
    int offset;
    int size;
} BufferView;

#pragma pack(push, 1)

typedef struct {
    uint32_t count;
    uint32_t instanceCount;
    uint32_t first;
    uint32_t baseInstance;
} DrawArraysIndirectCommand; 

typedef struct {
    uint32_t count;
    uint32_t instanceCount;
    uint32_t firstIndex;
    int32_t  baseVertex;
    uint32_t baseInstance;
} DrawElementsIndirectCommand;

#pragma pack(pop)

// --- OpenGL Constants ---

#define GL_DEPTH_BUFFER_BIT 0x0100
#define GL_STENCIL_BUFFER_BIT 0x0400
#define GL_COLOR_BUFFER_BIT 0x4000
#define GL_FRONT 0x0404
#define GL_BACK 0x0405
#define GL_CULL_FACE 0x0B44
#define GL_DEPTH_TEST 0x0B71
#define GL_STENCIL_TEST 0x0B90
#define GL_BLEND 0x0BE2
#define GL_TEXTURE_2D 0x0DE1
#define GL_UNSIGNED_SHORT 0x1403
#define GL_UNSIGNED_INT 0x1405
#define GL_DEPTH 0x1801
#define GL_STENCIL 0x1802
#define GL_VENDOR 0x1F00
#define GL_RENDERER 0x1F01
#define GL_VERSION 0x1F02
#define GL_NEAREST 0x2600
#define GL_LINEAR 0x2601
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_TEXTURE_WRAP_R 0x8072
#define GL_TEXTURE_MIN_LOD 0x813A
#define GL_TEXTURE_MAX_LOD 0x813B
#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE_CUBE_MAP 0x8513
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X 0x8515
#define GL_TEXTURE_LOD_BIAS 0x8501
#define GL_TEXTURE_COMPARE_MODE 0x884C
#define GL_TEXTURE_COMPARE_FUNC 0x884D
#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_STATIC_DRAW 0x88E4
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_MAX_DRAW_BUFFERS 0x8824
#define GL_MAX_VERTEX_ATTRIBS 0x8869
#define GL_MAX_TEXTURE_IMAGE_UNITS 0x8872
#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS 0x8B4D
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_INFO_LOG_LENGTH 0x8B84
#define GL_ACTIVE_UNIFORMS 0x8B86
#define GL_ACTIVE_ATTRIBUTES 0x8B89
#define GL_SHADING_LANGUAGE_VERSION 0x8B8C
#define GL_PIXEL_PACK_BUFFER 0x88EB
#define GL_PIXEL_UNPACK_BUFFER 0x88EC
#define GL_TEXTURE_2D_ARRAY 0x8C1A
#define GL_DEPTH_STENCIL_ATTACHMENT 0x821A
#define GL_DEPTH_STENCIL 0x84F9
#define GL_READ_FRAMEBUFFER 0x8CA8
#define GL_DRAW_FRAMEBUFFER 0x8CA9
#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_DEPTH_ATTACHMENT 0x8D00
#define GL_STENCIL_ATTACHMENT 0x8D20
#define GL_RENDERBUFFER 0x8D41
#define GL_MAX_SAMPLES 0x8D57
#define GL_COPY_READ_BUFFER 0x8F36
#define GL_COPY_WRITE_BUFFER 0x8F37
#define GL_UNIFORM_BUFFER 0x8A11
#define GL_MAX_COMBINED_UNIFORM_BLOCKS 0x8A2E
#define GL_MAX_UNIFORM_BUFFER_BINDINGS 0x8A2F
#define GL_MAX_UNIFORM_BLOCK_SIZE 0x8A30
#define GL_ACTIVE_UNIFORM_BLOCKS 0x8A36
#define GL_UNIFORM_BLOCK_DATA_SIZE 0x8A40
#define GL_PROGRAM_POINT_SIZE 0x8642
#define GL_TEXTURE_CUBE_MAP_SEAMLESS 0x884F
#define GL_PRIMITIVE_RESTART_FIXED_INDEX 0x8D69
#define GL_TEXTURE_MAX_ANISOTROPY 0x84FE
#define GL_COMPUTE_SHADER 0x91B9 
#define GL_SHADER_STORAGE_BUFFER 0x90D2
#define GL_READ_ONLY 0x88B8
#define GL_WRITE_ONLY 0x88B9
#define GL_READ_WRITE 0x88BA
#define GL_ALL_BARRIER_BITS 0xFFFFFFFF
#define GL_INVALID_INDEX 0xFFFFFFFFu
#define GL_SHADER_STORAGE_BARRIER_BIT 0x2000
#define GL_MAP_READ_BIT 0x0001
#define GL_MAP_WRITE_BIT 0x0002
#define GL_MAP_PERSISTENT_BIT 0x0040
#define GL_MAP_COHERENT_BIT 0x0080
#define GL_DYNAMIC_STORAGE_BIT 0x0100
#define GL_CLIENT_STORAGE_BIT 0x0200
#define GL_PERSISTENT_WRITE_FLAGS (GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT)
#define GL_STORAGE_FLAGS (GL_PERSISTENT_WRITE_FLAGS | GL_DYNAMIC_STORAGE_BIT | GL_CLIENT_STORAGE_BIT)
#define GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS 0x90DE
#define GL_ACTIVE_ATTRIBUTE_MAX_LENGTH  0x8B8A
#define GL_ACTIVE_UNIFORM_MAX_LENGTH    0x8B87
#define GL_UNIFORM_BLOCK_INDEX          0x8A3A
#define GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH 0x8A35
#define GL_UNPACK_ALIGNMENT 0x0CF5
#define GL_PACK_ALIGNMENT   0x0D05
#define GL_UNPACK_ROW_LENGTH 0x0CF2
#define GL_PACK_ROW_LENGTH   0x0D02
#define GL_UNPACK_SKIP_PIXELS 0x0CF4
#define GL_UNPACK_SKIP_ROWS 0x0CF3
#define GL_UNPACK_IMAGE_HEIGHT 0x806E
#define GL_UNIFORM                                 0x92E1
#define GL_UNIFORM_BLOCK                           0x92E2
#define GL_PROGRAM_INPUT                           0x92E3
#define GL_ACTIVE_RESOURCES                        0x92F5
#define GL_TYPE                                    0x92FA
#define GL_LOCATION                                0x930E
#define GL_MAX_NAME_LENGTH    0x92F6
#define GL_ARRAY_SIZE         0x92FB
#define GL_NAME_LENGTH        0x92F9
#define GL_BUFFER_DATA_SIZE    0x9303
#define GL_BUFFER_SIZE 0x8764
#define GL_BUFFER_IMMUTABLE_STORAGE  0x821F
#define GL_DRAW_INDIRECT_BUFFER 0x8F3F
#define GL_PARAMETER_BUFFER_ARB 0x80EE 
#define GL_COMMAND_BARRIER_BIT 0x00000040

#define GL_SYNC_GPU_COMMANDS_COMPLETE 0x9117
#define GL_SYNC_FLUSH_COMMANDS_BIT    0x00000001
#define GL_ALREADY_SIGNALED           0x911A
#define GL_TIMEOUT_EXPIRED            0x911B
#define GL_CONDITION_SATISFIED        0x911C
#define GL_WAIT_FAILED                0x911D
// 0xFFFFFFFFFFFFFFFFull
#define GL_TIMEOUT_IGNORED            18446744073709551615ULL 

#define GL_MIN_UNIFORM_BUFFER_BINDINGS 8
#define GL_ENGINE_MAX_VERTEX_ATTRIBS 64
#define GL_MIN_UBO_SIZE 0x4000
#define GL_MAX_UBO_SIZE 0x40000000
#define HGL_MIN_SAMPLES 1
#define HGL_MAX_SAMPLES 16

#ifndef GLboolean
typedef unsigned char GLboolean;
#endif

#ifndef GL_FALSE
#define GL_FALSE 0
#endif

#ifndef GL_TRUE
#define GL_TRUE 1
#endif

#ifndef GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT
    #define GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT 0x00000001
#endif
#ifndef GL_ELEMENT_ARRAY_BARRIER_BIT
    #define GL_ELEMENT_ARRAY_BARRIER_BIT       0x00000002
#endif
#ifndef GL_UNIFORM_BARRIER_BIT
    #define GL_UNIFORM_BARRIER_BIT             0x00000004
#endif
#ifndef GL_TEXTURE_FETCH_BARRIER_BIT
    #define GL_TEXTURE_FETCH_BARRIER_BIT       0x00000008
#endif
#ifndef GL_SHADER_STORAGE_BARRIER_BIT
    #define GL_SHADER_STORAGE_BARRIER_BIT      0x00002000
#endif

#ifndef GLbitfield
typedef unsigned int GLbitfield;
#endif

#ifndef GLenum
typedef unsigned int GLenum;
#endif

#ifndef GLuint
typedef unsigned int GLuint;
#endif

#ifndef GLint
typedef int GLint;
#endif

#ifndef GLsizei
typedef int  GLsizei;
#endif

#define GL_NO_ERROR 0

// -----------------------------------------------------------------------------
// Enum helpers
// -----------------------------------------------------------------------------

typedef void (*UniformUploadFn)(GLint location, GLsizei count, const void *ptr);

typedef enum UniformFunction {
    UF_1I = 0,
    UF_2I,
    UF_3I,
    UF_4I,

    UF_1B, // Booleans (uploaded as integers)
    UF_2B,
    UF_3B,
    UF_4B,

    UF_1U,
    UF_2U,
    UF_3U,
    UF_4U,

    UF_1F,
    UF_2F,
    UF_3F,
    UF_4F,

    UF_MAT2,
    UF_MAT2x3,
    UF_MAT2x4,
    UF_MAT3x2,
    UF_MAT3,
    UF_MAT3x4,
    UF_MAT4x2,
    UF_MAT4x3,
    UF_MAT4,

    UF_COUNT
} UniformFunction;

typedef enum ImageFormatTupleIndex {
    IF_INTERNAL_FORMAT = 0,
    IF_FORMAT,
    IF_TYPE,
    IF_BUFFER,
    IF_COMPONENTS,
    IF_PIXEL_SIZE,
    IF_COLOR,
    IF_FLAGS,
    IF_CLEAR_TYPE,
    IF_TUPLE_SIZE
} ImageFormatTupleIndex;

// --- Command Buffer Definitions ---

typedef enum CommandType {
    CMD_NOP                 = 0,
    
    // --- Graphics (1-9) ---
    CMD_CLEAR               = 1,
    CMD_BIND_PIPELINE       = 2,
    CMD_BIND_DESCRIPTOR_SET = 3,
    CMD_DRAW                = 4,
    CMD_DRAW_INDIRECT       = 5,

    // --- Compute (10-14) ---
    CMD_BIND_COMPUTE        = 10,
    CMD_DISPATCH            = 11,
    CMD_BARRIER             = 12,

    // --- Control Flow (15-19) ---
    CMD_LABEL               = 15, // (Usually handled by compiler/fixup)
    CMD_GOTO                = 16,
    CMD_CALL                = 17,
    CMD_RET                 = 18,

    // --- Memory Branching (20-24) ---
    CMD_SKIP_IF_ZERO        = 20,
    CMD_SKIP_IF_NOT_ZERO    = 21,
    CMD_RET_IF_ZERO         = 22,
    CMD_RET_IF_NOT_ZERO     = 23,

    // --- Registers & ALU (25-29) ---
    CMD_SET_ITER            = 25,
    CMD_JUMP_ITER           = 26,
    CMD_LOAD_REG            = 27,
    CMD_STORE_REG           = 28,
    CMD_ALU                 = 29,

    // --- Indirect Memory (30-34) ---
    CMD_LOAD_REG_INDIRECT   = 30,
    CMD_STORE_REG_INDIRECT  = 31,
    CMD_SKIP_REG_ZERO       = 32,
    CMD_SKIP_REG_NOT_ZERO   = 33,

    // --- Stack & Sync (35+) ---
    CMD_PUSH                = 35,
    CMD_POP                 = 36,
    CMD_SIGNAL_FENCE        = 40,
    CMD_WAIT_FENCE          = 41,
    CMD_SKIP_IF_NOT_READY   = 42,

    CMD_SET_UNIFORM         = 50, 

    // --- Debug (100+) ---
    CMD_PRINT               = 100,
    CMD_DUMP                = 101
} CommandType;

typedef struct CmdHeader {
    uint32_t type;
    uint32_t size; // Byte size of this packet (header + payload)
} CmdHeader;

typedef struct CmdBindPipeline {
    CmdHeader header;
    Pipeline *pipeline;
} CmdBindPipeline;

typedef struct CmdBindCompute {
    CmdHeader header;
    Compute *compute;
} CmdBindCompute;

typedef struct CmdBindDescriptorSet {
    CmdHeader header;
    DescriptorSet *set;
} CmdBindDescriptorSet;

typedef struct CmdDraw {
    CmdHeader header;
    int32_t vertex_count;
    int32_t instance_count;
    int32_t first;
    int32_t _pad; // Keeps struct aligned to 8 bytes
} CmdDraw;

typedef struct CmdDrawIndirect {
    CmdHeader header;
    Buffer *buffer;
    int32_t count;
    int32_t offset;
    int32_t stride;
    int32_t _pad;
} CmdDrawIndirect;

typedef struct CmdDispatch {
    CmdHeader header;
    int32_t x, y, z;
    int32_t _pad;
} CmdDispatch;

typedef struct CmdBarrier {
    CmdHeader header;
    int32_t flags;
    int32_t _pad;
} CmdBarrier;

typedef struct CmdClear {
    CmdHeader header;
    int32_t mask;
    int32_t _pad;
} CmdClear;

typedef struct CmdSkip {
    CmdHeader header;
    Buffer *buffer;
    uint32_t offset;
    uint32_t _pad;
} CmdSkip;

typedef struct CmdGoto {
    CmdHeader header;
    uint32_t target_offset; // Absolute offset from self->data
    uint32_t _pad;
} CmdGoto;

typedef struct CmdPrint {
    CmdHeader header;
    Buffer *buffer;    // If NULL, just prints message
    uint32_t offset;   // Offset in buffer to peek at
    char message[44];  // Static message (Total struct size: 64 bytes)
} CmdPrint;

typedef struct CmdDump {
    CmdHeader header;
    Buffer *buffer;
    uint32_t offset;
    uint32_t count;    // Number of elements to print
    uint32_t stride;   // Type: 0=float, 1=int, 2=uint
    char message[32];  // Prefix message (Struct total: 64 bytes)
} CmdDump;

typedef struct CmdCall {
    CmdHeader header;
    struct CommandBuffer *other; // The child buffer to execute
} CmdCall;

typedef struct CmdFence {
    CmdHeader header;
    Fence *fence;
} CmdFence;

typedef struct CmdSetIter {
    CmdHeader header;
    uint32_t reg;
    uint32_t value;
} CmdSetIter;

typedef struct CmdJumpIter {
    CmdHeader header;
    uint32_t reg;
    uint32_t target_offset;
} CmdJumpIter;

typedef struct CmdStoreReg {
    CmdHeader header;
    Buffer *buffer;
    uint32_t reg;
    uint32_t offset;
} CmdStoreReg;

typedef struct CmdLoadReg {
    CmdHeader header;
    Buffer *buffer;
    uint32_t reg;
    uint32_t offset;
} CmdLoadReg;

typedef struct CmdAlu {
    CmdHeader header;
    uint32_t reg_a;
    uint32_t reg_b;
    uint32_t op; // 0=add, 1=sub, 2=mul, 3=div, 4=and, 5=or
} CmdAlu;

typedef struct CmdRetCond {
    CmdHeader header;
    Buffer *buffer;
    uint32_t offset;
    uint32_t _pad;
} CmdRetCond;

typedef struct CmdSkipReg {
    CmdHeader header;
    uint32_t reg;
    uint32_t _pad;
} CmdSkipReg;

typedef struct CmdMemIndirect {
    CmdHeader header;
    Buffer *buffer;
    uint32_t reg;        // The data register (to load into or store from)
    uint32_t index_reg;  // The register holding the array index
    uint32_t base_offset;
    uint32_t stride;
} CmdMemIndirect;

typedef struct CmdStackOp {
    CmdHeader header;
    uint32_t reg;
    uint32_t _pad;
} CmdStackOp;

typedef struct CmdSetUniform {
    CmdHeader header;
    int32_t location;   // glGetUniformLocation result
    uint32_t reg;       // Register index (0-7)
    uint32_t type;      // 0=float, 1=int, 2=uint
} CmdSetUniform;

typedef struct CommandBuffer {
    PyObject_HEAD
    Context *ctx;
    
    // The Bytecode
    uint8_t *data;
    size_t size;
    size_t capacity;
    
    // Object Lifecycle Management
    // We must keep references to every Pipeline, Buffer, etc. used in the recording
    // so they don't get GC'd while this buffer exists.
    PyObject *ref_list; 
    // --- LABEL SYSTEM ---
    PyObject *labels;   // Dictionary: { "name": offset }
    PyObject *fixups;   // List of tuples: (bytecode_offset_to_patch, "label_name")
    int recording;
} CommandBuffer;

// MISC defs

#define SHARED_TRASH_INITIAL_CAPACITY 4096

#define CONTEXT_INFO_FORMAT "{szszszszsisisisisisisi}"

#define INDEX_TYPE_NONE 0


#endif // HYPERGL_DEFS_H