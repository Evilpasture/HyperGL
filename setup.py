import os
import re
import sys
import sysconfig
from setuptools import Extension, setup


extra_compile_args = []
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

ext = Extension(
    name='hypergl._hypergl_c',
    sources=['hypergl.c'],
    libraries=libraries,
    extra_compile_args=extra_compile_args,
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