# hypergl/__init__.py
from . import _hypergl_c
from ._hypergl_c import *

# Module Level Functions
context = _hypergl_c.context
init = _hypergl_c.init
cleanup = getattr(_hypergl_c, 'cleanup', None)
inspect = getattr(_hypergl_c, 'inspect', None)
camera = getattr(_hypergl_c, 'camera', None)

# Shared Logic & Helpers
bind = getattr(_hypergl_c, 'bind', None)
calcsize = getattr(_hypergl_c, 'calcsize', None)
loader = getattr(_hypergl_c, 'loader', None)

# Class Types (C-Extension)
Context = getattr(_hypergl_c, 'Context', None)
Buffer = getattr(_hypergl_c, 'Buffer', None)
Image = getattr(_hypergl_c, 'Image', None)
Pipeline = getattr(_hypergl_c, 'Pipeline', None)
Compute = getattr(_hypergl_c, 'Compute', None)
DescriptorSet = getattr(_hypergl_c, 'DescriptorSet', None)
Fence = getattr(_hypergl_c, 'Fence', None)
CommandBuffer = getattr(_hypergl_c, 'CommandBuffer', None)

# Exception
Error = getattr(_hypergl_c, 'Error', None)

# Export Constants and Compiler constructs from _hypergl.py
from ._hypergl import (
    ALREADY_SIGNALED, 
    TIMEOUT_EXPIRED, 
    CONDITION_SATISFIED, 
    WAIT_FAILED,
    TIMEOUT_IGNORED, 
    SYNC_GPU_COMMANDS_COMPLETE, 
    SYNC_FLUSH_COMMANDS_BIT,
    SceneCompiler,
    subroutine
)

from .hgl_ir import HGLCompiler

__all__ = [
    'Error',
    'init', 'cleanup', 'context', 'loader', 'inspect',
    'Context', 'Buffer', 'Image', 'Pipeline', 'Compute', 'DescriptorSet', 'Fence', 'CommandBuffer',
    'bind', 'camera', 'calcsize',
    'SceneCompiler', 'subroutine', 'HGLCompiler',
    # Constants
    'ALREADY_SIGNALED', 'TIMEOUT_EXPIRED', 'CONDITION_SATISFIED', 'WAIT_FAILED',
    'TIMEOUT_IGNORED', 'SYNC_GPU_COMMANDS_COMPLETE', 'SYNC_FLUSH_COMMANDS_BIT'
]