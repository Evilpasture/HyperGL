cmake_minimum_required(VERSION 4.0) # Using modern 4.x standards
project(hypergl 
    VERSION 0.1.0 
    DESCRIPTION "High-performance OpenGL bindings for free-threaded Python"
    LANGUAGES C
)

# --- Global Compiler Settings ---
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# --- Python Discovery ---
# Specifically targeting the free-threaded (GIL-disabled) build
find_package(Python 3.13 REQUIRED COMPONENTS Interpreter Development.Module)

# --- Extension Definition ---
# Mapping the directory structure seen in your 'dir' output
add_library(_hypergl_c MODULE 
    src/hypergl/hypergl.c
    src/hypergl/py_fast_parse.c
    src/hypergl/hypergl_arg_indices.c
)

# Ensure the output name is exactly what Python expects to import
set_target_properties(_hypergl_c PROPERTIES 
    OUTPUT_NAME "_hypergl_c"
    PREFIX ""
    SUFFIX ".pyd"
    C_VISIBILITY_PRESET hidden
)

# --- Include Directories ---
target_include_directories(_hypergl_c PRIVATE src/hypergl)
target_link_libraries(_hypergl_c PRIVATE Python::Module)

# --- Platform & Compiler Configuration ---

# 1. Windows / MSVC / Clang-CL Logic
if(WIN32)
    target_link_libraries(_hypergl_c PRIVATE opengl32 user32 gdi32)
    
    # Mirroring your setup.py macro definitions
    target_compile_definitions(_hypergl_c PRIVATE 
        Py_GIL_DISABLED=1
        HYPERGL_VERSION="${PROJECT_VERSION}"
        ENABLE_VALIDATION=$<IF:$<CONFIG:Debug>,1,0>
    )

    # Version-aware library handling
    # Prevents linker from searching for the standard lib when the threaded lib is needed
    set(PY_VER_NODOT "${Python_VERSION_MAJOR}${Python_VERSION_MINOR}")
    target_link_options(_hypergl_c PRIVATE 
        "LINKER:/NODEFAULTLIB:python${PY_VER_NODOT}.lib"
        "LINKER:/MACHINE:X64"
    )

    # --- Optimization and LTO for Clang ---
    if(CMAKE_C_COMPILER_ID MATCHES "Clang")
        # Compiler flags
        target_compile_options(_hypergl_c PRIVATE /clang:-O3 /clang:-flto)
        
        # Linker flags: Remove /clang:-flto here. 
        # lld-link handles the LTO bitcode automatically if /LTCG is present
        target_link_options(_hypergl_c PRIVATE /LTCG) 
    else()
        target_compile_options(_hypergl_c PRIVATE /O2)
    endif()

# 2. macOS Logic
elseif(APPLE)
    target_compile_options(_hypergl_c PRIVATE -Wno-writable-strings)
    target_link_libraries(_hypergl_c PRIVATE "-framework OpenGL" "-framework CoreFoundation")

# 3. Linux Logic
else()
    target_compile_options(_hypergl_c PRIVATE -O3 -fPIC)
endif()

# --- Installation ---
install(TARGETS _hypergl_c DESTINATION .)