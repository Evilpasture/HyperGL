import os
import re
import sys
import sysconfig

from setuptools import Extension, setup

extra_compile_args = []
extra_link_args = []
define_macros = []

if sys.platform.startswith('darwin'):
    extra_compile_args += ['-Wno-writable-strings']

if os.getenv('PYODIDE') or str(sysconfig.get_config_var('HOST_GNU_TYPE')).startswith('wasm'):
    with open('hypergl.js') as hypergl_js:
        code = re.sub(r'\s+', ' ', hypergl_js.read())
        define_macros += [('EXTERN_GL', f'"{code}"')]

if os.getenv('HYPERGL_COVERAGE'):
    extra_compile_args += ['-O0', '--coverage']
    extra_link_args += ['-O0', '--coverage']

if os.getenv('HYPERGL_WARNINGS'):
    if sys.platform.startswith('linux'):
        extra_compile_args += [
            '-g3',
            '-Wall',
            '-Wextra',
            '-Wconversion',
            '-Wdouble-promotion',
            '-Wno-unused-parameter',
            '-Wno-cast-function-type',
            '-Werror',
        ]
        extra_link_args += [
            '-fsanitize=undefined',
        ]

ext = Extension(
    name='hypergl',
    sources=['hypergl.c'],
    extra_compile_args=extra_compile_args,
    extra_link_args=extra_link_args,
    define_macros=define_macros,
)

with open('README.md') as readme:
    long_description = readme.read()

setup(
    name='hypergl',
    version='2.7.2',
    ext_modules=[ext],
    py_modules=['_hypergl'],
    license='MIT',
    platforms=['any'],
    description='OpenGL Pipelines for Python',
    long_description=long_description,
    long_description_content_type='text/markdown',
    author='Mai Huy Hiếu',
    author_email='huyhieu0607@gmail.com',
    url='https://github.com/Evilpasture/HyperGL/',
    project_urls={
        'Source': 'https://github.com/Evilpasture/HyperGL/',
        'Bug Tracker': 'https://github.com/Evilpasture/HyperGL/issues/',
    },
    keywords=[
        'OpenGL',
        'rendering',
        'graphics',
        'shader',
        'gpu',
    ],
    packages=['hypergl-stubs'],
    package_data={'hypergl-stubs': ['__init__.pyi']},
    include_package_data=True,
)
