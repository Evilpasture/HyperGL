# Window System Integration

HyperGL does not create windows (unless in headless mode). It relies on you to provide an OpenGL Context and a "Loader" function to find OpenGL commands.

Here is how to set up the major windowing libraries.

## 1. GLFW (Recommended)
The industry standard for learning and simple apps.

```python
import glfw
import hypergl

class GLFWLoader:
    def load_opengl_function(self, name):
        # GLFW returns a C-pointer (int/long)
        return glfw.get_proc_address(name)

def main():
    glfw.init()
    # Request OpenGL 4.5+ Core
    glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 4)
    glfw.window_hint(glfw.CONTEXT_VERSION_MINOR, 5)
    glfw.window_hint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE)
    
    window = glfw.create_window(800, 600, "HyperGL", None, None)
    glfw.make_context_current(window)
    
    # Initialize HyperGL with the loader
    hypergl.init(GLFWLoader())
    ctx = hypergl.context()
```

## 2. SDL2 (PySDL2)
Great for shipping games, handles audio/gamepads well.

```python
from sdl2 import *
import hypergl
import ctypes

class SDLLoader:
    def load_opengl_function(self, name):
        # SDL_GL_GetProcAddress returns a c_void_p
        ptr = SDL_GL_GetProcAddress(name.encode())
        return ctypes.cast(ptr, ctypes.c_void_p).value

def main():
    SDL_Init(SDL_INIT_VIDEO)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5)
    
    window = SDL_CreateWindow(b"HyperGL", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        800, 600, SDL_WINDOW_OPENGL)
        
    gl_ctx = SDL_GL_CreateContext(window)
    SDL_GL_MakeCurrent(window, gl_ctx)
    
    hypergl.init(SDLLoader())
```

## 3. PyGame (SDL2 backend)
Popular for beginners.

```python
import pygame
import hypergl

class PyGameLoader:
    def load_opengl_function(self, name):
        # PyGame wrappers often hide the raw pointer, careful here.
        # It's usually safer to use ctypes with SDL2 directly if PyGame fails.
        return 0 # PyGame doesn't expose a clean GetProcAddress!

# NOTE: PyGame Integration is tricky because it doesn't expose 
# a standard "GetProcAddress" Python API. 
# RECOMMENDED: Use `moderngl-window` or raw `SDL2`.
```

## 4. ModernGL-Window (`mglw`)
The easiest way to get up and running. HyperGL can coexist with `moderngl-window` by hijacking the context.

```python
import moderngl_window as mglw
import hypergl

class App(mglw.WindowConfig):
    gl_version = (4, 5)
    
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        
        # Define a loader that proxies mglw
        # mglw usually loads GL functions automatically, but HyperGL needs its own pointers
        # We can reuse GLFW loader logic if mglw uses GLFW backend.
        
        class MGLWLoader:
            def load_opengl_function(self, name):
                # Assumes GLFW backend
                import glfw 
                return glfw.get_proc_address(name)

        hypergl.init(MGLWLoader())
        self.ctx = hypergl.context()

    def render(self, time, frame_time):
        self.ctx.new_frame()
        # ...
        self.ctx.end_frame()

mglw.run_window_config(App)
```

## 5. PyQt6 / PySide6
For GUI Tools.

```python
from PyQt6.QtOpenGLWidgets import QOpenGLWidget
from PyQt6.QtGui import QSurfaceFormat
import hypergl

class HyperGLWidget(QOpenGLWidget):
    def initializeGL(self):
        # Qt's loader
        class QtLoader:
            def __init__(self, context):
                self.ctx = context
            def load_opengl_function(self, name):
                # getProcAddress returns a function pointer object
                func = self.ctx.getProcAddress(name)
                return int(func) if func else 0

        hypergl.init(QtLoader(self.context()))
        self.ctx = hypergl.context()

    def paintGL(self):
        self.ctx.new_frame()
        # Draw...
        self.ctx.end_frame()
```

## 6. Custom / OS Native
If you are doing something wild (like embedding in a C++ app or using raw Win32 API), you can write a `ctypes` loader.

### Windows (ctypes)
```python
import ctypes
from ctypes import windll

class Win32Loader:
    def load_opengl_function(self, name):
        # 1. Try WGL
        addr = windll.opengl32.wglGetProcAddress(name.encode())
        if addr: return addr
        # 2. Try Core DLL
        return ctypes.cast(getattr(windll.opengl32, name, 0), ctypes.c_void_p).value
```