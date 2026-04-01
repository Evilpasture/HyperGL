#pragma once

#include <Python.h>
#include <stdbool.h>
#include <stdint.h>

#if defined(__GNUC__) || defined(__clang__)
#    define PF_LIKELY(x) __builtin_expect(!!(x), 1)
#    define PF_UNLIKELY(x) __builtin_expect(!!(x), 0)
#    define PF_RESTRICT __restrict__
#    define PF_FORCE_INLINE inline __attribute__((always_inline))
#else
#    define PF_LIKELY(x) (x)
#    define PF_UNLIKELY(x) (x)
#    define PF_RESTRICT 
#    define PF_FORCE_INLINE inline
#endif

#define PF_FP_EMPTY_SLOT 0xFFFF

// --- 1. TYPES & STRUCTS ---
typedef struct {
    const char *name;
    const char *type_name;
    PyObject *interned;
    bool (*convert)(PyObject *, void *);
    PyTypeObject *type_guard;
    bool required;
} FastArgSpec;

typedef struct {
    const char* parser_name;
    FastArgSpec *specs;
    uint16_t *lookup_table;
    size_t count;
    size_t table_mask;
    uint64_t required_mask;
    uint64_t type_guard_mask;
} FastParser;

// --- 2. EXTERN DECLARATIONS (Cold Paths in .c) ---
// By keeping the error reporting functions in the .c file, we keep the inlined
// fast-path code extremely small in the instruction cache.
extern bool fp_report_missing(const FastParser *fp, uint64_t provided_mask);
extern bool fp_report_type_error(const FastParser *fp, size_t index, PyObject *val);
extern bool fp_report_multiple(const FastParser *fp, size_t index);
extern bool fp_report_too_many(const FastParser *fp, Py_ssize_t nargs);
extern bool fp_report_unexpected_kwarg(PyObject *key);

extern void fp_init_impl(FastParser *fp, FastArgSpec *specs, size_t count);
extern void fp_deinit(FastParser *fp);
extern bool fp_parse_legacy(PyObject *args, PyObject *kwargs, PyObject *unused, const FastParser *fp, void **targets);


// --- 3. PRIMITIVE CONVERTERS (Inlined) ---
[[maybe_unused]] PF_FORCE_INLINE static bool fp_conv_float(PyObject *o, void *t) {
    if (PF_UNLIKELY(o == Py_None)) { PyErr_SetString(PyExc_TypeError, "float cannot be None"); return false; }
    double v = PyFloat_AsDouble(o);
    if (PF_UNLIKELY(v == -1.0 && PyErr_Occurred())) return false;
    *(float *)t = (float)v;
    return true;
}
[[maybe_unused]] PF_FORCE_INLINE static bool fp_conv_double(PyObject *o, void *t) {
    if (PF_UNLIKELY(o == Py_None)) { PyErr_SetString(PyExc_TypeError, "double cannot be None"); return false; }
    double v = PyFloat_AsDouble(o);
    if (PF_UNLIKELY(v == -1.0 && PyErr_Occurred())) return false;
    *(double *)t = v;
    return true;
}
[[maybe_unused]] PF_FORCE_INLINE static bool fp_conv_int(PyObject *o, void *t) {
    long v = PyLong_AsLong(o);
    if (PF_UNLIKELY(v == -1 && PyErr_Occurred())) return false;
    *(int *)t = (int)v;
    return true;
}
[[maybe_unused]] PF_FORCE_INLINE static bool fp_conv_u32(PyObject *o, void *t) {
    unsigned long v = PyLong_AsUnsignedLongMask(o);
    if (PF_UNLIKELY(PyErr_Occurred())) return false;
    *(uint32_t *)t = (uint32_t)v;
    return true;
}
[[maybe_unused]] PF_FORCE_INLINE static bool fp_conv_u64(PyObject *o, void *t) {
    unsigned long long v = PyLong_AsUnsignedLongLong(o);
    if (PF_UNLIKELY(PyErr_Occurred())) return false;
    *(uint64_t *)t = (uint64_t)v;
    return true;
}
[[maybe_unused]] PF_FORCE_INLINE static bool fp_conv_bool(PyObject *o, void *t) {
    int v = PyObject_IsTrue(o);
    if (PF_UNLIKELY(v == -1)) return false;
    *(bool *)t = (bool)v;
    return true;
}
[[maybe_unused]] PF_FORCE_INLINE static bool fp_conv_pyobj(PyObject *o, void *t) {
    *(PyObject **)t = o;
    return true;
}

// Strictly C/Python primitives now.
#define FP_GET_CONVERTER(T) _Generic((T), \
    float: fp_conv_float, \
    double: fp_conv_double, \
    int: fp_conv_int, \
    uint32_t: fp_conv_u32, \
    uint64_t: fp_conv_u64, \
    bool: fp_conv_bool, \
    PyObject *: fp_conv_pyobj \
)

// Standard MACROS for primitives
#define FP_ARG(name_str, var) \
    {.name = (name_str), .convert = FP_GET_CONVERTER((typeof_unqual(var)){0}), .required = false}
#define FP_REQ_ARG(name_str, var) \
    {.name = (name_str), .convert = FP_GET_CONVERTER((typeof_unqual(var)){0}), .required = true}

// Custom domain struct converter macro
#define FP_CUSTOM_ARG(name_str, func_ptr, is_req) \
    {.name = (name_str), .convert = (func_ptr), .required = (is_req)}


// --- 4. THE HOT PATH (Inlined Vectorcall Engine) ---

PF_FORCE_INLINE static size_t fp_hash_ptr(PyObject *ptr, size_t mask) {
    uintptr_t v = (uintptr_t)ptr;
    return ((v >> 4) ^ (v >> 10)) & mask;
}

[[nodiscard]] PF_FORCE_INLINE static bool fp_parse_vector(
    PyObject *const *PF_RESTRICT args, 
    Py_ssize_t nargs, 
    PyObject *PF_RESTRICT kwnames, 
    const FastParser *PF_RESTRICT fp, 
    void *PF_RESTRICT *PF_RESTRICT targets) 
{
    uint64_t provided_mask = 0;
    const uint64_t tg_mask = fp->type_guard_mask; // Load once to register
    const size_t count = fp->count;
    const FastArgSpec *specs = fp->specs;

    // 1. Validate Positional Count
    if (PF_UNLIKELY(nargs > (Py_ssize_t)count)) {
        return fp_report_too_many(fp, nargs);
    }

    // 2. Positional Logic
    for (Py_ssize_t i = 0; i < nargs; ++i) {
        PyObject *val = args[i];
        const FastArgSpec *spec = &specs[i];

        // O(1) Bitwise check. If tg_mask is 0, this is naturally skipped.
        if (PF_UNLIKELY(tg_mask & (1ULL << i))) {
            if (PF_UNLIKELY(!Py_IS_TYPE(val, spec->type_guard) && 
                            !PyObject_TypeCheck(val, spec->type_guard))) {
                return fp_report_type_error(fp, i, val);
            }
        }

        if (PF_UNLIKELY(!spec->convert(val, targets[i]))) {
            return false; // Convert sets its own ValueError
        }
        provided_mask |= (1ULL << i);
    }

    // 3. Keywords Logic
    if (kwnames) {
        Py_ssize_t nkw = PyTuple_GET_SIZE(kwnames);
        PyObject *const *kw_vals = args + nargs;

        for (Py_ssize_t i = 0; i < nkw; ++i) {
            PyObject *key = PyTuple_GET_ITEM(kwnames, i);
            size_t idx = PF_FP_EMPTY_SLOT;

            // Fast Path: O(1) Hash Table Lookup
            if (fp->lookup_table) {
                size_t h = fp_hash_ptr(key, fp->table_mask);
                while (fp->lookup_table[h] != PF_FP_EMPTY_SLOT) {
                    size_t candidate = fp->lookup_table[h];
                    // Pointer comparison only (assumes interned strings)
                    if (PF_LIKELY(specs[candidate].interned == key)) {
                        idx = candidate;
                        break;
                    }
                    h = (h + 1) & fp->table_mask;
                }
            }

            // Slow Path: Linear fallback for small schemas OR un-interned string keys
            if (PF_UNLIKELY(idx == PF_FP_EMPTY_SLOT)) {
                for (size_t j = 0; j < count; ++j) {
                    if (specs[j].interned == key || 
                        PyUnicode_Compare(key, specs[j].interned) == 0) {
                        idx = j;
                        break;
                    }
                }
            }

            // Keyword Validation
            if (PF_UNLIKELY(idx == PF_FP_EMPTY_SLOT)) {
                return fp_report_unexpected_kwarg(key);
            }

            if (PF_UNLIKELY(provided_mask & (1ULL << idx))) {
                return fp_report_multiple(fp, idx);
            }

            // Type Guard & Conversion
            PyObject *val = kw_vals[i];
            const FastArgSpec *spec = &specs[idx];

            if (PF_UNLIKELY(tg_mask & (1ULL << idx))) {
                if (PF_UNLIKELY(!Py_IS_TYPE(val, spec->type_guard) && 
                                !PyObject_TypeCheck(val, spec->type_guard))) {
                    return fp_report_type_error(fp, idx, val);
                }
            }

            if (PF_UNLIKELY(!spec->convert(val, targets[idx]))) {
                return false;
            }
            provided_mask |= (1ULL << idx);
        }
    }

    // 4. Final Required Check
    if (PF_UNLIKELY((provided_mask & fp->required_mask) != fp->required_mask)) {
        return fp_report_missing(fp, provided_mask);
    }

    return true; 
}


// --- 5. PUBLIC MACROS ---
extern void ERROR_FastParse_First_Arg_Must_Be_PyObject_Ptr_Or_Vectorcall_Ptr(void);

#define FastParse_Unified(arg1, arg2, arg3, arg4, arg5) _Generic((arg1), \
    PyObject *const *: fp_parse_vector, \
    PyObject **: fp_parse_vector, \
    PyObject *: fp_parse_legacy, \
    default: ERROR_FastParse_First_Arg_Must_Be_PyObject_Ptr_Or_Vectorcall_Ptr)( \
    (arg1), (arg2), (arg3), (arg4), (arg5))

#define FastParse_Init(fp, specs, count) do { \
    static_assert((count) <= 64, "FastParse only supports up to 64 arguments"); \
    fp_init_impl(fp, specs, count); \
} while (0)