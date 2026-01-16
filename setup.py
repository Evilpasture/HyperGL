import os
import shutil
import re
import sys
import sysconfig
from setuptools import Extension, setup

# --- Metadata Extraction ---
try:
    import tomllib
except ImportError:
    try:
        import toml as tomllib
    except ImportError:
        tomllib = None

version = "0.0.0"
if tomllib:
    try:
        with open("pyproject.toml", "rb" if hasattr(tomllib, "load") else "r") as f:
            data = tomllib.load(f)
            version = data.get("project", {}).get("version", "0.0.0")
    except Exception:
        pass

def find_llvm_clang():
    """Attempts to find the path to clang-cl.exe"""
    which_clang = shutil.which("clang-cl")
    if which_clang:
        return which_clang

    potential_paths = [
        "C:\\Program Files\\LLVM\\bin\\clang-cl.exe",
        "C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\VC\\Tools\\Llvm\\x64\\bin\\clang-cl.exe",
        "C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\Community\\VC\\Tools\\Llvm\\x64\\bin\\clang-cl.exe"
    ]
    for path in potential_paths:
        if os.path.exists(path):
            return path
    return None

# --- Compiler Configuration ---
extra_compile_args = []
extra_link_args = []
define_macros = [('Py_GIL_DISABLED', '1'), ('HYPERGL_VERSION', f'"{version}"')]
libraries = []

if sys.platform == 'win32':
    # ARCH & STANDARDS: Force x64 and C11
    # We use /clang: to pass flags if we are using clang-cl, otherwise cl.exe ignores them
    clang_bin = find_llvm_clang()
    # Replace /O2 with /O3 if we found clang
    if clang_bin:
        extra_compile_args.append('/clang:--target=x86_64-pc-windows-msvc')
        # Aggressive clang-cl optimization
        optimization_flag = '/O3' 
    else:
        optimization_flag = '/O2'

    extra_compile_args += ['/std:c11', optimization_flag, '/Zi']
    libraries += ['opengl32', 'user32', 'gdi32']
    
    # Find the elusive .lib directory
    # 'platlib' or 'base' + 'libs' usually works on Windows
    py_lib_dir = os.path.join(sysconfig.get_config_var('base'), "libs")
    if not os.path.exists(py_lib_dir):
        # Fallback for some virtualenvs/GitHub-hosted versions
        py_lib_dir = os.path.join(sys.prefix, "libs")
        
    extra_link_args += [f'/LIBPATH:{py_lib_dir}', '/MACHINE:X64']

    # VERSION-AWARE NODEFAULTLIB
    # This prevents the linker from looking for 'python314.lib' when it needs 'python314t.lib'
    ver_nodot = "".join(map(str, sys.version_info[:2]))
    extra_link_args.append(f'/NODEFAULTLIB:python{ver_nodot}.lib')

elif sys.platform.startswith('linux'):
    extra_compile_args += ['-std=c11', '-O3', '-fPIC']
elif sys.platform.startswith('darwin'):
    extra_compile_args += ['-Wno-writable-strings', '-std=c11']
    extra_link_args += ['-framework', 'OpenGL', '-framework', 'CoreFoundation']

# --- Environment Toggles ---
if os.getenv('HYPERGL_VALIDATE', '1') == '1':
    define_macros.append(('ENABLE_VALIDATION', '1'))

if os.getenv('DISABLE_LOCKS', '0') == '1':
    define_macros.append(('DISABLE_LOCKS', '1'))

# --- Extension Definition ---
ext = Extension(
    name='hypergl._hypergl_c',
    sources=['hypergl.c'],
    libraries=libraries,
    extra_compile_args=extra_compile_args,
    extra_link_args=extra_link_args,
    define_macros=define_macros, # type:ignore
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