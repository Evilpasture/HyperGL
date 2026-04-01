#pragma once

#include "py_fast_parse.h"

// --- 1. SCHEMA DEFINITION ---
// X(IndexName, PythonKeyword, C-Type, IsRequired)
#define SCHEMA_PIPELINE(X) \
    X(IDX_PL_TEMPLATE,       "template",        PyObject *, 0) \
    X(IDX_PL_VERT_SHADER,    "vertex_shader",   PyObject *, 0) \
    X(IDX_PL_FRAG_SHADER,    "fragment_shader", PyObject *, 0) \
    X(IDX_PL_LAYOUT,         "layout",          PyObject *, 0) \
    X(IDX_PL_RESOURCES,      "resources",       PyObject *, 0) \
    X(IDX_PL_UNIFORMS,       "uniforms",        PyObject *, 0) \
    X(IDX_PL_DEPTH,          "depth",           PyObject *, 0) \
    X(IDX_PL_STENCIL,        "stencil",         PyObject *, 0) \
    X(IDX_PL_BLEND,          "blend",           PyObject *, 0) \
    X(IDX_PL_FRAMEBUFFER,    "framebuffer",     PyObject *, 0) \
    X(IDX_PL_VERT_BUFFERS,   "vertex_buffers",  PyObject *, 0) \
    X(IDX_PL_INDEX_BUFFER,   "index_buffer",    PyObject *, 0) \
    X(IDX_PL_SHORT_INDEX,    "short_index",     bool,       0) \
    X(IDX_PL_CULL_FACE,      "cull_face",       PyObject *, 0) \
    X(IDX_PL_TOPOLOGY,       "topology",        PyObject *, 0) \
    X(IDX_PL_VERT_COUNT,     "vertex_count",    int,        0) \
    X(IDX_PL_INST_COUNT,     "instance_count",  int,        0) \
    X(IDX_PL_FIRST_VERT,     "first_vertex",    int,        0) \
    X(IDX_PL_VIEWPORT,       "viewport",        PyObject *, 0) \
    X(IDX_PL_UNIFORM_DATA,   "uniform_data",    PyObject *, 0) \
    X(IDX_PL_VIEWPORT_DATA,  "viewport_data",   PyObject *, 0) \
    X(IDX_PL_RENDER_DATA,    "render_data",     PyObject *, 0) \
    X(IDX_PL_INCLUDES,       "includes",        PyObject *, 0)

#define SCHEMA_NEW_FRAME(X) \
    X(IDX_NF_RESET, "reset", bool, 0) \
    X(IDX_NF_CLEAR, "clear", bool, 0)
#define SCHEMA_END_FRAME(X) \
    X(IDX_EF_CLEAN, "clean", bool, 0) \
    X(IDX_EF_FLUSH, "flush", bool, 0)

// --- 2. GENERATE ENUM & COUNT ---
#define GEN_ENUM(ID, NAME, TYPE, REQ) ID,

#define DEFINE_INDEX_GROUP(GroupName, Schema) \
    typedef enum { Schema(GEN_ENUM) GroupName##_COUNT } GroupName##_Idx;

DEFINE_INDEX_GROUP(Pipeline, SCHEMA_PIPELINE)
DEFINE_INDEX_GROUP(NewFrame, SCHEMA_NEW_FRAME)
DEFINE_INDEX_GROUP(EndFrame, SCHEMA_END_FRAME)

// --- 3. DECLARE PARSER GLOBALLY ---
#define DECLARE_PARSER(ParserName, GroupName) \
    extern FastParser ParserName##Parser; \
    extern FastArgSpec ParserName##Specs[GroupName##_COUNT];

DECLARE_PARSER(Pipeline, Pipeline)
DECLARE_PARSER(NewFrame, NewFrame)
DECLARE_PARSER(EndFrame, EndFrame)

// Call this during your Module Initialization!
void hypergl_init_all_parsers(void);
void hypergl_free_all_parsers(void);