// MIT License

// Copyright (c) 2024 Szabolcs Dombi

// -----------------------------------------------------------------------------
// Includes & Macros
// -----------------------------------------------------------------------------

#include "hypergl_defs.h"

#ifndef EXTERN_GL
// This defines a static function pointer with the correct calling convention
#define RESOLVE(type, name, ...) static type(GL_API *name)(__VA_ARGS__)
#else
// This handles external linking, mapping the name to a prefixed version
#define RESOLVE(type, name, ...)                                               \
  extern type GL_API name(__VA_ARGS__) __asm__("hypergl_" #name)
#endif

// -----------------------------------------------------------------------------
// OpenGL Function Pointers (RESOLVE)
// -----------------------------------------------------------------------------

// -- State Management & Clearing --
RESOLVE(void, glCullFace, int);
RESOLVE(void, glClear, int);
RESOLVE(void, glDepthMask, int);
RESOLVE(void, glDisable, int);
RESOLVE(void, glEnable, int);
RESOLVE(void, glFlush);
RESOLVE(void, glDepthFunc, int);
RESOLVE(void, glGetIntegerv, int, int *);
RESOLVE(const char *, glGetString, int);
RESOLVE(int, glGetError);
RESOLVE(void, glViewport, int, int, int, int);
RESOLVE(void, glPixelStorei, int, int);

// -- Textures & Samplers --
RESOLVE(void, glTexParameteri, int, int, int);
RESOLVE(void, glTexImage2D, int, int, int, int, int, int, int, int,
        const void *);
RESOLVE(void, glTexSubImage2D, int, int, int, int, int, int, int, int,
        const void *);
RESOLVE(void, glBindTexture, int, int);
RESOLVE(void, glDeleteTextures, int, const unsigned int *);
RESOLVE(void, glGenTextures, int, GLuint *);
RESOLVE(void, glTexImage3D, int, int, int, int, int, int, int, int, int,
        const void *);
RESOLVE(void, glTexSubImage3D, int, int, int, int, int, int, int, int, int, int,
        const void *);
RESOLVE(void, glActiveTexture, int);
RESOLVE(void, glGenerateMipmap, int);
RESOLVE(void, glGenSamplers, int, int *);
RESOLVE(void, glDeleteSamplers, int, const GLuint *);
RESOLVE(void, glBindSampler, int, int);
RESOLVE(void, glSamplerParameteri, int, int, int);
RESOLVE(void, glSamplerParameterf, int, int, float);
RESOLVE(void, glBindImageTexture, int, int, int, int, int, int, int);

// -- Buffers (VBO, UBO, SSBO) --
RESOLVE(void, glReadBuffer, int);
RESOLVE(void, glReadPixels, int, int, int, int, int, int, void *);
RESOLVE(void, glBindBuffer, int, int);
RESOLVE(void, glDeleteBuffers, int, const unsigned int *);
RESOLVE(void, glGenBuffers, int, GLuint *);
RESOLVE(void, glBufferData, int, intptr, const void *, int);
RESOLVE(void, glBufferStorage, int, intptr, const void *, int);
RESOLVE(void, glBufferSubData, int, intptr, intptr, const void *);
RESOLVE(void, glGetBufferSubData, int, intptr, intptr, void *);
RESOLVE(void, glBindBufferRange, int, int, int, intptr, intptr);
RESOLVE(void, glClearBufferiv, int, int, const void *);
RESOLVE(void, glClearBufferuiv, int, int, const void *);
RESOLVE(void, glClearBufferfv, int, int, const void *);
RESOLVE(void, glClearBufferfi, int, int, float, int);
RESOLVE(void, glCopyBufferSubData, int, int, intptr, intptr, intptr);
RESOLVE(void, glBindBufferBase, int, int, int);
RESOLVE(void *, glMapBufferRange, int, intptr, intptr, int);
RESOLVE(int, glUnmapBuffer, int);
RESOLVE(void, glGetBufferParameteriv, unsigned int, unsigned int, int *);

// -- Blending & Stencil --
RESOLVE(void, glBlendFuncSeparate, int, int, int, int);
RESOLVE(void, glBlendEquationSeparate, int, int);
RESOLVE(void, glStencilOpSeparate, int, int, int, int);
RESOLVE(void, glStencilFuncSeparate, int, int, int, int);
RESOLVE(void, glStencilMaskSeparate, int, int);

// -- Shaders & Programs --
RESOLVE(void, glAttachShader, int, int);
RESOLVE(void, glDetachShader, int, int);
RESOLVE(void, glCompileShader, int);
RESOLVE(int, glCreateProgram);
RESOLVE(int, glCreateShader, int);
RESOLVE(void, glDeleteProgram, unsigned int);
RESOLVE(void, glDeleteShader, unsigned int);
RESOLVE(void, glGetProgramiv, int, int, int *);
RESOLVE(void, glGetProgramInfoLog, int, int, int *, char *);
RESOLVE(void, glGetShaderiv, int, int, int *);
RESOLVE(void, glGetShaderInfoLog, int, int, int *, char *);
RESOLVE(void, glLinkProgram, int);
RESOLVE(void, glShaderSource, int, int, const void *, const int *);
RESOLVE(void, glUseProgram, int);
RESOLVE(void, glDispatchCompute, int, int, int);
RESOLVE(void, glMemoryBarrier, int);
RESOLVE(void, glGetProgramInterfaceiv, int, int, int, int *);
RESOLVE(void, glGetProgramResourceiv, int, int, int, int, const int *, int,
        int *, int *);
RESOLVE(void, glGetProgramResourceName, int, int, int, int, int *, char *);

// -- Attributes & Uniforms --
RESOLVE(void, glEnableVertexAttribArray, int);
RESOLVE(void, glGetActiveAttrib, int, int, int, int *, int *, int *, char *);
RESOLVE(void, glGetActiveUniform, int, int, int, int *, int *, int *, char *);
RESOLVE(int, glGetAttribLocation, int, const char *);
RESOLVE(int, glGetUniformLocation, int, const char *);
RESOLVE(void, glVertexAttribPointer, int, int, int, int, int, intptr);
RESOLVE(void, glVertexAttribIPointer, int, int, int, int, intptr);
RESOLVE(void, glVertexAttribDivisor, int, int);
RESOLVE(int, glGetUniformBlockIndex, int, const char *);
RESOLVE(void, glGetActiveUniformBlockiv, int, int, int, int *);
RESOLVE(void, glGetActiveUniformBlockName, int, int, int, int *, char *);
RESOLVE(void, glUniformBlockBinding, GLuint, GLuint, GLuint);

// -- Uniform Setting --
RESOLVE(void, glUniform1i, int, int);
RESOLVE(void, glUniform1fv, int, int, const void *);
RESOLVE(void, glUniform2fv, int, int, const void *);
RESOLVE(void, glUniform3fv, int, int, const void *);
RESOLVE(void, glUniform4fv, int, int, const void *);
RESOLVE(void, glUniform1iv, int, int, const void *);
RESOLVE(void, glUniform2iv, int, int, const void *);
RESOLVE(void, glUniform3iv, int, int, const void *);
RESOLVE(void, glUniform4iv, int, int, const void *);
RESOLVE(void, glUniform1uiv, int, int, const void *);
RESOLVE(void, glUniform2uiv, int, int, const void *);
RESOLVE(void, glUniform3uiv, int, int, const void *);
RESOLVE(void, glUniform4uiv, int, int, const void *);
RESOLVE(void, glUniformMatrix2fv, int, int, int, const void *);
RESOLVE(void, glUniformMatrix3fv, int, int, int, const void *);
RESOLVE(void, glUniformMatrix4fv, int, int, int, const void *);
RESOLVE(void, glUniformMatrix2x3fv, int, int, int, const void *);
RESOLVE(void, glUniformMatrix3x2fv, int, int, int, const void *);
RESOLVE(void, glUniformMatrix2x4fv, int, int, int, const void *);
RESOLVE(void, glUniformMatrix4x2fv, int, int, int, const void *);
RESOLVE(void, glUniformMatrix3x4fv, int, int, int, const void *);
RESOLVE(void, glUniformMatrix4x3fv, int, int, int, const void *);

// -- Framebuffers --
RESOLVE(void, glBindRenderbuffer, int, int);
RESOLVE(void, glDeleteRenderbuffers, int, const unsigned int *);
RESOLVE(void, glGenRenderbuffers, int, GLuint *);
RESOLVE(void, glBindFramebuffer, int, int);
RESOLVE(void, glDeleteFramebuffers, int, const unsigned int *);
RESOLVE(void, glGenFramebuffers, int, int *);
RESOLVE(void, glFramebufferTexture2D, int, int, int, int, int);
RESOLVE(void, glFramebufferRenderbuffer, int, int, int, int);
RESOLVE(void, glBlitFramebuffer, int, int, int, int, int, int, int, int, int,
        int);
RESOLVE(void, glRenderbufferStorageMultisample, int, int, int, int, int);
RESOLVE(void, glFramebufferTextureLayer, int, int, int, int, int);
RESOLVE(void, glDrawBuffers, int, const int *);

// -- Vertex Arrays & Drawing --
RESOLVE(void, glBindVertexArray, int);
RESOLVE(void, glDeleteVertexArrays, int, const unsigned int *);
RESOLVE(void, glGenVertexArrays, int, int *);
RESOLVE(void, glDrawArraysInstanced, int, int, int, int);
RESOLVE(void, glDrawElementsInstanced, int, int, int, intptr, int);
RESOLVE(void, glMultiDrawArraysIndirect, int, const void *, int, int);
RESOLVE(void, glMultiDrawElementsIndirect, int, int, const void *, int, int);

// -- Queries & Extensions --
RESOLVE(void, glDeleteQueries, int, const unsigned int *);
RESOLVE(GLuint64, glGetTextureHandleARB, int);
RESOLVE(void, glMakeTextureHandleResidentARB, GLuint64);
RESOLVE(void, glMakeTextureHandleNonResidentARB, GLuint64);

// -----------------------------------------------------------------------------
// OpenGL Loader Logic
// -----------------------------------------------------------------------------

#ifndef EXTERN_GL

static void *load_opengl_function(PyObject *loader_function,
                                  const char *method) {
  PyObject *res = PyObject_CallFunction(loader_function, "(s)", method);
  if (!res) {
    return NULL;
  }
  void *ptr = PyLong_AsVoidPtr(res);
  if (!ptr && PyErr_Occurred()) {
    Py_DECREF(res);
    return NULL;
  }
  Py_DECREF(res);
  return ptr;
}

static int load_gl(PyObject *loader) {
  PyObject *loader_function =
      PyObject_GetAttrString(loader, "load_opengl_function");

  if (!loader_function) {
    PyErr_Format(PyExc_ValueError, "invalid loader");
    return -1;
  }

  PyObject *missing = PyList_New(0);

#define check(name)                                                            \
  do {                                                                         \
    if (!(name)) {                                                             \
      PyObject *_hgl_str = PyUnicode_FromString(#name);                        \
      if (_hgl_str)                                                            \
        PyList_Append(missing, _hgl_str);                                      \
      Py_XDECREF(_hgl_str);                                                    \
    }                                                                          \
  } while (0)

#define load(name)                                                             \
  do {                                                                         \
    void *temp_ptr = load_opengl_function(loader_function, #name);             \
    if (!temp_ptr && PyErr_Occurred()) {                                       \
      Py_DECREF(loader_function);                                              \
      Py_DECREF(missing);                                                      \
      return -1;                                                               \
    }                                                                          \
    memcpy((void *)&(name), (const void *)&temp_ptr, sizeof(void *));          \
    check(name);                                                               \
  } while (0)

  load(glCullFace);
  load(glClear);
  load(glTexParameteri);
  load(glTexImage2D);
  load(glDepthMask);
  load(glDisable);
  load(glEnable);
  load(glFlush);
  load(glDepthFunc);
  load(glReadBuffer);
  load(glReadPixels);
  load(glGetError);
  load(glGetIntegerv);
  load(glGetString);
  load(glViewport);
  load(glTexSubImage2D);
  load(glBindTexture);
  load(glDeleteTextures);
  load(glGenTextures);
  load(glTexImage3D);
  load(glTexSubImage3D);
  load(glActiveTexture);
  load(glBlendFuncSeparate);
  load(glBindBuffer);
  load(glDeleteBuffers);
  load(glGenBuffers);
  load(glBufferData);
  load(glBufferStorage);
  load(glBufferSubData);
  load(glGetBufferSubData);
  load(glBlendEquationSeparate);
  load(glDrawBuffers);
  load(glStencilOpSeparate);
  load(glStencilFuncSeparate);
  load(glStencilMaskSeparate);
  load(glAttachShader);
  load(glDetachShader);
  load(glCompileShader);
  load(glCreateProgram);
  load(glCreateShader);
  load(glDeleteProgram);
  load(glDeleteShader);
  load(glDeleteQueries);
  load(glEnableVertexAttribArray);
  load(glGetActiveAttrib);
  load(glGetActiveUniform);
  load(glGetAttribLocation);
  load(glGetProgramiv);
  load(glGetProgramInfoLog);
  load(glGetShaderiv);
  load(glGetShaderInfoLog);
  load(glGetUniformLocation);
  load(glLinkProgram);
  load(glShaderSource);
  load(glUseProgram);
  load(glUniform1i);
  load(glUniform1fv);
  load(glUniform2fv);
  load(glUniform3fv);
  load(glUniform4fv);
  load(glUniform1iv);
  load(glUniform2iv);
  load(glUniform3iv);
  load(glUniform4iv);
  load(glUniformMatrix2fv);
  load(glUniformMatrix3fv);
  load(glUniformMatrix4fv);
  load(glVertexAttribPointer);
  load(glUniformMatrix2x3fv);
  load(glUniformMatrix3x2fv);
  load(glUniformMatrix2x4fv);
  load(glUniformMatrix4x2fv);
  load(glUniformMatrix3x4fv);
  load(glUniformMatrix4x3fv);
  load(glBindBufferRange);
  load(glVertexAttribIPointer);
  load(glUniform1uiv);
  load(glUniform2uiv);
  load(glUniform3uiv);
  load(glUniform4uiv);
  load(glClearBufferiv);
  load(glClearBufferuiv);
  load(glClearBufferfv);
  load(glClearBufferfi);
  load(glBindRenderbuffer);
  load(glDeleteRenderbuffers);
  load(glGenRenderbuffers);
  load(glBindFramebuffer);
  load(glDeleteFramebuffers);
  load(glGenFramebuffers);
  load(glFramebufferTexture2D);
  load(glFramebufferRenderbuffer);
  load(glGenerateMipmap);
  load(glBlitFramebuffer);
  load(glRenderbufferStorageMultisample);
  load(glFramebufferTextureLayer);
  load(glBindVertexArray);
  load(glDeleteVertexArrays);
  load(glGenVertexArrays);
  load(glDrawArraysInstanced);
  load(glDrawElementsInstanced);
  load(glCopyBufferSubData);
  load(glGetUniformBlockIndex);
  load(glGetActiveUniformBlockiv);
  load(glGetActiveUniformBlockName);
  load(glUniformBlockBinding);
  load(glGenSamplers);
  load(glDeleteSamplers);
  load(glBindSampler);
  load(glSamplerParameteri);
  load(glSamplerParameterf);
  load(glVertexAttribDivisor);
  load(glDispatchCompute);
  load(glMemoryBarrier);
  load(glBindImageTexture);
  load(glBindBufferBase);
  load(glMapBufferRange);
  load(glUnmapBuffer);
  load(glPixelStorei);
  load(glGetProgramInterfaceiv);
  load(glGetProgramResourceiv);
  load(glGetProgramResourceName);
  load(glGetBufferParameteriv);

#define load_optional(name)                                                    \
  do {                                                                         \
    void *_opt = load_opengl_function(loader_function, #name);                 \
    if (!_opt && PyErr_Occurred()) {                                           \
      PyErr_Clear();                                                           \
    }                                                                          \
    memcpy((void *)&(name), (const void *)&_opt, sizeof(void *));              \
  } while (0)

  load_optional(glGetTextureHandleARB);
  load_optional(glMakeTextureHandleResidentARB);
  load_optional(glMakeTextureHandleNonResidentARB);
  load_optional(glMultiDrawArraysIndirect);
  load_optional(glMultiDrawElementsIndirect);

#undef load
#undef load_optional
#undef check

  Py_DECREF(loader_function);

  if (PyList_Size(missing) > 0) {
    PyErr_Format(PyExc_RuntimeError, "cannot load opengl functions: %R",
                 missing);
    Py_DECREF(missing);
    return -1;
  }

  Py_DECREF(missing);
  return 0;
}

#else

static void load_gl(PyObject *loader) {}

#endif

#define INTERNAL_CHECK(condition, message)                                     \
  if (!(condition)) {                                                          \
    PyErr_SetString(PyExc_RuntimeError, "HyperGL Internal Error: " message);   \
    return NULL;                                                               \
  }

#define INTERNAL_CHECK_LOCKED(condition, message, lock)                        \
  if (!(condition)) {                                                          \
    PyMutex_Unlock(lock);                                                      \
    PyErr_SetString(PyExc_RuntimeError, "HyperGL Internal Error: " message);   \
    return NULL;                                                               \
  }

// -----------------------------------------------------------------------------
// Helper Functions (State Binding & Internals)
// -----------------------------------------------------------------------------

static FORCE_INLINE void zeromem(void *NO_ALIAS data, int size) {
  // LLVM optimizes memset to SIMD instructions for known sizes
  memset(data, 0, size);
}

// -----------------------------------------------------------------------------
// Uniform Binding Helpers & Dispatch Table
// -----------------------------------------------------------------------------

// -- Wrappers --

// Integers
static void u_1iv(GLint l, GLsizei c, const void *p) { glUniform1iv(l, c, p); }
static void u_2iv(GLint l, GLsizei c, const void *p) { glUniform2iv(l, c, p); }
static void u_3iv(GLint l, GLsizei c, const void *p) { glUniform3iv(l, c, p); }
static void u_4iv(GLint l, GLsizei c, const void *p) { glUniform4iv(l, c, p); }

// Unsigned Integers
static void u_1uiv(GLint l, GLsizei c, const void *p) { glUniform1uiv(l, c, p); }
static void u_2uiv(GLint l, GLsizei c, const void *p) { glUniform2uiv(l, c, p); }
static void u_3uiv(GLint l, GLsizei c, const void *p) { glUniform3uiv(l, c, p); }
static void u_4uiv(GLint l, GLsizei c, const void *p) { glUniform4uiv(l, c, p); }

// Floats
static void u_1fv(GLint l, GLsizei c, const void *p) { glUniform1fv(l, c, p); }
static void u_2fv(GLint l, GLsizei c, const void *p) { glUniform2fv(l, c, p); }
static void u_3fv(GLint l, GLsizei c, const void *p) { glUniform3fv(l, c, p); }
static void u_4fv(GLint l, GLsizei c, const void *p) { glUniform4fv(l, c, p); }

// Matrices (Always Transpose = GL_FALSE)
// GLSL uses column-major matrices; transpose must be GL_FALSE per spec
static void u_mat2(GLint l, GLsizei c, const void *p)   { glUniformMatrix2fv(l, c, GL_FALSE, p); }
static void u_mat2x3(GLint l, GLsizei c, const void *p) { glUniformMatrix2x3fv(l, c, GL_FALSE, p); }
static void u_mat2x4(GLint l, GLsizei c, const void *p) { glUniformMatrix2x4fv(l, c, GL_FALSE, p); }
static void u_mat3x2(GLint l, GLsizei c, const void *p) { glUniformMatrix3x2fv(l, c, GL_FALSE, p); }
static void u_mat3(GLint l, GLsizei c, const void *p)   { glUniformMatrix3fv(l, c, GL_FALSE, p); }
static void u_mat3x4(GLint l, GLsizei c, const void *p) { glUniformMatrix3x4fv(l, c, GL_FALSE, p); }
static void u_mat4x2(GLint l, GLsizei c, const void *p) { glUniformMatrix4x2fv(l, c, GL_FALSE, p); }
static void u_mat4x3(GLint l, GLsizei c, const void *p) { glUniformMatrix4x3fv(l, c, GL_FALSE, p); }
static void u_mat4(GLint l, GLsizei c, const void *p)   { glUniformMatrix4fv(l, c, GL_FALSE, p); }

// -- Dispatch Table --

static const UniformUploadFn uniform_upload_table[UF_COUNT] = {
    [UF_1I] = u_1iv,   [UF_2I] = u_2iv,   [UF_3I] = u_3iv,   [UF_4I] = u_4iv,
    [UF_1B] = u_1iv,   [UF_2B] = u_2iv,   [UF_3B] = u_3iv,   [UF_4B] = u_4iv,

    [UF_1U] = u_1uiv,  [UF_2U] = u_2uiv,  [UF_3U] = u_3uiv,  [UF_4U] = u_4uiv,
    [UF_1F] = u_1fv,   [UF_2F] = u_2fv,   [UF_3F] = u_3fv,   [UF_4F] = u_4fv,

    [UF_MAT2]   = u_mat2,   [UF_MAT2x3] = u_mat2x3, [UF_MAT2x4] = u_mat2x4,
    [UF_MAT3x2] = u_mat3x2, [UF_MAT3]   = u_mat3,   [UF_MAT3x4] = u_mat3x4,
    [UF_MAT4x2] = u_mat4x2, [UF_MAT4x3] = u_mat4x3, [UF_MAT4]   = u_mat4,
};

// Ensure the table covers all enum values at compile time
_Static_assert(
    sizeof(uniform_upload_table) / sizeof(uniform_upload_table[0]) == UF_COUNT,
    "uniform_upload_table must match UniformFunction enum"
);

static void bind_uniforms(const Pipeline *self) {
  const UniformHeader *const header =
      (UniformHeader *)self->uniform_layout_buffer.buf;
  const char *const data = (char *)self->uniform_data_buffer.buf;

  for (int i = 0; i < header->count; ++i) {
    const UniformFunction func = (UniformFunction)header->binding[i].function;
    const void *ptr = data + header->binding[i].offset;

    if (LIKELY(
        func >= 0 && 
        func < UF_COUNT && 
        uniform_upload_table[func] != NULL
    )) {
      uniform_upload_table[func](
          (GLint)header->binding[i].location,
          (GLsizei)header->binding[i].count,
          ptr
      );
    } else {
#ifdef DEBUG
      fprintf(stderr,
          "[HyperGL] Invalid or unbound uniform function: %d\n",
          func
      );
      abort();
#endif
    }
  }
}

static FORCE_INLINE void bind_viewport_internal(Context *self,
                                                const Viewport *viewport) {
  Viewport *c = &self->current_viewport;
  if (viewport->x != c->x || viewport->y != c->y ||
      viewport->width != c->width || viewport->height != c->height) {
    glViewport(viewport->x, viewport->y, viewport->width, viewport->height);
    self->current_viewport = *viewport;
  }
}

UNUSED static FORCE_INLINE void bind_viewport(Context *self,
                                              const Viewport *viewport) {
  PyMutex_Lock(&self->state_lock);
  bind_viewport_internal(self, viewport);
  PyMutex_Unlock(&self->state_lock);
}

static FORCE_INLINE void
bind_global_settings_internal(Context *self, GlobalSettings *settings) {
  if (self->current_global_settings == settings) {
    return;
  }

  if (settings->cull_face) {
    glEnable(GL_CULL_FACE);
    glCullFace(settings->cull_face);
  } else {
    glDisable(GL_CULL_FACE);
  }

  if (settings->depth_enabled) {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(settings->depth_func);
    glDepthMask(settings->depth_write);
    self->current_depth_mask = settings->depth_write;
  } else {
    glDisable(GL_DEPTH_TEST);
  }

  if (settings->stencil_enabled) {
    glEnable(GL_STENCIL_TEST);
    glStencilMaskSeparate(GL_FRONT, settings->stencil_front.write_mask);
    glStencilMaskSeparate(GL_BACK, settings->stencil_back.write_mask);
    glStencilFuncSeparate(GL_FRONT, settings->stencil_front.compare_op,
                          settings->stencil_front.reference,
                          settings->stencil_front.compare_mask);
    glStencilFuncSeparate(GL_BACK, settings->stencil_back.compare_op,
                          settings->stencil_back.reference,
                          settings->stencil_back.compare_mask);
    glStencilOpSeparate(GL_FRONT, settings->stencil_front.fail_op,
                        settings->stencil_front.depth_fail_op,
                        settings->stencil_front.pass_op);
    glStencilOpSeparate(GL_BACK, settings->stencil_back.fail_op,
                        settings->stencil_back.depth_fail_op,
                        settings->stencil_back.pass_op);
    self->current_stencil_mask = settings->stencil_front.write_mask;
  } else {
    glDisable(GL_STENCIL_TEST);
  }

  if (settings->blend_enabled) {
    glEnable(GL_BLEND);
    glBlendEquationSeparate(settings->blend.op_color, settings->blend.op_alpha);
    glBlendFuncSeparate(settings->blend.src_color, settings->blend.dst_color,
                        settings->blend.src_alpha, settings->blend.dst_alpha);
  } else {
    glDisable(GL_BLEND);
  }

  Py_XINCREF(settings);
  Py_XSETREF(self->current_global_settings, settings);
}

UNUSED static FORCE_INLINE void bind_global_settings(Context *self,
                                                     GlobalSettings *settings) {
  PyMutex_Lock(&self->state_lock);
  bind_global_settings_internal(self, settings);
  PyMutex_Unlock(&self->state_lock);
}

static FORCE_INLINE void bind_read_framebuffer_internal(Context *self,
                                                        int framebuffer) {
  if (UNLIKELY(self->current_read_framebuffer != framebuffer)) {
    self->current_read_framebuffer = framebuffer;
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
  }
}

static FORCE_INLINE void bind_read_framebuffer(Context *self, int framebuffer) {
  PyMutex_Lock(&self->state_lock);
  bind_read_framebuffer_internal(self, framebuffer);
  PyMutex_Unlock(&self->state_lock);
}

static FORCE_INLINE void bind_draw_framebuffer_internal(Context *self,
                                                        int framebuffer) {
  if (UNLIKELY(self->current_draw_framebuffer != framebuffer)) {
    self->current_draw_framebuffer = framebuffer;
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
  }
}

static FORCE_INLINE void bind_draw_framebuffer(Context *self, int framebuffer) {
  PyMutex_Lock(&self->state_lock);
  bind_draw_framebuffer_internal(self, framebuffer);
  PyMutex_Unlock(&self->state_lock);
}

static FORCE_INLINE void bind_program_internal(Context *self, int program) {
  if (UNLIKELY(self->current_program != program)) {
    self->current_program = program;
    glUseProgram(program);
  }
}

static FORCE_INLINE void bind_program(Context *self, int program) {
  PyMutex_Lock(&self->state_lock);
  bind_program_internal(self, program);
  PyMutex_Unlock(&self->state_lock);
}

static FORCE_INLINE void bind_vertex_array_internal(Context *self,
                                                    int vertex_array) {
  if (UNLIKELY(self->current_vertex_array != vertex_array)) {
    self->current_vertex_array = vertex_array;
    glBindVertexArray(vertex_array);
  }
}

static FORCE_INLINE void bind_vertex_array(Context *self, int vertex_array) {
  PyMutex_Lock(&self->state_lock);
  bind_vertex_array_internal(self, vertex_array);
  PyMutex_Unlock(&self->state_lock);
}

static FORCE_INLINE void bind_descriptor_set_internal(Context *self,
                                                      DescriptorSet *set) {
  if (self->current_descriptor_set == set) {
    return;
  }

  Py_XINCREF(set);
  Py_XDECREF(self->current_descriptor_set);
  self->current_descriptor_set = set;

  // Uniform Buffers
  for (int i = 0; i < set->uniform_buffers.binding_count; ++i) {
    if (set->uniform_buffers.binding[i].buffer) {
      glBindBufferRange(GL_UNIFORM_BUFFER, i,
                        set->uniform_buffers.binding[i].buffer->buffer,
                        set->uniform_buffers.binding[i].offset,
                        set->uniform_buffers.binding[i].size);
    }
  }

  // Storage Buffers
  for (int i = 0; i < set->storage_buffers.binding_count; ++i) {
    if (set->storage_buffers.binding[i].buffer) {
      glBindBufferRange(GL_SHADER_STORAGE_BUFFER, i,
                        set->storage_buffers.binding[i].buffer->buffer,
                        set->storage_buffers.binding[i].offset,
                        set->storage_buffers.binding[i].size);
    }
  }

  // Samplers
  for (int i = 0; i < set->samplers.binding_count; ++i) {
    if (set->samplers.binding[i].image) {
      glActiveTexture(GL_TEXTURE0 + i);
      glBindTexture(set->samplers.binding[i].image->target,
                    set->samplers.binding[i].image->image);
      glBindSampler(i, set->samplers.binding[i].sampler->obj);
    }
  }
}

static FORCE_INLINE void bind_descriptor_set(Context *self,
                                             DescriptorSet *set) {
  if (self->current_descriptor_set != set) {
    bind_descriptor_set_internal(self, set);
  }
}

// -----------------------------------------------------------------------------
// Utilities & Type Conversions
// -----------------------------------------------------------------------------

static int startswith(const char *str, const char *prefix) {
  if (!str || !prefix) {
    return 0;
  }
  while (*prefix) {
    if (*str == '\0' || *prefix != *str) {
      return 0;
    }
    prefix++;
    str++;
  }
  return 1;
}

static inline int to_int(PyObject *ptr) {
  PyObject *obj = ptr;
  if (!obj || obj == Py_None) {
    return 0;
  }

  const long val = PyLong_AsLong(obj);
  if (val == -1 && PyErr_Occurred()) {
    PyErr_Clear();
    return 0;
  }
  return (int)val;
}

static inline unsigned to_uint(PyObject *obj) {
  if (!obj || obj == Py_None) {
    return 0;
  }
  const unsigned long val = PyLong_AsUnsignedLong(obj);
  if (val == (unsigned long)-1 && PyErr_Occurred()) {
    PyErr_Clear();
    return 0;
  }
  return (unsigned)val;
}

static inline float to_float(PyObject *obj) {
  if (!obj || obj == Py_None) {
    return 0.0F;
  }
  const double val = PyFloat_AsDouble(obj);
  if (val == -1.0 && PyErr_Occurred()) {
    PyErr_Clear();
    return 0.0F;
  }
  return (float)val;
}

static inline int least_one(int value) { return value > 1 ? value : 1; }

#define FETCH_HELPER_DICT(st, helper, attr_name, out_var)                      \
  PyObject *out_var = PyObject_GetAttr(helper, (st)->str_##attr_name);         \
  if (!(out_var)) {                                                            \
    PyErr_Clear();                                                             \
    return 0;                                                                  \
  }

// Adapted to use ModuleState for caching
static int get_vertex_format(const ModuleState *state, PyObject *helper,
                             PyObject *name, VertexFormat *res) {
  if (!helper || !name || name == Py_None) {
    return 0;
  }

  FETCH_HELPER_DICT(state, helper, VERTEX_FORMAT, lookup);

  PyObject *tup = PyDict_GetItem(lookup, name);
  if (!tup || !PyTuple_Check(tup) || PyTuple_Size(tup) < 4) {
    goto fail;
  }

  PyObject *o0 = PyTuple_GetItem(tup, 0);
  PyObject *o1 = PyTuple_GetItem(tup, 1);
  PyObject *o2 = PyTuple_GetItem(tup, 2);
  PyObject *o3 = PyTuple_GetItem(tup, 3);

  if (!o0 || !o1 || !o2 || !o3) {
    goto fail;
  }

  res->type = to_int(o0);
  res->size = to_int(o1);
  res->normalize = to_int(o2);
  res->integer = to_int(o3);

  Py_DECREF(lookup);
  return 1;

fail:
  Py_DECREF(lookup);
  return 0;
}

// Adapted to use ModuleState for caching
static int get_image_format(const ModuleState *state, PyObject *helper,
                            PyObject *name, ImageFormat *res) {
  if (!helper || !name || name == Py_None) {
    return 0;
  }

  FETCH_HELPER_DICT(state, helper, IMAGE_FORMAT, lookup);

  PyObject *tup = PyDict_GetItem(lookup, name);
  if (!tup || !PyTuple_Check(tup) || PyTuple_Size(tup) < 9) {
    goto fail;
  }

  PyObject *items[9];
  for (int i = 0; i < 9; i++) {
    items[i] = PyTuple_GetItem(tup, i);
    if (!items[i]) {
      goto fail;
    }
  }

  res->internal_format = to_int(items[0]);
  res->format = to_int(items[1]);
  res->type = to_int(items[2]);
  res->buffer = to_int(items[3]);
  res->components = to_int(items[4]);
  res->pixel_size = to_int(items[5]);
  res->color = to_int(items[6]);
  res->flags = to_int(items[7]);

  res->clear_type = '\0';
  if (PyUnicode_Check(items[8])) {
    Py_ssize_t size;
    const char *str = PyUnicode_AsUTF8AndSize(items[8], &size);
    if (!str) {
      goto fail;
    }
    res->clear_type = (size > 0) ? str[0] : '\0';
  }

  Py_DECREF(lookup);
  return 1;

fail:
  Py_DECREF(lookup);
  return 0;
}

// Adapted to use ModuleState for caching
static int get_buffer_access(const ModuleState *state, PyObject *helper,
                             PyObject *name, int *res) {
  if (!helper || helper == Py_None || !name || name == Py_None) {
    return 0;
  }

  FETCH_HELPER_DICT(state, helper, BUFFER_ACCESS, lookup);

  PyObject *value = PyDict_GetItem(lookup, name);

  if (!value) {
    Py_DECREF(lookup);
    return 0;
  }

  int v = to_int(value);
  if (PyErr_Occurred()) {
    PyErr_Clear();
    Py_DECREF(lookup);
    return 0;
  }
  *res = v;
  Py_DECREF(lookup);
  return 1;
}

// Adapted to use ModuleState for caching
static int get_topology(const ModuleState *state, PyObject *helper,
                        PyObject *name, int *res) {
  if (!helper || helper == Py_None || !name || name == Py_None) {
    return 0;
  }

  FETCH_HELPER_DICT(state, helper, TOPOLOGY, lookup);

  PyObject *value = PyDict_GetItem(lookup, name);
  if (!value) {
    goto fail;
  }

  int overflow = 0;
  long val = PyLong_AsLongAndOverflow(value, &overflow);
  if (overflow || (val == -1 && PyErr_Occurred())) {
    goto fail;
  }

  if (val < INT_MIN || val > INT_MAX) {
    goto fail;
  }

  *res = (int)val;
  Py_DECREF(lookup);
  return 1;

fail:
  PyErr_Clear();
  Py_DECREF(lookup);
  return 0;
}

static int count_mipmaps(int width, int height) {
  if (width <= 0 || height <= 0) {
    return 0;
  }

  int size = width > height ? width : height;
  int levels = 0;

  while (size > 0) {
    levels++;
    size >>= 1;
  }
  return levels;
}

static void remove_dict_value(PyObject *dict, const PyObject *obj) {
  PyObject *key = NULL;
  PyObject *value = NULL;
  Py_ssize_t pos = 0;
  while (PyDict_Next(dict, &pos, &key, &value)) {
    if (value == obj) {
      PyDict_DelItem(dict, key);
      break;
    }
  }
}

static PyObject *new_ref(void *obj) { return Py_XNewRef((PyObject *)obj); }

UNUSED static int valid_mem(PyObject *mem, const Py_ssize_t size) {
  if (!PyMemoryView_Check(mem)) {
    return 0;
  }
  Py_buffer view;
  if (PyObject_GetBuffer(mem, &view, PyBUF_SIMPLE)) {
    return 0;
  }
  const int mem_size = (int)view.len;
  PyBuffer_Release(&view);
  return size < 0 || mem_size == size;
}

static int to_int_pair(IntPair *value, PyObject *obj, const int x,
                       const int y) {
  if (obj == Py_None) {
    value->x = x;
    value->y = y;
    return 1;
  }

  PyObject *seq =
      PySequence_Fast(obj, "[HyperGL] expected a sequence of 2 ints");
  if (!seq)
    return 0;

  if (PySequence_Fast_GET_SIZE(seq) != 2) {
    Py_DECREF(seq);
    return 0;
  }

  PyObject **items = PySequence_Fast_ITEMS(seq);

  value->x = to_int(items[0]);
  value->y = to_int(items[1]);

  Py_DECREF(seq);

  if (PyErr_Occurred()) {
    PyErr_Clear();
    return 0;
  }

  return 1;
}

static int to_viewport(Viewport *value, PyObject *obj, int x, int y, int width,
                       int height) {
  if (obj == Py_None) {
    value->x = x;
    value->y = y;
    value->width = width;
    value->height = height;
    return 1;
  }

  PyObject *seq =
      PySequence_Fast(obj, "[HyperGL] viewport must be a sequence of 4 ints");
  if (!seq)
    return 0;

  if (PySequence_Fast_GET_SIZE(seq) != 4) {
    Py_DECREF(seq);
    return 0;
  }

  PyObject **items = PySequence_Fast_ITEMS(seq);

  value->x = to_int(items[0]);
  value->y = to_int(items[1]);
  value->width = to_int(items[2]);
  value->height = to_int(items[3]);

  Py_DECREF(seq);

  if (PyErr_Occurred()) {
    PyErr_Clear();
    return 0;
  }

  return 1;
}

// -----------------------------------------------------------------------------
// Traverse & Clear (GC)
// -----------------------------------------------------------------------------

static int GLObject_traverse(GLObject *self, visitproc visit, void *arg) {
  Py_VISIT(Py_TYPE(self)); // Crucial: Visit the type itself in heap types
  Py_VISIT(self->extra);
  return 0;
}

static int GLObject_clear(GLObject *self) {
  Py_CLEAR(self->extra);
  return 0;
}

static int ImageFace_traverse(const ImageFace *self, visitproc visit,
                              void *arg) {
  Py_VISIT(self->ctx);
  Py_VISIT(self->image);
  Py_VISIT(self->framebuffer);
  Py_VISIT(self->size);
  return 0;
}

static int ImageFace_clear(ImageFace *self) {
  Py_CLEAR(self->ctx);
  Py_CLEAR(self->image);
  Py_CLEAR(self->framebuffer);
  Py_CLEAR(self->size);
  return 0;
}

// Helper for SharedTrash support
static void gl_object_init(const Context *restrict ctx, GLObject *restrict obj,
                           int id, int type) {
  obj->obj = id;
  obj->type = type;
  obj->uses = 1;
  obj->extra = NULL;
  obj->trash = ctx->trash_shared;
  if (obj->trash) {
    Atomic_Increment(&obj->trash->ref_count);
  }
}

// -----------------------------------------------------------------------------
// Builders (Framebuffers, VAOs, Samplers, Programs)
// -----------------------------------------------------------------------------

static GLObject *build_framebuffer(Context *self,
                                   PyObject *attachments) // HAS GC_TRACK
{
  PyObject *cache_obj = NULL;
  if (PyDict_GetItemRef(self->framebuffer_cache, attachments, &cache_obj) ==
      1) {
    GLObject *cache = (GLObject *)cache_obj;
    Atomic_Increment(&cache->uses);
    return cache;
  }

  PyObject *color_attachments = PyTuple_GetItem(attachments, 1);
  int color_attachment_count = (int)PyTuple_Size(color_attachments);
  if (color_attachment_count > MAX_ATTACHMENTS) {
    PyErr_Format(PyExc_ValueError, "Too many color attachments (max %d)",
                 MAX_ATTACHMENTS);
    return NULL;
  }
  PyObject *depth_stencil_attachment = PyTuple_GetItem(attachments, 2);

  int framebuffer = 0;

  PyMutex_Lock(&self->state_lock);

  // Double Check Cache (Optimization)
  if (PyDict_GetItemRef(self->framebuffer_cache, attachments, &cache_obj) ==
      1) {
    GLObject *cache = (GLObject *)cache_obj;
    Atomic_Increment(&cache->uses);
    PyMutex_Unlock(&self->state_lock);
    return cache; // caller now owns the reference
  }

  // Save State
  int prev_draw = self->current_draw_framebuffer;
  int prev_read = self->current_read_framebuffer;

  glGenFramebuffers(1, &framebuffer);

  // Bind using INTERNAL (unsafe) binders since we hold the lock
  bind_draw_framebuffer_internal(self, framebuffer);
  bind_read_framebuffer_internal(self, framebuffer);

  for (int i = 0; i < color_attachment_count; ++i) {
    ImageFace *const face = (ImageFace *)PyTuple_GetItem(color_attachments, i);
    if (face->image->renderbuffer) {
      glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                                GL_RENDERBUFFER, face->image->image);
    } else if (face->image->cubemap) {
      glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                             GL_TEXTURE_CUBE_MAP_POSITIVE_X + face->layer,
                             face->image->image, face->level);
    } else if (face->image->array) {
      glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                                face->image->image, face->level, face->layer);
    } else {
      glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                             GL_TEXTURE_2D, face->image->image, face->level);
    }
  }

  if (depth_stencil_attachment != Py_None) {
    ImageFace *face = (ImageFace *)depth_stencil_attachment;
    int buffer = face->image->fmt.buffer;
    int attachment = buffer == GL_DEPTH     ? GL_DEPTH_ATTACHMENT
                     : buffer == GL_STENCIL ? GL_STENCIL_ATTACHMENT
                                            : GL_DEPTH_STENCIL_ATTACHMENT;
    if (face->image->renderbuffer) {
      glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, attachment,
                                GL_RENDERBUFFER, face->image->image);
    } else if (face->image->cubemap) {
      glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attachment,
                             GL_TEXTURE_CUBE_MAP_POSITIVE_X + face->layer,
                             face->image->image, face->level);
    } else if (face->image->array) {
      glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, attachment,
                                face->image->image, face->level, face->layer);
    } else {
      glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attachment, GL_TEXTURE_2D,
                             face->image->image, face->level);
    }
  }

  int draw_buffers[MAX_ATTACHMENTS];
  for (int i = 0; i < color_attachment_count; ++i) {
    draw_buffers[i] = GL_COLOR_ATTACHMENT0 + i;
  }

  glDrawBuffers(color_attachment_count, draw_buffers);
  glReadBuffer(color_attachment_count ? GL_COLOR_ATTACHMENT0 : 0);

  // Restore State
  bind_draw_framebuffer_internal(self, prev_draw);
  bind_read_framebuffer_internal(self, prev_read);

  PyMutex_Unlock(&self->state_lock);

  GLObject *res = PyObject_GC_New(GLObject, self->module_state->GLObject_type);
  if (!res) {
    // Safe to delete without lock because it is currently unbound
    glDeleteFramebuffers(1, (const unsigned int *)&framebuffer);
    return NULL;
  }

  // Use init helper to link SharedTrash
  gl_object_init(self, res, framebuffer, TRASH_FRAMEBUFFER);

  PyObject_GC_Track(res);

  PyObject *existing = NULL;
  int set_status = PyDict_SetDefaultRef(self->framebuffer_cache, attachments,
                                        (PyObject *)res, &existing);

  if (set_status < 0) { // Error
    glDeleteFramebuffers(1, (const GLuint *)&res->obj);
    Py_DECREF(res);
    return NULL;
  }

  if (set_status > 0) {
    glDeleteFramebuffers(1, (const GLuint *)&res->obj);
    Py_DECREF(res);
    GLObject *winner = (GLObject *)existing;
    Atomic_Increment(&winner->uses);
    return winner;
  }
  return res;
}

static GLObject *build_vertex_array(Context *self,
                                    PyObject *bindings) // HAS GC_TRACK
{
  PyObject *cache_obj = NULL;
  // Check cache first (Fast path)
  if (PyDict_GetItemRef(self->vertex_array_cache, bindings, &cache_obj) == 1) {
    GLObject *cache = (GLObject *)cache_obj;
    Atomic_Increment(&cache->uses);
    return cache;
  }

  int length = (int)PyTuple_Size(bindings);
  PyObject *index_buffer = PyTuple_GetItem(bindings, 0);
  if (index_buffer != Py_None &&
      !PyObject_TypeCheck(index_buffer, self->module_state->Buffer_type)) {
    PyErr_SetString(PyExc_TypeError, "[HyperGL] index_buffer must be a Buffer");
    return NULL;
  }

  int vertex_array = 0;

  // Critical Section: Creation
  PyMutex_Lock(&self->state_lock);

  // Double-check cache inside lock
  if (PyDict_GetItemRef(self->vertex_array_cache, bindings, &cache_obj) == 1) {
    GLObject *cache = (GLObject *)cache_obj;
    Atomic_Increment(&cache->uses);
    PyMutex_Unlock(&self->state_lock);
    return cache;
  }

  int previous_vao = self->current_vertex_array;

  glGenVertexArrays(1, &vertex_array);
  // Use internal binder to update state tracking + GL
  bind_vertex_array_internal(self, vertex_array);

  for (int i = 1; i < length; i += 6) {
    Buffer *buffer = (Buffer *)PyTuple_GetItem(bindings, i + 0);
    int location = to_int(PyTuple_GetItem(bindings, i + 1));
    int offset = to_int(PyTuple_GetItem(bindings, i + 2));
    int stride = to_int(PyTuple_GetItem(bindings, i + 3));
    int divisor = to_int(PyTuple_GetItem(bindings, i + 4));
    VertexFormat fmt;

    if (!get_vertex_format(self->module_state, self->module_state->helper,
                           PyTuple_GetItem(bindings, i + 5), &fmt)) {
      glDeleteVertexArrays(1, (const unsigned int *)&vertex_array);
      bind_vertex_array_internal(self, previous_vao);
      PyMutex_Unlock(&self->state_lock);
      PyErr_Format(PyExc_ValueError, "invalid vertex format at index %d", i);
      return NULL;
    }

    glBindBuffer(GL_ARRAY_BUFFER, buffer->buffer);
    if (fmt.integer) {
      glVertexAttribIPointer(location, fmt.size, fmt.type, stride,
                             (intptr)offset);
    } else {
      glVertexAttribPointer(location, fmt.size, fmt.type, fmt.normalize, stride,
                            (intptr)offset);
    }
    glVertexAttribDivisor(location, divisor);
    glEnableVertexAttribArray(location);
  }

  if (index_buffer != Py_None) {
    // Element buffer binds to the currently bound VAO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ((Buffer *)index_buffer)->buffer);
  }

  // Restore state
  bind_vertex_array_internal(self, previous_vao);

  PyMutex_Unlock(&self->state_lock);

  GLObject *res = PyObject_GC_New(GLObject, self->module_state->GLObject_type);
  if (!res) {
    glDeleteVertexArrays(1, (const unsigned int *)&vertex_array);
    return NULL;
  }

  // Init with SharedTrash logic
  gl_object_init(self, res, vertex_array, TRASH_VERTEX_ARRAY);

  PyObject_GC_Track(res);

  PyObject *existing = NULL;
  int set_status = PyDict_SetDefaultRef(self->vertex_array_cache, bindings,
                                        (PyObject *)res, &existing);

  if (set_status < 0) { // Dict Error
    glDeleteVertexArrays(1, (const unsigned int *)&vertex_array);
    Py_DECREF(res);
    return NULL;
  }

  if (set_status > 0) {
    // Race lost
    glDeleteVertexArrays(1, (const unsigned int *)&vertex_array);
    Py_DECREF(res);

    GLObject *winner = (GLObject *)existing;
    Atomic_Increment(&winner->uses);
    return winner;
  }

  return res;
}

static GLObject *build_sampler(Context *self, PyObject *params) {
  PyObject *cache_obj = NULL;
  int found = PyDict_GetItemRef(self->sampler_cache, params, &cache_obj);
  if (found < 0) {
    return NULL;
  }
  if (found == 1) {
    GLObject *cache = (GLObject *)cache_obj;
    Atomic_Increment(&cache->uses);
    return cache;
  }

  if (!PyTuple_Check(params) || PyTuple_Size(params) < 11) {
    PyErr_Format(
        PyExc_TypeError,
        "sampler params must be a tuple of at least 11 elements (got %zd)",
        PyTuple_Check(params) ? PyTuple_Size(params) : 0);
    return NULL;
  }

  int sampler = 0;

  // Lock GL creation
  PyMutex_Lock(&self->state_lock);
  glGenSamplers(1, &sampler);

  glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER,
                      to_int(PyTuple_GetItem(params, 0)));
  glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER,
                      to_int(PyTuple_GetItem(params, 1)));
  glSamplerParameterf(sampler, GL_TEXTURE_MIN_LOD,
                      to_float(PyTuple_GetItem(params, 2)));
  glSamplerParameterf(sampler, GL_TEXTURE_MAX_LOD,
                      to_float(PyTuple_GetItem(params, 3)));

  float lod_bias = to_float(PyTuple_GetItem(params, 4));
  if (lod_bias != 0.0F) {
    glSamplerParameterf(sampler, GL_TEXTURE_LOD_BIAS, lod_bias);
  }

  glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S,
                      to_int(PyTuple_GetItem(params, 5)));
  glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T,
                      to_int(PyTuple_GetItem(params, 6)));
  glSamplerParameteri(sampler, GL_TEXTURE_WRAP_R,
                      to_int(PyTuple_GetItem(params, 7)));
  glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_MODE,
                      to_int(PyTuple_GetItem(params, 8)));
  glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_FUNC,
                      to_int(PyTuple_GetItem(params, 9)));

  float max_anisotropy = to_float(PyTuple_GetItem(params, 10));
  if (max_anisotropy != 1.0F) {
    glSamplerParameterf(sampler, GL_TEXTURE_MAX_ANISOTROPY, max_anisotropy);
  }
  PyMutex_Unlock(&self->state_lock);

  GLObject *res = PyObject_GC_New(GLObject, self->module_state->GLObject_type);
  if (!res) {
    PyMutex_Lock(&self->state_lock);
    glDeleteSamplers(1, (const GLuint *)&sampler);
    PyMutex_Unlock(&self->state_lock);
    return NULL;
  }

  gl_object_init(self, res, sampler, TRASH_SAMPLER);

  PyObject_GC_Track(res);

  PyObject *existing = NULL;
  int set_status = PyDict_SetDefaultRef(self->sampler_cache, params,
                                        (PyObject *)res, &existing);

  if (set_status < 0) {
    glDeleteSamplers(1, (const GLuint *)&res->obj);
    Py_DECREF(res);
    return NULL;
  }

  if (set_status == 1) {
    glDeleteSamplers(1, (const GLuint *)&res->obj);
    Py_DECREF(res);
    GLObject *winner = (GLObject *)existing;
    Atomic_Increment(&winner->uses);
    return winner;
  }

  return res;
}

static DescriptorSetBuffers build_descriptor_set_buffers(const Context *self,
                                                         PyObject *bindings) {
  DescriptorSetBuffers res;
  zeromem(&res, sizeof(res));

  if (!PyTuple_Check(bindings)) {
    PyErr_SetString(PyExc_TypeError,
                    "[HyperGL] buffer bindings must be a tuple");
    return res;
  }

  int length = (int)PyTuple_Size(bindings);
  if ((length % 4) != 0) {
    PyErr_SetString(PyExc_TypeError, "[HyperGL] buffer bindings must be "
                                     "(binding, buffer, offset, size) quads");
    return res;
  }

  for (int i = 0; i < length; i += 4) {
    int binding = to_int(PyTuple_GetItem(bindings, i + 0));
    if (PyErr_Occurred())
      goto error_cleanup;

    if (binding < 0 || binding >= MAX_BUFFER_BINDINGS) {
      PyErr_Format(
          PyExc_IndexError,
          "[HyperGL] Buffer binding %d exceeds MAX_BUFFER_BINDINGS (%d)",
          binding, MAX_BUFFER_BINDINGS);
      goto error_cleanup;
    }

    if (res.binding[binding].buffer) {
      PyErr_Format(PyExc_ValueError, "[HyperGL] duplicate buffer binding %d",
                   binding);
      goto error_cleanup;
    }

    PyObject *buf_obj = PyTuple_GetItem(bindings, i + 1);
    if (!PyObject_TypeCheck(buf_obj, self->module_state->Buffer_type)) {
      PyErr_SetString(PyExc_TypeError,
                      "[HyperGL] binding index 1 must be a Buffer object");
      goto error_cleanup;
    }

    int offset = to_int(PyTuple_GetItem(bindings, i + 2));
    int size = to_int(PyTuple_GetItem(bindings, i + 3));

    if (offset < 0 || size < 0) {
      PyErr_SetString(PyExc_ValueError,
                      "[HyperGL] buffer offset and size must be non-negative");
      goto error_cleanup;
    }

    if (PyErr_Occurred())
      goto error_cleanup;

    res.binding[binding].buffer = (Buffer *)new_ref(buf_obj);
    res.binding[binding].offset = offset;
    res.binding[binding].size = size;
    res.binding_count =
        (res.binding_count > binding + 1) ? res.binding_count : (binding + 1);
  }

  return res;

error_cleanup:
  for (int j = 0; j < MAX_BUFFER_BINDINGS; ++j) {
    Py_XDECREF(res.binding[j].buffer);
  }
  zeromem(&res, sizeof(res));
  return res;
}

static DescriptorSetSamplers build_descriptor_set_samplers(Context *self,
                                                           PyObject *bindings) {
  DescriptorSetSamplers res;
  zeromem(&res, sizeof(res));

  if (!PyTuple_Check(bindings)) {
    PyErr_SetString(PyExc_TypeError, "[HyperGL] bindings must be a tuple");
    return res;
  }

  int length = (int)PyTuple_Size(bindings);
  if ((length % 3) != 0) {
    PyErr_SetString(
        PyExc_TypeError,
        "[HyperGL] bindings must be triples of (binding, image, params)");
    return res;
  }

  for (int i = 0; i < length; i += 3) {
    int binding = to_int(PyTuple_GetItem(bindings, i + 0));
    if (PyErr_Occurred()) {
      goto error_cleanup;
    }

    if (binding < 0 || binding >= MAX_SAMPLER_BINDINGS) {
      PyErr_Format(
          PyExc_IndexError,
          "[HyperGL] Binding index %d exceeds MAX_SAMPLER_BINDINGS (%d)",
          binding, MAX_SAMPLER_BINDINGS);
      goto error_cleanup;
    }

    PyObject *img_obj = PyTuple_GetItem(bindings, i + 1);
    if (!PyObject_TypeCheck(img_obj, self->module_state->Image_type)) {
      PyErr_SetString(PyExc_TypeError,
                      "[HyperGL] binding index 1 must be an Image object");
      goto error_cleanup;
    }
    Image *image = (Image *)img_obj;

    GLObject *sampler = build_sampler(self, PyTuple_GetItem(bindings, i + 2));

    if (!sampler) {
      // build_sampler already set the Python error (e.g. out of memory or bad
      // params)
      goto error_cleanup;
    }

    if (res.binding[binding].sampler) {
      Py_DECREF(sampler);
      PyErr_Format(PyExc_ValueError, "[HyperGL] duplicate sampler binding %d",
                   binding);
      goto error_cleanup;
    }

    res.binding[binding].sampler = sampler;
    res.binding[binding].image = (Image *)new_ref(image);
    res.binding_count =
        res.binding_count > (binding + 1) ? res.binding_count : (binding + 1);
  }

  return res;

error_cleanup:
  // Rollback references to avoid leaks
  for (int j = 0; j < MAX_SAMPLER_BINDINGS; ++j) {
    Py_XDECREF(res.binding[j].sampler);
    Py_XDECREF(res.binding[j].image);
  }
  zeromem(&res, sizeof(res));
  return res;
}

static void descriptor_set_buffers_clear(DescriptorSetBuffers *buffers) {
  for (int i = 0; i < MAX_BUFFER_BINDINGS; ++i) {
    Py_XDECREF(buffers->binding[i].buffer);
    buffers->binding[i].buffer = NULL;
  }
  buffers->binding_count = 0;
}

static void descriptor_set_samplers_clear(DescriptorSetSamplers *samplers) {
  for (int i = 0; i < MAX_SAMPLER_BINDINGS; ++i) {
    Py_XDECREF(samplers->binding[i].sampler);
    Py_XDECREF(samplers->binding[i].image);
    samplers->binding[i].sampler = NULL;
    samplers->binding[i].image = NULL;
  }
  samplers->binding_count = 0;
}

static DescriptorSet *build_descriptor_set(Context *self, PyObject *bindings) {
  PyObject *cache;
  if (PyDict_GetItemRef(self->descriptor_set_cache, bindings, &cache) > 0) {
    DescriptorSet *ds = (DescriptorSet *)cache;
    Atomic_Increment(&ds->uses);
    return ds;
  }

  if (!PyTuple_Check(bindings) || PyTuple_Size(bindings) < 2) {
    PyErr_SetString(
        PyExc_TypeError,
        "[HyperGL] bindings must be a tuple of at least 2 elements");
    return NULL;
  }

  DescriptorSet *res =
      PyObject_New(DescriptorSet, self->module_state->DescriptorSet_type);
  if (!res)
    return NULL;
  zeromem(res, sizeof(*res));

  res->uniform_buffers =
      build_descriptor_set_buffers(self, PyTuple_GetItem(bindings, 0));
  if (PyErr_Occurred())
    goto error_cleanup;

  res->samplers =
      build_descriptor_set_samplers(self, PyTuple_GetItem(bindings, 1));
  if (PyErr_Occurred())
    goto error_cleanup;

  if (PyTuple_Size(bindings) > 2) {
    res->storage_buffers =
        build_descriptor_set_buffers(self, PyTuple_GetItem(bindings, 2));
    if (PyErr_Occurred())
      goto error_cleanup;
  } else {
    zeromem(&res->storage_buffers, sizeof(res->storage_buffers));
  }

  res->uses = 1;

  PyObject *existing = NULL;
  int result = PyDict_SetDefaultRef(self->descriptor_set_cache, bindings,
                                    (PyObject *)res, &existing);

  if (result < 0)
    goto error_cleanup;

  if (result > 0) {
    Py_DECREF(res);
    DescriptorSet *ds = (DescriptorSet *)existing;
    Atomic_Increment(&ds->uses);
    return ds; // SetDefaultRef gave us a strong ref, return it.
  }

  return res;

error_cleanup:
  // Important: sub-clearers must handle NULL pointers/empty structs safely
  descriptor_set_buffers_clear(&res->uniform_buffers);
  descriptor_set_samplers_clear(&res->samplers);
  descriptor_set_buffers_clear(&res->storage_buffers);
  Py_XDECREF(res);
  return NULL;
}

static GlobalSettings *build_global_settings(const Context *self,
                                             PyObject *settings) {
  PyObject *cache;
  if (PyDict_GetItemRef(self->global_settings_cache, settings, &cache) > 0) {
    GlobalSettings *gs = (GlobalSettings *)cache;
    Atomic_Increment(&gs->uses);
    return gs;
  }

  if (!PyTuple_Check(settings)) {
    PyErr_SetString(PyExc_TypeError, "[HyperGL] settings must be a tuple");
    return NULL;
  }

  GlobalSettings *res =
      PyObject_New(GlobalSettings, self->module_state->GlobalSettings_type);
  if (!res) {
    return NULL;
  }
  zeromem(res, sizeof(*res));

  int it = 0;
  int length = (int)PyTuple_Size(settings);

// Helper macro to safely grab items and check for NULL (out of bounds)
#define GET_ITEM() (it < length ? PyTuple_GetItem(settings, it++) : NULL)

  res->attachments = to_int(GET_ITEM());
  res->cull_face = to_int(GET_ITEM());
  res->depth_enabled = PyObject_IsTrue(GET_ITEM());

  if (res->depth_enabled) {
    res->depth_func = to_int(GET_ITEM());
    res->depth_write = PyObject_IsTrue(GET_ITEM());
  }

  res->stencil_enabled = PyObject_IsTrue(GET_ITEM());
  if (res->stencil_enabled) {
    res->stencil_front.fail_op = to_int(GET_ITEM());
    res->stencil_front.pass_op = to_int(GET_ITEM());
    res->stencil_front.depth_fail_op = to_int(GET_ITEM());
    res->stencil_front.compare_op = to_int(GET_ITEM());
    res->stencil_front.compare_mask = to_int(GET_ITEM());
    res->stencil_front.write_mask = to_int(GET_ITEM());
    res->stencil_front.reference = to_int(GET_ITEM());

    res->stencil_back.fail_op = to_int(GET_ITEM());
    res->stencil_back.pass_op = to_int(GET_ITEM());
    res->stencil_back.depth_fail_op = to_int(GET_ITEM());
    res->stencil_back.compare_op = to_int(GET_ITEM());
    res->stencil_back.compare_mask = to_int(GET_ITEM());
    res->stencil_back.write_mask = to_int(GET_ITEM());
    res->stencil_back.reference = to_int(GET_ITEM());
  }

  res->blend_enabled = to_int(GET_ITEM());
  if (res->blend_enabled) {
    res->blend.op_color = to_int(GET_ITEM());
    res->blend.op_alpha = to_int(GET_ITEM());
    res->blend.src_color = to_int(GET_ITEM());
    res->blend.dst_color = to_int(GET_ITEM());
    res->blend.src_alpha = to_int(GET_ITEM());
    res->blend.dst_alpha = to_int(GET_ITEM());
  }
#undef GET_ITEM

  // Check if we hit a NULL during the process
  if (PyErr_Occurred()) {
    Py_DECREF(res);
    return NULL;
  }

  res->uses = 1;

  PyObject *existing = NULL;
  int result = PyDict_SetDefaultRef(self->global_settings_cache, settings,
                                    (PyObject *)res, &existing);

  if (result < 0) {
    Py_DECREF(res);
    return NULL;
  }

  if (result > 0) {
    Py_DECREF(res);
    GlobalSettings *gs = (GlobalSettings *)existing;
    Atomic_Increment(&gs->uses);
    return gs;
  }

  return res;
}

static GLObject *compile_shader(Context *self, PyObject *pair) {
  PyObject *cache;
  if (PyDict_GetItemRef(self->shader_cache, pair, &cache) > 0) {
    GLObject *obj = (GLObject *)cache;
    Atomic_Increment(&obj->uses);
    return obj;
  }

  if (!PyTuple_Check(pair) || PyTuple_Size(pair) < 2) {
    PyErr_SetString(PyExc_TypeError,
                    "[HyperGL] shader pair must be (source, type)");
    return NULL;
  }

  PyObject *code = PyTuple_GetItem(pair, 0);
  if (!PyBytes_Check(code)) {
    PyErr_SetString(PyExc_TypeError, "[HyperGL] shader source must be bytes");
    return NULL;
  }

  const char *src = PyBytes_AsString(code);
  int type = to_int(PyTuple_GetItem(pair, 1));

  int shader = 0;
  int shader_compiled = 0;

  // Lock GL operations
  PyMutex_Lock(&self->state_lock);

  shader = glCreateShader(type);
  if (!shader) {
    PyMutex_Unlock(&self->state_lock);
    PyErr_SetString(PyExc_RuntimeError, "[HyperGL] glCreateShader failed");
    return NULL;
  }

  glShaderSource(shader, 1, (const void *)&src, NULL);
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_compiled);

  if (!shader_compiled) {
    int log_size = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_size);

    // Unlock before calling Python helpers to avoid deadlocks
    char *temp_log = PyMem_Malloc(log_size + 1);
    if (temp_log) {
      glGetShaderInfoLog(shader, log_size, &log_size, temp_log);
    }

    glDeleteShader(shader);
    PyMutex_Unlock(&self->state_lock);

    if (temp_log) {
      PyObject *log_text = PyBytes_FromString(temp_log);
      PyMem_Free(temp_log);
      if (log_text) {
        Py_XDECREF(PyObject_CallMethod(self->module_state->helper,
                                       "compile_error", "(OiN)", code, type,
                                       log_text));
      }
    }
    return NULL;
  }
  PyMutex_Unlock(&self->state_lock);

  GLObject *res = PyObject_New(GLObject, self->module_state->GLObject_type);
  if (!res) {
    // Shader must be deleted since it won't be owned
    PyMutex_Lock(&self->state_lock);
    glDeleteShader(shader);
    PyMutex_Unlock(&self->state_lock);
    return NULL;
  }

  // Use helper for SharedTrash linkage
  gl_object_init(self, res, shader, TRASH_SHADER);

  PyObject *existing = NULL;
  int result = PyDict_SetDefaultRef(self->shader_cache, pair, (PyObject *)res,
                                    &existing);

  if (result < 0) {
    glDeleteShader(shader);
    Py_DECREF(res);
    return NULL;
  }

  if (result > 0) {
    Py_DECREF(res); // Our 'res' dies, dealloc queues the trash.
    GLObject *obj = (GLObject *)existing;
    Atomic_Increment(&obj->uses);
    return obj;
  }

  return res;
}

static PyObject *program_interface(Context *self, int program) {
  // Using Logic A (gl 3.3 compatible) but with B's memory safety precautions
  bind_program(self, program);

  int num_attribs = 0;
  int num_uniforms = 0;
  int num_blocks = 0;
  int max_a_len = 0;
  int max_u_len = 0;
  int max_b_len = 0;

  // Query counts
  glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &num_attribs);
  glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &num_uniforms);
  glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &num_blocks);

  // Query max lengths
  glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_a_len);
  glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_u_len);
  glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &max_b_len);

  int max_name_len = max_a_len;
  if (max_u_len > max_name_len) {
    max_name_len = max_u_len;
  }
  if (max_b_len > max_name_len) {
    max_name_len = max_b_len;
  }

  if (max_name_len < 256) {
    max_name_len = 256;
  }

  char *name_buf = (char *)PyMem_Malloc(max_name_len);
  if (!name_buf) {
    return PyErr_NoMemory();
  }

  PyObject *attributes = PyList_New(num_attribs);
  PyObject *uniforms = PyList_New(num_uniforms);
  PyObject *uniform_buffers = PyList_New(num_blocks);

  if (!attributes || !uniforms || !uniform_buffers) {
    goto error_cleanup;
  }

  for (int i = 0; i < num_attribs; ++i) {
    int size = 0;
    int type = 0;
    int length = 0;
    glGetActiveAttrib(program, i, max_name_len, &length, &size, &type,
                      name_buf);
    int location = glGetAttribLocation(program, name_buf);
    PyObject *item =
        Py_BuildValue("{s:s,s:i,s:i,s:i}", "name", name_buf, "location",
                      location, "gltype", type, "size", size);
    if (!item) {
      goto error_cleanup;
    }
    PyList_SET_ITEM(attributes, i, item);
  }

  for (int i = 0; i < num_uniforms; ++i) {
    int size = 0;
    int type = 0;
    int length = 0;
    glGetActiveUniform(program, i, max_name_len, &length, &size, &type,
                       name_buf);
    int location = glGetUniformLocation(program, name_buf);
    PyObject *item =
        Py_BuildValue("{s:s,s:i,s:i,s:i}", "name", name_buf, "location",
                      location, "gltype", type, "size", size);
    if (!item) {
      goto error_cleanup;
    }
    PyList_SET_ITEM(uniforms, i, item);
  }

  for (int i = 0; i < num_blocks; ++i) {
    int size = 0;
    int length = 0;
    glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_DATA_SIZE, &size);
    glGetActiveUniformBlockName(program, i, max_name_len, &length, name_buf);

    int index = i;
    PyObject *item = Py_BuildValue("{s:s,s:i,s:i}", "name", name_buf, "size",
                                   size, "index", index);
    if (!item) {
      goto error_cleanup;
    }
    PyList_SET_ITEM(uniform_buffers, i, item);
  }

  PyMem_Free(name_buf);
  return Py_BuildValue("NNN", attributes, uniforms, uniform_buffers);

error_cleanup:
  if (name_buf) {
    PyMem_Free(name_buf);
  }
  Py_XDECREF(attributes);
  Py_XDECREF(uniforms);
  Py_XDECREF(uniform_buffers);
  return NULL;
}

static GLObject *compile_compute_program(Context *self, PyObject *includes,
                                         PyObject *source) {
  PyObject *key = NULL;
  PyObject *temp_bytes = NULL;
  GLObject *res = NULL;
  int shader = 0;
  int program = 0;

  key = PyTuple_Pack(2, includes ? includes : Py_None, source);
  if (!key) {
    return NULL;
  }

  PyObject *cache;
  if (PyDict_GetItemRef(self->program_cache, key, &cache) > 0) {
    GLObject *obj = (GLObject *)cache;
    Atomic_Increment(&obj->uses);
    res = obj;
    goto cleanup;
  }

  const char *src = NULL;
  if (PyUnicode_Check(source)) {
    temp_bytes = PyUnicode_AsUTF8String(source);
    if (!temp_bytes) {
      goto cleanup;
    }
    src = PyBytes_AsString(temp_bytes);
  } else if (PyBytes_Check(source)) {
    src = PyBytes_AsString(source);
  } else {
    PyErr_SetString(PyExc_TypeError,
                    "[HyperGL] Compute source must be str or bytes");
    goto cleanup;
  }

  // Lock GL creation
  PyMutex_Lock(&self->state_lock);

  shader = glCreateShader(GL_COMPUTE_SHADER);
  if (shader == 0) {
    PyMutex_Unlock(&self->state_lock);
    PyErr_SetString(PyExc_RuntimeError, "[HyperGL] glCreateShader failed.");
    goto cleanup;
  }

  glShaderSource(shader, 1, (const void *)&src, NULL);
  glCompileShader(shader);

  int compiled = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if (!compiled) {
    int log_size = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_size);

    char *temp_log = PyMem_Malloc(log_size + 1);
    if (temp_log) {
      glGetShaderInfoLog(shader, log_size, &log_size, temp_log);
    }
    PyMutex_Unlock(&self->state_lock);

    if (temp_log) {
      PyErr_Format(PyExc_ValueError, "[HyperGL] Compute Compile Error:\n%s",
                   temp_log);
      PyMem_Free(temp_log);
    }
    glDeleteShader(shader);
    shader = 0;
    goto cleanup;
  }

  program = glCreateProgram();
  if (!program) {
    PyMutex_Unlock(&self->state_lock);
    PyErr_SetString(PyExc_RuntimeError, "[HyperGL] glCreateProgram failed.");
    glDeleteShader(shader);
    shader = 0;
    goto cleanup;
  }

  glAttachShader(program, shader);
  glLinkProgram(program);
  glDetachShader(program, shader);
  glDeleteShader(shader); // Can delete immediately after linking
  shader = 0;

  int linked = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &linked);
  if (!linked) {
    int log_size = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_size);
    char *temp_log = PyMem_Malloc(log_size + 1);
    if (temp_log) {
      glGetProgramInfoLog(program, log_size, &log_size, temp_log);
    }
    PyMutex_Unlock(&self->state_lock);

    if (temp_log) {
      PyErr_Format(PyExc_ValueError, "[HyperGL] Compute Link Error:\n%s",
                   temp_log);
      PyMem_Free(temp_log);
    }
    glDeleteProgram(program);
    program = 0;
    goto cleanup;
  }
  PyMutex_Unlock(&self->state_lock);

  res = PyObject_New(GLObject, self->module_state->GLObject_type);
  if (!res) {
    goto cleanup;
  }

  gl_object_init(self, res, program, TRASH_PROGRAM);

  res->extra = program_interface(self, program);

  if (!res->extra) {
    Py_DECREF(res);
    res = NULL;
    program = 0;
    goto cleanup;
  }

  PyObject *existing = NULL;
  int result = PyDict_SetDefaultRef(self->program_cache, key, (PyObject *)res,
                                    &existing);

  if (result < 0) {
    Py_DECREF(res);
    res = NULL;
  } else if (result > 0) {
    Py_DECREF(res);
    res = (GLObject *)existing;
    Atomic_Increment(&res->uses);
  }

cleanup:
  // If shader wasn't deleted (e.g. early error), delete it now.
  // Note: If we succeeded, shader is already deleted.
  if (shader) {
    // Need lock to delete? Yes.
    PyMutex_Lock(&self->state_lock);
    glDeleteShader(shader);
    PyMutex_Unlock(&self->state_lock);
  }
  // Only delete program if we didn't successfully wrap it in a GLObject
  if (program && !res) {
    PyMutex_Lock(&self->state_lock);
    glDeleteProgram(program);
    PyMutex_Unlock(&self->state_lock);
  }

  Py_XDECREF(key);
  Py_XDECREF(temp_bytes);
  return res;
}

static GLObject *compile_program(Context *self, PyObject *includes,
                                 PyObject *vert, PyObject *frag,
                                 PyObject *layout) {
  PyObject *tup = PyObject_CallMethod(self->module_state->helper, "program",
                                      "(OOOO)", vert, frag, layout, includes);
  if (!tup) {
    return NULL;
  }

  PyObject *cache;
  if (PyDict_GetItemRef(self->program_cache, tup, &cache) > 0) {
    Py_DECREF(tup);
    GLObject *obj = (GLObject *)cache;
    Atomic_Increment(&obj->uses);
    return obj;
  }

  GLObject *vertex_shader = NULL;
  GLObject *fragment_shader = NULL;
  GLObject *res = NULL;
  int program = 0;

  vertex_shader = compile_shader(self, PyTuple_GetItem(tup, 0));
  if (!vertex_shader) {
    goto cleanup;
  }

  fragment_shader = compile_shader(self, PyTuple_GetItem(tup, 1));
  if (!fragment_shader) {
    goto cleanup;
  }

  // Lock Linking
  PyMutex_Lock(&self->state_lock);

  program = glCreateProgram();
  if (!program) {
    PyMutex_Unlock(&self->state_lock);
    PyErr_SetString(PyExc_RuntimeError, "[HyperGL] glCreateProgram failed.");
    goto cleanup;
  }

  glAttachShader(program, vertex_shader->obj);
  glAttachShader(program, fragment_shader->obj);
  glLinkProgram(program);

  int linked = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &linked);
  if (!linked) {
    int log_size = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_size);
    char *temp_log = PyMem_Malloc(log_size + 1);
    if (temp_log) {
      glGetProgramInfoLog(program, log_size, &log_size, temp_log);
    }

    glDeleteProgram(program);
    PyMutex_Unlock(&self->state_lock);

    if (temp_log) {
      PyObject *log_text = PyBytes_FromString(temp_log);
      PyMem_Free(temp_log);
      if (log_text) {
        PyObject *ret = PyObject_CallMethod(
            self->module_state->helper, "linker_error", "(OOO)",
            PyTuple_GetItem(tup, 0), PyTuple_GetItem(tup, 1), log_text);
        Py_XDECREF(ret);
        Py_DECREF(log_text);
      }
    }
    program = 0;
    goto cleanup;
  }
  PyMutex_Unlock(&self->state_lock);

  res = PyObject_New(GLObject, self->module_state->GLObject_type);
  if (!res) {
    goto cleanup;
  }

  gl_object_init(self, res, program, TRASH_PROGRAM);

  res->extra = program_interface(self, program);
  if (!res->extra) {
    Py_DECREF(res); // triggers trash
    res = NULL;
    program = 0;
    goto cleanup;
  }

  PyObject *existing = NULL;
  int result = PyDict_SetDefaultRef(self->program_cache, tup, (PyObject *)res,
                                    &existing);

  if (result < 0) {
    Py_DECREF(res);
    res = NULL;
  } else if (result > 0) {
    Py_DECREF(res);
    res = (GLObject *)existing;
    Atomic_Increment(&res->uses);
  }

cleanup:
  if (program && !res) {
    PyMutex_Lock(&self->state_lock);
    glDeleteProgram(program);
    PyMutex_Unlock(&self->state_lock);
  }

  Py_XDECREF(vertex_shader);
  Py_XDECREF(fragment_shader);
  Py_XDECREF(tup);
  return res;
}

static ImageFace *build_image_face(Image *self, PyObject *key) // HAS GC_TRACK
{
  PyObject *cache_obj = NULL;
  // Fast path: Check cache
  if (PyDict_GetItemRef(self->faces, key, &cache_obj) > 0) {
    return (ImageFace *)cache_obj;
  }

  int layer = to_int(PyTuple_GetItem(key, 0));
  int level = to_int(PyTuple_GetItem(key, 1));

  int width = (self->width >> level);
  if (width < 1) {
    width = 1;
  }
  int height = (self->height >> level);
  if (height < 1) {
    height = 1;
  }

  ImageFace *res =
      PyObject_GC_New(ImageFace, self->ctx->module_state->ImageFace_type);
  if (!res) {
    return NULL;
  }

  res->size = NULL;
  res->framebuffer = NULL;

  res->ctx = (Context *)Py_NewRef(self->ctx);
  res->image = (Image *)Py_NewRef(self);
  res->size = Py_BuildValue("(ii)", width, height);
  if (!res->size) {
    Py_DECREF(res);
    return NULL;
  }
  res->width = width;
  res->height = height;
  res->layer = layer;
  res->level = level;
  res->samples = self->samples;
  res->flags = self->fmt.flags;
  res->framebuffer = NULL;

  // We pass 'res' (the ImageFace) to the framebuffer builder so it knows
  // which texture and level to attach to the FBO.
  PyObject *attachments = NULL;
  if (self->fmt.color) {
    attachments =
        Py_BuildValue("((ii)(O)O)", width, height, (PyObject *)res, Py_None);
  } else {
    attachments = Py_BuildValue("((ii)()O)", width, height, (PyObject *)res);
  }

  if (attachments) {
    res->framebuffer = build_framebuffer(self->ctx, attachments);
    Py_DECREF(attachments);
    if (!res->framebuffer) {
      Py_DECREF(res);
      return NULL;
    }
  }

  PyObject_GC_Track(res);

  PyObject *existing = NULL;
  int status =
      PyDict_SetDefaultRef(self->faces, key, (PyObject *)res, &existing);

  if (status < 0) { // Error
    PyObject_GC_UnTrack(res);
    Py_DECREF(res);
    return NULL;
  }

  if (status > 0) { // Race Lost
    PyObject_GC_UnTrack(res);
    Py_DECREF(res); // The dealloc will handle cleaning up res->framebuffer
    return (ImageFace *)existing;
  }

  return res;
}

// -----------------------------------------------------------------------------
// Image Manipulation Utilities
// -----------------------------------------------------------------------------

static void clear_bound_image(const Image *self) {
  // NOTE: Caller must hold self->ctx->state_lock

  // We check if the mask is NOT enabled (1 or 0xFF), but the format implies we
  // need it.
  int modify_depth =
      (self->ctx->current_depth_mask != 1) &&
      (self->fmt.buffer == GL_DEPTH || self->fmt.buffer == GL_DEPTH_STENCIL);

  int modify_stencil =
      (self->ctx->current_stencil_mask != 0xff) &&
      (self->fmt.buffer == GL_STENCIL || self->fmt.buffer == GL_DEPTH_STENCIL);

  // CRITICAL: Invalidate the settings cache if we touch the state.
  if (modify_depth || modify_stencil) {
    self->ctx->current_global_settings = NULL;
  }

  if (modify_depth) {
    glDepthMask(1);
    self->ctx->current_depth_mask = 1;
  }

  if (modify_stencil) {
    glStencilMaskSeparate(GL_FRONT, 0xff);
    self->ctx->current_stencil_mask = 0xff;
  }

  switch (self->fmt.clear_type) {
  case 'f':
    glClearBufferfv(self->fmt.buffer, 0, self->clear_value.clear_floats);
    break;
  case 'i':
    glClearBufferiv(self->fmt.buffer, 0, self->clear_value.clear_ints);
    break;
  case 'u':
    glClearBufferuiv(self->fmt.buffer, 0, self->clear_value.clear_uints);
    break;
  case 'x':
    glClearBufferfi(self->fmt.buffer, 0, self->clear_value.clear_floats[0],
                    self->clear_value.clear_ints[1]);
    break;
  default:
    break;
  }
}

static PyObject *blit_image_face(const ImageFace *src, PyObject *target_arg,
                                 PyObject *offset_arg, PyObject *size_arg,
                                 PyObject *crop_arg, const int filter) {
  if (Py_TYPE(target_arg) == src->image->ctx->module_state->Image_type) {
    Image *image = (Image *)target_arg;
    if (image->array || image->cubemap) {
      PyErr_Format(PyExc_TypeError,
                   "[HyperGL] cannot blit to whole cubemap or array images");
      return NULL;
    }
    target_arg = PyTuple_GetItem(image->layers, 0);
  }

  if (target_arg != Py_None &&
      Py_TYPE(target_arg) != src->image->ctx->module_state->ImageFace_type) {
    PyErr_Format(PyExc_TypeError,
                 "[HyperGL] target must be an Image or ImageFace or None");
    return NULL;
  }

  ImageFace *target = target_arg != Py_None ? (ImageFace *)target_arg : NULL;

  if (target && src->image->fmt.color != target->image->fmt.color) {
    PyErr_Format(PyExc_TypeError,
                 "[HyperGL] cannot blit between color and depth images");
    return NULL;
  }

  if (target && target->image->samples > 1) {
    PyErr_Format(PyExc_TypeError,
                 "[HyperGL] cannot blit to multisampled images");
    return NULL;
  }

  Viewport crop;
  if (!to_viewport(&crop, crop_arg, 0, 0, src->width, src->height)) {
    PyErr_Format(PyExc_TypeError,
                 "[HyperGL] the crop must be a tuple of 4 ints");
    return NULL;
  }

  IntPair offset;
  if (!to_int_pair(&offset, offset_arg, 0, 0)) {
    PyErr_Format(PyExc_TypeError,
                 "[HyperGL] the offset must be a tuple of 2 ints");
    return NULL;
  }

  IntPair size;
  if (!to_int_pair(&size, size_arg, crop.width, crop.height)) {
    PyErr_Format(PyExc_TypeError,
                 "[HyperGL] the size must be a tuple of 2 ints");
    return NULL;
  }

  int scaled = (crop.width != size.x && crop.width != -size.x) ||
               (crop.height != size.y && crop.height != -size.y);
  if (src->image->samples > 1 && scaled) {
    PyErr_Format(PyExc_TypeError,
                 "[HyperGL] multisampled images cannot be scaled");
    return NULL;
  }

  if (!target && src->image->samples > 1 && src->image->ctx->is_gles) {
    PyErr_Format(PyExc_TypeError, "[HyperGL] multisampled images needs to be "
                                  "downsampled before blitting to the screen");
    return NULL;
  }

  offset.x -= size.x < 0 ? size.x : 0;
  offset.y -= size.y < 0 ? size.y : 0;

  PyMutex_Lock(&src->ctx->state_lock);

  if (src->ctx->is_lost) {
    PyMutex_Unlock(&src->ctx->state_lock);
    PyErr_Format(PyExc_RuntimeError, "[HyperGL] the context is lost");
    return NULL;
  }
  int buffer = src->image->fmt.color
                   ? GL_COLOR_BUFFER_BIT
                   : (GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  int target_framebuffer =
      target ? target->framebuffer->obj : src->ctx->default_framebuffer->obj;
  bind_read_framebuffer_internal(src->image->ctx, src->framebuffer->obj);
  bind_draw_framebuffer_internal(src->image->ctx, target_framebuffer);
  glBlitFramebuffer(crop.x, crop.y, crop.x + crop.width, crop.y + crop.height,
                    offset.x, offset.y, offset.x + size.x, offset.y + size.y,
                    buffer, filter ? GL_LINEAR : GL_NEAREST);
  PyMutex_Unlock(&src->ctx->state_lock);
  Py_RETURN_NONE;
}

static int parse_size_and_offset(const ImageFace *self, PyObject *size_arg,
                                 PyObject *offset_arg, IntPair *size,
                                 IntPair *offset) {
  if (size_arg == Py_None && offset_arg != Py_None) {
    PyErr_Format(PyExc_ValueError,
                 "[HyperGL] the size is required when the offset is not None");
    return 0;
  }

  if (!to_int_pair(size, size_arg, self->width, self->height)) {
    PyErr_Format(PyExc_TypeError,
                 "[HyperGL] the size must be a tuple of 2 ints");
    return 0;
  }

  if (!to_int_pair(offset, offset_arg, 0, 0)) {
    PyErr_Format(PyExc_TypeError,
                 "[HyperGL] the offset must be a tuple of 2 ints");
    return 0;
  }

  if (size->x <= 0 || size->y <= 0 || size->x > self->width ||
      size->y > self->height) {
    PyErr_Format(PyExc_ValueError, "[HyperGL] invalid size");
    return 0;
  }

  if (offset->x < 0 || offset->y < 0 || size->x + offset->x > self->width ||
      size->y + offset->y > self->height) {
    PyErr_Format(PyExc_ValueError, "[HyperGL] invalid offset");
    return 0;
  }

  return 1;
}

static PyObject *read_image_face(ImageFace *src, IntPair size, IntPair offset,
                                 PyObject *into) {
  if (!src->ctx || src->ctx->is_lost) {
    PyErr_SetString(PyExc_RuntimeError, "[HyperGL] context lost");
    return NULL;
  }
  if (!src->framebuffer) {
    PyErr_SetString(PyExc_RuntimeError, "[HyperGL] invalid framebuffer");
    return NULL;
  }
  if (src->image->samples > 1) {
    PyObject *temp =
        PyObject_CallMethod((PyObject *)src->image->ctx, "image", "((ii)O)",
                            size.x, size.y, src->image->format);
    if (!temp) {
      return NULL;
    }

    // Perform Resolve
    PyObject *blit = PyObject_CallMethod(
        (PyObject *)src, "blit", "(O(ii)(ii)(iiii))", temp, 0, 0, size.x,
        size.y, offset.x, offset.y, size.x, size.y);
    if (!blit) {
      Py_DECREF(temp);
      return NULL;
    }
    Py_DECREF(blit);

    // Read from Resolved
    PyObject *res =
        PyObject_CallMethod(temp, "read", "(OOO)", Py_None, Py_None, into);

    // Return temp to pool (Using 'O' so we keep ownership for the manual DECREF
    // below)
    PyObject *release = PyObject_CallMethod((PyObject *)src->image->ctx,
                                            "release", "(O)", temp);
    Py_XDECREF(release);
    Py_DECREF(temp);

    return res; // res is NULL if the 'read' call failed, which is correct
  }

  int write_size = size.x * size.y * src->image->fmt.pixel_size;

  if (into == Py_None) {
    PyObject *res = PyBytes_FromStringAndSize(NULL, write_size);
    if (!res) {
      return NULL;
    }

    PyMutex_Lock(&src->ctx->state_lock);
    bind_read_framebuffer_internal(src->ctx, src->framebuffer->obj);
    glReadPixels(offset.x, offset.y, size.x, size.y, src->image->fmt.format,
                 src->image->fmt.type, PyBytes_AsString(res));
    PyMutex_Unlock(&src->ctx->state_lock);
    return res;
  }

  BufferView *buffer_view = NULL;
  if (Py_TYPE(into) == src->ctx->module_state->Buffer_type) {
    buffer_view = (BufferView *)PyObject_CallMethod(into, "view", NULL);
  } else if (Py_TYPE(into) == src->ctx->module_state->BufferView_type) {
    buffer_view = (BufferView *)Py_NewRef(into);
  }

  if (buffer_view) {
    if (write_size > buffer_view->size) {
      Py_DECREF(buffer_view);
      PyErr_Format(PyExc_ValueError, "[HyperGL] invalid size");
      return NULL;
    }

    char *ptr = (char *)((unsigned char *)NULL + buffer_view->offset);
    PyMutex_Lock(&src->ctx->state_lock);
    bind_read_framebuffer_internal(src->ctx, src->framebuffer->obj);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(offset.x, offset.y, size.x, size.y, src->image->fmt.format,
                 src->image->fmt.type, ptr);
    PyMutex_Unlock(&src->ctx->state_lock);
    Py_DECREF(buffer_view);
    Py_RETURN_NONE;
  }

  Py_buffer view;
  if (PyObject_GetBuffer(into, &view, PyBUF_WRITABLE)) {
    return NULL;
  }

  if (write_size > (int)view.len) {
    PyBuffer_Release(&view);
    PyErr_Format(PyExc_ValueError, "[HyperGL] invalid write size");
    return NULL;
  }

  PyMutex_Lock(&src->ctx->state_lock);
  bind_read_framebuffer_internal(src->ctx, src->framebuffer->obj);
  glReadPixels(offset.x, offset.y, size.x, size.y, src->image->fmt.format,
               src->image->fmt.type, view.buf);
  PyMutex_Unlock(&src->ctx->state_lock);

  PyBuffer_Release(&view);
  Py_RETURN_NONE;
}

// -----------------------------------------------------------------------------
// Initialization & Module Loading
// -----------------------------------------------------------------------------

static int init_gl_limits(ModuleState *state) {
  // Zero first so missing queries are obvious in debug
  memset(&state->limits, 0, sizeof(Limits));

  glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS,
                &state->limits.max_uniform_buffer_bindings);

  glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE,
                &state->limits.max_uniform_block_size);

  glGetIntegerv(GL_MAX_COMBINED_UNIFORM_BLOCKS,
                &state->limits.max_combined_uniform_blocks);

  glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
                &state->limits.max_combined_texture_image_units);

  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &state->limits.max_vertex_attribs);

  glGetIntegerv(GL_MAX_DRAW_BUFFERS, &state->limits.max_draw_buffers);

  glGetIntegerv(GL_MAX_SAMPLES, &state->limits.max_samples);

  // Only exists on newer GL — guard it
#ifdef GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS
  glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS,
                &state->limits.max_shader_storage_buffer_bindings);
#else
  state->limits.max_shader_storage_buffer_bindings = 0;
#endif

#ifdef DEBUG
  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    PyErr_Format(PyExc_RuntimeError,
                 "[HyperGL] OpenGL error while querying limits (0x%x)", err);
    return -1;
  }
#endif

  return 0;
}

static int init_internal(ModuleState *module_state, PyObject *module_obj,
                         PyObject *loader) {
  PyObject *new_loader = NULL;

  // Mark old context as lost
  if (module_state->default_context &&
      module_state->default_context != Py_None) {
    ((Context *)module_state->default_context)->is_lost = 1;
  }

  // Resolve Loader
  if (loader == Py_None) {
#ifdef _WIN64
    new_loader = Py_NewRef(module_obj);
#else
    new_loader = PyObject_CallMethod(module_state->helper, "loader", NULL);
#endif
  } else {
    new_loader = Py_NewRef(loader);
  }

  if (!new_loader) {
    return -1;
  }

  // Load OpenGL (Must be done on main thread usually, but here we assume caller
  // handles thread affinity)
  if (load_gl(new_loader) < 0) {
    Py_DECREF(new_loader);
    return -1;
  }

  // Swap State
  PyObject *old_loader = module_state->default_loader;
  PyObject *old_context = module_state->default_context;

  module_state->default_loader = new_loader;
  module_state->default_context = new_ref(Py_None);

  if (init_gl_limits(module_state) < 0) {
    return -1;
  }

  module_state->gl_initialized = 1;

  Py_XDECREF(old_loader);
  Py_XDECREF(old_context);
  return 0;
}

static PyObject *meth_init(PyObject *self, PyObject *args, PyObject *kwargs) {
  static char *keywords[] = {"loader", "headless", NULL};
  PyObject *loader = Py_None;
  PyObject *new_loader = NULL;
  int headless = 0;

  ModuleState *module_state = PyModule_GetState(self);
  if (!module_state) {
    return NULL;
  }

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|Op", keywords, &loader,
                                   &headless)) {
    return NULL;
  }

  PyMutex_Lock(&module_state->setup_lock);

  // 1. Resolve Loader
  if (loader != Py_None) {
    new_loader = Py_NewRef(loader);
  } else {
    int use_python_loader = 1;
#ifdef _WIN64
    if (!headless) {
      use_python_loader = 0;
    }
#endif

    if (use_python_loader) {
      new_loader =
          PyObject_CallMethod(module_state->helper, "loader", "(i)", headless);
    } else {
      new_loader = Py_NewRef(self);
    }
  }

  // 2. Check if resolution failed
  if (!new_loader) {
    // ensure an error exists
    if (!PyErr_Occurred()) {
      PyErr_SetString(PyExc_RuntimeError, "[HyperGL] Failed to resolve loader");
    }
    goto finally;
  }

  // 3. Internal Init
  // init_internal() does NOT steal a reference to new_loader
  if (init_internal(module_state, self, new_loader) < 0) {
    if (!PyErr_Occurred()) {
      PyErr_SetString(PyExc_RuntimeError, "[HyperGL] Internal init failed");
    }
    goto finally;
  }

finally:
  // Handle cleanup in ONE place
  Py_XDECREF(new_loader);
  PyMutex_Unlock(&module_state->setup_lock);

  // If an error was set anywhere above, returning NULL tells Python to raise it
  if (PyErr_Occurred()) {
    return NULL;
  }

  Py_RETURN_NONE;
}

static int get_limit(const int pname, const int min, const int max) {
  int value = 0;
  glGetIntegerv(pname, &value);

  // Check if the query actually worked
  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
// Only print in Beta/Debug mode
#ifdef DEBUG
    fprintf(stderr,
            "[HyperGL] Warning: Limit 0x%x query failed (Error 0x%x). "
            "Defaulting to %d\n",
            pname, err, min);
#endif
    return min;
  }

  // Clamping logic is great—keep it.
  // It prevents your C code from allocating 0 or INT_MAX memory.
  if (value < min) {
#ifdef DEBUG
    fprintf(stderr, "[HyperGL] Limit 0x%x (%d) below minimum. Clamped to %d\n",
            pname, value, min);
#endif
    return min;
  }
  if (value > max) {
    return max;
  }

  return value;
}

static PyObject *meth_cleanup(PyObject *self, PyObject *args) {
  ModuleState *module_state = (ModuleState *)PyModule_GetState(self);
  Context *ctx = (Context *)module_state->default_context;

  if (ctx != (Context *)Py_None && !ctx->is_lost) {
    Py_XDECREF(
        PyObject_CallMethod((PyObject *)ctx, "release", "s", "shader_cache"));
    Py_XDECREF(PyObject_CallMethod((PyObject *)ctx, "release", "s", "all"));
    ctx->is_lost = 1;
  }

  Py_CLEAR(module_state->default_context);
  module_state->default_context = Py_NewRef(Py_None);

  Py_CLEAR(module_state->default_loader);
  module_state->default_loader = Py_NewRef(Py_None);

  Py_RETURN_NONE;
}

// -----------------------------------------------------------------------------
// Resource Management (Trash & Release)
// -----------------------------------------------------------------------------

// Safe decref for list items while unlocking state_lock
static void safe_decref_list(Context *self, PyObject **list, int count,
                             int is_locked) {
  if (count == 0) {
    return;
  }

  // If we are locked, we must unlock before DECREFing to avoid
  // dealloc-deadlocks
  if (is_locked) {
    PyMutex_Unlock(&self->state_lock);
  }

  for (int i = 0; i < count; ++i) {
    Py_XDECREF(list[i]);
  }

  if (is_locked) {
    PyMutex_Lock(&self->state_lock);
  }
}

// Flush Trash using SharedTrash (Thread-Safe)
void flush_trash(const Context *self) {
  SharedTrash *shared = self->trash_shared;
  if (!shared) {
    return;
  }

  TrashItem *to_delete = NULL;
  size_t count = 0;
  // int invalid_type = 0;

  // CRITICAL SECTION: Just swap the pointer!
  PyMutex_Lock(&shared->lock);
  if (shared->count > 0) {
    count = shared->count;
    to_delete = shared->bin; // Steal the full bin

    // Allocate a new empty bin for the producers
    size_t old_capacity = shared->capacity;
    shared->bin = PyMem_Malloc(old_capacity * sizeof(TrashItem));
    if (shared->bin) {
      shared->capacity = old_capacity;
      shared->count = 0;
    } else {
      shared->bin = to_delete;
      shared->count = count;
      to_delete = NULL;
    }
  }
  PyMutex_Unlock(&shared->lock);

  if (!to_delete) {
    return;
  }

  if (self->is_lost) {
    PyMem_Free(to_delete);
    return;
  }
  // Delete OpenGL resources OUTSIDE the lock
  Py_BEGIN_ALLOW_THREADS for (size_t i = 0; i < count; i++) {
    unsigned int id = (unsigned int)to_delete[i].id;
    if (id == 0) {
      continue;
    }
    switch (to_delete[i].type) {
    case TRASH_BUFFER:
      glDeleteBuffers(1, &id);
      break;
    case TRASH_TEXTURE:
      glDeleteTextures(1, &id);
      break;
    case TRASH_RENDERBUFFER:
      glDeleteRenderbuffers(1, &id);
      break;
    case TRASH_FRAMEBUFFER:
      glDeleteFramebuffers(1, &id);
      break;
    case TRASH_VERTEX_ARRAY:
      glDeleteVertexArrays(1, &id);
      break;
    case TRASH_PROGRAM:
      glDeleteProgram(id);
      break;
    case TRASH_SHADER:
      glDeleteShader(id);
      break;
    case TRASH_SAMPLER:
      glDeleteSamplers(1, &id);
      break;
    case TRASH_QUERY:
      glDeleteQueries(1, &id);
      break;
    default: {
      continue;
    }
    }
  }
  Py_END_ALLOW_THREADS

      PyMem_Free(to_delete);
}

static void enqueue_trash(SharedTrash *trash, int id, int type) {
  if (!trash || id <= 0) { // assumes all GL objects are unsigned
    return;
  }

  PyMutex_Lock(&trash->lock);

  if (trash->count >= trash->capacity) {
    size_t old_cap = trash->capacity;
    size_t new_cap = old_cap ? old_cap * 2 : 64;

    TrashItem *new_bin = PyMem_Realloc(trash->bin, new_cap * sizeof(TrashItem));

    if (!new_bin) {
      // Allocation failed → drop item (lossy but safe)
      PyMutex_Unlock(&trash->lock);
      return;
    }

    trash->bin = new_bin;
    trash->capacity = new_cap;
  }

  trash->bin[trash->count++] = (TrashItem){id, type};

  PyMutex_Unlock(&trash->lock);
}

static void release_descriptor_set(Context *self, DescriptorSet *set,
                                   int is_locked) {
  if (!set)
    return;

  // Atomic Decrement for cache usage tracking
  if (Atomic_Decrement(&set->uses) > 0) {
    return;
  }

  Py_INCREF(set); // Protect against self-deletion during cleanup

  PyObject *kill_list[(MAX_SAMPLER_BINDINGS * 2) + (MAX_BUFFER_BINDINGS * 2)];
  int kill_count = 0;

  for (int i = 0; i < set->samplers.binding_count; ++i) {
    GLObject *sampler = set->samplers.binding[i].sampler;
    if (!sampler)
      continue;

    Py_INCREF(sampler);

    if (Atomic_Decrement(&sampler->uses) == 0) {
      remove_dict_value(self->sampler_cache, (PyObject *)sampler);
      if (!self->is_lost && sampler->obj) {
        if (!is_locked)
          PyMutex_Lock(&self->state_lock);
        glDeleteSamplers(1, (const unsigned int *)&sampler->obj);
        if (!is_locked)
          PyMutex_Unlock(&self->state_lock);
        sampler->obj = 0;
      }
    }
    kill_list[kill_count++] = (PyObject *)sampler;
  }

  for (int i = 0; i < set->uniform_buffers.binding_count; ++i) {
    if (set->uniform_buffers.binding[i].buffer)
      kill_list[kill_count++] =
          (PyObject *)set->uniform_buffers.binding[i].buffer;
    set->uniform_buffers.binding[i].buffer = NULL;
  }

  for (int i = 0; i < set->storage_buffers.binding_count; ++i) {
    if (set->storage_buffers.binding[i].buffer)
      kill_list[kill_count++] =
          (PyObject *)set->storage_buffers.binding[i].buffer;
    set->storage_buffers.binding[i].buffer = NULL;
  }

  for (int i = 0; i < set->samplers.binding_count; ++i) {
    if (set->samplers.binding[i].image)
      kill_list[kill_count++] = (PyObject *)set->samplers.binding[i].image;

    set->samplers.binding[i].sampler = NULL;
    set->samplers.binding[i].image = NULL;
  }

  remove_dict_value(self->descriptor_set_cache, (PyObject *)set);

  safe_decref_list(self, kill_list, kill_count, is_locked);
  Py_DECREF(set);
}

static void release_global_settings(Context *self, GlobalSettings *settings,
                                    int is_locked) {
  if (!settings)
    return;

  if (Atomic_Decrement(&settings->uses) != 0) {
    return;
  }

  if (!is_locked)
    PyMutex_Lock(&self->state_lock);

  if (Atomic_Load(&settings->uses) == 0) {
    remove_dict_value(self->global_settings_cache, (PyObject *)settings);
    if (self->current_global_settings == settings) {
      self->current_global_settings = NULL;
    }
  }

  if (!is_locked)
    PyMutex_Unlock(&self->state_lock);

  if (is_locked)
    PyMutex_Unlock(&self->state_lock);
  Py_DECREF(settings);
  if (is_locked)
    PyMutex_Lock(&self->state_lock);
}

static void release_framebuffer(Context *self, GLObject *framebuffer) {
  if (!framebuffer)
    return;

  if (Atomic_Decrement(&framebuffer->uses) == 0) {
    remove_dict_value(self->framebuffer_cache, (PyObject *)framebuffer);

    if (framebuffer->obj && !self->is_lost) {
      bind_draw_framebuffer(self, 0);
      bind_read_framebuffer(self, 0);
      glDeleteFramebuffers(1, (const unsigned int *)&framebuffer->obj);
      framebuffer->obj = 0;
    }

    self->current_viewport.x = -1;
    self->current_viewport.y = -1;
    self->current_viewport.width = -1;
    self->current_viewport.height = -1;
  }
}

static void release_program(Context *self, GLObject *program) {
  if (!program)
    return;

  if (Atomic_Decrement(&program->uses) == 0) {
    remove_dict_value(self->program_cache, (PyObject *)program);

    if (program->obj && !self->is_lost) {
      bind_program(self, 0);
      glDeleteProgram(program->obj);
      program->obj = 0;
    }
  }
}

static void release_vertex_array(Context *self, GLObject *vertex_array) {
  if (!vertex_array)
    return;

  if (Atomic_Decrement(&vertex_array->uses) == 0) {
    remove_dict_value(self->vertex_array_cache, (PyObject *)vertex_array);

    if (vertex_array->obj && !self->is_lost) {
      bind_vertex_array(self, 0);
      glDeleteVertexArrays(1, (const unsigned int *)&vertex_array->obj);
      vertex_array->obj = 0;
    }
  }
}

// -----------------------------------------------------------------------------
// Type: Context
// -----------------------------------------------------------------------------

static int Context_traverse(Context *self, visitproc visit, void *arg) {
  // 3.14t requires visiting the type itself
  Py_VISIT(Py_TYPE(self));

  // Caches and Dicts
  Py_VISIT(self->descriptor_set_cache);
  Py_VISIT(self->global_settings_cache);
  Py_VISIT(self->sampler_cache);
  Py_VISIT(self->vertex_array_cache);
  Py_VISIT(self->framebuffer_cache);
  Py_VISIT(self->program_cache);
  Py_VISIT(self->shader_cache);
  Py_VISIT(self->includes);
  Py_VISIT(self->info_dict);

  // Visit custom structs that start with PyObject_HEAD
  Py_VISIT((PyObject *)self->default_framebuffer);
  Py_VISIT((PyObject *)self->current_descriptor_set);
  Py_VISIT((PyObject *)self->current_global_settings);

  // SharedTrash is malloc'd C struct, not visited
  return 0;
}

static int Context_clear(Context *self) {
  Py_CLEAR(self->descriptor_set_cache);
  Py_CLEAR(self->global_settings_cache);
  Py_CLEAR(self->sampler_cache);
  Py_CLEAR(self->vertex_array_cache);
  Py_CLEAR(self->framebuffer_cache);
  Py_CLEAR(self->program_cache);
  Py_CLEAR(self->shader_cache);
  Py_CLEAR(self->includes);
  Py_CLEAR(self->info_dict);

  Py_CLEAR(self->default_framebuffer);
  Py_CLEAR(self->current_descriptor_set);
  Py_CLEAR(self->current_global_settings);

  return 0;
}

static PyObject *Context_new(PyTypeObject *type, PyObject *args,
                             PyObject *kwargs) // HAS GC_TRACK
{
  ModuleState *module_state = PyType_GetModuleState(type);
  INTERNAL_CHECK(module_state,
                 "Could not retrieve module state from Context type");

  PyMutex_Lock(&module_state->setup_lock);

  // Singleton check
  if (module_state->default_context != Py_None) {
    PyObject *existing = Py_NewRef(module_state->default_context);
    PyMutex_Unlock(&module_state->setup_lock);
    return existing;
  }

  // Initialization logic
  if (!module_state->gl_initialized) {
    PyObject *module = PyType_GetModule(type);
    if (module) {
      // Unlock before calling Python code to prevent deadlock
      PyMutex_Unlock(&module_state->setup_lock);
      int ret = init_internal(module_state, module, Py_None);
      PyMutex_Lock(&module_state->setup_lock);

      if (ret < 0) {
        PyMutex_Unlock(&module_state->setup_lock);
        return NULL;
      }
    } else {
      PyMutex_Unlock(&module_state->setup_lock);
      PyErr_SetString(PyExc_RuntimeError,
                      "[HyperGL] Could not locate module instance");
      return NULL;
    }
  }

  // Allocate default framebuffer wrapper
  if (!module_state->GLObject_type) {
    PyErr_SetString(PyExc_RuntimeError, "[HyperGL] GLObject_type is NULL");
    PyMutex_Unlock(&module_state->setup_lock);
    return NULL;
  }

  GLObject *default_framebuffer =
      PyObject_GC_New(GLObject, module_state->GLObject_type);
  if (!default_framebuffer) {
    PyMutex_Unlock(&module_state->setup_lock);
    return NULL;
  }
  default_framebuffer->obj = 0;
  default_framebuffer->uses = 1;
  default_framebuffer->extra = NULL;
  default_framebuffer->trash = NULL; // Default FBO is never deleted

  // Allocate Context instance
  Context *res = (Context *)type->tp_alloc(type, 0);
  if (!res) {
    Py_DECREF(default_framebuffer);
    PyMutex_Unlock(&module_state->setup_lock);
    return NULL;
  }

  PyObject_GC_UnTrack((PyObject *)res);

  res->module_state = module_state;
  res->trash_shared = NULL;
  res->descriptor_set_cache = NULL;
  res->global_settings_cache = NULL;
  res->sampler_cache = NULL;
  res->vertex_array_cache = NULL;
  res->program_cache = NULL;
  res->shader_cache = NULL;
  res->includes = NULL;
  res->framebuffer_cache = NULL;

  // --- Shared Trash Allocation ---
  SharedTrash *shared = PyMem_Malloc(sizeof(SharedTrash));
  if (!shared) {
    Py_DECREF(res);
    Py_DECREF(default_framebuffer);
    PyMutex_Unlock(&module_state->setup_lock);
    return PyErr_NoMemory();
  }

  shared->bin = PyMem_Malloc(4096 * sizeof(TrashItem));
  if (!shared->bin) {
    PyMem_Free(shared);
    Py_DECREF(res);
    Py_DECREF(default_framebuffer);
    PyMutex_Unlock(&module_state->setup_lock);
    return PyErr_NoMemory();
  }

  // Initialize the mutex in the shared struct (Zeroing it is enough for
  // PyMutex)
  memset(&shared->lock, 0, sizeof(PyMutex));
  shared->count = 0;
  shared->capacity = 4096;
  shared->ref_count = 1; // 1 ref held by the Context itself
  res->trash_shared = shared;

  // --- Initialize Caches ---
  res->descriptor_set_cache = PyDict_New();
  res->global_settings_cache = PyDict_New();
  res->sampler_cache = PyDict_New();
  res->vertex_array_cache = PyDict_New();
  res->program_cache = PyDict_New();
  res->shader_cache = PyDict_New();
  res->includes = PyDict_New();
  res->framebuffer_cache = Py_BuildValue("{OO}", Py_None, default_framebuffer);

  if (!res->descriptor_set_cache || !res->global_settings_cache ||
      !res->sampler_cache || !res->vertex_array_cache || !res->program_cache ||
      !res->shader_cache || !res->includes || !res->framebuffer_cache)
    goto fail;

  Py_INCREF(default_framebuffer);
  res->default_framebuffer = default_framebuffer;

  // Validate GL functions
  if (!glGetIntegerv) {
    PyErr_SetString(
        PyExc_RuntimeError,
        "[HyperGL] OpenGL functions not loaded. Initialization failed.");
    goto fail;
  }

  // --- Fill Limits ---
  res->module_state->limits.max_uniform_buffer_bindings =
      get_limit(GL_MAX_UNIFORM_BUFFER_BINDINGS, 8, MAX_BUFFER_BINDINGS);
  res->module_state->limits.max_uniform_block_size =
      get_limit(GL_MAX_UNIFORM_BLOCK_SIZE, 0x4000, 0x40000000);
  res->module_state->limits.max_combined_uniform_blocks =
      get_limit(GL_MAX_COMBINED_UNIFORM_BLOCKS, 8, MAX_BUFFER_BINDINGS);
  res->module_state->limits.max_combined_texture_image_units =
      get_limit(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, 8, MAX_SAMPLER_BINDINGS);
  res->module_state->limits.max_vertex_attribs =
      get_limit(GL_MAX_VERTEX_ATTRIBS, 8, 64);
  res->module_state->limits.max_draw_buffers =
      get_limit(GL_MAX_DRAW_BUFFERS, 8, 64);
  res->module_state->limits.max_samples = get_limit(GL_MAX_SAMPLES, 1, 16);
  res->module_state->limits.max_shader_storage_buffer_bindings =
      get_limit(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, 0, MAX_BUFFER_BINDINGS);

  const char *raw_version = glGetString(GL_VERSION);
  const char *raw_vendor = glGetString(GL_VENDOR);
  const char *raw_renderer = glGetString(GL_RENDERER);
  const char *raw_glsl = glGetString(GL_SHADING_LANGUAGE_VERSION);

  if (raw_version && !startswith(raw_version, "WebGL")) {
    glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
  }
  if (raw_version && !startswith(raw_version, "OpenGL ES")) {
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
  }

  res->is_gles = (raw_version && startswith(raw_version, "OpenGL ES")) ? 1 : 0;
  res->is_webgl = (raw_version && startswith(raw_version, "WebGL")) ? 1 : 0;

  PyObject *tmp = Py_BuildValue(
      "{szszszszsisisisisisisi}", "vendor", raw_vendor ? raw_vendor : "Unknown",
      "renderer", raw_renderer ? raw_renderer : "Unknown", "version",
      raw_version ? raw_version : "Unknown", "glsl",
      raw_glsl ? raw_glsl : "Unknown", "max_uniform_buffer_bindings",
      res->module_state->limits.max_uniform_buffer_bindings,
      "max_uniform_block_size",
      res->module_state->limits.max_uniform_block_size,
      "max_combined_uniform_blocks",
      res->module_state->limits.max_combined_uniform_blocks,
      "max_combined_texture_image_units",
      res->module_state->limits.max_combined_texture_image_units,
      "max_vertex_attribs", res->module_state->limits.max_vertex_attribs,
      "max_draw_buffers", res->module_state->limits.max_draw_buffers,
      "max_samples", res->module_state->limits.max_samples);
  if (!tmp)
    goto fail;
  res->info_dict = tmp;

  int max_texture_image_units =
      get_limit(GL_MAX_TEXTURE_IMAGE_UNITS, 8, MAX_SAMPLER_BINDINGS + 1);
  res->default_texture_unit = GL_TEXTURE0 + (max_texture_image_units - 1);
  if (res->default_texture_unit < 1)
    res->default_texture_unit = 1;

  Py_XSETREF(module_state->default_context, Py_NewRef((PyObject *)res));
  Py_DECREF(default_framebuffer);
  PyMutex_Unlock(&module_state->setup_lock);

  // If not already tracked (standard python), track it.
  if (!PyObject_GC_IsTracked((PyObject *)res)) {
    PyObject_GC_Track((PyObject *)res);
  }

  return (PyObject *)res;

fail:
  // If we failed, dealloc will handle cleaning up caches and shared trash
  // because ref_count was set to 1.
  Py_DECREF(res);
  Py_DECREF(default_framebuffer);
  PyMutex_Unlock(&module_state->setup_lock);
  return NULL;
}

static PyObject *meth_context(PyObject *self, PyObject *arg) {
  ModuleState *state = (ModuleState *)PyModule_GetState(self);
  return PyObject_CallNoArgs((PyObject *)state->Context_type);
}

// -----------------------------------------------------------------------------
// Type: Buffer
// -----------------------------------------------------------------------------

static Buffer *Context_meth_buffer(Context *self, PyObject *args,
                                   PyObject *kwargs) {
  static char *keywords[] = {"data",    "size",    "access",   "index",
                             "uniform", "storage", "external", NULL};

  PyObject *data = Py_None;
  PyObject *size_arg = Py_None;
  PyObject *access_arg = Py_None;
  int index = 0;
  int uniform = 0;
  int storage = 0;
  int external = 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|O$OOpppi", keywords, &data,
                                   &size_arg, &access_arg, &index, &uniform,
                                   &storage, &external)) {
    return NULL;
  }

  if (self->is_lost) {
    PyErr_Format(PyExc_RuntimeError, "[HyperGL] the context is lost");
    return NULL;
  }

  if (size_arg != Py_None && !PyLong_CheckExact(size_arg)) {
    PyErr_Format(PyExc_TypeError, "[HyperGL] the size must be an int");
    return NULL;
  }

  if (data == Py_None && size_arg == Py_None) {
    PyErr_Format(PyExc_ValueError, "[HyperGL] data or size is required");
    return NULL;
  }

  if (data != Py_None && size_arg != Py_None) {
    PyErr_Format(PyExc_ValueError, "[HyperGL] data and size are exclusive");
    return NULL;
  }

  int size = 0;
  if (size_arg != Py_None) {
    size = to_int(size_arg);
    if (size <= 0) {
      PyErr_Format(PyExc_ValueError, "[HyperGL] invalid size");
      return NULL;
    }
  }

  int target = storage ? GL_SHADER_STORAGE_BUFFER
                       : (uniform ? GL_UNIFORM_BUFFER
                          : index ? GL_ELEMENT_ARRAY_BUFFER
                                  : GL_ARRAY_BUFFER);

  // We process the memoryview here to avoid doing it inside the lock.
  void *initial_data_ptr = NULL;
  Py_buffer view;
  int have_view = 0;

  if (data != Py_None) {
    // Get contiguous memoryview (Returns New Reference)
    data = PyMemoryView_GetContiguous(data, PyBUF_READ, 'C');
    if (!data)
      return NULL;

    if (PyObject_GetBuffer(data, &view, PyBUF_SIMPLE) < 0) {
      Py_DECREF(data);
      return NULL;
    }

    size = (int)view.len;
    if (size == 0) {
      PyBuffer_Release(&view);
      Py_DECREF(data);
      PyErr_Format(PyExc_ValueError, "[HyperGL] invalid size");
      return NULL;
    }

    initial_data_ptr = view.buf;
    have_view = 1;
  }

  if (access_arg == Py_None) {
    access_arg = uniform ? self->module_state->str_dynamic_draw
                         : self->module_state->str_static_draw;
  }

  int access;
  if (!get_buffer_access(self->module_state, self->module_state->helper,
                         access_arg, &access)) {
    if (have_view) {
      PyBuffer_Release(&view);
      Py_DECREF(data);
    }
    PyErr_Format(PyExc_ValueError, "[HyperGL] invalid access");
    return NULL;
  }

  int buffer = 0;

  PyMutex_Lock(&self->state_lock);

  // Handle dependencies on other state
  if (target == GL_ELEMENT_ARRAY_BUFFER) {
    // Ensures we don't accidentally bind an element buffer to the wrong VAO
    bind_vertex_array_internal(self, 0);
  }

  if (target == GL_UNIFORM_BUFFER) {
    // Invalidate descriptor set cache if manipulating UBOs directly
    Py_XSETREF(self->current_descriptor_set, NULL);
  }

  if (external) {
    buffer = external;
    // If data is provided for an external buffer, we write it now.
    if (initial_data_ptr) {
      glBindBuffer(target, buffer);
      glBufferSubData(target, 0, size, initial_data_ptr);
    }
  } else {
    glGenBuffers(1, (GLuint *)&buffer);
    glBindBuffer(target, buffer);
    // SSBO
    if (target == GL_SHADER_STORAGE_BUFFER) {
      glBufferStorage(target, size, initial_data_ptr,
                      GL_PERSISTENT_WRITE_FLAGS | GL_DYNAMIC_STORAGE_BIT |
                          GL_CLIENT_STORAGE_BIT); // 0x0002 | 0x0040 | 0x0080 |
                                                  // 0x0100 | 0x0200
    } else {
      glBufferData(target, size, initial_data_ptr, access);
    }
  }

  PyMutex_Unlock(&self->state_lock);

  if (have_view) {
    PyBuffer_Release(&view);
    // We are done with 'data'. It was consumed by glBufferData.
    Py_DECREF(data);
  }

  Buffer *res = PyObject_New(Buffer, self->module_state->Buffer_type);
  if (!res) {
    // Rollback
    if (!external && buffer) {
      PyMutex_Lock(&self->state_lock);
      glDeleteBuffers(1, (const unsigned int *)&buffer);
      PyMutex_Unlock(&self->state_lock);
    }
    return NULL;
  }

  res->ctx = self;
  res->buffer = buffer;
  res->target = target;
  res->size = size;
  res->access = access;

  // Initialize struct members to safe defaults
  res->mapped_ptr = NULL;
  res->memoryview = NULL;
  res->is_persistently_mapped = 0;

  return res;
}

static PyObject *Buffer_meth_bind(const Buffer *self, PyObject *args) {
  int unit;
  if (!PyArg_ParseTuple(args, "i", &unit))
    return NULL;

  VALIDATE(unit >= 0 && unit < self->ctx->module_state->limits
                                   .max_shader_storage_buffer_bindings,
           PyExc_ValueError, "[HyperGL] Binding unit %d out of range", unit);

  VALIDATE(self->buffer != 0, PyExc_RuntimeError,
           "[HyperGL] Buffer has been released");

  VALIDATE(self->target == GL_SHADER_STORAGE_BUFFER, PyExc_TypeError,
           "[HyperGL] Only Storage Buffers can be bound");

  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, unit, self->buffer);
  Py_RETURN_NONE;
}

static int Buffer_getbuffer(Buffer *self, Py_buffer *view, int flags) {
  if (!self->mapped_ptr) {
    PyErr_SetString(PyExc_BufferError, "[HyperGL] Buffer not mapped");
    return -1;
  }

  view->buf = self->mapped_ptr;
  view->len = self->size;
  view->readonly = 0;
  view->itemsize = 1;
  view->format = NULL;
  view->ndim = 1;
  view->shape = &view->len;
  view->strides = &view->itemsize;
  view->suboffsets = NULL;
  view->internal = NULL;

  Py_INCREF(self); // memoryview keeps buffer alive
  view->obj = (PyObject *)self;

  return 0;
}

static PyObject *Buffer_meth_map(Buffer *self, PyObject *args) {
  if (self->memoryview) {
    return Py_XNewRef(self->memoryview);
  }

  if (self->buffer == 0) {
    PyErr_SetString(PyExc_RuntimeError, "[HyperGL] Cannot map buffer ID 0");
    return NULL;
  }

  VALIDATE(self->target == GL_SHADER_STORAGE_BUFFER, PyExc_TypeError,
           "[HyperGL] Mapping only supported for SSBOs (ID: %d)", self->buffer);

  VALIDATE(self->size % 16 == 0, PyExc_ValueError,
           "[HyperGL] SSBO size (%d) must be 16-byte aligned", self->size);

  if (!self->mapped_ptr) {
    PyMutex_Lock(&self->ctx->state_lock);

    glBindBuffer(self->target, self->buffer);

    self->mapped_ptr = glMapBufferRange(self->target, 0, self->size,
                                        GL_PERSISTENT_WRITE_FLAGS);

    if (!self->mapped_ptr) {
      GLenum err = glGetError();
      printf("[HyperGL] glMapBufferRange FAILED. GL_ERROR: 0x%X\n", err);
      PyMutex_Unlock(&self->ctx->state_lock);
      PyErr_Format(
          PyExc_RuntimeError,
          "[HyperGL] glMapBufferRange failed (GL_ERR: 0x%X). See console.",
          err);
      return NULL;
    }

    self->is_persistently_mapped = 1;
    PyMutex_Unlock(&self->ctx->state_lock);
  }

  Py_buffer view;
  if (Buffer_getbuffer(self, &view, PyBUF_WRITE) < 0)
    return NULL;

  PyObject *mv = PyMemoryView_FromBuffer(&view);
  if (!mv) {
    PyBuffer_Release(&view);
    return NULL;
  }

  self->memoryview = mv;
  // Buffer_getbuffer INCREF'd self, FromBuffer created mv.
  // self->memoryview holds a strong ref to the view.
  // We return a New Ref to the caller.
  return Py_XNewRef(
      self->memoryview); // Caching this might cause a reference cycle.
}

static PyObject *Buffer_meth_unmap(Buffer *self, PyObject *args) {
  if (!self->mapped_ptr) {
    Py_RETURN_NONE;
  }

  // If it's persistent, unmapping is usually an error in logic
  // unless we are destroying the buffer.
  if (self->is_persistently_mapped) {
    // For Persistent buffers, we only unmap during dealloc/garbage collection
    // But if the user explicitly calls .unmap(), we allow it but clear the
    // view.
  }

  PyMutex_Lock(&self->ctx->state_lock);

  // If the Python user still has a handle (e.g., 'm = buf.map()'),
  // unmapping is a recipe for a Segfault in the physics thread.
  if (self->memoryview && Py_REFCNT(self->memoryview) > 1) {
    PyMutex_Unlock(&self->ctx->state_lock);
    PyErr_SetString(PyExc_BufferError, "[HyperGL] Cannot unmap; physics thread "
                                       "or user still holds the memoryview");
    return NULL;
  }

  glBindBuffer(self->target, self->buffer);
  glUnmapBuffer(self->target);
  self->mapped_ptr = NULL;
  self->is_persistently_mapped = 0;

  // Clear the view so it can't be used again
  Py_CLEAR(self->memoryview);

  PyMutex_Unlock(&self->ctx->state_lock);
  Py_RETURN_NONE;
}

static PyObject *Buffer_meth_write_texture_handle(const Buffer *self,
                                                  PyObject *args,
                                                  PyObject *kwargs) {
  static char *keywords[] = {"offset", "image", NULL};
  PyObject *image_obj;
  int offset;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "iO", keywords, &offset,
                                   &image_obj))
    return NULL;

  if (!PyObject_TypeCheck(image_obj, self->ctx->module_state->Image_type)) {
    PyErr_SetString(PyExc_TypeError,
                    "[HyperGL] Argument must be an Image object");
    return NULL;
  }

  Image *img = (Image *)image_obj;

  // Auto-fetch handle if missing
  if (img->bindless_handle == 0) {
    if (!glGetTextureHandleARB) {
      PyErr_SetString(PyExc_RuntimeError, "[HyperGL] Bindless not supported");
      return NULL;
    }
    PyMutex_Lock(&self->ctx->state_lock);
    img->bindless_handle = glGetTextureHandleARB(img->image);
    PyMutex_Unlock(&self->ctx->state_lock);
    if (img->bindless_handle == 0) {
      PyErr_SetString(PyExc_RuntimeError,
                      "[HyperGL] Failed to get texture handle");
      return NULL;
    }
  }

  if (offset < 0 || offset + sizeof(GLuint64) > (size_t)self->size) {
    PyErr_SetString(PyExc_ValueError, "[HyperGL] Offset out of bounds");
    return NULL;
  }

  // Write handle
  if (self->mapped_ptr) {
    // If mapped (persistent SSBO), write directly
    PyMutex_Lock(&self->ctx->state_lock);
    *(GLuint64 *)((char *)self->mapped_ptr + offset) = img->bindless_handle;
    PyMutex_Unlock(&self->ctx->state_lock);
  } else {
    // Otherwise use SubData
    PyMutex_Lock(&self->ctx->state_lock);
    glBindBuffer(self->target, self->buffer);
    glBufferSubData(self->target, offset, sizeof(GLuint64),
                    &img->bindless_handle);
    PyMutex_Unlock(&self->ctx->state_lock);
  }

  Py_RETURN_NONE;
}

static PyObject *Buffer_meth_write(const Buffer *self, PyObject *args,
                                   PyObject *kwargs) {
  static char *keywords[] = {"data", "offset", NULL};
  PyObject *data;
  int offset = 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|i", keywords, &data,
                                   &offset)) {
    return NULL;
  }

  VALIDATE(!self->ctx->is_lost, PyExc_RuntimeError,
           "[HyperGL] the context is lost");
  VALIDATE(offset >= 0 && offset <= self->size, PyExc_ValueError,
           "[HyperGL] invalid offset");

  BufferView *buffer_view = NULL;
  if (Py_TYPE(data) == self->ctx->module_state->Buffer_type) {
    buffer_view = (BufferView *)PyObject_CallMethod(data, "view", NULL);
  } else if (Py_TYPE(data) == self->ctx->module_state->BufferView_type) {
    buffer_view = (BufferView *)new_ref(data);
  }

  if (buffer_view) {
    if (buffer_view->size + offset > self->size) {
      Py_DECREF(buffer_view);
      PyErr_Format(PyExc_ValueError, "[HyperGL] invalid size");
      return NULL;
    }

    PyMutex_Lock(&self->ctx->state_lock);

    glBindBuffer(GL_COPY_READ_BUFFER, buffer_view->buffer->buffer);
    glBindBuffer(GL_COPY_WRITE_BUFFER, self->buffer);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER,
                        buffer_view->offset, offset, buffer_view->size);
    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

    PyMutex_Unlock(&self->ctx->state_lock);

    Py_DECREF(buffer_view);
    Py_RETURN_NONE;
  }

  PyObject *mem = PyMemoryView_GetContiguous(data, PyBUF_READ, 'C');
  if (!mem)
    return NULL;

  Py_buffer view;
  if (PyObject_GetBuffer(mem, &view, PyBUF_SIMPLE)) {
    Py_DECREF(mem);
    return NULL;
  }

  int data_size = (int)view.len;
  if (data_size + offset > self->size) {
    PyBuffer_Release(&view);
    Py_DECREF(mem);
    PyErr_Format(PyExc_ValueError, "[HyperGL] invalid size");
    return NULL;
  }

  if (data_size > 0) {
    PyMutex_Lock(&self->ctx->state_lock);

    // Logic to prevent breaking VAO/Descriptor state
    if (self->target == GL_ELEMENT_ARRAY_BUFFER) {
      bind_vertex_array_internal(self->ctx, 0);
    }

    if (self->target == GL_UNIFORM_BUFFER) {
      Py_XSETREF(self->ctx->current_descriptor_set, NULL);
    }

    // Use a neutral binding point (GL_COPY_WRITE_BUFFER)
    glBindBuffer(GL_COPY_WRITE_BUFFER, self->buffer);
    glBufferSubData(GL_COPY_WRITE_BUFFER, offset, data_size, view.buf);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

    PyMutex_Unlock(&self->ctx->state_lock);
  }

  PyBuffer_Release(&view);
  Py_DECREF(mem);
  Py_RETURN_NONE;
}

static PyObject *Buffer_meth_read(Buffer *self, PyObject *args,
                                  PyObject *kwargs) {
  static char *keywords[] = {"size", "offset", "into", NULL};

  PyObject *size_arg = Py_None;
  int offset = 0;
  PyObject *into = Py_None;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|OiO", keywords, &size_arg,
                                   &offset, &into)) {
    return NULL;
  }

  if (self->ctx->is_lost) {
    PyErr_Format(PyExc_RuntimeError, "[HyperGL] the context is lost");
    return NULL;
  }

  if (offset < 0 || offset > self->size) {
    PyErr_Format(PyExc_ValueError, "[HyperGL] invalid offset");
    return NULL;
  }

  if (size_arg != Py_None && !PyLong_CheckExact(size_arg)) {
    PyErr_Format(PyExc_TypeError, "[HyperGL] the size must be an int");
    return NULL;
  }

  int size = self->size - offset;
  if (size_arg != Py_None) {
    size = to_int(size_arg);
    if (size < 0) {
      PyErr_Format(PyExc_ValueError, "[HyperGL] invalid size");
      return NULL;
    }
  }

  if (size < 0 || size + offset > self->size) {
    PyErr_Format(PyExc_ValueError, "[HyperGL] invalid size");
    return NULL;
  }

  if (self->target == GL_ELEMENT_ARRAY_BUFFER) {
    bind_vertex_array(self->ctx, 0);
  }

  if (self->target == GL_UNIFORM_BUFFER) {
    self->ctx->current_descriptor_set = NULL;
  }

  glBindBuffer(self->target, self->buffer);

  if (into == Py_None) {
    PyObject *res = PyBytes_FromStringAndSize(NULL, size);
    glGetBufferSubData(self->target, offset, size, PyBytes_AsString(res));
    return res;
  }

  if (Py_TYPE(into) == self->ctx->module_state->Buffer_type) {
    PyObject *chunk =
        PyObject_CallMethod((PyObject *)self, "view", "(ii)", size, offset);
    return PyObject_CallMethod(into, "write", "(N)", chunk);
  }

  if (Py_TYPE(into) == self->ctx->module_state->BufferView_type) {
    BufferView *buffer_view = (BufferView *)into;
    if (size > buffer_view->size) {
      PyErr_Format(PyExc_ValueError, "[HyperGL] invalid size");
      return NULL;
    }
    PyObject *chunk =
        PyObject_CallMethod((PyObject *)self, "view", "(ii)", size, offset);
    return PyObject_CallMethod((PyObject *)buffer_view->buffer, "write", "(Ni)",
                               chunk, buffer_view->offset);
  }

  Py_buffer view;
  if (PyObject_GetBuffer(into, &view, PyBUF_WRITABLE)) {
    return NULL;
  }

  if (size > (int)view.len) {
    PyErr_Format(PyExc_ValueError, "[HyperGL] invalid size");
    return NULL;
  }

  Py_BEGIN_ALLOW_THREADS glBindBuffer(self->target, self->buffer);
  glGetBufferSubData(self->target, offset, size, view.buf);
  Py_END_ALLOW_THREADS

      PyBuffer_Release(&view);
  Py_RETURN_NONE;
}

static BufferView *Buffer_meth_view(Buffer *self, PyObject *args,
                                    PyObject *kwargs) {
  static char *keywords[] = {"size", "offset", NULL};

  PyObject *size_arg = Py_None;
  int offset = 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|Oi", keywords, &size_arg,
                                   &offset)) {
    return NULL;
  }

  int size = self->size - offset;
  if (size_arg != Py_None) {
    size = to_int(size_arg);
  }

  if (self->ctx->is_lost) {
    PyErr_Format(PyExc_RuntimeError, "[HyperGL] the context is lost");
    return NULL;
  }

  if (offset < 0 || offset > self->size) {
    PyErr_Format(PyExc_ValueError, "[HyperGL] invalid offset");
    return NULL;
  }

  if (size < 0 || offset + size > self->size) {
    PyErr_Format(PyExc_ValueError, "[HyperGL] invalid size");
    return NULL;
  }

  BufferView *res =
      PyObject_New(BufferView, self->ctx->module_state->BufferView_type);
  res->buffer = (Buffer *)new_ref(self);
  res->offset = offset;
  res->size = size;
  return res;
}

// -----------------------------------------------------------------------------
// Type: Image
// -----------------------------------------------------------------------------

static int Image_write_internal(const Image *self, PyObject *data) {
  if (self->ctx->is_lost) {
    PyErr_SetString(PyExc_RuntimeError, "[HyperGL] the context is lost");
    return -1;
  }
  if (self->renderbuffer) {
    PyErr_SetString(PyExc_RuntimeError,
                    "[HyperGL] cannot write to renderbuffer");
    return -1;
  }
  if (self->image == 0) {
    PyErr_SetString(PyExc_RuntimeError, "[HyperGL] invalid image object");
    return -1;
  }

  Py_buffer view;
  // We use PyBUF_SIMPLE to get a contiguous pointer
  if (PyObject_GetBuffer(data, &view, PyBUF_SIMPLE) < 0) {
    return -1;
  }

  int bpp = self->fmt.pixel_size;
  Py_ssize_t layers = self->cubemap ? 6 : (self->array ? self->array : 1);
  Py_ssize_t expected_size =
      (Py_ssize_t)self->width * self->height * bpp * layers;

  if (view.len < expected_size) {
    PyErr_Format(
        PyExc_ValueError,
        "[HyperGL] data buffer too small (expected %zd bytes, got %zd)",
        expected_size, view.len);
    PyBuffer_Release(&view);
    return -1;
  }

  PyMutex_Lock(&self->ctx->state_lock);

  glActiveTexture(self->ctx->default_texture_unit);
  glBindTexture(self->target, self->image);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
  glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

  if (self->cubemap) {
    Py_ssize_t face_size = (Py_ssize_t)self->width * self->height * bpp;
    char *ptr = (char *)view.buf;

    for (int i = 0; i < 6; ++i) {
      glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, self->width,
                      self->height, self->fmt.format, self->fmt.type,
                      ptr + (i * face_size));
    }
  } else if (self->array) {
    glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, 0);
    glTexSubImage3D(self->target, 0, 0, 0, 0, self->width, self->height,
                    self->array, self->fmt.format, self->fmt.type, view.buf);
  } else {
    glTexSubImage2D(self->target, 0, 0, 0, self->width, self->height,
                    self->fmt.format, self->fmt.type, view.buf);
  }

  PyMutex_Unlock(&self->ctx->state_lock);

  PyBuffer_Release(&view);
  return 0;
}

static int Image_traverse(const Image *self, visitproc visit, void *arg) {
  Py_VISIT(self->ctx);
  Py_VISIT(self->size);
  Py_VISIT(self->format);
  Py_VISIT(self->faces);
  Py_VISIT(self->layers);
  return 0;
}

static int Image_clear(Image *self) {
  Py_CLEAR(self->ctx);
  Py_CLEAR(self->size);
  Py_CLEAR(self->format);
  Py_CLEAR(self->faces);
  Py_CLEAR(self->layers);
  return 0;
}

static Image *Context_meth_image(Context *self, PyObject *args,
                                 PyObject *kwargs) { // HAS GC_TRACK
  ModuleState *state = self->module_state;
  static char *keywords[] = {"size",     "format", "data",    "samples",
                             "array",    "levels", "texture", "cubemap",
                             "external", NULL};

  int width, height, samples = 1, array = 0, cubemap = 0, levels = 1,
                     external = 0;
  PyObject *format = state->str_rgba8unorm;
  PyObject *data = Py_None;
  PyObject *texture = Py_None;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "(ii)|O!OiiiOpi", keywords,
                                   &width, &height, &PyUnicode_Type, &format,
                                   &data, &samples, &array, &levels, &texture,
                                   &cubemap, &external)) {
    return NULL;
  }

  // --- Validation Logic ---
  int max_levels = count_mipmaps(width, height);
  if (levels <= 0)
    levels = max_levels;
  if (self->is_lost) {
    PyErr_SetString(PyExc_RuntimeError, "[HyperGL] context lost");
    return NULL;
  }

  VALIDATE(texture == Py_True || texture == Py_False || texture == Py_None,
           PyExc_TypeError, "[HyperGL] invalid texture parameter");
  VALIDATE(!(samples > 1 && texture == Py_True), PyExc_TypeError,
           "[HyperGL] for multisampled images texture must be False");
  VALIDATE(samples >= 1 && !(samples & (samples - 1)) && samples <= 16,
           PyExc_ValueError, "[HyperGL] samples must be 1, 2, 4, 8 or 16");
  VALIDATE(array >= 0, PyExc_ValueError,
           "[HyperGL] array must not be negative");
  VALIDATE(levels <= max_levels, PyExc_ValueError, "[HyperGL] too many levels");
  VALIDATE(!(cubemap && array), PyExc_TypeError,
           "[HyperGL] cubemap arrays are not supported");
  VALIDATE(!(samples > 1 && (array || cubemap)), PyExc_TypeError,
           "[HyperGL] multisampled array or cubemap images are not supported");
  VALIDATE(!(texture == Py_False && (array || cubemap)), PyExc_TypeError,
           "[HyperGL] for array or cubemap images texture must be True");
  VALIDATE(!(data != Py_None && samples > 1), PyExc_ValueError,
           "[HyperGL] cannot write to multisampled images");
  VALIDATE(!(data != Py_None && texture == Py_False), PyExc_ValueError,
           "[HyperGL] cannot write to renderbuffers");
  VALIDATE(width > 0 && height > 0, PyExc_ValueError,
           "[HyperGL] invalid image size");
  if (external) {
    VALIDATE(external > 0, PyExc_ValueError,
             "[HyperGL] external must be a valid GL object");
  }

  char renderbuffer = (samples > 1 || texture == Py_False) ? 1 : 0;
  int target = cubemap ? GL_TEXTURE_CUBE_MAP
               : array ? GL_TEXTURE_2D_ARRAY
                       : GL_TEXTURE_2D;
  if (samples > self->module_state->limits.max_samples)
    samples = self->module_state->limits.max_samples;

  ImageFormat fmt;
  if (!get_image_format(state, state->helper, format, &fmt)) {
    PyErr_SetString(PyExc_ValueError, "[HyperGL] invalid image format");
    return NULL;
  }

  // --- Critical Section: OpenGL State ---
  int image = 0;
  PyMutex_Lock(&self->state_lock);

  if (external) {
    image = external;
  } else if (renderbuffer) {
    glGenRenderbuffers(1, (GLuint *)&image);
    glBindRenderbuffer(GL_RENDERBUFFER, image);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples > 1 ? samples : 0,
                                     fmt.internal_format, width, height);
  } else {
    glGenTextures(1, (GLuint *)&image);
    glActiveTexture(self->default_texture_unit);
    glBindTexture(target, image);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    for (int level = 0; level < levels; ++level) {
      int w = least_one(width >> level);
      int h = least_one(height >> level);
      if (cubemap) {
        for (int i = 0; i < 6; ++i)
          glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, level,
                       fmt.internal_format, w, h, 0, fmt.format, fmt.type,
                       NULL);
      } else if (array) {
        glTexImage3D(target, level, fmt.internal_format, w, h, array, 0,
                     fmt.format, fmt.type, NULL);
      } else {
        glTexImage2D(target, level, fmt.internal_format, w, h, 0, fmt.format,
                     fmt.type, NULL);
      }
    }
  }
  PyMutex_Unlock(&self->state_lock);

  if (state->Image_type == NULL) {
    PyErr_SetString(PyExc_RuntimeError,
                    "HyperGL Internal Error: Image type not loaded");
    return NULL;
  }

  // --- Object Allocation ---
  Image *res = PyObject_GC_New(Image, state->Image_type);
  if (!res) {
    return NULL;
  }
  res->ctx = NULL;
  res->size = NULL;
  res->format = NULL;
  res->faces = NULL;
  res->layers = NULL;

  res->ctx = (Context *)new_ref(self);
  res->size = Py_BuildValue("(ii)", width, height);
  res->format = new_ref(format);
  res->faces = PyDict_New();
  res->fmt = fmt;
  res->image = image;
  res->width = width;
  res->height = height;
  res->samples = samples;
  res->array = array;
  res->cubemap = cubemap;
  res->target = target;
  res->renderbuffer = renderbuffer;
  res->external = external;
  res->layer_count = (array ? array : 1) * (cubemap ? 6 : 1);
  res->level_count = levels;

  res->bindless_handle = 0;
  res->is_resident = 0;

  if (fmt.buffer == GL_DEPTH || fmt.buffer == GL_DEPTH_STENCIL) {
    res->clear_value.clear_floats[0] = 1.0F;
  }

  res->layers = PyTuple_New(res->layer_count);
  if (!res->layers) {
    Py_DECREF(res);
    return NULL;
  }

  for (int i = 0; i < res->layer_count; ++i) {
    PyObject *key = Py_BuildValue("(ii)", i, 0);
    if (!key) {
      Py_DECREF(res);
      return NULL;
    }
    ImageFace *face = build_image_face(res, key);
    Py_DECREF(key);
    if (!face) {
      Py_DECREF(res); // triggers dealloc -> cleanup
      return NULL;
    }
    PyTuple_SetItem(res->layers, i, (PyObject *)face);
  }

  // --- Data Upload ---
  if (data != Py_None) {
    if (Image_write_internal(res, data) < 0) {
      Py_DECREF(res);
      return NULL;
    }
  }

  PyObject_GC_Track(res);
  return res;
}

static PyObject *Image_meth_get_handle(Image *self, PyObject *args) {
  if (self->ctx->is_lost) {
    PyErr_Format(PyExc_RuntimeError, "[HyperGL] context lost");
    return NULL;
  }

  // Lazy creation of the handle
  if (self->bindless_handle == 0) {
    if (!glGetTextureHandleARB) {
      PyErr_SetString(PyExc_RuntimeError,
                      "[HyperGL] Bindless textures not supported "
                      "(GL_ARB_bindless_texture missing)");
      return NULL;
    }

    PyMutex_Lock(&self->ctx->state_lock);
    // Ensure texture is bound or just pass ID if DSA is supported,
    // but glGetTextureHandleARB usually takes the texture ID directly.
    self->bindless_handle = glGetTextureHandleARB(self->image);
    PyMutex_Unlock(&self->ctx->state_lock);

    if (self->bindless_handle == 0) {
      PyErr_SetString(PyExc_RuntimeError,
                      "[HyperGL] Failed to create texture handle");
      return NULL;
    }
  }

  return PyLong_FromUnsignedLongLong(self->bindless_handle);
}

static PyObject *Image_meth_make_resident(Image *self, PyObject *args) {
  if (self->ctx->is_lost) {
    PyErr_SetString(PyExc_RuntimeError, "[HyperGL] context lost");
    return NULL;
  }

  int resident = 1; // Default True
  if (!PyArg_ParseTuple(args, "|p", &resident))
    return NULL;

  if (self->bindless_handle == 0) {
    PyErr_SetString(
        PyExc_RuntimeError,
        "[HyperGL] Texture has no handle. Call get_handle() first.");
    return NULL;
  }

  PyMutex_Lock(&self->ctx->state_lock);
  if (resident && !self->is_resident) {
    glMakeTextureHandleResidentARB(self->bindless_handle);
    if (glGetError() == GL_NO_ERROR)
      self->is_resident = 1;
    else
      PyErr_SetString(PyExc_RuntimeError,
                      "[HyperGL] Failed to make texture handle resident");
  } else if (!resident && self->is_resident) {
    glMakeTextureHandleNonResidentARB(self->bindless_handle);
    if (glGetError() == GL_NO_ERROR)
      self->is_resident = 0;
    else
      PyErr_SetString(PyExc_RuntimeError,
                      "[HyperGL] Failed to make texture handle non-resident");
  }
  PyMutex_Unlock(&self->ctx->state_lock);

  Py_RETURN_NONE;
}

static PyObject *Image_meth_clear(const Image *self, PyObject *args) {
  if (self->ctx->is_lost) {
    PyErr_Format(PyExc_RuntimeError, "[HyperGL] the context is lost");
    return NULL;
  }

  const int count = (int)PyTuple_Size(self->layers);

  // Get all FBO IDs into a stack-allocated or temporary C array.
  // This happens while we still own the GIL/Thread State.
  int *fbo_ids = (int *)PyMem_Malloc(count * sizeof(int));
  if (!fbo_ids)
    return PyErr_NoMemory();

  for (int i = 0; i < count; ++i) {
    ImageFace *face = (ImageFace *)PyTuple_GetItem(self->layers, i);
    fbo_ids[i] = face->framebuffer->obj;
  }

  Py_BEGIN_ALLOW_THREADS PyMutex_Lock(&self->ctx->state_lock);

  int prev_draw = self->ctx->current_draw_framebuffer;

  for (int i = 0; i < count; ++i) {
    if (self->ctx->current_draw_framebuffer != fbo_ids[i]) {
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_ids[i]);
      self->ctx->current_draw_framebuffer = fbo_ids[i];
    }
    clear_bound_image(self);
  }

  // Restore state
  if (self->ctx->current_draw_framebuffer != prev_draw) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, prev_draw);
    self->ctx->current_draw_framebuffer = prev_draw;
  }

  PyMutex_Unlock(&self->ctx->state_lock);
  Py_END_ALLOW_THREADS

      PyMem_Free(fbo_ids);
  Py_RETURN_NONE;
}

static PyObject *Image_meth_write(const Image *self, PyObject *args,
                                  PyObject *kwargs) {
  static char *keywords[] = {"data", "size", "offset", "layer", "level", NULL};

  PyObject *data;
  PyObject *size_arg = Py_None;
  PyObject *offset_arg = Py_None;
  PyObject *layer_arg = Py_None;
  int level = 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|OOOi", keywords, &data,
                                   &size_arg, &offset_arg, &layer_arg,
                                   &level)) {
    return NULL;
  }

  if (self->ctx->is_lost) {
    PyErr_Format(PyExc_RuntimeError, "[HyperGL] the context is lost");
    return NULL;
  }

  int level_w = least_one(self->width >> level);
  int level_h = least_one(self->height >> level);
  int layer = (layer_arg != Py_None) ? to_int(layer_arg) : 0;

  IntPair size;
  IntPair offset;
  if (!to_int_pair(&size, size_arg, level_w, level_h) ||
      !to_int_pair(&offset, offset_arg, 0, 0)) {
    PyErr_Format(PyExc_TypeError,
                 "[HyperGL] size and offset must be tuples of 2 ints");
    return NULL;
  }

  if (size.x <= 0 || size.y <= 0 || size.x + offset.x > level_w ||
      size.y + offset.y > level_h) {
    PyErr_Format(PyExc_ValueError,
                 "[HyperGL] invalid size or offset for level %d", level);
    return NULL;
  }

  if (layer < 0 || layer >= self->layer_count ||
      (layer_arg != Py_None && !self->cubemap && !self->array)) {
    PyErr_Format(PyExc_ValueError, "[HyperGL] invalid layer selection");
    return NULL;
  }

  int expected_size = size.x * size.y * self->fmt.pixel_size;
  if (layer_arg == Py_None)
    expected_size *= self->layer_count;

  BufferView *buffer_view = NULL;
  Py_buffer view = {0};
  PyObject *mem = NULL;

  if (Py_TYPE(data) == self->ctx->module_state->Buffer_type ||
      Py_TYPE(data) == self->ctx->module_state->BufferView_type) {
    buffer_view = (Py_TYPE(data) == self->ctx->module_state->Buffer_type)
                      ? (BufferView *)PyObject_CallMethod(data, "view", NULL)
                      : (BufferView *)Py_NewRef(data);
  } else {
    mem = PyMemoryView_GetContiguous(data, PyBUF_READ, 'C');
    if (!mem) {
      return NULL;
    }
    if (PyObject_GetBuffer(mem, &view, PyBUF_SIMPLE)) {
      Py_DECREF(mem);
      return NULL;
    }
  }

  if ((buffer_view ? buffer_view->size : (int)view.len) != expected_size) {
    PyErr_Format(PyExc_ValueError, "[HyperGL] data size mismatch: expected %d",
                 expected_size);
    goto cleanup;
  }

  PyMutex_Lock(&self->ctx->state_lock);

  glActiveTexture(self->ctx->default_texture_unit);
  glBindTexture(self->target, self->image);

  void *pixels =
      buffer_view ? (unsigned char *)NULL + buffer_view->offset : view.buf;

  if (buffer_view)
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer_view->buffer->buffer);

  if (self->cubemap) {
    int stride = size.x * size.y * self->fmt.pixel_size;
    if (layer_arg != Py_None) {
      glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + layer, level, offset.x,
                      offset.y, size.x, size.y, self->fmt.format,
                      self->fmt.type, pixels);
    } else {
      for (int i = 0; i < 6; ++i) {
        glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, level, offset.x,
                        offset.y, size.x, size.y, self->fmt.format,
                        self->fmt.type, (char *)pixels + ((size_t)stride * i));
      }
    }
  } else if (self->array) {
    int depth = (layer_arg != Py_None) ? 1 : self->layer_count;
    glTexSubImage3D(self->target, level, offset.x, offset.y,
                    (layer_arg != Py_None ? layer : 0), size.x, size.y, depth,
                    self->fmt.format, self->fmt.type, pixels);
  } else {
    glTexSubImage2D(self->target, level, offset.x, offset.y, size.x, size.y,
                    self->fmt.format, self->fmt.type, pixels);
  }

  if (buffer_view)
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

  PyMutex_Unlock(&self->ctx->state_lock);

  if (mem) {
    PyBuffer_Release(&view);
    Py_DECREF(mem);
  }
  Py_XDECREF(buffer_view);
  Py_RETURN_NONE;

cleanup:
  if (mem) {
    PyBuffer_Release(&view);
    Py_DECREF(mem);
  }
  Py_XDECREF(buffer_view);
  return NULL;
}

static PyObject *Image_meth_mipmaps(const Image *self, PyObject *args) {
  if (self->renderbuffer) {
    PyErr_Format(PyExc_TypeError,
                 "[HyperGL] cannot generate mipmaps for renderbuffers");
    return NULL;
  }

  if (self->ctx->is_lost) {
    PyErr_Format(PyExc_RuntimeError, "[HyperGL] the context is lost");
    return NULL;
  }

  PyMutex_Lock(&self->ctx->state_lock);

  glActiveTexture(self->ctx->default_texture_unit);
  glBindTexture(self->target, self->image);
  glGenerateMipmap(self->target);

  PyMutex_Unlock(&self->ctx->state_lock);

  Py_RETURN_NONE;
}

static PyObject *Image_meth_read(const Image *self, PyObject *args,
                                 PyObject *kwargs) {
  static char *keywords[] = {"size", "offset", "into", NULL};

  PyObject *size_arg = Py_None;
  PyObject *offset_arg = Py_None;
  PyObject *into = Py_None;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|OOO", keywords, &size_arg,
                                   &offset_arg, &into)) {
    return NULL;
  }

  IntPair size;
  IntPair offset;
  ImageFace *first_layer = (ImageFace *)PyTuple_GetItem(self->layers, 0);
  if (!parse_size_and_offset(first_layer, size_arg, offset_arg, &size,
                             &offset)) {
    return NULL;
  }

  if (self->ctx->is_lost) {
    PyErr_Format(PyExc_RuntimeError, "[HyperGL] the context is lost");
    return NULL;
  }

  if (self->array || self->cubemap) {
    if (into != Py_None) {
      PyErr_Format(PyExc_TypeError,
                   "[HyperGL] cannot read into user buffer for layered images");
      return NULL;
    }

    int write_size = size.x * size.y * self->fmt.pixel_size;
    PyObject *res = PyBytes_FromStringAndSize(NULL, (Py_ssize_t)write_size *
                                                        self->layer_count);
    for (int i = 0; i < self->layer_count; ++i) {
      ImageFace *src = (ImageFace *)PyTuple_GetItem(self->layers, i);
      PyObject *chunk = PyMemoryView_FromMemory(PyBytes_AsString(res) +
                                                    ((size_t)write_size * i),
                                                write_size, PyBUF_WRITE);
      PyObject *temp = read_image_face(src, size, offset, chunk);
      if (!temp) {
        Py_DECREF(chunk);
        Py_DECREF(res);
        return NULL;
      }
      Py_DECREF(chunk);
      Py_DECREF(temp);
    }
    return res;
  }

  return read_image_face(first_layer, size, offset, into);
}

static PyObject *Image_meth_blit(const Image *self, PyObject *args,
                                 PyObject *kwargs) {
  static char *keywords[] = {"target", "offset", "size",
                             "crop",   "filter", NULL};

  PyObject *target = Py_None;
  PyObject *offset = Py_None;
  PyObject *size = Py_None;
  PyObject *crop = Py_None;
  int filter = 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|OOOOp", keywords, &target,
                                   &offset, &size, &crop, &filter)) {
    return NULL;
  }

  if (self->ctx->is_lost) {
    PyErr_Format(PyExc_RuntimeError, "[HyperGL] the context is lost");
    return NULL;
  }

  ImageFace *src = (ImageFace *)PyTuple_GetItem(self->layers, 0);
  return blit_image_face(src, target, offset, size, crop, filter);
}

static ImageFace *Image_meth_face(Image *self, PyObject *args,
                                  PyObject *kwargs) {
  static char *keywords[] = {"layer", "level", NULL};

  int layer = 0;
  int level = 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|ii", keywords, &layer,
                                   &level)) {
    return NULL;
  }

  if (self->ctx->is_lost) {
    PyErr_Format(PyExc_RuntimeError, "[HyperGL] the context is lost");
    return NULL;
  }

  if (layer < 0 || layer >= self->layer_count) {
    PyErr_Format(PyExc_ValueError, "[HyperGL] invalid layer");
    return NULL;
  }

  if (level > self->level_count) {
    PyErr_Format(PyExc_ValueError, "[HyperGL] invalid level");
    return NULL;
  }

  PyObject *key = Py_BuildValue("(ii)", layer, level);
  ImageFace *res = build_image_face(self, key);
  Py_DECREF(key);
  return res;
}

static PyObject *Image_get_clear_value(const Image *self, void *closure) {
  if (self->fmt.clear_type == 'x') {
    return Py_BuildValue("dI", (double)self->clear_value.clear_floats[0],
                         self->clear_value.clear_uints[1]);
  }
  if (self->fmt.components == 1) {
    if (self->fmt.clear_type == 'f') {
      return PyFloat_FromDouble(self->clear_value.clear_floats[0]);
    }
    if (self->fmt.clear_type == 'i') {
      return PyLong_FromLong(self->clear_value.clear_ints[0]);
    }
    if (self->fmt.clear_type == 'u') {
      return PyLong_FromUnsignedLong(self->clear_value.clear_uints[0]);
    }
  }
  PyObject *res = PyTuple_New(self->fmt.components);
  for (int i = 0; i < self->fmt.components; ++i) {
    if (self->fmt.clear_type == 'f') {
      PyTuple_SetItem(res, i,
                      PyFloat_FromDouble(self->clear_value.clear_floats[i]));
    } else if (self->fmt.clear_type == 'i') {
      PyTuple_SetItem(res, i, PyLong_FromLong(self->clear_value.clear_ints[i]));
    } else if (self->fmt.clear_type == 'u') {
      PyTuple_SetItem(
          res, i, PyLong_FromUnsignedLong(self->clear_value.clear_uints[i]));
    }
  }
  return res;
}

static int Image_set_clear_value(Image *self, PyObject *value, void *closure) {
  if (self->fmt.components == 1) {
    if (self->fmt.clear_type == 'f' && !PyFloat_CheckExact(value)) {
      PyErr_Format(PyExc_TypeError,
                   "[HyperGL] the clear value must be a float");
      return -1;
    }
    if (self->fmt.clear_type == 'i' && !PyLong_CheckExact(value)) {
      PyErr_Format(PyExc_TypeError, "[HyperGL] the clear value must be an int");
      return -1;
    }
    if (self->fmt.clear_type == 'f') {
      self->clear_value.clear_floats[0] = to_float(value);
    } else if (self->fmt.clear_type == 'i') {
      self->clear_value.clear_ints[0] = to_int(value);
    } else if (self->fmt.clear_type == 'u') {
      self->clear_value.clear_uints[0] = to_uint(value);
    }
    return 0;
  }
  PyObject *values = PySequence_Tuple(value);
  if (!values) {
    PyErr_Clear();
    PyErr_Format(PyExc_TypeError, "[HyperGL] the clear value must be a tuple");
    return -1;
  }

  int size = (int)PyTuple_Size(values);

  if (size != self->fmt.components) {
    Py_DECREF(values);
    PyErr_Format(PyExc_ValueError, "[HyperGL] invalid clear value size");
    return -1;
  }

  if (self->fmt.clear_type == 'f') {
    for (int i = 0; i < self->fmt.components; ++i) {
      self->clear_value.clear_floats[i] = to_float(PyTuple_GetItem(values, i));
    }
  } else if (self->fmt.clear_type == 'i') {
    for (int i = 0; i < self->fmt.components; ++i) {
      self->clear_value.clear_ints[i] = to_int(PyTuple_GetItem(values, i));
    }
  } else if (self->fmt.clear_type == 'u') {
    for (int i = 0; i < self->fmt.components; ++i) {
      self->clear_value.clear_uints[i] = to_uint(PyTuple_GetItem(values, i));
    }
  } else if (self->fmt.clear_type == 'x') {
    self->clear_value.clear_floats[0] = to_float(PyTuple_GetItem(values, 0));
    self->clear_value.clear_ints[1] = to_int(PyTuple_GetItem(values, 1));
  }
  if (PyErr_Occurred()) {
    Py_DECREF(values);
    return -1;
  }
  Py_DECREF(values);
  return 0;
}

// -----------------------------------------------------------------------------
// Type: Pipeline
// -----------------------------------------------------------------------------

static Pipeline *Context_meth_pipeline(Context *self, PyObject *args,
                                       PyObject *kwargs) {
  // 1. Variable Declarations
  PyObject *create_kwargs = NULL;
  GLObject *program = NULL;
  PyObject *uniform_layout = NULL;
  PyObject *validate = NULL;
  PyObject *layout_bindings = NULL;
  PyObject *template_obj = NULL;
  PyObject *framebuffer_attachments = NULL;
  PyObject *vertex_array_bindings = NULL;
  PyObject *resource_bindings = NULL;
  PyObject *settings = NULL;
  PyObject *uniforms = NULL;
  PyObject *uniform_data = NULL;
  GLObject *framebuffer = NULL;
  GLObject *vertex_array = NULL;
  DescriptorSet *descriptor_set = NULL;
  GlobalSettings *global_settings = NULL;
  Pipeline *res = NULL;

  // 2. Argument Parsing
  static char *keywords[] = {
      "vertex_shader", "fragment_shader", "layout",       "resources",
      "uniforms",      "depth",           "stencil",      "blend",
      "framebuffer",   "vertex_buffers",  "index_buffer", "short_index",
      "cull_face",     "topology",        "vertex_count", "instance_count",
      "first_vertex",  "viewport",        "uniform_data", "viewport_data",
      "render_data",   "includes",        NULL,
  };

  PyObject *vertex_shader = NULL;
  PyObject *fragment_shader = NULL;
  PyObject *layout = self->module_state->empty_tuple;
  PyObject *resources = self->module_state->empty_tuple;
  PyObject *arg_uniforms = Py_None;
  PyObject *depth = Py_None;
  PyObject *stencil = Py_None;
  PyObject *blend = Py_None;
  PyObject *framebuffer_arg = NULL;
  PyObject *vertex_buffers = self->module_state->empty_tuple;
  PyObject *index_buffer = Py_None;
  int short_index = 0;
  PyObject *cull_face = self->module_state->str_none;
  PyObject *topology_arg = self->module_state->str_triangles;
  int vertex_count = 0;
  int instance_count = 1;
  int first_vertex = 0;
  PyObject *viewport = Py_None;
  PyObject *arg_uniform_data = Py_None;
  PyObject *viewport_data = Py_None;
  PyObject *render_data = Py_None;
  PyObject *includes = Py_None;

  if (PyTuple_GET_SIZE(args) != 0 || !kwargs) {
    PyErr_Format(PyExc_TypeError,
                 "[HyperGL] pipeline only takes keyword-only arguments");
    return NULL;
  }

  Pipeline *template = NULL;
  template_obj = PyDict_GetItemString(kwargs, "template");
  if (template_obj) {
    Py_INCREF(template_obj);
    template = (Pipeline *)template_obj;
  }

  if (template &&
      Py_TYPE((PyObject *)template) != self->module_state->Pipeline_type) {
    PyErr_Format(PyExc_ValueError, "[HyperGL] invalid template");
    return NULL;
  }

  if (template) {
    if (PyDict_GetItemString(kwargs, "vertex_shader") ||
        PyDict_GetItemString(kwargs, "fragment_shader") ||
        PyDict_GetItemString(kwargs, "layout") ||
        PyDict_GetItemString(kwargs, "includes")) {
      PyErr_Format(PyExc_ValueError, "[HyperGL] cannot use template with "
                                     "shader/layout/includes specified");
      return NULL;
    }
    create_kwargs = PyDict_Copy(template->create_kwargs);
    if (!create_kwargs)
      goto fail;
    PyDict_Update(create_kwargs, kwargs);
    PyDict_DelItemString(create_kwargs, "template");
  } else {
    create_kwargs = PyDict_Copy(kwargs);
    if (!create_kwargs)
      goto fail;
  }

  if (!PyArg_ParseTupleAndKeywords(
          args, create_kwargs, "|$O!O!OOOOOOOOOpOOiiiOOOOO", keywords,
          &PyUnicode_Type, &vertex_shader, &PyUnicode_Type, &fragment_shader,
          &layout, &resources, &arg_uniforms, &depth, &stencil, &blend,
          &framebuffer_arg, &vertex_buffers, &index_buffer, &short_index,
          &cull_face, &topology_arg, &vertex_count, &instance_count,
          &first_vertex, &viewport, &arg_uniform_data, &viewport_data,
          &render_data, &includes)) {
    goto fail;
  }

  if (self->is_lost) {
    PyErr_SetString(PyExc_RuntimeError, "[HyperGL] context lost");
    goto fail;
  }
  if (!vertex_shader || !fragment_shader || !framebuffer_arg) {
    PyErr_SetString(PyExc_TypeError, "[HyperGL] missing required args");
    goto fail;
  }

  Viewport viewport_value;
  if (!to_viewport(&viewport_value, viewport, 0, 0, 0, 0))
    goto fail;

  int topology;
  if (!get_topology(self->module_state, self->module_state->helper,
                    topology_arg, &topology))
    goto fail;

  // 4. Logic & Creation
  if (template) {
    program = (GLObject *)new_ref(template->program);
    Atomic_Increment(&program->uses);
  } else {
    program =
        compile_program(self, includes != Py_None ? includes : self->includes,
                        vertex_shader, fragment_shader, layout);
  }
  if (!program)
    goto fail;

  if (arg_uniforms != Py_None) {
    PyObject *tuple =
        PyObject_CallMethod(self->module_state->helper, "uniforms", "(OOO)",
                            program->extra, arg_uniforms, arg_uniform_data);
    if (!tuple)
      goto fail;
    // try not to break things here. these are borrowed refs.
    PyObject *item0 = PyTuple_GetItem(tuple, 0);
    PyObject *item1 = PyTuple_GetItem(tuple, 1);
    PyObject *item2 = PyTuple_GetItem(tuple, 2);

    uniforms = PyDictProxy_New(item0);
    if (!uniforms) {
      Py_DECREF(tuple);
      goto fail;
    }

    uniform_layout = new_ref(item1);
    uniform_data = new_ref(item2);
    Py_DECREF(tuple);
  } else {
    uniforms = NULL;
    if (arg_uniform_data != Py_None) {
      uniform_data = new_ref(arg_uniform_data);
    } else {
      uniform_data = NULL;
    }
  }

  validate = PyObject_CallMethod(self->module_state->helper, "validate",
                                 "(OOOOO)", program->extra, layout, resources,
                                 vertex_buffers, self->info_dict);
  if (!validate)
    goto fail;

  layout_bindings = PyObject_CallMethod(self->module_state->helper,
                                        "layout_bindings", "(O)", layout);
  if (!layout_bindings)
    goto fail;
  if (!PyList_Check(layout_bindings)) {
    PyErr_SetString(PyExc_TypeError,
                    "[HyperGL] layout_bindings must be a list");
    goto fail;
  }

  int layout_count = (int)PyList_Size(layout_bindings);
  if (layout_count > 0) {
    PyMutex_Lock(&self->state_lock);
    int prev_program = self->current_program;
    bind_program_internal(self, program->obj);
    for (int i = 0; i < layout_count; ++i) {
      PyObject *obj = PyList_GetItem(layout_bindings, i);
      if (!PyTuple_Check(obj) || PyTuple_GET_SIZE(obj) < 2) {
        PyErr_SetString(PyExc_TypeError, "[HyperGL] invalid layout binding");
        PyMutex_Unlock(&self->state_lock);
        goto fail;
      }
      PyObject *name = PyTuple_GetItem(obj, 0);
      int binding = to_int(PyTuple_GetItem(obj, 1));

      const char *name_str = PyUnicode_AsUTF8(name);
      if (!name_str) {
        PyMutex_Unlock(&self->state_lock);
        goto fail;
      }

      int location = glGetUniformLocation(program->obj, name_str);
      if (location >= 0)
        glUniform1i(location, binding);
      else {
        GLuint index = glGetUniformBlockIndex(program->obj, name_str);
        if (index != GL_INVALID_INDEX) {
          glUniformBlockBinding(program->obj, index, (GLuint)binding);
        }
      }
    }
    bind_program_internal(self, prev_program);
    PyMutex_Unlock(&self->state_lock);
  }

  framebuffer_attachments =
      PyObject_CallMethod(self->module_state->helper, "framebuffer_attachments",
                          "(O)", framebuffer_arg);
  if (!framebuffer_attachments)
    goto fail;

  if (framebuffer_attachments != Py_None && viewport == Py_None) {
    PyObject *size = PyTuple_GetItem(framebuffer_attachments, 0);
    viewport_value.width = to_int(PyTuple_GetItem(size, 0));
    viewport_value.height = to_int(PyTuple_GetItem(size, 1));
  }

  // All GLObject builders must internally acquire ctx->state_lock
  framebuffer = build_framebuffer(self, framebuffer_attachments);
  if (!framebuffer)
    goto fail;

  vertex_array_bindings =
      PyObject_CallMethod(self->module_state->helper, "vertex_array_bindings",
                          "(OO)", vertex_buffers, index_buffer);
  if (!vertex_array_bindings)
    goto fail;

  vertex_array = build_vertex_array(self, vertex_array_bindings);
  if (!vertex_array)
    goto fail;

  resource_bindings = PyObject_CallMethod(
      self->module_state->helper, "resource_bindings", "(O)", resources);
  if (!resource_bindings)
    goto fail;

  descriptor_set = build_descriptor_set(self, resource_bindings);
  if (!descriptor_set)
    goto fail;

  settings = PyObject_CallMethod(self->module_state->helper, "settings",
                                 "(OOOOO)", cull_face, depth, stencil, blend,
                                 framebuffer_attachments);
  if (!settings)
    goto fail;

  global_settings = build_global_settings(self, settings);
  if (!global_settings)
    goto fail;

  // 5. Allocation
  res = PyObject_GC_New(Pipeline, self->module_state->Pipeline_type);
  if (!res)
    goto fail;

  // Init pointers to NULL
  res->ctx = NULL;
  res->create_kwargs = NULL;
  res->framebuffer = NULL;
  res->vertex_array = NULL;
  res->program = NULL;
  res->uniforms = NULL;
  res->uniform_layout = NULL;
  res->uniform_data = NULL;
  res->descriptor_set = NULL;
  res->global_settings = NULL;
  res->viewport_data = NULL;
  res->render_data = NULL;

  zeromem(&res->uniform_layout_buffer, sizeof(Py_buffer));
  zeromem(&res->uniform_data_buffer, sizeof(Py_buffer));
  zeromem(&res->viewport_data_buffer, sizeof(Py_buffer));
  zeromem(&res->render_data_buffer, sizeof(Py_buffer));
  zeromem(&res->params, sizeof(RenderParameters));
  zeromem(&res->viewport, sizeof(Viewport));

  res->ctx = self;
  Py_INCREF(self);

  // Steal References (Ownership transfer from locals to struct)
  res->create_kwargs = create_kwargs;
  create_kwargs = NULL;
  res->framebuffer = framebuffer;
  framebuffer = NULL;
  res->vertex_array = vertex_array;
  vertex_array = NULL;
  res->program = program;
  program = NULL;
  res->uniforms = uniforms;
  uniforms = NULL;
  res->uniform_layout = uniform_layout;
  uniform_layout = NULL;
  res->uniform_data = uniform_data;
  uniform_data = NULL;
  res->descriptor_set = descriptor_set;
  descriptor_set = NULL;
  res->global_settings = global_settings;
  global_settings = NULL;

  res->topology = topology;
  res->viewport = viewport_value;
  res->params.vertex_count = vertex_count;
  res->params.instance_count = instance_count;
  res->params.first_vertex = first_vertex;
  res->index_type = (index_buffer != Py_None)
                        ? (short_index ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT)
                        : 0;
  res->index_size = short_index ? 2 : 4;

  // 6. MemoryViews
  if (viewport_data == Py_None) {
    Py_buffer view = {0};
    view.buf = (void *)&res->viewport;
    view.len = sizeof(Viewport);
    view.readonly = 0;
    view.itemsize = 1;
    view.format = NULL;
    view.ndim = 1;
    view.shape = &view.len;
    view.strides = &view.itemsize;
    view.suboffsets = NULL;
    view.obj = (PyObject *)res;
    Py_INCREF(res);

    res->viewport_data = PyMemoryView_FromBuffer(&view);
    if (!res->viewport_data) {
      Py_DECREF(res);
      goto fail;
    }
  } else {
    res->viewport_data = new_ref(viewport_data);
  }

  if (render_data == Py_None) {
    Py_buffer view = {0};
    view.buf = (void *)&res->params;
    view.len = sizeof(RenderParameters);
    view.readonly = 0;
    view.itemsize = 1;
    view.format = NULL;
    view.ndim = 1;
    view.shape = &view.len;
    view.strides = &view.itemsize;
    view.suboffsets = NULL;
    view.obj = (PyObject *)res;
    Py_INCREF(res);

    res->render_data = PyMemoryView_FromBuffer(&view);
    if (!res->render_data) {
      Py_DECREF(res);
      goto fail;
    }
  } else {
    res->render_data = new_ref(render_data);
  }

  // 7. Buffer Acquisition
  if (PyObject_GetBuffer(res->viewport_data, &res->viewport_data_buffer,
                         PyBUF_SIMPLE) < 0)
    goto fail;
  if (PyObject_GetBuffer(res->render_data, &res->render_data_buffer,
                         PyBUF_SIMPLE) < 0)
    goto fail;

  if (res->uniforms) {
    if (PyObject_GetBuffer(res->uniform_layout, &res->uniform_layout_buffer,
                           PyBUF_SIMPLE) < 0)
      goto fail;
    if (PyObject_GetBuffer(res->uniform_data, &res->uniform_data_buffer,
                           PyBUF_SIMPLE) < 0)
      goto fail;
  }

  PyObject_GC_Track(res);

  Py_XDECREF(validate);
  Py_XDECREF(layout_bindings);
  Py_XDECREF(framebuffer_attachments);
  Py_XDECREF(vertex_array_bindings);
  Py_XDECREF(resource_bindings);
  Py_XDECREF(settings);

  return res;

fail:
  if (res && res->viewport_data_buffer.obj)
    PyBuffer_Release(&res->viewport_data_buffer);

  if (res && res->render_data_buffer.obj)
    PyBuffer_Release(&res->render_data_buffer);

  if (res && res->uniform_layout_buffer.obj)
    PyBuffer_Release(&res->uniform_layout_buffer);

  if (res && res->uniform_data_buffer.obj)
    PyBuffer_Release(&res->uniform_data_buffer);

  Py_XDECREF(create_kwargs);
  Py_XDECREF(program);
  Py_XDECREF(uniforms);
  Py_XDECREF(uniform_layout);
  Py_XDECREF(uniform_data);
  Py_XDECREF(template_obj);
  if (framebuffer)
    Py_DECREF(framebuffer);
  if (vertex_array)
    Py_DECREF(vertex_array);
  if (descriptor_set)
    Py_DECREF(descriptor_set);
  if (global_settings)
    Py_DECREF(global_settings);

  Py_XDECREF(validate);
  Py_XDECREF(layout_bindings);
  Py_XDECREF(framebuffer_attachments);
  Py_XDECREF(vertex_array_bindings);
  Py_XDECREF(resource_bindings);
  Py_XDECREF(settings);

  if (res)
    Py_DECREF(res);
  return NULL;
}

static int Pipeline_traverse(const Pipeline *self, visitproc visit, void *arg) {
  Py_VISIT(self->ctx);
  Py_VISIT(self->create_kwargs);
  Py_VISIT(self->descriptor_set);
  Py_VISIT(self->global_settings);
  Py_VISIT(self->framebuffer);
  Py_VISIT(self->vertex_array);
  Py_VISIT(self->program);
  Py_VISIT(self->uniforms);
  Py_VISIT(self->uniform_layout);
  Py_VISIT(self->uniform_data);
  Py_VISIT(self->viewport_data);
  Py_VISIT(self->render_data);
  return 0;
}

static int Pipeline_clear(Pipeline *self) {
  Py_CLEAR(self->ctx);
  Py_CLEAR(self->create_kwargs);
  Py_CLEAR(self->descriptor_set);
  Py_CLEAR(self->global_settings);
  Py_CLEAR(self->framebuffer);
  Py_CLEAR(self->vertex_array);
  Py_CLEAR(self->program);
  Py_CLEAR(self->uniforms);
  Py_CLEAR(self->uniform_layout);
  Py_CLEAR(self->uniform_data);
  Py_CLEAR(self->viewport_data);
  Py_CLEAR(self->render_data);
  return 0;
}

static PyObject *
Pipeline_meth_render(const Pipeline *self,
                     PyObject *args) // LGTM. Don’t overthink this path.
                                     // Indirect handles the scaling problem.
{
  if (self->ctx->is_lost) {
    PyErr_Format(PyExc_RuntimeError, "[HyperGL] the context is lost");
    return NULL;
  }

  PyMutex_Lock(&self->ctx->state_lock);

  Viewport *viewport = (Viewport *)self->viewport_data_buffer.buf;

  bind_viewport_internal(self->ctx, viewport);
  bind_global_settings_internal(self->ctx, self->global_settings);
  bind_draw_framebuffer_internal(self->ctx, self->framebuffer->obj);
  bind_program_internal(self->ctx, self->program->obj);
  bind_vertex_array_internal(self->ctx, self->vertex_array->obj);
  bind_descriptor_set_internal(self->ctx, self->descriptor_set);

  if (self->uniforms) {
    bind_uniforms(self);
  }

  RenderParameters *params = (RenderParameters *)self->render_data_buffer.buf;
  if (self->index_type) {
    intptr offset = (intptr)params->first_vertex * (intptr)self->index_size;
    glDrawElementsInstanced(self->topology, params->vertex_count,
                            self->index_type, offset, params->instance_count);
  } else {
    glDrawArraysInstanced(self->topology, params->first_vertex,
                          params->vertex_count, params->instance_count);
  }

  PyMutex_Unlock(&self->ctx->state_lock);

  Py_RETURN_NONE;
}

static PyObject *Pipeline_meth_render_indirect(const Pipeline *self,
                                               PyObject *args,
                                               PyObject *kwargs) {
  static char *keywords[] = {"buffer", "count", "offset", "stride", NULL};
  PyObject *buffer_obj;
  int draw_count;
  int offset = 0;
  int stride = self->index_type ? sizeof(DrawElementsIndirectCommand)
                                : sizeof(DrawArraysIndirectCommand);

  int user_stride = -1;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "Oi|ii", keywords, &buffer_obj,
                                   &draw_count, &offset, &user_stride)) {
    return NULL;
  }

  if (draw_count < 0) {
    PyErr_SetString(PyExc_ValueError, "[HyperGL] count must be >= 0");
    return NULL;
  }

  if (offset < 0) {
    PyErr_SetString(PyExc_ValueError, "[HyperGL] offset must be >= 0");
    return NULL;
  }

  if (user_stride > 0) {
    stride = user_stride;
  }

  if (self->ctx->is_lost) {
    PyErr_Format(PyExc_RuntimeError, "[HyperGL] context lost");
    return NULL;
  }

  // --- AZDO Check ---
  if (self->index_type) {
    if (!glMultiDrawElementsIndirect) {
      PyErr_SetString(PyExc_RuntimeError,
                      "[HyperGL] glMultiDrawElementsIndirect not "
                      "supported/loaded on this hardware.");
      return NULL;
    }
  } else {
    if (!glMultiDrawArraysIndirect) {
      PyErr_SetString(PyExc_RuntimeError,
                      "[HyperGL] glMultiDrawArraysIndirect not "
                      "supported/loaded on this hardware.");
      return NULL;
    }
  }
  // ------------------

  if (!PyObject_TypeCheck(buffer_obj, self->ctx->module_state->Buffer_type)) {
    PyErr_SetString(PyExc_TypeError,
                    "[HyperGL] buffer must be a Buffer object");
    return NULL;
  }
  Buffer *indirect_buffer = (Buffer *)buffer_obj;

  PyMutex_Lock(&self->ctx->state_lock);

  Viewport *viewport = (Viewport *)self->viewport_data_buffer.buf;
  Viewport v1 = self->ctx->current_viewport;
  Viewport v2 = self->viewport;

  // 1. Bind State
  if (v1.x != v2.x || v1.y != v2.y || v1.width != v2.width ||
      v1.height != v2.height)
    bind_viewport_internal(self->ctx, viewport);
  if (self->ctx->current_global_settings != self->global_settings)
    bind_global_settings_internal(self->ctx, self->global_settings);
  if (self->ctx->current_draw_framebuffer != self->framebuffer->obj)
    bind_draw_framebuffer_internal(self->ctx, self->framebuffer->obj);
  if (self->ctx->current_program != self->program->obj)
    bind_program_internal(self->ctx, self->program->obj);
  if (self->ctx->current_vertex_array != self->vertex_array->obj)
    bind_vertex_array_internal(self->ctx, self->vertex_array->obj);
  if (self->ctx->current_descriptor_set != self->descriptor_set)
    bind_descriptor_set_internal(self->ctx, self->descriptor_set);

  if (self->uniforms) {
    bind_uniforms(self);
  }

  // 2. Bind Indirect Buffer
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirect_buffer->buffer);

  intptr_t command_size = self->index_type ? sizeof(DrawElementsIndirectCommand)
                                           : sizeof(DrawArraysIndirectCommand);

  if (stride < command_size || (stride % 4) != 0) {
    PyErr_SetString(PyExc_ValueError, "[HyperGL] invalid indirect stride");
    PyMutex_Unlock(&self->ctx->state_lock);
    return NULL;
  }

  intptr_t byte_offset = (intptr_t)offset * command_size;
  size_t required = byte_offset + ((size_t)draw_count * stride);
  if (required > indirect_buffer->size) {
    PyErr_SetString(PyExc_ValueError, "[HyperGL] indirect buffer too small");
    PyMutex_Unlock(&self->ctx->state_lock);
    return NULL;
  }
  // 3. Issue Draw Call (TODO: should use the commented one when ready)
  // if (indirect_buffer->gpu_dirty) {
  //     glMemoryBarrier(GL_COMMAND_BARRIER_BIT);
  //     indirect_buffer->gpu_dirty = 0;
  // }
  glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
  const void *indirect_offset =
      (const void *)((unsigned char *)NULL + byte_offset);
  if (self->index_type) {
    // Indexed Draw
    glMultiDrawElementsIndirect(self->topology, self->index_type,
                                indirect_offset, draw_count, stride);
  } else {
    // Array Draw
    glMultiDrawArraysIndirect(self->topology, indirect_offset, draw_count,
                              stride);
  }

  PyMutex_Unlock(&self->ctx->state_lock);
  Py_RETURN_NONE;
}

static PyObject *Pipeline_get_viewport(const Pipeline *self, void *closure) {
  return Py_BuildValue("(iiii)", self->viewport.x, self->viewport.y,
                       self->viewport.width, self->viewport.height);
}

static int Pipeline_set_viewport(Pipeline *self, PyObject *viewport,
                                 void *closure) {
  if (!to_viewport(&self->viewport, viewport, 0, 0, 0, 0)) {
    PyErr_Format(PyExc_TypeError,
                 "[HyperGL] the viewport must be a tuple of 4 ints");
    return -1;
  }
  return 0;
}

// -----------------------------------------------------------------------------
// Type: Compute
// -----------------------------------------------------------------------------

static PyObject *Compute_meth_run(Compute *self, PyObject *args) {
  int x = 1;
  int y = 1;
  int z = 1;
  if (!PyArg_ParseTuple(args, "|iii", &x, &y, &z)) {
    return NULL;
  }

  VALIDATE(x > 0 && y > 0 && z > 0, PyExc_ValueError,
           "[HyperGL] Dispatch dimensions must be positive");

  if (self->ctx->is_lost) {
    PyErr_Format(PyExc_RuntimeError, "[HyperGL] the context is lost");
    return NULL;
  }

  PyMutex_Lock(&self->ctx->state_lock);

  bind_program_internal(self->ctx, self->program->obj);

  bind_descriptor_set(self->ctx, self->descriptor_set);

  if (self->uniforms) {
    bind_uniforms((Pipeline *)self);
  }
  glDispatchCompute(x, y, z);
  glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT |
                  GL_SHADER_STORAGE_BARRIER_BIT | GL_COMMAND_BARRIER_BIT);
  // glMemoryBarrier(GL_ALL_BARRIER_BITS); // sanity check
  PyMutex_Unlock(&self->ctx->state_lock);

  Py_RETURN_NONE;
}

static Compute *Context_meth_compute(Context *self, PyObject *args,
                                     PyObject *kwargs) {
  static char *keywords[] = {"compute_shader", "resources", "uniforms",
                             "uniform_data", NULL};

  PyObject *compute_shader = NULL;
  PyObject *resources = self->module_state->empty_tuple;
  PyObject *uniforms_arg = Py_None;
  PyObject *uniform_data = Py_None;
  PyObject *shader_bytes = NULL;
  GLObject *program = NULL;
  PyObject *uniforms_proxy = NULL;
  PyObject *uniform_layout = NULL;
  PyObject *uniform_data_blob = NULL;
  PyObject *resource_bindings = NULL;
  DescriptorSet *descriptor_set = NULL;
  Compute *res = NULL;
  PyObject *tuple = NULL;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|OOO", keywords,
                                   &compute_shader, &resources, &uniforms_arg,
                                   &uniform_data)) {
    return NULL;
  }

  // --- Shader Processing ---
  if (PyUnicode_Check(compute_shader)) {
    shader_bytes = PyUnicode_AsUTF8String(compute_shader);
    if (!shader_bytes)
      goto fail;
  } else if (PyBytes_Check(compute_shader)) {
    shader_bytes = compute_shader;
    Py_INCREF(shader_bytes);
  } else {
    PyErr_Format(PyExc_TypeError,
                 "[HyperGL] compute_shader must be str or bytes");
    goto fail;
  }

  program = compile_compute_program(self, NULL, shader_bytes);
  Py_CLEAR(shader_bytes);
  if (!program)
    goto fail;

  // --- Uniforms Processing ---
  if (uniforms_arg != Py_None) {
    tuple = PyObject_CallMethod(self->module_state->helper, "uniforms", "(OOO)",
                                program->extra, uniforms_arg, uniform_data);

    if (!tuple)
      goto fail;

    PyObject *uniform_dict = PyTuple_GetItem(tuple, 0);
    if (!uniform_dict || !PyDict_Check(uniform_dict)) {
      PyErr_Format(PyExc_RuntimeError,
                   "[HyperGL] uniforms helper returned invalid layout");
      goto fail;
    }

    uniforms_proxy = PyDictProxy_New(uniform_dict);
    if (!uniforms_proxy)
      goto fail;

    uniform_layout = PyTuple_GetItem(tuple, 1);
    Py_XINCREF(uniform_layout);

    uniform_data_blob = PyTuple_GetItem(tuple, 2);
    Py_XINCREF(uniform_data_blob);

    Py_CLEAR(tuple);
  }

  // --- Resource Processing ---
  resource_bindings = PyObject_CallMethod(
      self->module_state->helper, "resource_bindings", "(O)", resources);
  if (!resource_bindings)
    goto fail;

  descriptor_set = build_descriptor_set(self, resource_bindings);
  Py_CLEAR(resource_bindings);
  if (!descriptor_set)
    goto fail;

  // --- Object Creation ---
  res = PyObject_New(Compute, self->module_state->Compute_type);
  if (!res)
    goto fail;

  res->ctx = self;
  res->program = program;
  res->descriptor_set = descriptor_set;
  res->uniforms = uniforms_proxy;
  res->uniform_layout = uniform_layout;
  res->uniform_data = uniform_data_blob;
  res->workgroup_size = NULL;
  res->create_kwargs = NULL;
  res->global_settings = NULL;
  res->framebuffer = NULL;
  res->vertex_array = NULL;
  res->viewport_data = NULL;
  res->render_data = NULL;
  res->topology = 0;
  res->index_type = 0;
  res->index_size = 0;

  zeromem(&res->uniform_layout_buffer, sizeof(Py_buffer));
  zeromem(&res->uniform_data_buffer, sizeof(Py_buffer));
  zeromem(&res->viewport_data_buffer, sizeof(Py_buffer));
  zeromem(&res->render_data_buffer, sizeof(Py_buffer));
  zeromem(&res->params, sizeof(RenderParameters));
  zeromem(&res->viewport, sizeof(Viewport));

  // --- Buffer Acquisition ---
  if (res->uniforms) {
    if (PyObject_GetBuffer(res->uniform_layout, &res->uniform_layout_buffer,
                           PyBUF_SIMPLE))
      goto fail;
    if (PyObject_GetBuffer(res->uniform_data, &res->uniform_data_buffer,
                           PyBUF_SIMPLE))
      goto fail;
  }

  return res;

fail:
  Py_XDECREF(shader_bytes);
  Py_XDECREF(uniforms_proxy);
  Py_XDECREF(uniform_layout);
  Py_XDECREF(uniform_data_blob);
  Py_XDECREF(tuple);
  Py_XDECREF(resource_bindings);
  Py_XDECREF(descriptor_set);
  if (res)
    Py_DECREF(res);

  return NULL;
}

// -----------------------------------------------------------------------------
// Other Methods: Context Pack/NewFrame, Inspection, Helpers
// -----------------------------------------------------------------------------

static PyObject *Context_meth_pack_indirect(Context *self, PyObject *args,
                                            PyObject *kwargs) {
  static char *keywords[] = {"commands", "indexed", NULL};
  PyObject *commands;
  int indexed = 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|p", keywords, &commands,
                                   &indexed))
    return NULL;

  // OPTIMIZATION: Convert outer sequence to a fast array
  PyObject *fast_commands =
      PySequence_Fast(commands, "[HyperGL] commands must be a sequence");
  if (!fast_commands) {
    return NULL;
  }

  Py_ssize_t count = PySequence_Fast_GET_SIZE(fast_commands);
  if (count == 0) {
    Py_DECREF(fast_commands);
    return PyBytes_FromStringAndSize("", 0);
  }

  // Determine sizes
  const Py_ssize_t stride = indexed ? 20 : 16;
  const Py_ssize_t total_size = count * stride;

  PyObject *result = PyBytes_FromStringAndSize(NULL, total_size);
  if (!result) {
    Py_DECREF(fast_commands);
    return NULL;
  }

  unsigned char *base_ptr = (unsigned char *)PyBytes_AsString(result);
  // Zero out memory so padding bytes (if any) or implicit zeros are clean
  memset(base_ptr, 0, total_size);

  PyObject **items = PySequence_Fast_ITEMS(fast_commands);
  int error_occurred = 0;

  for (Py_ssize_t i = 0; i < count; i++) {
    // OPTIMIZATION: Direct pointer access (borrowed ref)
    PyObject *cmd = items[i];

    // OPTIMIZATION: Convert inner sequence to fast array
    PyObject *fast_cmd =
        PySequence_Fast(cmd, "[HyperGL] Command is not a sequence");
    if (!fast_cmd) {
      error_occurred = 1;
      break;
    }

    Py_ssize_t inputs = PySequence_Fast_GET_SIZE(fast_cmd);
    PyObject **cmd_items = PySequence_Fast_ITEMS(fast_cmd);

    unsigned char *current_struct_ptr = base_ptr + (i * stride);

    if (indexed) {
      // --- INDEXED PATH ---
      if (inputs != 4 && inputs != 5) {
        PyErr_Format(PyExc_ValueError,
                     "[HyperGL] Indexed command %zd must have 4 or 5 values",
                     i);
        Py_DECREF(fast_cmd);
        error_occurred = 1;
        break;
      }

      struct DrawElementsIndirectCommand {
        uint32_t count;
        uint32_t instanceCount;
        uint32_t firstIndex;
        int32_t baseVertex;
        uint32_t baseInstance;
      } *out = (void *)current_struct_ptr;

      // 1. count
      unsigned long long val = PyLong_AsUnsignedLongLong(cmd_items[0]);
      if (val == (unsigned long long)-1 && PyErr_Occurred())
        goto index_error;
      if (val > UINT32_MAX)
        goto range_error;
      out->count = (uint32_t)val;

      // 2. instanceCount
      val = PyLong_AsUnsignedLongLong(cmd_items[1]);
      if (val == (unsigned long long)-1 && PyErr_Occurred())
        goto index_error;
      if (val > UINT32_MAX)
        goto range_error;
      out->instanceCount = (uint32_t)val;

      // 3. firstIndex
      val = PyLong_AsUnsignedLongLong(cmd_items[2]);
      if (val == (unsigned long long)-1 && PyErr_Occurred())
        goto index_error;
      if (val > UINT32_MAX)
        goto range_error;
      out->firstIndex = (uint32_t)val;

      if (inputs == 5) {
        // 4. baseVertex (Signed)
        const long long sval = PyLong_AsLongLong(cmd_items[3]);
        if (sval == -1 && PyErr_Occurred())
          goto index_error;
        if (sval < INT32_MIN || sval > INT32_MAX)
          goto range_error;
        out->baseVertex = (int32_t)sval;

        // 5. baseInstance
        val = PyLong_AsUnsignedLongLong(cmd_items[4]);
        if (val == (unsigned long long)-1 && PyErr_Occurred())
          goto index_error;
        if (val > UINT32_MAX)
          goto range_error;
        out->baseInstance = (uint32_t)val;
      } else {
        // inputs == 4
        // baseVertex is implicitly 0
        out->baseVertex = 0;

        // 4. baseInstance (taken from item index 3)
        val = PyLong_AsUnsignedLongLong(cmd_items[3]);
        if (val == (unsigned long long)-1 && PyErr_Occurred())
          goto index_error;
        if (val > UINT32_MAX)
          goto range_error;
        out->baseInstance = (uint32_t)val;
      }

    } else {
      // --- ARRAY PATH ---
      if (inputs != 4) {
        PyErr_Format(
            PyExc_ValueError,
            "[HyperGL] Non-indexed command %zd must have exactly 4 values", i);
        Py_DECREF(fast_cmd);
        error_occurred = 1;
        break;
      }

      struct DrawArraysIndirectCommand {
        uint32_t count;
        uint32_t instanceCount;
        uint32_t first;
        uint32_t baseInstance;
      } *out = (void *)current_struct_ptr;

      // 1. count
      unsigned long long val = PyLong_AsUnsignedLongLong(cmd_items[0]);
      if (val == (unsigned long long)-1 && PyErr_Occurred())
        goto array_error;
      if (val > UINT32_MAX)
        goto range_error;
      out->count = (uint32_t)val;

      // 2. instanceCount
      val = PyLong_AsUnsignedLongLong(cmd_items[1]);
      if (val == (unsigned long long)-1 && PyErr_Occurred())
        goto array_error;
      if (val > UINT32_MAX)
        goto range_error;
      out->instanceCount = (uint32_t)val;

      // 3. first
      val = PyLong_AsUnsignedLongLong(cmd_items[2]);
      if (val == (unsigned long long)-1 && PyErr_Occurred())
        goto array_error;
      if (val > UINT32_MAX)
        goto range_error;
      out->first = (uint32_t)val;

      // 4. baseInstance
      val = PyLong_AsUnsignedLongLong(cmd_items[3]);
      if (val == (unsigned long long)-1 && PyErr_Occurred())
        goto array_error;
      if (val > UINT32_MAX)
        goto range_error;
      out->baseInstance = (uint32_t)val;
    }

    Py_DECREF(fast_cmd);
    continue;

  // Error handling block
  index_error:
  array_error:
    error_occurred = 1;
    Py_DECREF(fast_cmd);
    break;

  range_error:
    PyErr_SetString(PyExc_ValueError,
                    "[HyperGL] Value out of range for indirect buffer");
    error_occurred = 1;
    Py_DECREF(fast_cmd);
    break;
  }

  Py_DECREF(fast_commands);

  if (error_occurred) {
    Py_DECREF(result);
    return NULL;
  }

  return result;
}

static PyObject *Context_meth_new_frame(Context *self, PyObject *args,
                                        PyObject *kwargs) {
  static char *keywords[] = {"reset", "clear", NULL};
  int reset = 1;
  int clear = 1;

  // Fast path: if no arguments are provided, skip expensive parsing
  if (PyTuple_GET_SIZE(args) > 0 || (kwargs && PyDict_Size(kwargs) > 0)) {
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|pp", keywords, &reset,
                                     &clear))
      return NULL;
  }

  if (self->is_lost) {
    PyErr_Format(PyExc_RuntimeError, "[HyperGL] the context is lost");
    return NULL;
  }

  // Process deletion queue
  flush_trash(self);

  // Holders for objects to be decremented OUTSIDE the lock
  PyObject *trash_desc = NULL;
  GlobalSettings *trash_settings = NULL;

  PyMutex_Lock(&self->state_lock);

  // 1. RESET LOGIC
  if (reset) {
    // Release DescriptorSet
    if (self->current_descriptor_set) {
      trash_desc = (PyObject *)self->current_descriptor_set;
      release_descriptor_set(self, self->current_descriptor_set, 1);
      self->current_descriptor_set = NULL;
    }

    // Release GlobalSettings
    if (self->current_global_settings) {
      trash_settings = self->current_global_settings;
      release_global_settings(self, trash_settings, 1);
      self->current_global_settings = NULL;
    }

    // Reset bitfields
    self->is_stencil_default = 0;
    self->is_mask_default = 0;
    self->is_blend_default = 0;

    // Reset cached IDs
    self->current_viewport = (Viewport){-1, -1, -1, -1};
    self->current_read_framebuffer = -1;
    self->current_draw_framebuffer = -1;
    self->current_program = -1;
    self->current_vertex_array = -1;
    self->current_depth_mask = 0;
    self->current_stencil_mask = 0;

    // CRITICAL FIX: Invalidate shadow state.
    // We set everything to UNKNOWN (-1) so next calls force an update.
    // This handles cases where external C/ctypes calls might have dirtied the
    // GL state.
    memset(&self->gl_state, GL_STATE_UNKNOWN, sizeof(self->gl_state));
  }

  // 2. CLEAR LOGIC
  if (clear) {
    int default_fbo = self->default_framebuffer->obj;
    // Only bind if we aren't already bound
    if (self->current_draw_framebuffer != default_fbo) {
      bind_draw_framebuffer_internal(self, default_fbo);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  }

  // 3. STATE ENFORCEMENT
  // Apply standard features for the new frame.
  // Thanks to the shadow state check, these are free if already enabled.

  if (!self->is_webgl) {
    LOCKED_GL_ENABLE_STATE(GL_PRIMITIVE_RESTART_FIXED_INDEX, primitive_restart);
  }

  if (!self->is_gles) {
    LOCKED_GL_ENABLE_STATE(GL_PROGRAM_POINT_SIZE, program_point_size);
    LOCKED_GL_ENABLE_STATE(GL_TEXTURE_CUBE_MAP_SEAMLESS, seamless_cube);
  }

  PyMutex_Unlock(&self->state_lock);

  // Cleanup python objects outside the lock
  Py_XDECREF(trash_desc);
  // Py_XDECREF((PyObject *)trash_settings);

  Py_RETURN_NONE;
}

static PyObject *Context_meth_end_frame(Context *self, PyObject *args,
                                        PyObject *kwargs) {
  static char *keywords[] = {"clean", "flush", NULL};
  int clean = 1;
  int flush = 1;

  if (PyTuple_GET_SIZE(args) > 0 || (kwargs && PyDict_Size(kwargs) > 0)) {
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|pp", keywords, &clean,
                                     &flush))
      return NULL;
  }

  if (self->is_lost) {
    PyErr_Format(PyExc_RuntimeError, "[HyperGL] the context is lost");
    return NULL;
  }

  flush_trash(self);

  PyObject *trash_desc = NULL;
  GlobalSettings *trash_settings = NULL;

  PyMutex_Lock(&self->state_lock);

  if (clean) {
    // 1. Unbind GL Objects
    if (self->current_draw_framebuffer != 0)
      bind_draw_framebuffer_internal(self, 0);
    if (self->current_program != 0)
      bind_program_internal(self, 0);
    if (self->current_vertex_array != 0)
      bind_vertex_array_internal(self, 0);

    // 2. Release Internal Resources
    if (self->current_descriptor_set) {
      trash_desc = (PyObject *)self->current_descriptor_set;
      release_descriptor_set(self, self->current_descriptor_set, 1);
      self->current_descriptor_set = NULL;
    }

    if (self->current_global_settings) {
      trash_settings = self->current_global_settings;
      release_global_settings(self, trash_settings, 1);
      self->current_global_settings = NULL;
    }

    // 3. Reset Active Texture
    // Safety check: GL_TEXTURE0 is 0x84C0, not 0.
    // If default_texture_unit is 0, it's likely uninitialized.
    if (self->default_texture_unit != 0) {
      glActiveTexture(self->default_texture_unit);
    }

    // 4. Disable States (using Shadow Cache)
    LOCKED_GL_DISABLE_STATE(GL_CULL_FACE, cull_face);
    LOCKED_GL_DISABLE_STATE(GL_DEPTH_TEST, depth_test);
    LOCKED_GL_DISABLE_STATE(GL_STENCIL_TEST, stencil_test);
    LOCKED_GL_DISABLE_STATE(GL_BLEND, blend);

    if (!self->is_webgl) {
      LOCKED_GL_DISABLE_STATE(GL_PRIMITIVE_RESTART_FIXED_INDEX,
                              primitive_restart);
    }

    if (!self->is_gles) {
      LOCKED_GL_DISABLE_STATE(GL_PROGRAM_POINT_SIZE, program_point_size);
      LOCKED_GL_DISABLE_STATE(GL_TEXTURE_CUBE_MAP_SEAMLESS, seamless_cube);
    }

    // Reset Python-side flags
    self->is_blend_default = 0;
    self->is_stencil_default = 0;
    self->is_mask_default = 0;
  }

  if (flush) {
    glFlush();
  }

  PyMutex_Unlock(&self->state_lock);

  Py_XDECREF(trash_desc);
  // Py_XDECREF((PyObject *)trash_settings);

  Py_RETURN_NONE;
}

static PyObject *Context_meth_release(Context *self, PyObject *arg) {
  // Uses enqueue_trash logic where applicable,
  // or helper functions defined in the codebase.

  if (Py_TYPE(arg) == self->module_state->Buffer_type) {
    Buffer *buffer = (Buffer *)arg;
    Py_XDECREF(buffer->memoryview);
    buffer->memoryview = NULL;

    if (!self->is_lost && buffer->buffer) {
      // Direct delete requested by user, but safe to queue it
      enqueue_trash(self->trash_shared, buffer->buffer, TRASH_BUFFER);
      buffer->buffer = 0;
    }
  } else if (Py_TYPE(arg) == self->module_state->Image_type) {
    Image *image = (Image *)arg;
    if (image->faces) {
      PyDict_Clear(image->faces);
    }

    if (!self->is_lost && image->image) {
      int type = image->renderbuffer ? TRASH_RENDERBUFFER : TRASH_TEXTURE;
      enqueue_trash(self->trash_shared, image->image, type);
      image->image = 0;
    }
  } else if (Py_TYPE(arg) == self->module_state->Pipeline_type) {
    Pipeline *pipeline = (Pipeline *)arg;

    release_descriptor_set(self, pipeline->descriptor_set, 0);
    release_global_settings(self, pipeline->global_settings, 0);

    release_framebuffer(self, pipeline->framebuffer);
    release_program(self, pipeline->program);
    release_vertex_array(self, pipeline->vertex_array);

    if (pipeline->uniforms) {
      PyBuffer_Release(&pipeline->uniform_layout_buffer);
      PyBuffer_Release(&pipeline->uniform_data_buffer);
    }
    PyBuffer_Release(&pipeline->viewport_data_buffer);
    PyBuffer_Release(&pipeline->render_data_buffer);
  } else if (Py_TYPE(arg) == self->module_state->Compute_type) {
    Compute *compute = (Compute *)arg;
    release_descriptor_set(self, compute->descriptor_set, 0);
    release_program(self, compute->program);
    if (compute->uniforms) {
      PyBuffer_Release(&compute->uniform_layout_buffer);
      PyBuffer_Release(&compute->uniform_data_buffer);
    }
  } else if (PyUnicode_CheckExact(arg) &&
             !PyUnicode_CompareWithASCIIString(arg, "shader_cache")) {
    PyObject *key;
    PyObject *value;
    Py_ssize_t pos = 0;
    while (PyDict_Next(self->shader_cache, &pos, &key, &value)) {
      const GLObject *shader = (GLObject *)value;
      // Shaders are GLObjects, so we must manually enqueue or trust their
      // dealloc. Since we are clearing the cache, we own the ref.
      if (!self->is_lost) {
        glDeleteShader(shader->obj);
      }
    }
    PyDict_Clear(self->shader_cache);
  } else if (PyUnicode_CheckExact(arg) &&
             !PyUnicode_CompareWithASCIIString(arg, "all")) {
    PyGC_Collect();
  }
  Py_RETURN_NONE;
}

static PyObject *Context_get_screen(const Context *self, void *closure) {
  return PyLong_FromLong(self->default_framebuffer->obj);
}

static int Context_set_screen(const Context *self, PyObject *value,
                              void *closure) {
  if (!PyLong_CheckExact(value)) {
    PyErr_Format(PyExc_TypeError, "[HyperGL] screen must be an int");
    return -1;
  }
  self->default_framebuffer->obj = to_int(value);
  return 0;
}

static PyObject *Context_get_loader(const Context *self, void *closure) {
  return new_ref(self->module_state->default_loader);
}

static PyObject *inspect_descriptor_set(const DescriptorSet *set) {
  PyObject *res = PyList_New(0);

  // 1. Uniform Buffers
  for (int i = 0; i < set->uniform_buffers.binding_count; ++i) {
    const Buffer *buf = set->uniform_buffers.binding[i].buffer;
    if (buf) {
      PyObject *obj = Py_BuildValue(
          "{sssisisisi}", "type", "uniform_buffer", "binding", i, "buffer_id",
          buf->buffer, "offset", set->uniform_buffers.binding[i].offset, "size",
          set->uniform_buffers.binding[i].size);
      if (PyList_Append(res, obj) < 0) {
        Py_DECREF(obj);
        Py_DECREF(res);
        return NULL;
      }
      Py_DECREF(obj);
    }
  }

  // 2. Storage Buffers (SSBOs)
  for (int i = 0; i < set->storage_buffers.binding_count; ++i) {
    Buffer *buf = set->storage_buffers.binding[i].buffer;
    if (buf) {
      PyObject *obj = Py_BuildValue(
          "{sssisisisi}", "type", "storage_buffer", "binding", i, "buffer_id",
          buf->buffer, "offset", set->storage_buffers.binding[i].offset, "size",
          set->storage_buffers.binding[i].size);
      if (PyList_Append(res, obj) < 0) {
        Py_DECREF(obj);
        Py_DECREF(res);
        return NULL;
      }
      Py_DECREF(obj);
    }
  }

  // 3. Samplers / Bindless Textures
  for (int i = 0; i < set->samplers.binding_count; ++i) {
    const Image *img = set->samplers.binding[i].image;
    const GLObject *samp = set->samplers.binding[i].sampler;

    if (img) {
      // Gather Image Details
      PyObject *handle_obj;
      if (img->bindless_handle != 0) {
        handle_obj = PyLong_FromUnsignedLongLong(img->bindless_handle);
      } else {
        Py_INCREF(Py_None);
        handle_obj = Py_None;
      }
      // Determine dimensions based on type
      // int d = (img->cubemap) ? 3 : (img->array ? 3 : 2);
      PyObject *dims =
          Py_BuildValue("(iii)", img->width, img->height, img->array);

      const int samp_id = samp ? samp->obj : 0;

      PyObject *resident = img->is_resident ? Py_True : Py_False;
      PyObject *target = PyLong_FromLong(img->target);

      Py_INCREF(resident);

      if (!dims) {
        Py_DECREF(handle_obj);
        Py_DECREF(resident);
        Py_DECREF(target);
        Py_DECREF(res);
        return NULL;
      }

      PyObject *obj = Py_BuildValue(
          "{sssisisi sOsOsOsOsO}", "type", "sampler", "binding", i,
          "sampler_id", samp_id, "texture_id", img->image,
          // Extra details
          "handle", handle_obj, "dimensions", dims, "format",
          img->format, // The Python string/tuple stored in Image struct
          "resident", resident, "target", target);
      if (!obj) {
        Py_DECREF(res);
        return NULL;
      }
      Py_DECREF(handle_obj);
      Py_DECREF(resident);
      Py_DECREF(target);

      if (PyList_Append(res, obj) < 0) {
        Py_DECREF(obj);
        Py_DECREF(res);
        return NULL;
      }
      Py_DECREF(dims);
      Py_DECREF(obj);
    }
  }
  return res;
}

static PyObject *meth_inspect(PyObject *self, PyObject *arg) {
  const ModuleState *module_state = (ModuleState *)PyModule_GetState(self);

  if (Py_TYPE(arg) == module_state->Buffer_type) {
    const Buffer *buffer = (Buffer *)arg;
    return Py_BuildValue("{sssi}", "type", "buffer", "buffer", buffer->buffer);
  }
  if (Py_TYPE(arg) == module_state->Image_type) {
    const Image *image = (Image *)arg;
    const char *gltype = image->renderbuffer ? "renderbuffer" : "texture";
    return Py_BuildValue("{sssi}", "type", "image", gltype, image->image);
  }
  if (Py_TYPE(arg) == module_state->ImageFace_type) {
    const ImageFace *face = (ImageFace *)arg;
    if (!face->framebuffer) {
      return Py_BuildValue("{ss}", "type", "image_face (uninitialized)");
    }
    return Py_BuildValue("{sssi}", "type", "image_face", "framebuffer",
                         face->framebuffer->obj);
  }
  if (Py_TYPE(arg) == module_state->Pipeline_type) {
    Pipeline *pipeline = (Pipeline *)arg;
    if (!pipeline->program || !pipeline->framebuffer ||
        !pipeline->vertex_array) {
      return Py_BuildValue("{ss}", "type", "pipeline (uninitialized)");
    }

    // Helper defined in segment 2
    PyObject *resources = inspect_descriptor_set(pipeline->descriptor_set);
    if (!resources) {
      return NULL;
    }

    PyObject *interface_obj =
        pipeline->program->extra ? pipeline->program->extra : Py_None;

    return Py_BuildValue("{sssOsNsisisi}", "type", "pipeline", "interface",
                         interface_obj, "resources", resources, "framebuffer",
                         pipeline->framebuffer->obj, "vertex_array",
                         pipeline->vertex_array->obj, "program",
                         pipeline->program->obj);
  }
  Py_RETURN_NONE;
}

static PyObject *ImageFace_meth_clear(const ImageFace *self, PyObject *args) {
  if (self->ctx->is_lost) {
    PyErr_Format(PyExc_RuntimeError, "[HyperGL] the context is lost");
    return NULL;
  }

  PyMutex_Lock(&self->ctx->state_lock);

  const int prev_draw = self->ctx->current_draw_framebuffer;

  if (self->ctx->current_draw_framebuffer != self->framebuffer->obj) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, self->framebuffer->obj);
    self->ctx->current_draw_framebuffer = self->framebuffer->obj;
  }

  clear_bound_image(self->image);

  if (self->ctx->current_draw_framebuffer != prev_draw) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, prev_draw);
    self->ctx->current_draw_framebuffer = prev_draw;
  }

  PyMutex_Unlock(&self->ctx->state_lock);
  Py_RETURN_NONE;
}

static PyObject *ImageFace_meth_read(ImageFace *self, PyObject *args,
                                     PyObject *kwargs) {
  static char *keywords[] = {"size", "offset", "into", NULL};
  PyObject *size_arg = Py_None;
  PyObject *offset_arg = Py_None;
  PyObject *into = Py_None;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|OOO", keywords, &size_arg,
                                   &offset_arg, &into)) {
    return NULL;
  }

  if (self->ctx->is_lost) {
    PyErr_Format(PyExc_RuntimeError, "[HyperGL] the context is lost");
    return NULL;
  }

  IntPair size;
  IntPair offset;
  if (!parse_size_and_offset(self, size_arg, offset_arg, &size, &offset)) {
    return NULL;
  }

  return read_image_face(self, size, offset, into);
}

static PyObject *ImageFace_meth_blit(const ImageFace *self, PyObject *args,
                                     PyObject *kwargs) {
  static char *keywords[] = {"target", "offset", "size",
                             "crop",   "filter", NULL};
  PyObject *target = Py_None;
  PyObject *offset = Py_None;
  PyObject *size = Py_None;
  PyObject *crop = Py_None;
  int filter = 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|OOOOp", keywords, &target,
                                   &offset, &size, &crop, &filter)) {
    return NULL;
  }

  if (self->ctx->is_lost) {
    PyErr_Format(PyExc_RuntimeError, "[HyperGL] the context is lost");
    return NULL;
  }

  return blit_image_face(self, target, offset, size, crop, filter);
}

// -----------------------------------------------------------------------------
// Math & Camera Helper
// -----------------------------------------------------------------------------
static const double PI = 3.14159265358979323846;

typedef struct vec3 {
  double x, y, z;
} vec3;

static CONST_FUNC FORCE_INLINE vec3 sub(const vec3 a, const vec3 b) {
  return (vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

static CONST_FUNC FORCE_INLINE vec3 normalize(const vec3 a) {
  const double sql = (a.x * a.x) + (a.y * a.y) + (a.z * a.z);
  const double epsilon = 1e-30;
  if (!(sql > epsilon)) // catches zero, tiny, negative, NaN
    return (vec3){0, 0, 0};

  const double inv_l = 1.0 / sqrt(sql);
  return (vec3){a.x * inv_l, a.y * inv_l, a.z * inv_l};
}

static CONST_FUNC FORCE_INLINE vec3 cross(const vec3 a, const vec3 b) {
  return (vec3){(a.y * b.z) - (a.z * b.y), (a.z * b.x) - (a.x * b.z),
                (a.x * b.y) - (a.y * b.x)};
}

static CONST_FUNC FORCE_INLINE double dot(const vec3 a, const vec3 b) {
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

static CONST_FUNC FORCE_INLINE double deg_to_rad(double deg) {
  return deg * (PI / 180.0);
}

static PyObject *meth_camera(PyObject *self, PyObject *args, PyObject *kwargs) {
  static char *keywords[] = {"eye",  "target", "up",   "fov",  "aspect",
                             "near", "far",    "size", "clip", NULL};

  vec3 eye;
  vec3 target = {0.0, 0.0, 0.0};
  vec3 up = {0.0, 0.0, 1.0};
  double fov = 60.0;
  double aspect = 1.0;
  double znear = 0.1;
  double zfar = 1000.0;
  double size = 1.0;
  int clip = 0;

  int args_ok = PyArg_ParseTupleAndKeywords(
      args, kwargs, "(ddd)|(ddd)(ddd)dddddp", keywords, &eye.x, &eye.y, &eye.z,
      &target.x, &target.y, &target.z, &up.x, &up.y, &up.z, &fov, &aspect,
      &znear, &zfar, &size, &clip);

  if (!args_ok)
    return NULL;

  const vec3 f = normalize(sub(target, eye));
  const vec3 s = normalize(cross(f, up));
  const vec3 u = cross(s, f);
  const vec3 t = {-dot(s, eye), -dot(u, eye), -dot(f, eye)};

  if (!fov) {
    const double r1 = size;
    const double r2 = r1 * aspect;
    const double r3 = clip ? 1.0 / (zfar - znear) : 2.0 / (zfar - znear);
    const double r4 =
        clip ? znear / (zfar - znear) : (zfar + znear) / (zfar - znear);

    float res[] = {
        (float)(s.x / r2), (float)(u.x / r1), (float)(r3 * f.x),        0.0F,
        (float)(s.y / r2), (float)(u.y / r1), (float)(r3 * f.y),        0.0F,
        (float)(s.z / r2), (float)(u.z / r1), (float)(r3 * f.z),        0.0F,
        (float)(t.x / r2), (float)(t.y / r1), (float)((r3 * t.z) - r4), 1.0F,
    };
    return PyBytes_FromStringAndSize((char *)res, 64);
  }

  const double r1 = tan(deg_to_rad(fov * 0.5));
  const double r2 = r1 * aspect;
  const double r3 =
      clip ? zfar / (zfar - znear) : (zfar + znear) / (zfar - znear);
  const double r4 = clip ? (zfar * znear) / (zfar - znear)
                         : (2.0 * zfar * znear) / (zfar - znear);

  float res[] = {
      (float)(s.x / r2), (float)(u.x / r1), (float)(r3 * f.x),
      (float)f.x,        (float)(s.y / r2), (float)(u.y / r1),
      (float)(r3 * f.y), (float)f.y,        (float)(s.z / r2),
      (float)(u.z / r1), (float)(r3 * f.z), (float)f.z,
      (float)(t.x / r2), (float)(t.y / r1), (float)((r3 * t.z) - r4),
      (float)t.z,
  };
  return PyBytes_FromStringAndSize((char *)res, 64);
}

// -----------------------------------------------------------------------------
// Deallocators
// -----------------------------------------------------------------------------

static void Context_dealloc(Context *self) {
  // 1. Untrack
  if (PyObject_GC_IsTracked((PyObject *)self))
    PyObject_GC_UnTrack(self);

  // 2. Clear Caches
  Context_clear(self);

  // 3. Handle Shared Trash
  // The context holds one reference to the shared trash struct.
  if (self->trash_shared) {
    SharedTrash *shared = self->trash_shared;
    // Clean up remaining GL resources if any
    flush_trash(self);

    // Decrement ref count; if 0, free the C memory
    if (Atomic_Decrement(&shared->ref_count) == 0) {
      if (shared->bin)
        PyMem_Free(shared->bin);
      PyMem_Free(shared);
    }
    self->trash_shared = NULL;
  }

  Py_TYPE(self)->tp_free((PyObject *)self);
}

static int Buffer_traverse(const Buffer *self, visitproc visit, void *arg) {
  Py_VISIT(self->memoryview);
  Py_VISIT(self->ctx);
  return 0;
}

static int Buffer_clear(Buffer *self) {
  // Use XSETREF or XDECREF + NULL to ensure idempotency
  Py_XSETREF(self->memoryview, NULL);
  Py_XSETREF(self->ctx, NULL);
  return 0;
}

static void Buffer_dealloc(Buffer *self) {
  // 1. STOP THE GC
  PyObject_GC_UnTrack(self);

  // 2. USE THE DATA WHILE WE STILL HAVE IT
  // We need 'ctx' and 'buffer' ID to clean up the GPU side.
  if (self->ctx && !self->ctx->is_lost) {

    // A. Handle Unmapping (Must happen before deletion)
    if (self->mapped_ptr) {
      PyMutex_Lock(&self->ctx->state_lock);
      glBindBuffer(self->target, self->buffer);
      glUnmapBuffer(self->target);
      self->mapped_ptr = NULL;
      PyMutex_Unlock(&self->ctx->state_lock);
    }

    // B. Enqueue for the "Double Flush" Trash System
    if (self->buffer && self->ctx->trash_shared) {
      enqueue_trash(self->ctx->trash_shared, self->buffer, TRASH_BUFFER);
    }
  }

  // 3. NOW BREAK THE PYTHON CYCLES
  // This sets self->memoryview and self->ctx to NULL.
  Buffer_clear(self);

  // 4. FREE THE C STRUCT
  Py_TYPE(self)->tp_free((PyObject *)self);
}

// You must ensure that if an Image is destroyed, the handle is treated
// correctly. Interestingly, glDeleteTextures implicitly destroys the handle, so
// specific cleanup logic for the handle itself isn't strictly required unless
// you need to ensure it is non-resident before deletion. It is good practice to
// ensure it is non-resident.
static void Image_dealloc(Image *self) {
  if (PyObject_GC_IsTracked((PyObject *)self))
    PyObject_GC_UnTrack(self);

  if (self->bindless_handle && self->is_resident && self->ctx &&
      !self->ctx->is_lost) {
    // We technically need the lock here, but dealloc is tricky with locks.
    // However, since we are destroying the object, we just want to ensure GL is
    // happy. If the context is still alive, we should try to make it
    // non-resident. Note: Doing GL calls in dealloc can be dangerous if on
    // wrong thread. Ideally, the user calls .make_resident(False) manually. But
    // the GL driver often handles cleanup on texture deletion.
  }

  // NOTE: The existing enqueue_trash system puts the Texture ID into a queue to
  // be deleted later on the main thread (via new_frame). When glDeleteTextures
  // is finally called on that ID, the driver invalidates the handle. No extra
  // code is strictly needed there for the C extension, provided the user
  // understands that accessing a handle after the image object dies is UB
  // (Undefined Behavior).

  Context *ctx = self->ctx;
  int image_id = self->image;

  if (image_id && !self->external && ctx && !ctx->is_lost &&
      ctx->trash_shared) {
    int type = self->renderbuffer ? TRASH_RENDERBUFFER : TRASH_TEXTURE;
    self->bindless_handle = 0;
    self->is_resident = 0;
    enqueue_trash(ctx->trash_shared, image_id, type);
  }

  Image_clear(self);
  Py_TYPE(self)->tp_free(self);
}

static void Pipeline_dealloc(Pipeline *self) {
  Context *ctx = self->ctx;
  if (PyObject_GC_IsTracked((PyObject *)self))
    PyObject_GC_UnTrack(self);
  if (ctx) {
    release_program(ctx, self->program);
    release_framebuffer(ctx, self->framebuffer);
    release_vertex_array(ctx, self->vertex_array);
  }
  Pipeline_clear(self);
  Py_TYPE(self)->tp_free((PyObject *)self);
}

static void Compute_dealloc(Compute *self) {
  Py_DECREF(self->descriptor_set);
  Py_DECREF(self->program);
  Py_XDECREF(self->uniforms);
  Py_XDECREF(self->uniform_layout);
  Py_XDECREF(self->uniform_data);

  PyBuffer_Release(&self->uniform_layout_buffer);
  PyBuffer_Release(&self->uniform_data_buffer);

  Py_TYPE(self)->tp_free((PyObject *)self);
}

static void ImageFace_dealloc(ImageFace *self) {
  if (PyObject_GC_IsTracked((PyObject *)self))
    PyObject_GC_UnTrack(self);

  ImageFace_clear(self);
  Py_TYPE(self)->tp_free((PyObject *)self);
}

static void BufferView_dealloc(BufferView *self) {
  Py_DECREF(self->buffer);
  PyObject_Del(self);
}

static void DescriptorSet_dealloc(DescriptorSet *self) {
  // Clean up potentially leaked references
  for (int i = 0; i < self->uniform_buffers.binding_count; ++i) {
    Py_XDECREF(self->uniform_buffers.binding[i].buffer);
  }
  for (int i = 0; i < self->storage_buffers.binding_count; ++i) {
    Py_XDECREF(self->storage_buffers.binding[i].buffer);
  }
  for (int i = 0; i < self->samplers.binding_count; ++i) {
    Py_XDECREF(self->samplers.binding[i].sampler);
    Py_XDECREF(self->samplers.binding[i].image);
  }
  PyObject_Del(self);
}

static void GlobalSettings_dealloc(GlobalSettings *self) { PyObject_Del(self); }

static void GLObject_dealloc(GLObject *self) {
  if (PyObject_GC_IsTracked((PyObject *)self))
    PyObject_GC_UnTrack(self);

  // Use SharedTrash
  if (Atomic_Decrement(&self->uses) == 0 && self->trash) {
    enqueue_trash(self->trash, self->obj, self->type);
  }

  // Release hold on trash mechanism
  if (self->trash) {
    if (Atomic_Decrement(&self->trash->ref_count) == 0) {
      if (self->trash->bin)
        PyMem_Free(self->trash->bin);
      PyMem_Free(self->trash);
    }
  }

  Py_XDECREF(self->extra);
  Py_TYPE(self)->tp_free((PyObject *)self);
}

// -----------------------------------------------------------------------------
// Python Module Definitions
// -----------------------------------------------------------------------------

static PyMethodDef Context_methods[] = {
    {"buffer", (PyCFunction)Context_meth_buffer, METH_VARARGS | METH_KEYWORDS,
     NULL},
    {"pack_indirect", (PyCFunction)Context_meth_pack_indirect,
     METH_VARARGS | METH_KEYWORDS, NULL},
    {"image", (PyCFunction)Context_meth_image, METH_VARARGS | METH_KEYWORDS,
     NULL},
    {"pipeline", (PyCFunction)Context_meth_pipeline,
     METH_VARARGS | METH_KEYWORDS, NULL},
    {"compute", (PyCFunction)Context_meth_compute, METH_VARARGS | METH_KEYWORDS,
     NULL},
    {"new_frame", (PyCFunction)Context_meth_new_frame,
     METH_VARARGS | METH_KEYWORDS, NULL},
    {"end_frame", (PyCFunction)Context_meth_end_frame,
     METH_VARARGS | METH_KEYWORDS, NULL},
    {"release", (PyCFunction)Context_meth_release, METH_O, NULL},
    {NULL, NULL, 0, NULL},
};

static PyGetSetDef Context_getset[] = {
    {"screen", (getter)Context_get_screen, (setter)Context_set_screen, NULL,
     NULL},
    {"loader", (getter)Context_get_loader, NULL, NULL, NULL},
    {0},
};

static PyMemberDef Context_members[] = {
    {"includes", Py_T_OBJECT_EX, offsetof(Context, includes), Py_READONLY, NULL},
    {"info", Py_T_OBJECT_EX, offsetof(Context, info_dict), Py_READONLY, NULL},
    {"lost", Py_T_BOOL, offsetof(Context, is_lost), 0, NULL},
    {0},
};

static PyMethodDef Buffer_methods[] = {
    {"write", (PyCFunction)Buffer_meth_write, METH_VARARGS | METH_KEYWORDS,
     NULL},
    {"read", (PyCFunction)Buffer_meth_read, METH_VARARGS | METH_KEYWORDS, NULL},
    {"view", (PyCFunction)Buffer_meth_view, METH_VARARGS | METH_KEYWORDS, NULL},
    {"map", (PyCFunction)Buffer_meth_map, METH_NOARGS, NULL},
    {"unmap", (PyCFunction)Buffer_meth_unmap, METH_NOARGS, NULL},
    {"bind", (PyCFunction)Buffer_meth_bind, METH_VARARGS, NULL},
    {"write_texture_handle", (PyCFunction)Buffer_meth_write_texture_handle,
     METH_VARARGS | METH_KEYWORDS, NULL},
    {0},
};

static PyMemberDef Buffer_members[] = {
    {"size", Py_T_INT, offsetof(Buffer, size), Py_READONLY, NULL},
    {0},
};

static PyMethodDef Image_methods[] = {
    {"clear", (PyCFunction)Image_meth_clear, METH_NOARGS, NULL},
    {"write", (PyCFunction)Image_meth_write, METH_VARARGS | METH_KEYWORDS,
     NULL},
    {"read", (PyCFunction)Image_meth_read, METH_VARARGS | METH_KEYWORDS, NULL},
    {"mipmaps", (PyCFunction)Image_meth_mipmaps, METH_NOARGS, NULL},
    {"blit", (PyCFunction)Image_meth_blit, METH_VARARGS | METH_KEYWORDS, NULL},
    {"face", (PyCFunction)Image_meth_face, METH_VARARGS | METH_KEYWORDS, NULL},
    {"get_handle", (PyCFunction)Image_meth_get_handle, METH_NOARGS, NULL},
    {"make_resident", (PyCFunction)Image_meth_make_resident, METH_VARARGS,
     NULL},
    {0},
};

static PyGetSetDef Image_getset[] = {
    {"clear_value", (getter)Image_get_clear_value,
     (setter)Image_set_clear_value, "The clear value of the image", NULL},
    {0}};

static PyMemberDef Image_members[] = {
    {"size", Py_T_OBJECT_EX, offsetof(Image, size), Py_READONLY, NULL},
    {"format", Py_T_OBJECT_EX, offsetof(Image, format), Py_READONLY, NULL},
    {"samples", Py_T_INT, offsetof(Image, samples), Py_READONLY, NULL},
    {"array", Py_T_INT, offsetof(Image, array), Py_READONLY, NULL},
    {"renderbuffer", Py_T_BOOL, offsetof(Image, renderbuffer), Py_READONLY, NULL},
    {0},
};

static PyMethodDef Pipeline_methods[] = {
    {"render", (PyCFunction)Pipeline_meth_render, METH_NOARGS, NULL},
    {"render_indirect", (PyCFunction)Pipeline_meth_render_indirect,
     METH_VARARGS | METH_KEYWORDS, NULL},
    {0},
};

static PyGetSetDef Pipeline_getset[] = {
    {"viewport", (getter)Pipeline_get_viewport, (setter)Pipeline_set_viewport,
     NULL, NULL},
    {0},
};

static PyMemberDef Pipeline_members[] = {
    {"vertex_count", Py_T_INT, offsetof(Pipeline, params.vertex_count), 0, NULL},
    {"instance_count", Py_T_INT, offsetof(Pipeline, params.instance_count), 0,
     NULL},
    {"first_vertex", Py_T_INT, offsetof(Pipeline, params.first_vertex), 0, NULL},
    {"uniforms", Py_T_OBJECT_EX, offsetof(Pipeline, uniforms), Py_READONLY, NULL},
    {0},
};

static PyMemberDef Compute_members[] = {
    {"uniforms", Py_T_OBJECT_EX, offsetof(Compute, uniforms), Py_READONLY, "uniforms"},
    {"uniform_layout", Py_T_OBJECT_EX, offsetof(Compute, uniform_layout), Py_READONLY,
     "uniform_layout"},
    {"uniform_data", Py_T_OBJECT_EX, offsetof(Compute, uniform_data), Py_READONLY,
     "uniform_data"},
    {0},
};

static PyMethodDef ImageFace_methods[] = {
    {"clear", (PyCFunction)ImageFace_meth_clear, METH_NOARGS, NULL},
    {"read", (PyCFunction)ImageFace_meth_read, METH_VARARGS | METH_KEYWORDS,
     NULL},
    {"blit", (PyCFunction)ImageFace_meth_blit, METH_VARARGS | METH_KEYWORDS,
     NULL},
    {0},
};

static PyMemberDef ImageFace_members[] = {
    {"image", Py_T_OBJECT_EX, offsetof(ImageFace, image), Py_READONLY, NULL},
    {"size", Py_T_OBJECT_EX, offsetof(ImageFace, size), Py_READONLY, NULL},
    {"layer", Py_T_INT, offsetof(ImageFace, layer), Py_READONLY, NULL},
    {"level", Py_T_INT, offsetof(ImageFace, level), Py_READONLY, NULL},
    {"samples", Py_T_INT, offsetof(ImageFace, samples), Py_READONLY, NULL},
    {"flags", Py_T_INT, offsetof(ImageFace, flags), Py_READONLY, NULL},
    {0},
};

static PyMethodDef Compute_methods[] = {
    {"run", (PyCFunction)Compute_meth_run, METH_VARARGS, NULL},
    {0},
};

// -----------------------------------------------------------------------------
// Type Slots & Specs
// -----------------------------------------------------------------------------

static PyType_Slot Context_slots[] = {
    {Py_tp_new, (void *)Context_new},
    {Py_tp_methods, Context_methods},
    {Py_tp_getset, Context_getset},
    {Py_tp_members, Context_members},
    {Py_tp_dealloc, (void *)Context_dealloc},
    {Py_tp_traverse, (void *)Context_traverse},
    {Py_tp_clear, (void *)Context_clear},
    {0, NULL},
};

static PyType_Slot Buffer_slots[] = {
    {Py_tp_methods, Buffer_methods},
    {Py_tp_members, Buffer_members},
    {Py_bf_getbuffer, (void *)Buffer_getbuffer},
    {Py_tp_dealloc, (void *)Buffer_dealloc},
    {Py_tp_traverse, (void *)Buffer_traverse},
    {Py_tp_clear, (void *)Buffer_clear},
    {0},
};

static PyType_Slot Image_slots[] = {{Py_tp_dealloc, (void *)Image_dealloc},
                                    {Py_tp_traverse, (void *)Image_traverse},
                                    {Py_tp_clear, (void *)Image_clear},
                                    {Py_tp_methods, Image_methods},
                                    {Py_tp_members, Image_members},
                                    {Py_tp_getset, Image_getset},
                                    {0}};

static PyType_Slot Pipeline_slots[] = {
    {Py_tp_methods, Pipeline_methods},
    {Py_tp_getset, Pipeline_getset},
    {Py_tp_members, Pipeline_members},
    {Py_tp_dealloc, (void *)Pipeline_dealloc},
    {Py_tp_traverse, (void *)Pipeline_traverse},
    {Py_tp_clear, (void *)Pipeline_clear},
    {0},
};

static PyType_Slot Compute_slots[] = {
    {Py_tp_methods, Compute_methods},
    {Py_tp_members, Compute_members},
    {Py_tp_dealloc, (void *)Compute_dealloc},
    {0},
};

static PyType_Slot ImageFace_slots[] = {
    {Py_tp_methods, ImageFace_methods},
    {Py_tp_members, ImageFace_members},
    {Py_tp_dealloc, (void *)ImageFace_dealloc},
    {Py_tp_traverse, (void *)ImageFace_traverse},
    {Py_tp_clear, (void *)ImageFace_clear},
    {0},
};

static PyType_Slot BufferView_slots[] = {
    {Py_tp_dealloc, (void *)BufferView_dealloc},
    {0},
};

static PyType_Slot DescriptorSet_slots[] = {
    {Py_tp_dealloc, (void *)DescriptorSet_dealloc},
    {0},
};

static PyType_Slot GlobalSettings_slots[] = {
    {Py_tp_dealloc, (void *)GlobalSettings_dealloc},
    {0},
};

static PyType_Slot GLObject_slots[] = {
    {Py_tp_dealloc, (void *)GLObject_dealloc},
    {Py_tp_traverse, (void *)GLObject_traverse},
    {Py_tp_clear, (void *)GLObject_clear},
    {0},
};

static PyType_Spec Context_spec = {"hypergl.Context", sizeof(Context), 0,
                                   Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC,
                                   Context_slots};
static PyType_Spec Buffer_spec = {"hypergl.Buffer", sizeof(Buffer), 0,
                                  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC,
                                  Buffer_slots};
static PyType_Spec Image_spec = {
    "hypergl.Image", sizeof(Image), 0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC, Image_slots};
static PyType_Spec Pipeline_spec = {"hypergl.Pipeline", sizeof(Pipeline), 0,
                                    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC,
                                    Pipeline_slots};
static PyType_Spec Compute_spec = {"hypergl.Compute", sizeof(Compute), 0,
                                   Py_TPFLAGS_DEFAULT, Compute_slots};
static PyType_Spec ImageFace_spec = {"hypergl.ImageFace", sizeof(ImageFace), 0,
                                     Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC,
                                     ImageFace_slots};
static PyType_Spec BufferView_spec = {"hypergl.BufferView", sizeof(BufferView),
                                      0, Py_TPFLAGS_DEFAULT, BufferView_slots};
static PyType_Spec DescriptorSet_spec = {
    "hypergl.DescriptorSet", sizeof(DescriptorSet), 0, Py_TPFLAGS_DEFAULT,
    DescriptorSet_slots};
static PyType_Spec GlobalSettings_spec = {
    "hypergl.GlobalSettings", sizeof(GlobalSettings), 0, Py_TPFLAGS_DEFAULT,
    GlobalSettings_slots};
static PyType_Spec GLObject_spec = {"hypergl.GLObject", sizeof(GLObject), 0,
                                    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC,
                                    GLObject_slots};

// -----------------------------------------------------------------------------
// Module Execution & Registration
// -----------------------------------------------------------------------------

static int module_exec(PyObject *self) {
  ModuleState *state = (ModuleState *)PyModule_GetState(self);
  if (!state)
    return -1;

  // --- read VERSION ---
  PyModule_AddObject(self, "__version__",
                     PyUnicode_FromString(HYPERGL_VERSION));

  memset(&state->global_lock, 0, sizeof(PyMutex));
  memset(&state->setup_lock, 0, sizeof(PyMutex));

  state->helper = PyImport_ImportModule("hypergl._hypergl");
  if (!state->helper)
    return -1;

  state->empty_tuple = PyTuple_New(0);
  state->str_none = PyUnicode_FromString("none");
  state->str_triangles = PyUnicode_FromString("triangles");
  state->str_static_draw = PyUnicode_FromString("static_draw");
  state->str_dynamic_draw = PyUnicode_FromString("dynamic_draw");
  state->str_rgba8unorm = PyUnicode_InternFromString("rgba8unorm");
  state->str_TOPOLOGY = PyUnicode_InternFromString("TOPOLOGY");
  state->str_BUFFER_ACCESS = PyUnicode_InternFromString("BUFFER_ACCESS");
  state->str_IMAGE_FORMAT = PyUnicode_InternFromString("IMAGE_FORMAT");
  state->str_VERTEX_FORMAT = PyUnicode_InternFromString("VERTEX_FORMAT");

  state->default_loader = new_ref(Py_None);
  state->default_context = new_ref(Py_None);
  state->HyperGLError = PyErr_NewException("hypergl.Error", NULL, NULL);
  PyModule_AddObject(self, "Error", new_ref(state->HyperGLError));

#define CREATE_TYPE(obj_type, spec)                                            \
  state->obj_type =                                                            \
      (PyTypeObject *)PyType_FromModuleAndSpec(self, &(spec), NULL);           \
  if (!state->obj_type)                                                        \
    return -1;

  CREATE_TYPE(Context_type, Context_spec);
  CREATE_TYPE(Buffer_type, Buffer_spec);
  CREATE_TYPE(Image_type, Image_spec);
  CREATE_TYPE(Pipeline_type, Pipeline_spec);
  CREATE_TYPE(Compute_type, Compute_spec);
  CREATE_TYPE(ImageFace_type, ImageFace_spec);
  CREATE_TYPE(BufferView_type, BufferView_spec);
  CREATE_TYPE(DescriptorSet_type, DescriptorSet_spec);
  CREATE_TYPE(GlobalSettings_type, GlobalSettings_spec);
  CREATE_TYPE(GLObject_type, GLObject_spec);

#undef CREATE_TYPE

  PyModule_AddObject(self, "Context", new_ref(state->Context_type));
  PyModule_AddObject(self, "Buffer", new_ref(state->Buffer_type));
  PyModule_AddObject(self, "Image", new_ref(state->Image_type));
  PyModule_AddObject(self, "ImageFace", new_ref(state->ImageFace_type));
  PyModule_AddObject(self, "BufferView", new_ref(state->BufferView_type));
  PyModule_AddObject(self, "Pipeline", new_ref(state->Pipeline_type));
  PyModule_AddObject(self, "Compute", new_ref(state->Compute_type));

  PyObject *loader = PyObject_GetAttrString(state->helper, "loader");
  if (loader)
    PyModule_AddObject(self, "loader", loader);

  PyObject *calcsize = PyObject_GetAttrString(state->helper, "calcsize");
  if (calcsize)
    PyModule_AddObject(self, "calcsize", calcsize);

  PyObject *bind = PyObject_GetAttrString(state->helper, "bind");
  if (bind)
    PyModule_AddObject(self, "bind", bind);

#ifdef EXTERN_GL
  PyModule_AddObject(self, "_extern_gl", PyUnicode_FromString(EXTERN_GL));
#else
  PyModule_AddObject(self, "_extern_gl", new_ref(Py_None));
#endif

  return 0;
}

static PyModuleDef_Slot module_slots[] = {
    {Py_mod_exec, (void *)module_exec},
    // Tells the Python interpreter to go away and let us do our own locking
    // instead.
    {Py_mod_gil, Py_MOD_GIL_NOT_USED},
    {0, NULL}};

#ifdef _WIN64
static PyObject *meth_load_opengl_function(PyObject *self, PyObject *arg) {
  ModuleState *state = (ModuleState *)PyModule_GetState(self);
  const char *name = PyUnicode_AsUTF8(arg);

  PyMutex_Lock(&state->global_lock);
  if (!state->opengl_handle) {
    state->opengl_handle = LoadLibraryA("opengl32");
    if (state->opengl_handle) {
      state->wglGetProcAddress = (void *(*)(const char *))GetProcAddress(
          state->opengl_handle, "wglGetProcAddress");
    }
  }

  void *h = state->opengl_handle;
  void *(*wgl)(const char *) = state->wglGetProcAddress;
  PyMutex_Unlock(&state->global_lock);

  if (!h)
    return PyErr_SetString(PyExc_RuntimeError, "[HyperGL] No GL DLL"), NULL;

  void *proc = (void *)GetProcAddress(h, name);
  if (!proc && wgl)
    proc = wgl(name);

  return proc ? PyLong_FromVoidPtr(proc)
              : (PyErr_Format(PyExc_RuntimeError,
                              "[HyperGL] GL func %s not found", name),
                 NULL);
}
#endif

static PyMethodDef module_methods[] = {
    {"init", (PyCFunction)meth_init, METH_VARARGS | METH_KEYWORDS, NULL},
    {"cleanup", (PyCFunction)meth_cleanup, METH_NOARGS, NULL},
    {"context", (PyCFunction)meth_context, METH_NOARGS, NULL},
    {"inspect", (PyCFunction)meth_inspect, METH_O, NULL},
    {"camera", (PyCFunction)meth_camera, METH_VARARGS | METH_KEYWORDS, NULL},
#ifdef _WIN64
    {"load_opengl_function", (PyCFunction)meth_load_opengl_function, METH_O,
     NULL},
#endif
    {0},
};

static int module_traverse(PyObject *m, visitproc visit, void *arg) {
  ModuleState *state = (ModuleState *)PyModule_GetState(m);

  Py_VISIT(state->helper);
  Py_VISIT(state->empty_tuple);
  Py_VISIT(state->str_none);
  Py_VISIT(state->str_triangles);
  Py_VISIT(state->str_static_draw);
  Py_VISIT(state->str_dynamic_draw);
  Py_VISIT(state->str_rgba8unorm);
  Py_VISIT(state->str_TOPOLOGY);
  Py_VISIT(state->str_BUFFER_ACCESS);
  Py_VISIT(state->str_IMAGE_FORMAT);
  Py_VISIT(state->str_VERTEX_FORMAT);
  Py_VISIT(state->default_loader);
  Py_VISIT(state->default_context);
  Py_VISIT(state->HyperGLError);

  Py_VISIT(state->Context_type);
  Py_VISIT(state->Buffer_type);
  Py_VISIT(state->Image_type);
  Py_VISIT(state->Pipeline_type);
  Py_VISIT(state->Compute_type);
  Py_VISIT(state->ImageFace_type);
  Py_VISIT(state->BufferView_type);
  Py_VISIT(state->DescriptorSet_type);
  Py_VISIT(state->GlobalSettings_type);
  Py_VISIT(state->GLObject_type);

  return 0;
}

static int module_clear(PyObject *self) {
  ModuleState *state = (ModuleState *)PyModule_GetState(self);
  if (state) {
    Py_CLEAR(state->helper);
    Py_CLEAR(state->empty_tuple);
    Py_CLEAR(state->str_none);
    Py_CLEAR(state->str_triangles);
    Py_CLEAR(state->str_static_draw);
    Py_CLEAR(state->str_dynamic_draw);
    Py_CLEAR(state->str_rgba8unorm);
    Py_CLEAR(state->str_TOPOLOGY);
    Py_CLEAR(state->str_BUFFER_ACCESS);
    Py_CLEAR(state->str_IMAGE_FORMAT);
    Py_CLEAR(state->str_VERTEX_FORMAT);
    Py_CLEAR(state->default_loader);
    Py_CLEAR(state->default_context);
    Py_CLEAR(state->HyperGLError);
    Py_CLEAR(state->Context_type);
    Py_CLEAR(state->Buffer_type);
    Py_CLEAR(state->Image_type);
    Py_CLEAR(state->Pipeline_type);
    Py_CLEAR(state->Compute_type);
    Py_CLEAR(state->ImageFace_type);
    Py_CLEAR(state->BufferView_type);
    Py_CLEAR(state->DescriptorSet_type);
    Py_CLEAR(state->GlobalSettings_type);
    Py_CLEAR(state->GLObject_type);
  }
  return 0;
}

static void module_free(void *m) {
  ModuleState *state = m;
  module_clear((PyObject *)m);
#ifdef _WIN64
  if (state->opengl_handle) {
    FreeLibrary((HMODULE)state->opengl_handle);
    state->opengl_handle = NULL;
  }
#endif
}

static PyModuleDef module_def = {
    PyModuleDef_HEAD_INIT, "_hypergl_c",   NULL,
    sizeof(ModuleState),   module_methods, module_slots,
    module_traverse,       module_clear,   (freefunc)module_free,
};

PyMODINIT_FUNC PyInit__hypergl_c(void) { return PyModuleDef_Init(&module_def); }
