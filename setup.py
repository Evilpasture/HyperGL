import subprocess
import shutil
import os
import re
import sys
import sysconfig
from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext

YELLOW = "\033[33m"
RESET = "\033[0m"

class ClangBuildExt(build_ext):
    def build_extensions(self):
        clang_path = find_llvm_clang()
        if clang_path:
            old_spawn = self.compiler.spawn
            
            def clang_spawn(cmd, *args, **kwargs):
                cmd = list(cmd)
                executable = os.path.basename(cmd[0]).lower()
                
                # 1. Handle the Compiler
                if executable == "cl.exe":
                    cmd[0] = clang_path # Point to clang-cl.exe
                    
                # 2. Handle the Linker
                elif executable == "link.exe":
                    # Find lld-link.exe in the same directory as clang-cl
                    lld_path = os.path.join(os.path.dirname(clang_path), "lld-link.exe")
                    if os.path.exists(lld_path):
                        cmd[0] = lld_path
                        # Strip out flags lld-link hates (like the compiler-specific /clang: ones)
                        cmd = [arg for arg in cmd if not arg.startswith('/clang:')]
                        # Ensure /LTCG is present for LTO compatibility
                        if '/LTCG' not in cmd:
                            cmd.append('/LTCG')
                    else:
                        print(f"Warning: lld-link not found at {lld_path}, falling back to default link.exe")
                        
                return old_spawn(cmd, *args, **kwargs)
            
            self.compiler.spawn = clang_spawn
            # Using the yellow color we talked about!
            print(f"\033[33m--- HIJACKED: Swapping cl.exe -> {clang_path} ---\033[0m")
            
        super().build_extensions()

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
    # 1. Trust the user's PATH first
    which_clang = shutil.which("clang-cl")
    if which_clang:
        print("Clang-Cl found.", which_clang)
        return which_clang

    # 2. Use vswhere to find the Visual Studio installation path
    try:
        # We look for any VS instance that has the LLVM component installed
        vswhere_path = os.path.expandvars("%ProgramFiles(x86)%\\Microsoft Visual Studio\\Installer\\vswhere.exe")
        if os.path.exists(vswhere_path):
            vs_path = subprocess.check_output([
                vswhere_path, 
                "-latest", 
                "-products", "*", 
                "-requires", "Microsoft.VisualStudio.Component.VC.Llvm.Clang", 
                "-property", "installationPath"
            ]).decode().strip()
            
            if vs_path:
                # VS keeps Clang in a specific subfolder structure
                # We search for clang-cl.exe within that folder
                for root, dirs, files in os.walk(os.path.join(vs_path, "VC\\Tools\\Llvm")):
                    if "clang-cl.exe" in files:
                        if "x64" in root.lower() and "arm64" not in root.lower():
                            print("Clang-Cl found via vswhere")
                            return os.path.join(root, "clang-cl.exe")
    except Exception:
        pass

    # 3. Last ditch effort: Common LLVM standalone path
    standard_llvm = "C:\\Program Files\\LLVM\\bin\\clang-cl.exe"
    if os.path.exists(standard_llvm):
        return standard_llvm
    print(f"{YELLOW}Warning:{RESET} Clang-Cl not found.")
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
        # Aggressive clang-cl optimization
        extra_compile_args.append('/clang:--target=x86_64-pc-windows-msvc')
        optimization_flag = '/clang:-O3' 
        extra_compile_args.append('/clang:-flto')
        extra_link_args.append('/clang:-flto') # Tells the linker to use the LLVM plugin
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
    cmdclass={'build_ext': ClangBuildExt}, # This overrides the default build behavior
    ext_modules=[ext],
    packages=['hypergl', 'hypergl-stubs'],
    package_data={
        'hypergl': ['_hypergl.py'],
        'hypergl-stubs': ['*.pyi'],
    },
    include_package_data=True,
)