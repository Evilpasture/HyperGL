import os
import shutil
import re
import sys
import sysconfig
from setuptools import Extension, setup

# Attempt to import tomllib (Python 3.11+) or toml (legacy)
try:
    import tomllib
except ImportError:
    try:
        import toml as tomllib
    except ImportError:
        tomllib = None

def find_llvm_clang():
    """Attempts to find the path to clang-cl.exe"""
    # 1. Check if it's already in the PATH
    which_clang = shutil.which("clang-cl")
    if which_clang:
        return which_clang

    # 2. Check common Windows install locations
    potential_paths = [
        "C:\\Program Files\\LLVM\\bin\\clang-cl.exe",
        "C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\VC\\Tools\\Llvm\\x64\\bin\\clang-cl.exe",
        "C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\Community\\VC\\Tools\\Llvm\\x64\\bin\\clang-cl.exe",
        "C:\\Program Files (x86)\\Microsoft Visual Studio\\18\\BuildTools\\VC\\Tools\\Llvm\\bin\\clang-cl.exe"
    ]

    for path in potential_paths:
        if os.path.exists(path):
            return path
    return None

# --- Metadata Extraction ---
version = "0.0.0"
if tomllib:
    try:
        with open("pyproject.toml", "rb" if hasattr(tomllib, "load") else "r") as f:
            data = tomllib.load(f)
            version = data.get("project", {}).get("version", "0.0.0")
    except FileNotFoundError:
        pass

# --- Compiler Configuration ---
extra_compile_args = []
extra_link_args = []
define_macros = [('Py_GIL_DISABLED', '1'), ('HYPERGL_VERSION', f'"{version}"')]
libraries = []

clang_bin = find_llvm_clang()

if sys.platform == 'win32':
    # Force x64 target to prevent architecture mismatch
    extra_compile_args += ['--target=x86_64-pc-windows-msvc', '/std:c11', '/O2', '/Zi']
    libraries += ['opengl32', 'user32', 'gdi32']
    
    # Linker specific fixes for 3.14t
    # /NODEFAULTLIB prevents the linker from hunting for the non-existent python314.lib
    extra_link_args += ['/link', '/MACHINE:X64', '/NODEFAULTLIB:python314.lib']

    if clang_bin:
        print(f"--- Using LLVM/Clang: {clang_bin}")
        os.environ["CC"] = clang_bin
        os.environ["CXX"] = clang_bin
        # Monkeypatch setuptools to ensure it actually uses clang-cl on Windows
        try:
            from setuptools import distutils
            import distutils._msvccompiler as msvc
            msvc.MSVCCompiler.cc = clang_bin
            msvc.MSVCCompiler.linker = clang_bin
        except ImportError:
            print("--- Warning: Could not monkeypatch MSVCCompiler. Build might fallback to cl.exe.")
elif sys.platform.startswith('linux'):
    extra_compile_args += ['-std=c11', '-O3', '-fPIC']
elif sys.platform.startswith('darwin'):
    extra_compile_args += ['-Wno-writable-strings', '-std=c11']

# --- Custom Logic: Pyodide / WASM ---
if os.getenv('PYODIDE') or str(sysconfig.get_config_var('HOST_GNU_TYPE')).startswith('wasm'):
    try:
        with open('hypergl.js') as hypergl_js:
            code = re.sub(r'\s+', ' ', hypergl_js.read())
            code = code.replace('"', '\\"') 
            define_macros += [('EXTERN_GL', f'"{code}"')]
    except FileNotFoundError:
        print("--- Warning: hypergl.js not found for WASM build.")

# --- Custom Logic: Environment Toggles ---
if os.getenv('HYPERGL_VALIDATE', '1') == '1':
    define_macros.append(('ENABLE_VALIDATION', '1'))
    print("--- Build Mode: ENABLE_VALIDATION=1")
else:
    print("--- Build Mode: Performance (Validation Disabled)")

if os.getenv('DISABLE_LOCKS', '0') == '1':
    define_macros.append(('DISABLE_LOCKS', '1'))
    print("--- Build Mode: DISABLE_LOCKS=1")

# --- Extension Definition ---
ext = Extension(
    name='hypergl._hypergl_c',
    sources=['hypergl.c'],
    libraries=libraries,
    extra_compile_args=extra_compile_args,
    extra_link_args=extra_link_args,
    define_macros=define_macros, # type: ignore
)

setup(
    name='hypergl',
    version=version,
    ext_modules=[ext],
    packages=['hypergl', 'hypergl-stubs'],
    package_data={
        'hypergl': ['_hypergl.py'],
        'hypergl-stubs': ['*.pyi'],
    },
    include_package_data=True,
)