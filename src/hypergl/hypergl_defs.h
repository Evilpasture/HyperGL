#pragma once

#include <Python.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#define HGL_MAGIC 0x424C4748 // "HGLB" (HyperGL Binary) in Little Endian

// Current VM Specification version
#define HGL_ISA_MAJOR 2
#define HGL_ISA_MINOR 0

typedef struct HGLHeader {
  uint32_t magic;     // Must be HGL_MAGIC
  uint16_t major;     // Breaking changes
  uint16_t minor;     // New features (backward compatible)
  uint32_t data_size; // Size of bytecode following this header
  uint32_t _reserved; // Alignment padding
} HGLHeader;

// --- Compiler Hints ---
#if defined(__GNUC__) || defined(__clang__)
#define LIKELY(x) __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#define FORCE_INLINE __attribute__((always_inline)) inline
#define NO_ALIAS __restrict
#define PURE_FUNC __attribute__((pure))
#define CONST_FUNC __attribute__((const))
#define UNUSED __attribute__((unused))
#else
#define LIKELY(x) (x)
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
static int PyDict_SetDefaultRef(PyObject *d, PyObject *key,
                                PyObject *default_value, PyObject **result) {
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
static constexpr size_t MAX_ATTACHMENTS = 8;
static constexpr size_t MIN_BUFFER_BINDINGS = 8;
static constexpr size_t MAX_BUFFER_BINDINGS = 8;
static constexpr size_t MAX_SAMPLER_BINDINGS = 16;

// --- Platform Specifics & Atomic Macros ---
#include <stdatomic.h>
// For the ref-counting/state flags in your Shadow Buffers
typedef _Atomic int atomic_int_t;

#define Atomic_Increment(ptr) atomic_fetch_add(ptr, 1)
#define Atomic_Decrement(ptr) atomic_fetch_sub(ptr, 1)
#define Atomic_Load(ptr)      atomic_load(ptr)

#ifdef _WIN32
#include <windows.h>
#define GL_API __stdcall
#else
#define GL_API
#endif

// --- Macros ---
#ifdef DISABLE_LOCKS
#undef PyMutex_Lock
#undef PyMutex_Unlock
#define PyMutex_Lock(m) ((void)0)
#define PyMutex_Unlock(m) ((void)0)
#endif

#if ENABLE_VALIDATION
#define VALIDATE(cond, exc, fmt, ...)                                          \
  do {                                                                         \
    if (!(cond)) {                                                             \
      PyErr_Format((exc), (fmt), ##__VA_ARGS__);                               \
      return NULL;                                                             \
    }                                                                          \
  } while (0)

#define VALIDATE_PTR(ptr, exc, fmt, ...)                                       \
  do {                                                                         \
    if (!(ptr)) {                                                              \
      if (!PyErr_Occurred())                                                   \
        PyErr_Format((exc), (fmt), ##__VA_ARGS__);                             \
      return NULL;                                                             \
    }                                                                          \
  } while (0)

#define VALIDATE_LOCKED(ctx, cond, exc, fmt, ...)                              \
  do {                                                                         \
    if (!(cond)) {                                                             \
      PyMutex_Unlock(&(ctx)->state_lock);                                      \
      PyErr_Format((exc), (fmt), ##__VA_ARGS__);                               \
      return NULL;                                                             \
    }                                                                          \
  } while (0)
#else
#define VALIDATE(cond, exc, fmt, ...) ((void)0)
#define VALIDATE_PTR(ptr, exc, fmt, ...) ((void)0)
#define VALIDATE_LOCKED(ctx, cond, exc, fmt, ...) ((void)0)
#endif

#define GL_STATE_UNKNOWN (-1)
#define GL_STATE_DISABLED 0
#define GL_STATE_ENABLED 1

// OPTIMIZATION: Zero-cost state filtering.
// NOTE: These macros MUST be called while self->state_lock is HELD.
#define LOCKED_GL_ENABLE_STATE(GL_ENUM, FIELD)                                 \
  do {                                                                         \
    if (self->gl_state.FIELD != GL_STATE_ENABLED) {                            \
      glEnable(GL_ENUM);                                                       \
      self->gl_state.FIELD = GL_STATE_ENABLED;                                 \
    }                                                                          \
  } while (0)

#define LOCKED_GL_DISABLE_STATE(GL_ENUM, FIELD)                                \
  do {                                                                         \
    if (self->gl_state.FIELD != GL_STATE_DISABLED) {                           \
      glDisable(GL_ENUM);                                                      \
      self->gl_state.FIELD = GL_STATE_DISABLED;                                \
    }                                                                          \
  } while (0)

// --- Struct Definitions ---

typedef unsigned long long GLuint64;
typedef struct HyperGLSync *GLsync;
typedef Py_ssize_t intptr;

typedef struct VertexFormat {
  int type;
  int size;
  int normalize;
  int integer;
} VertexFormat;

typedef struct ImageFormat {
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

typedef struct UniformBinding {
  int function;
  int location;
  int count;
  int offset;
} UniformBinding;

typedef struct UniformHeader {
  int count;
  UniformBinding binding[1];
} UniformHeader;

typedef struct StencilSettings {
  int fail_op;
  int pass_op;
  int depth_fail_op;
  int compare_op;
  int compare_mask;
  int write_mask;
  int reference;
} StencilSettings;

typedef struct Viewport {
  int x;
  int y;
  int width;
  int height;
} Viewport;

typedef union ClearValue {
  float clear_floats[4];
  int clear_ints[4];
  unsigned clear_uints[4];
} ClearValue;

typedef struct IntPair {
  int x;
  int y;
} IntPair;

typedef struct Limits {
  int max_uniform_buffer_bindings;
  int max_uniform_block_size;
  int max_combined_uniform_blocks;
  int max_combined_texture_image_units;
  int max_vertex_attribs;
  int max_draw_buffers;
  int max_samples;
  int max_shader_storage_buffer_bindings;
} Limits;

typedef struct ModuleState {
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
  TRASH_BUFFER = 1,
  TRASH_TEXTURE = 2,
  TRASH_RENDERBUFFER = 3,
  TRASH_FRAMEBUFFER = 4,
  TRASH_VERTEX_ARRAY = 5,
  TRASH_PROGRAM = 6,
  TRASH_SHADER = 7,
  TRASH_SAMPLER = 8,
  TRASH_QUERY = 9,
  TRASH_FENCE = 10
} TrashType;

typedef struct {
  uint64_t id;
  int type;
} TrashItem;

typedef struct Fence {
  PyObject_HEAD struct Context *ctx;
  GLsync sync;
} Fence;

typedef struct SharedTrash {
  PyMutex lock;
  TrashItem *bin;
  size_t count;
  size_t capacity;
  atomic_size_t ref_count;

  // Background cleanup support
  unsigned long owner_thread_id;
  char *is_lost_ptr; // Points to Context->is_lost
} SharedTrash;

typedef struct GLObject {
  PyObject_HEAD atomic_size_t uses;
  int obj;
  int type;
  SharedTrash *trash;
  PyObject *extra;
} GLObject;

typedef struct BufferBinding {
  struct Buffer *buffer;
  int offset;
  int size;
} BufferBinding;

typedef struct SamplerBinding {
  GLObject *sampler;
  struct Image *image;
} SamplerBinding;

typedef struct DescriptorSetBuffers {
  int binding_count;
  BufferBinding binding[MAX_BUFFER_BINDINGS];
} DescriptorSetBuffers;

typedef struct DescriptorSetSamplers {
  int binding_count;
  SamplerBinding binding[MAX_SAMPLER_BINDINGS];
} DescriptorSetSamplers;

typedef struct DescriptorSet {
  PyObject_HEAD atomic_size_t uses;
  DescriptorSetBuffers uniform_buffers;
  DescriptorSetBuffers storage_buffers;
  DescriptorSetSamplers samplers;
} DescriptorSet;

typedef struct BlendState {
  int op_color;
  int op_alpha;
  int src_color;
  int dst_color;
  int src_alpha;
  int dst_alpha;
} BlendState;

typedef struct GlobalSettings {
  PyObject_HEAD atomic_size_t uses;
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

typedef struct {
  atomic_size_t draw_calls;
  atomic_size_t pipeline_swaps;
  atomic_size_t set_swaps;
  atomic_size_t dispatch_calls;
} ContextStats;

typedef struct Context {
  PyObject_HEAD ModuleState *module_state;
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

  double start_time;      // App start (absolute)
  double last_frame_time; // Last submit time
  float frame_delta;      // Difference (float32 for registers)

  uint32_t vm_seed; // The random state for the VM

  // UPDATED: 'char' to match Py_T_BOOL expectations
  char is_lost;
  char is_gles;
  char is_webgl;

  // Bitfields are generally fine, but ensure careful access
  unsigned int is_mask_default : 1;
  unsigned int is_stencil_default : 1;
  unsigned int is_blend_default : 1;
  unsigned int padding_bits : 5;

  // Raw OS handles for migration
  void *hdc;   // The Device Context (Windows)
  void *hglrc; // The OpenGL Context (Windows)
  unsigned long thread_id;

  ContextStats stats;

  GLsync last_work_fence;

  GLStateShadow gl_state;
  Viewport current_viewport;
} Context;

typedef struct Buffer {
  PyObject_HEAD Context *ctx;
  char name[64];
  int buffer;
  int target;
  int size;
  int access;
  int is_persistently_mapped;
  // int gpu_dirty; // TODO: implement this across every single GPU call
  void *mapped_ptr;
  PyObject *memoryview;
} Buffer;

typedef struct Image {
  PyObject_HEAD Context *ctx;
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
  char name[64];
  char renderbuffer;

  int layer_count;
  int level_count;

  int external;

  GLuint64 bindless_handle;
  int is_resident;
} Image;

typedef struct RenderParameters {
  int vertex_count;
  int instance_count;
  int first_vertex;
} RenderParameters;

typedef struct Pipeline {
  PyObject_HEAD Context *ctx;
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

typedef struct Compute {
  PyObject_HEAD Context *ctx;
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

typedef struct ImageFace {
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

typedef struct BufferView {
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
  int32_t baseVertex;
  uint32_t baseInstance;
} DrawElementsIndirectCommand;

#pragma pack(pop)

// --- OpenGL Constants ---

static constexpr size_t GL_DEPTH_BUFFER_BIT = 0x0100;
static constexpr size_t GL_STENCIL_BUFFER_BIT = 0x0400;
static constexpr size_t GL_COLOR_BUFFER_BIT = 0x4000;
static constexpr size_t GL_FRONT = 0x0404;
static constexpr size_t GL_BACK = 0x0405;
static constexpr size_t GL_CULL_FACE = 0x0B44;
static constexpr size_t GL_DEPTH_TEST = 0x0B71;
static constexpr size_t GL_STENCIL_TEST = 0x0B90;
static constexpr size_t GL_BLEND = 0x0BE2;
static constexpr size_t GL_TEXTURE_2D = 0x0DE1;
static constexpr size_t GL_UNSIGNED_SHORT = 0x1403;
static constexpr size_t GL_UNSIGNED_INT = 0x1405;
static constexpr size_t GL_DEPTH = 0x1801;
static constexpr size_t GL_STENCIL = 0x1802;
static constexpr size_t GL_VENDOR = 0x1F00;
static constexpr size_t GL_RENDERER = 0x1F01;
static constexpr size_t GL_VERSION = 0x1F02;
static constexpr size_t GL_NEAREST = 0x2600;
static constexpr size_t GL_LINEAR = 0x2601;
static constexpr size_t GL_TEXTURE_MAG_FILTER = 0x2800;
static constexpr size_t GL_TEXTURE_MIN_FILTER = 0x2801;
static constexpr size_t GL_TEXTURE_MAX_LEVEL = 0x813D;
static constexpr size_t GL_TEXTURE_WRAP_S = 0x2802;
static constexpr size_t GL_TEXTURE_WRAP_T = 0x2803;
static constexpr size_t GL_TEXTURE_WRAP_R = 0x8072;
static constexpr size_t GL_TEXTURE_MIN_LOD = 0x813A;
static constexpr size_t GL_TEXTURE_MAX_LOD = 0x813B;
static constexpr size_t GL_TEXTURE0 = 0x84C0;
static constexpr size_t GL_TEXTURE_CUBE_MAP = 0x8513;
static constexpr size_t GL_TEXTURE_CUBE_MAP_POSITIVE_X = 0x8515;
static constexpr size_t GL_TEXTURE_LOD_BIAS = 0x8501;
static constexpr size_t GL_TEXTURE_COMPARE_MODE = 0x884C;
static constexpr size_t GL_TEXTURE_COMPARE_FUNC = 0x884D;
static constexpr size_t GL_ARRAY_BUFFER = 0x8892;
static constexpr size_t GL_ELEMENT_ARRAY_BUFFER = 0x8893;
static constexpr size_t GL_STATIC_DRAW = 0x88E4;
static constexpr size_t GL_DYNAMIC_DRAW = 0x88E8;
static constexpr size_t GL_MAX_DRAW_BUFFERS = 0x8824;
static constexpr size_t GL_MAX_VERTEX_ATTRIBS = 0x8869;
static constexpr size_t GL_MAX_TEXTURE_IMAGE_UNITS = 0x8872;
static constexpr size_t GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS = 0x8B4D;
static constexpr size_t GL_COMPILE_STATUS = 0x8B81;
static constexpr size_t GL_LINK_STATUS = 0x8B82;
static constexpr size_t GL_INFO_LOG_LENGTH = 0x8B84;
static constexpr size_t GL_ACTIVE_UNIFORMS = 0x8B86;
static constexpr size_t GL_ACTIVE_ATTRIBUTES = 0x8B89;
static constexpr size_t GL_SHADING_LANGUAGE_VERSION = 0x8B8C;
static constexpr size_t GL_PIXEL_PACK_BUFFER = 0x88EB;
static constexpr size_t GL_PIXEL_UNPACK_BUFFER = 0x88EC;
static constexpr size_t GL_TEXTURE_2D_ARRAY = 0x8C1A;
static constexpr size_t GL_DEPTH_STENCIL_ATTACHMENT = 0x821A;
static constexpr size_t GL_DEPTH_STENCIL = 0x84F9;
static constexpr size_t GL_READ_FRAMEBUFFER = 0x8CA8;
static constexpr size_t GL_DRAW_FRAMEBUFFER = 0x8CA9;
static constexpr size_t GL_COLOR_ATTACHMENT0 = 0x8CE0;
static constexpr size_t GL_DEPTH_ATTACHMENT = 0x8D00;
static constexpr size_t GL_STENCIL_ATTACHMENT = 0x8D20;
static constexpr size_t GL_RENDERBUFFER = 0x8D41;
static constexpr size_t GL_MAX_SAMPLES = 0x8D57;
static constexpr size_t GL_COPY_READ_BUFFER = 0x8F36;
static constexpr size_t GL_COPY_WRITE_BUFFER = 0x8F37;
static constexpr size_t GL_UNIFORM_BUFFER = 0x8A11;
static constexpr size_t GL_MAX_COMBINED_UNIFORM_BLOCKS = 0x8A2E;
static constexpr size_t GL_MAX_UNIFORM_BUFFER_BINDINGS = 0x8A2F;
static constexpr size_t GL_MAX_UNIFORM_BLOCK_SIZE = 0x8A30;
static constexpr size_t GL_ACTIVE_UNIFORM_BLOCKS = 0x8A36;
static constexpr size_t GL_UNIFORM_BLOCK_DATA_SIZE = 0x8A40;
static constexpr size_t GL_PROGRAM_POINT_SIZE = 0x8642;
static constexpr size_t GL_TEXTURE_CUBE_MAP_SEAMLESS = 0x884F;
static constexpr size_t GL_PRIMITIVE_RESTART_FIXED_INDEX = 0x8D69;
static constexpr size_t GL_TEXTURE_MAX_ANISOTROPY = 0x84FE;
static constexpr size_t GL_COMPUTE_SHADER = 0x91B9;
static constexpr size_t GL_SHADER_STORAGE_BUFFER = 0x90D2;
static constexpr size_t GL_READ_ONLY = 0x88B8;
static constexpr size_t GL_WRITE_ONLY = 0x88B9;
static constexpr size_t GL_READ_WRITE = 0x88BA;
static constexpr size_t GL_ALL_BARRIER_BITS = 0xFFFFFFFF;
static constexpr size_t GL_INVALID_INDEX = 0xFFFFFFFFu;
static constexpr size_t GL_SHADER_STORAGE_BARRIER_BIT = 0x2000;
static constexpr size_t GL_MAP_READ_BIT = 0x0001;
static constexpr size_t GL_MAP_WRITE_BIT = 0x0002;
static constexpr size_t GL_MAP_PERSISTENT_BIT = 0x0040;
static constexpr size_t GL_MAP_COHERENT_BIT = 0x0080;
static constexpr size_t GL_DYNAMIC_STORAGE_BIT = 0x0100;
static constexpr size_t GL_CLIENT_STORAGE_BIT = 0x0200;
static constexpr size_t GL_PERSISTENT_WRITE_FLAGS =
    (GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
static constexpr size_t GL_STORAGE_FLAGS =
    (GL_PERSISTENT_WRITE_FLAGS | GL_DYNAMIC_STORAGE_BIT |
     GL_CLIENT_STORAGE_BIT);
static constexpr size_t GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS = 0x90DE;
static constexpr size_t GL_ACTIVE_ATTRIBUTE_MAX_LENGTH = 0x8B8A;
static constexpr size_t GL_ACTIVE_UNIFORM_MAX_LENGTH = 0x8B87;
static constexpr size_t GL_UNIFORM_BLOCK_INDEX = 0x8A3A;
static constexpr size_t GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH = 0x8A35;
static constexpr size_t GL_UNPACK_ALIGNMENT = 0x0CF5;
static constexpr size_t GL_PACK_ALIGNMENT = 0x0D05;
static constexpr size_t GL_UNPACK_ROW_LENGTH = 0x0CF2;
static constexpr size_t GL_PACK_ROW_LENGTH = 0x0D02;
static constexpr size_t GL_UNPACK_SKIP_PIXELS = 0x0CF4;
static constexpr size_t GL_UNPACK_SKIP_ROWS = 0x0CF3;
static constexpr size_t GL_UNPACK_IMAGE_HEIGHT = 0x806E;
static constexpr size_t GL_UNIFORM = 0x92E1;
static constexpr size_t GL_UNIFORM_BLOCK = 0x92E2;
static constexpr size_t GL_PROGRAM_INPUT = 0x92E3;
static constexpr size_t GL_ACTIVE_RESOURCES = 0x92F5;
static constexpr size_t GL_TYPE = 0x92FA;
static constexpr size_t GL_LOCATION = 0x930E;
static constexpr size_t GL_MAX_NAME_LENGTH = 0x92F6;
static constexpr size_t GL_ARRAY_SIZE = 0x92FB;
static constexpr size_t GL_NAME_LENGTH = 0x92F9;
static constexpr size_t GL_BUFFER_DATA_SIZE = 0x9303;
static constexpr size_t GL_BUFFER_SIZE = 0x8764;
static constexpr size_t GL_BUFFER_IMMUTABLE_STORAGE = 0x821F;
static constexpr size_t GL_DRAW_INDIRECT_BUFFER = 0x8F3F;
static constexpr size_t GL_PARAMETER_BUFFER_ARB = 0x80EE;
static constexpr size_t GL_COMMAND_BARRIER_BIT = 0x00000040;
static constexpr size_t GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT = 0x00004000;
static constexpr size_t GL_BUFFER_UPDATE_BARRIER_BIT = 0x00000200;

static constexpr size_t GL_SYNC_GPU_COMMANDS_COMPLETE = 0x9117;
static constexpr size_t GL_SYNC_FLUSH_COMMANDS_BIT = 0x00000001;
static constexpr size_t GL_ALREADY_SIGNALED = 0x911A;
static constexpr size_t GL_TIMEOUT_EXPIRED = 0x911B;
static constexpr size_t GL_CONDITION_SATISFIED = 0x911C;
static constexpr size_t GL_WAIT_FAILED = 0x911D;
// 0xFFFFFFFFFFFFFFFFull
static constexpr size_t GL_TIMEOUT_IGNORED = 18446744073709551615ULL;

static constexpr size_t GL_MIN_UNIFORM_BUFFER_BINDINGS = 8;
static constexpr size_t GL_ENGINE_MAX_VERTEX_ATTRIBS = 64;
static constexpr size_t GL_MIN_UBO_SIZE = 0x4000;
static constexpr size_t GL_MAX_UBO_SIZE = 0x40000000;
static constexpr size_t HGL_MIN_SAMPLES = 1;
static constexpr size_t HGL_MAX_SAMPLES = 16;
static constexpr size_t GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT = 0x00000001;
static constexpr size_t GL_ELEMENT_ARRAY_BARRIER_BIT = 0x00000002;
static constexpr size_t GL_UNIFORM_BARRIER_BIT = 0x00000004;
static constexpr size_t GL_TEXTURE_FETCH_BARRIER_BIT = 0x00000008;

#ifndef GLboolean
typedef unsigned char GLboolean;
#endif

#ifndef GL_FALSE
static constexpr GLboolean GL_FALSE = 0;
#endif

#ifndef GL_TRUE
static constexpr GLboolean GL_TRUE = 1;
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
typedef int GLsizei;
#endif

static constexpr size_t GL_NO_ERROR = 0;

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
  CMD_NOP = 0x00,

  // --- 0x01 - 0x0F: Graphics (Drawing & Binding) ---
  CMD_CLEAR = 0x01,
  CMD_BIND_PIPELINE = 0x02,
  CMD_BIND_DESCRIPTOR_SET = 0x03,
  CMD_DRAW = 0x04,
  CMD_DRAW_INDIRECT = 0x05,
  CMD_DRAW_INDIRECT_COUNT = 0x06,
  CMD_BIND_SET_DRAW = 0x07, // Super Opcode
  CMD_BIND_SET_DRAW_INDIRECT = 0x08,

  // --- 0x10 - 0x17: Compute ---
  CMD_BIND_COMPUTE = 0x10,
  CMD_DISPATCH = 0x11,
  CMD_BARRIER = 0x12,

  // --- 0x18 - 0x2F: Control Flow (Branching & Subroutines) ---
  CMD_GOTO = 0x18,
  CMD_CALL = 0x19,
  CMD_RET = 0x1A,
  CMD_JUMP_TABLE = 0x1B,
  CMD_LABEL = 0x1F, // Usually marker-only

  // --- 0x30 - 0x3F: Conditional Branching (Memory/Reg) ---
  CMD_SKIP_IF_ZERO = 0x30,
  CMD_SKIP_IF_NOT_ZERO = 0x31,
  CMD_RET_IF_ZERO = 0x32,
  CMD_RET_IF_NOT_ZERO = 0x33,
  CMD_SKIP_REG_ZERO = 0x34,
  CMD_SKIP_REG_NOT_ZERO = 0x35,

  // --- 0x40 - 0x5F: Registers, ALU & Logic ---
  CMD_SET_ITER = 0x40,
  CMD_JUMP_ITER = 0x41,
  CMD_LOAD_REG = 0x42,
  CMD_STORE_REG = 0x43,
  CMD_ALU = 0x44,
  CMD_GEN_RAND = 0x45,
  CMD_GET_TIME = 0x46,
  CMD_GET_DELTA = 0x47,
  CMD_ASSERT_REG = 0x48,
  CMD_PUSH = 0x49,
  CMD_POP = 0x4A,
  CMD_CMP = 0x4B,
  CMD_SIN_COS = 0x4C,
  CMD_GET_STAT = 0x4D,
  CMD_FCMP = 0x4E,

  // --- 0x60 - 0x6F: Indirect Memory (Complex Addressing) ---
  CMD_LOAD_REG_INDIRECT = 0x60,
  CMD_STORE_REG_INDIRECT = 0x61,
  CMD_COPY_BUFFER = 0x62,

  // --- 0x70 - 0x7F: Synchronization & Resources ---
  CMD_SIGNAL_FENCE = 0x70,
  CMD_WAIT_FENCE = 0x71,
  CMD_SKIP_IF_NOT_READY = 0x72,
  CMD_SET_UNIFORM = 0x75,
  CMD_SET_BUFFER_OFFSET = 0x76,

  // --- 0x80+: Debug & Telemetry ---
  CMD_PRINT = 0x80,
  CMD_DUMP = 0x81
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
  Buffer *buffer;   // If NULL, just prints message
  uint32_t offset;  // Offset in buffer to peek at
  char message[44]; // Static message (Total struct size: 64 bytes)
} CmdPrint;

typedef struct CmdDump {
  CmdHeader header;
  Buffer *buffer;
  uint32_t offset;
  uint32_t count;   // Number of elements to print
  uint32_t stride;  // Type: 0=float, 1=int, 2=uint
  char message[32]; // Prefix message (Struct total: 64 bytes)
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
  uint32_t reg;       // The data register (to load into or store from)
  uint32_t index_reg; // The register holding the array index
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
  int32_t location; // glGetUniformLocation result
  uint32_t reg;     // Register index (0-7)
  uint32_t type;    // 0=float, 1=int, 2=uint
} CmdSetUniform;

typedef struct CmdDrawIndirectCount {
  CmdHeader header;
  Buffer *buffer;         // The command data (Draw structs)
  Buffer *count_buffer;   // The buffer containing the single uint32 count
  int32_t offset;         // Offset in buffer
  int32_t count_offset;   // Offset in count_buffer
  int32_t max_draw_count; // Safety limit
  int32_t stride;
} CmdDrawIndirectCount;

typedef struct CmdBindSetDraw {
  CmdHeader header;
  DescriptorSet *set;
  int32_t vertex_count;
  int32_t instance_count;
  int32_t first;
  int32_t _pad;
} CmdBindSetDraw;

typedef struct CmdJumpTable {
  CmdHeader header;
  uint32_t reg;       // The register holding the index
  uint32_t count;     // Number of slots in the table
  uint32_t targets[]; // Absolute offsets in bytecode
} CmdJumpTable;

typedef struct CmdAssertReg {
  CmdHeader header;
  uint32_t reg;   // Register to check
  uint32_t value; // Value to compare against
  uint32_t op;    // 0:==, 1:!=, 2:<, 3:>, 4:<=, 5:>=
} CmdAssertReg;

typedef struct CmdGenRand {
  CmdHeader header;
  uint32_t reg; // Target register to store the random number
} CmdGenRand;

typedef struct CmdGetTime {
  CmdHeader header;
  uint32_t reg;
} CmdGetTime;

typedef struct CmdCmp {
  CmdHeader header;
  uint32_t reg_dest; // Where to store the result (0 or 1)
  uint32_t reg_a;    // Left side
  uint32_t reg_b;    // Right side
  uint32_t op;       // 0:==, 1:!=, 2:<, 3:>, 4:<=, 5:>=
} CmdCmp;

typedef struct CmdSinCos {
  CmdHeader header;
  uint32_t reg_in;  // Input (float radians)
  uint32_t reg_sin; // Output Sine (float)
  uint32_t reg_cos; // Output Cosine (float)
} CmdSinCos;

typedef struct CmdBindSetDrawIndirect {
  CmdHeader header;
  DescriptorSet *set; // Pointer 1
  Buffer *buffer;     // Pointer 2 (Indirect data)
  int32_t count;
  int32_t offset;
  int32_t stride;
  int32_t _pad;
} CmdBindSetDrawIndirect;

typedef struct CmdSetBufferOffset {
  CmdHeader header;
  Buffer *buffer;   // The buffer to bind
  uint32_t slot;    // Binding index (0-MAX)
  uint32_t reg_off; // Register holding the byte offset
  uint32_t size;    // Fixed size of the range
  uint32_t type;    // 0: Uniform Buffer, 1: Storage Buffer
} CmdSetBufferOffset;

typedef struct CmdGetStat {
  CmdHeader header;
  uint32_t reg;     // Dest register
  uint32_t stat_id; // 0:draws, 1:pipe_swaps, 2:set_swaps, 3:dispatch
} CmdGetStat;

typedef struct CmdCopyBuffer {
  CmdHeader header;
  Buffer *src;       // Pointer 1
  Buffer *dst;       // Pointer 2
  uint32_t reg_src;  // i[reg] = source byte offset
  uint32_t reg_dst;  // i[reg] = destination byte offset
  uint32_t reg_size; // i[reg] = byte size to copy
} CmdCopyBuffer;

typedef struct CommandBuffer {
  PyObject_HEAD Context *ctx;
  char name[64];

  // The Bytecode
  uint8_t *data;
  size_t size;
  size_t capacity;

  // Object Lifecycle Management
  // We must keep references to every Pipeline, Buffer, etc. used in the
  // recording so they don't get GC'd while this buffer exists.
  PyObject *ref_list;
  // --- LABEL SYSTEM ---
  PyObject *labels; // Dictionary: { "name": offset }
  PyObject *fixups; // List of tuples: (bytecode_offset_to_patch, "label_name")
  int recording;
} CommandBuffer;

typedef enum {
  HGL_STATUS_OK = 0,
  HGL_STATUS_ERR_STACK_OVER = 1,
  HGL_STATUS_ERR_STACK_UNDER = 2,
  HGL_STATUS_ERR_INVALID_OP = 3,
  HGL_STATUS_ERR_SIGNAL = 4, // Ctrl+C
  HGL_STATUS_ERR_NESTED_LIMIT = 5,
  HGL_STATUS_ERR_UNFINISHED = 6,
  HGL_STATUS_ERR_BUDGET = 7,
  HGL_STATUS_ERR_ASSERT = 8
} HGLStatus;

// MISC defs

static constexpr size_t SHARED_TRASH_INITIAL_CAPACITY = 4096;

static constexpr size_t INDEX_TYPE_NONE = 0;