#pragma once

#include <Python.h>
#include <stdbool.h>
#include <stdint.h>

// --- Portable Compiler Hints ---
#if defined(__GNUC__) || defined(__clang__)
#    define PF_LIKELY(x) __builtin_expect(!!(x), 1)
#    define PF_UNLIKELY(x) __builtin_expect(!!(x), 0)
#    define PF_FORCE_INLINE inline __attribute__((always_inline))
#else
#    define PF_LIKELY(x) (x)
#    define PF_UNLIKELY(x) (x)
#    define PF_FORCE_INLINE inline
#endif

// --- 1. TYPE CONSTRUCTORS (Inlined) ---

[[nodiscard]] PF_FORCE_INLINE static PyObject *fb_from_float(float v) {
    return PyFloat_FromDouble((double)v);
}
[[nodiscard]] PF_FORCE_INLINE static PyObject *fb_from_int(int v) {
    return PyLong_FromLong((long)v);
}
[[nodiscard]] PF_FORCE_INLINE static PyObject *fb_from_u32(uint32_t v) {
    return PyLong_FromUnsignedLong((unsigned long)v);
}
[[nodiscard]] PF_FORCE_INLINE static PyObject *fb_from_u64(uint64_t v) {
    return PyLong_FromUnsignedLongLong((unsigned long long)v);
}
[[nodiscard]] PF_FORCE_INLINE static PyObject *fb_from_str(const char *v) {
    return PyUnicode_FromString(v);
}
[[nodiscard]] PF_FORCE_INLINE static PyObject *fb_from_bool(bool v) {
    PyObject *res = v ? Py_True : Py_False;
    Py_INCREF(res);
    return res;
}
[[nodiscard]] PF_FORCE_INLINE static PyObject *fb_incref(PyObject *v) {
    Py_XINCREF(v);
    return v;
}

// --- 2. THE C23 COMPILE-TIME ROUTER ---

extern PyObject *PY_FASTBUILD_UNSUPPORTED_TYPE_ERROR(void);

#define FB_VAL(x) _Generic((x), \
    float: fb_from_float, \
    double: PyFloat_FromDouble, \
    int: fb_from_int, \
    long: PyLong_FromLong, \
    long long: PyLong_FromLongLong, \
    unsigned int: fb_from_u32, \
    unsigned long: PyLong_FromUnsignedLong, \
    unsigned long long: fb_from_u64, \
    bool: fb_from_bool, \
    char *: fb_from_str, \
    const char *: fb_from_str, \
    PyObject *: fb_incref, \
    default: PY_FASTBUILD_UNSUPPORTED_TYPE_ERROR)(x)

// --- 3. THE CONTAINER PACKERS ---

[[nodiscard]] PF_FORCE_INLINE static PyObject *fb_pack_tuple(size_t n, PyObject **arr) {
    for (size_t i = 0; i < n; i++) if (PF_UNLIKELY(!arr[i])) goto error;
    PyObject *t = PyTuple_New((Py_ssize_t)n);
    if (PF_UNLIKELY(!t)) goto error;
    for (size_t i = 0; i < n; i++) PyTuple_SET_ITEM(t, i, arr[i]); 
    return t;
error:
    for (size_t i = 0; i < n; i++) Py_XDECREF(arr[i]);
    return nullptr;
}

[[nodiscard]] PF_FORCE_INLINE static PyObject *fb_pack_list(size_t n, PyObject **arr) {
    for (size_t i = 0; i < n; i++) if (PF_UNLIKELY(!arr[i])) goto error;
    PyObject *l = PyList_New((Py_ssize_t)n);
    if (PF_UNLIKELY(!l)) goto error;
    for (size_t i = 0; i < n; i++) PyList_SET_ITEM(l, i, arr[i]);
    return l;
error:
    for (size_t i = 0; i < n; i++) Py_XDECREF(arr[i]);
    return nullptr;
}

[[nodiscard]] PF_FORCE_INLINE static PyObject *fb_pack_dict(size_t n, PyObject **arr) {
    for (size_t i = 0; i < n; i++) if (PF_UNLIKELY(!arr[i])) goto error;
    if (PF_UNLIKELY(n % 2 != 0)) goto error;
    PyObject *d = PyDict_New();
    if (PF_UNLIKELY(!d)) goto error;
    for (size_t i = 0; i < n; i += 2) {
        if (PF_UNLIKELY(PyDict_SetItem(d, arr[i], arr[i + 1]) < 0)) {
            Py_DECREF(d);
            goto error;
        }
    }
    for (size_t i = 0; i < n; i++) Py_DECREF(arr[i]);
    return d;
error:
    for (size_t i = 0; i < n; i++) Py_XDECREF(arr[i]);
    return nullptr;
}

// --- 4. PREPROCESSOR MAPPING ---
#define FB_EXPAND(x) x
#define FB_NARGS_IMPL(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, N, ...) N
#define FB_NARGS(...) FB_NARGS_IMPL(0 __VA_OPT__(,) __VA_ARGS__, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define FB_MAP_1(x) FB_VAL(x)
#define FB_MAP_2(x, ...) FB_VAL(x), FB_EXPAND(FB_MAP_1(__VA_ARGS__))
#define FB_MAP_3(x, ...) FB_VAL(x), FB_EXPAND(FB_MAP_2(__VA_ARGS__))
#define FB_MAP_4(x, ...) FB_VAL(x), FB_EXPAND(FB_MAP_3(__VA_ARGS__))
#define FB_MAP_5(x, ...) FB_VAL(x), FB_EXPAND(FB_MAP_4(__VA_ARGS__))
#define FB_MAP_6(x, ...) FB_VAL(x), FB_EXPAND(FB_MAP_5(__VA_ARGS__))
#define FB_MAP_7(x, ...) FB_VAL(x), FB_EXPAND(FB_MAP_6(__VA_ARGS__))
#define FB_MAP_8(x, ...) FB_VAL(x), FB_EXPAND(FB_MAP_7(__VA_ARGS__))
#define FB_MAP_9(x, ...) FB_VAL(x), FB_EXPAND(FB_MAP_8(__VA_ARGS__))
#define FB_MAP_10(x, ...) FB_VAL(x), FB_EXPAND(FB_MAP_9(__VA_ARGS__))
#define FB_MAP_11(x, ...) FB_VAL(x), FB_EXPAND(FB_MAP_10(__VA_ARGS__))
#define FB_MAP_12(x, ...) FB_VAL(x), FB_EXPAND(FB_MAP_11(__VA_ARGS__))
#define FB_MAP_13(x, ...) FB_VAL(x), FB_EXPAND(FB_MAP_12(__VA_ARGS__))
#define FB_MAP_14(x, ...) FB_VAL(x), FB_EXPAND(FB_MAP_13(__VA_ARGS__))
#define FB_MAP_15(x, ...) FB_VAL(x), FB_EXPAND(FB_MAP_14(__VA_ARGS__))
#define FB_MAP_16(x, ...) FB_VAL(x), FB_EXPAND(FB_MAP_15(__VA_ARGS__))

#define FB_CONCAT_IMPL(a, b) a##b
#define FB_CONCAT(a, b) FB_CONCAT_IMPL(a, b)
#define FB_MAP(...) FB_EXPAND(FB_CONCAT(FB_MAP_, FB_NARGS(__VA_ARGS__))(__VA_ARGS__))

// --- 5. PUBLIC API ---
#define FastKey(parser_ptr, idx) ((parser_ptr)->specs[(idx)].interned)
#define FastBuild_Value(x) FB_VAL(x)
#define FastBuild_Tuple(...) fb_pack_tuple(FB_NARGS(__VA_ARGS__), FB_NARGS(__VA_ARGS__) ? (PyObject *[]){__VA_OPT__(FB_MAP(__VA_ARGS__))} : nullptr)
#define FastBuild_List(...)  fb_pack_list(FB_NARGS(__VA_ARGS__), FB_NARGS(__VA_ARGS__) ? (PyObject *[]){__VA_OPT__(FB_MAP(__VA_ARGS__))} : nullptr)
#define FastBuild_Dict(...)  fb_pack_dict(FB_NARGS(__VA_ARGS__), (PyObject *[]){FB_MAP(__VA_ARGS__)})