# hypergl/__init__.py
from . import _hypergl_c

context = _hypergl_c.context
init = _hypergl_c.init

bind = getattr(_hypergl_c, 'bind', None)
camera = getattr(_hypergl_c, 'camera', None)
calcsize = getattr(_hypergl_c, 'calcsize', None)
loader = getattr(_hypergl_c, 'loader', None)
cleanup = getattr(_hypergl_c, 'cleanup', None)
inspect = getattr(_hypergl_c, 'inspect', None)

Context = getattr(_hypergl_c, 'Context', None)
Buffer = getattr(_hypergl_c, 'Buffer', None)
Image = getattr(_hypergl_c, 'Image', None)
Pipeline = getattr(_hypergl_c, 'Pipeline', None)
Compute = getattr(_hypergl_c, 'Compute', None)

__all__ = [
    'init', 'cleanup', 'context', 'loader', 'inspect',
    'Context', 'Buffer', 'Image', 'Pipeline', 'Compute',
    'bind', 'camera', 'calcsize'
]