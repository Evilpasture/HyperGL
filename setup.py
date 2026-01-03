import os
import re
import sys
import sysconfig
from setuptools import Extension, setup

# USE_ASAN = os.getenv('USE_ASAN') == '1'

extra_compile_args = []
# extra_link_args = []
define_macros = [('Py_GIL_DISABLED', '1')]
libraries = []

if sys.platform == 'win32':
    extra_compile_args += ['/std:c11', '/O2', '/Zi']
    libraries += ['opengl32', 'user32', 'gdi32']
elif sys.platform.startswith('linux'):
    extra_compile_args += ['-std=c11', '-O3', '-fPIC']
elif sys.platform.startswith('darwin'):
    extra_compile_args += ['-Wno-writable-strings', '-std=c11']

if os.getenv('PYODIDE') or str(sysconfig.get_config_var('HOST_GNU_TYPE')).startswith('wasm'):
    with open('hypergl.js') as hypergl_js:
        code = re.sub(r'\s+', ' ', hypergl_js.read())
        code = code.replace('"', '\\"') 
        define_macros += [('EXTERN_GL', f'"{code}"')]

if os.getenv('HYPERGL_VALIDATE', '1') == '1':
    define_macros.append(('ENABLE_VALIDATION', '1'))
    print("Building with ENABLE_VALIDATION=1")
else:
    print("Building for performance (Validation Disabled)")

if os.getenv('DISABLE_LOCKS', '0') == '1':
    define_macros.append(('DISABLE_LOCKS', '1'))
    print("Building with DISABLE_LOCKS=1")
else:
    print("Building without DEBUG")

# if sys.platform == 'win32':
#     # Standard flags
#     extra_compile_args += ['/std:c11', '/O2', '/Zi']
#     libraries += ['opengl32', 'user32', 'gdi32']
    
#     if USE_ASAN:
#         # ASan specific flags for MSVC
#         # We must disable /incremental and /ZI (Edit and Continue) as they conflict with ASan
#         extra_compile_args += ['/fsanitize=address']
#         extra_link_args = [
#             '/INCREMENTAL:NO',
#             '/DEBUG',
#             '/WHOLEARCHIVE:clang_rt.asan_dynamic-x86_64.lib',
#             '/WHOLEARCHIVE:clang_rt.asan_dynamic_runtime_thunk-x86_64.lib'
#         ]
#         libraries += ['clang_rt.asan_dynamic-x86_64', 'clang_rt.asan_dynamic_runtime_thunk-x86_64']
#         # Remove conflicting flags if they exist
#         if '/Zi' in extra_compile_args: extra_compile_args.remove('/Zi')
#         extra_compile_args.append('/Z7') # Use C7 compatible debug info instead
#         print("MSVC Building with AddressSanitizer enabled")

ext = Extension(
    name='hypergl._hypergl_c',
    sources=['hypergl.c'],
    libraries=libraries,
    extra_compile_args=extra_compile_args,
    # extra_link_args=extra_link_args,
    define_macros=define_macros, # type: ignore
)

setup(
    name='hypergl',
    ext_modules=[ext],
    packages=['hypergl', 'hypergl-stubs'],
    package_data={
        'hypergl': ['_hypergl.py'],
        'hypergl-stubs': ['*.pyi'],
    },
    include_package_data=True,
)