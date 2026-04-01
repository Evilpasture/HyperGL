#include "py_fast_parse.h"

#define MAX_ARG_LIMIT 64
#define FP_HASH_THRESHOLD 8

// --- Cold Error Reporting Paths ---
bool fp_report_missing(const FastParser *fp, uint64_t provided_mask) {
    for (size_t i = 0; i < fp->count; i++) {
        if ((int)fp->specs[i].required && !(provided_mask & (1ULL << i))) {
            PyErr_Format(PyExc_TypeError, "required argument '%s' missing", fp->specs[i].name);
            return false;
        }
    }
    return false;
}

bool fp_report_type_error(const FastParser *fp, size_t index, PyObject *val) {
    const FastArgSpec *spec = &fp->specs[index];
    const char *expected_type = spec->type_name ? spec->type_name : spec->type_guard->tp_name;
    PyErr_Format(PyExc_TypeError, "argument '%s' must be %s, not %.200s", spec->name, expected_type, Py_TYPE(val)->tp_name);
    return false;
}

bool fp_report_multiple(const FastParser *fp, size_t index) {
    PyErr_Format(PyExc_TypeError, "argument '%s' got multiple values", fp->specs[index].name);
    return false;
}

bool fp_report_too_many(const FastParser *fp, Py_ssize_t nargs) {
    PyErr_Format(PyExc_TypeError, "too many positional arguments (expected %zu, got %zd)", fp->count, nargs);
    return false;
}

bool fp_report_unexpected_kwarg(PyObject *key) {
    PyErr_Format(PyExc_TypeError, "unexpected keyword argument '%U'", key);
    return false;
}

// --- Initialization & Teardown ---
void fp_init_impl(FastParser *fp, FastArgSpec *specs, size_t count) {
    if (count > MAX_ARG_LIMIT) Py_FatalError("FastParse: Argument count exceeds bitmask limit of 64.");

    fp->specs = specs;
    fp->count = count;
    fp->required_mask = 0;
    fp->type_guard_mask = 0;
    fp->lookup_table = nullptr;

    for (size_t i = 0; i < count; i++) {
        if (specs[i].name) {
            specs[i].interned = PyUnicode_InternFromString(specs[i].name);
            [[maybe_unused]] const char* cache = PyUnicode_AsUTF8(specs[i].interned);
        }
        if (specs[i].required) fp->required_mask |= (1ULL << i);
        if (specs[i].type_guard) fp->type_guard_mask |= (1ULL << i);
    }

    if (count > FP_HASH_THRESHOLD) {
        size_t table_size = 1;
        while (table_size < (count * 2)) table_size <<= 1;
        fp->table_mask = table_size - 1;
        
        // Use standard Python Allocator
        fp->lookup_table = (uint16_t *)PyMem_RawMalloc(table_size * sizeof(uint16_t));
        if (!fp->lookup_table) Py_FatalError("FastParse: Failed to allocate lookup table.");

        for (size_t i = 0; i < table_size; i++) fp->lookup_table[i] = PF_FP_EMPTY_SLOT;

        for (size_t i = 0; i < count; i++) {
            size_t h = fp_hash_ptr(fp->specs[i].interned, fp->table_mask);
            while (fp->lookup_table[h] != PF_FP_EMPTY_SLOT) h = (h + 1) & fp->table_mask;
            fp->lookup_table[h] = (uint16_t)i;
        }
    }
}

void fp_deinit(FastParser *fp) {
    if (!fp) return;
    if (fp->specs) {
        for (size_t i = 0; i < fp->count; i++) {
            Py_XDECREF(fp->specs[i].interned);
            fp->specs[i].interned = nullptr;
        }
    }
    if (fp->lookup_table) {
        PyMem_RawFree(fp->lookup_table);
        fp->lookup_table = nullptr;
    }
}

// --- The Legacy Parser (Too complex to inline effectively) ---
[[nodiscard]] bool fp_parse_legacy(PyObject *args, PyObject *kwargs, [[maybe_unused]] PyObject *unused, const FastParser *fp, void **targets) {
    uint64_t provided_mask = 0;
    const size_t count = fp->count;
    const FastArgSpec *specs = fp->specs;

    if (args) {
        Py_ssize_t nargs = PyTuple_GET_SIZE(args);
        if (nargs > (Py_ssize_t)count) return fp_report_too_many(fp, nargs);
        for (Py_ssize_t i = 0; i < nargs; ++i) {
            provided_mask |= (1ULL << i);
            if (PF_UNLIKELY(!specs[i].convert(PyTuple_GET_ITEM(args, i), targets[i]))) return false;
        }
    }

    if (kwargs) {
        PyObject *key, *val;
        Py_ssize_t pos = 0;
        while (PyDict_Next(kwargs, &pos, &key, &val)) {
            size_t idx = PF_FP_EMPTY_SLOT;
            if (fp->lookup_table) {
                size_t h = fp_hash_ptr(key, fp->table_mask);
                while (fp->lookup_table[h] != PF_FP_EMPTY_SLOT) {
                    size_t candidate = fp->lookup_table[h];
                    if (specs[candidate].interned == key) { idx = candidate; break; }
                    h = (h + 1) & fp->table_mask;
                }
            }
            if (PF_UNLIKELY(idx == PF_FP_EMPTY_SLOT)) {
                for (size_t i = 0; i < count; ++i) {
                    if (specs[i].interned == key || PyUnicode_Compare(key, specs[i].interned) == 0) { idx = i; break; }
                }
            }
            if (idx == PF_FP_EMPTY_SLOT) {
                return fp_report_unexpected_kwarg(key);
            }
            if (PF_UNLIKELY(provided_mask & (1ULL << idx))) return fp_report_multiple(fp, idx);
            provided_mask |= (1ULL << idx);
            if (PF_UNLIKELY(!specs[idx].convert(val, targets[idx]))) return false;
        }
    }

    if (PF_UNLIKELY((provided_mask & fp->required_mask) != fp->required_mask)) return fp_report_missing(fp, provided_mask);
    return (PyErr_Occurred() == nullptr);
}