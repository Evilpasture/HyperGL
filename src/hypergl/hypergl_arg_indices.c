#include "hypergl_arg_indices.h"
#include <string.h>

// --- 1. REGISTRY INFRASTRUCTURE ---
static FastParser *parser_registry[128];
static int parser_registry_count = 0;

// Helper to count entries at compile time
#define COUNT_X(ID, NAME, TYPE, REQ) +1

#define ALLOC_PARSER(ParserName, GroupName) \
    FastParser ParserName##Parser; \
    FastArgSpec ParserName##Specs[GroupName##_COUNT];

#define GEN_SPEC(ID, NAME, TYPE, REQ) \
    [ID] = {.name = (NAME), .type_name = #TYPE, .required = (bool)(REQ), .convert = FP_GET_CONVERTER((TYPE){0})},

#define INIT_PARSER(ParserName, GroupName, Schema) do { \
    static_assert((0 Schema(COUNT_X)) == GroupName##_COUNT, "Schema length mismatch for " #ParserName); \
    FastArgSpec temp[] = {Schema(GEN_SPEC)}; \
    memcpy(ParserName##Specs, temp, sizeof(temp)); \
    fp_init_impl(&ParserName##Parser, ParserName##Specs, GroupName##_COUNT); \
} while(0)

#define REGISTER_PARSER(ParserName) \
    do { \
        ParserName##Parser.parser_name = #ParserName; \
        parser_registry[parser_registry_count++] = &ParserName##Parser; \
    } while (0)

#define SETUP_PARSER(ParserName, GroupName, Schema) do { \
    INIT_PARSER(ParserName, GroupName, Schema); \
    REGISTER_PARSER(ParserName); \
} while(0)

// --- 2. ALLOCATIONS ---
ALLOC_PARSER(Pipeline, Pipeline)
ALLOC_PARSER(NewFrame, NewFrame)
ALLOC_PARSER(EndFrame, EndFrame)
ALLOC_PARSER(RenderIndirect, RenderIndirect)

// --- 3. INITIALIZATION & TEARDOWN ---

void hypergl_init_all_parsers(void) {
    SETUP_PARSER(Pipeline, Pipeline, SCHEMA_PIPELINE);
    SETUP_PARSER(NewFrame, NewFrame, SCHEMA_NEW_FRAME);
    SETUP_PARSER(EndFrame, EndFrame, SCHEMA_END_FRAME);
    SETUP_PARSER(RenderIndirect, RenderIndirect, SCHEMA_RENDER_INDIRECT);
}

void hypergl_free_all_parsers(void) {
    for (int i = 0; i < parser_registry_count; i++) {
        fp_deinit(parser_registry[i]);
    }
}

// Optional: Useful for debugging API schemas
void fp_dump_schemas_json(FILE *out) {
    fprintf(out, "{\n");
    for (int i = 0; i < parser_registry_count; i++) {
        FastParser *fp = parser_registry[i];
        fprintf(out, "  \"%s\": [\n", fp->parser_name);
        for (size_t j = 0; j < fp->count; j++) {
            fprintf(out, "    {\"name\": \"%s\", \"type\": \"%s\"}%s\n",
                    fp->specs[j].name, fp->specs[j].type_name, 
                    (j == fp->count - 1) ? "" : ",");
        }
        fprintf(out, "  ]%s\n", (i == parser_registry_count - 1) ? "" : ",");
    }
    fprintf(out, "}\n");
}