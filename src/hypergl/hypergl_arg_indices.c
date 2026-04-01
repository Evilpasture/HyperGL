#include "hypergl_arg_indices.h"
#include <string.h>

// Helper to count entries at compile time
#define COUNT_X(ID, NAME, TYPE, REQ) +1

#define ALLOC_PARSER(ParserName, GroupName) \
    FastParser ParserName##Parser; \
    FastArgSpec ParserName##Specs[GroupName##_COUNT];

#define GEN_SPEC(ID, NAME, TYPE, REQ) \
    [ID] = {.name = (NAME), .type_name = #TYPE, .required = (bool)(REQ), .convert = FP_GET_CONVERTER((TYPE){0})},

#define INIT_PARSER(ParserName, GroupName, Schema) do { \
    static_assert((0 Schema(COUNT_X)) == GroupName##_COUNT, "Schema length mismatch"); \
    FastArgSpec temp[] = {Schema(GEN_SPEC)}; \
    memcpy(ParserName##Specs, temp, sizeof(temp)); \
    fp_init_impl(&ParserName##Parser, ParserName##Specs, GroupName##_COUNT); \
} while(0)


// 1. Allocate memory
ALLOC_PARSER(Pipeline, Pipeline)

// 2. Init Function (Call this in PyMODINIT_FUNC)
void hypergl_init_all_parsers(void) {
    INIT_PARSER(Pipeline, Pipeline, SCHEMA_PIPELINE);
}

// 3. Deinit Function (Call this when module unloads)
void hypergl_free_all_parsers(void) {
    fp_deinit(&PipelineParser);
}