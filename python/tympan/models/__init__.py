import abc
import sys
import os
from contextlib import contextmanager

from ._common import (
    Spectrum,
)


class Infrastructure(metaclass=abc.ABCMeta):
    """Infrastructure"""

    def __init__(self):
        pass

    @abc.abstractproperty
    def name(self):
        """Name of infrastructure"""

    @abc.abstractproperty
    def is_emitting(self):
        """Emissivity of infrastructure"""

    @abc.abstractmethod
    def export_mesh(self):
        """Return points, triangles"""

    @abc.abstractproperty
    def material_by_face(self):
        """Return a material_by_face mapping"""


class Building(Infrastructure):
    """Building Infrastructure"""


class Screen(Infrastructure):
    """Screen Infrastructure"""


class Machine(Infrastructure):
    """Machine Infrastructure"""


@contextmanager
def filter_output(verbose=False, to=os.devnull, err_to=None):
    if verbose:
        yield
        return
    if err_to is None:
        err_to = to
    with _stdout_redirected(to=to, stdout=sys.stdout):
        with _stdout_redirected(to=err_to, stdout=sys.stderr):
            yield

# low level output redirection, see
# http://stackoverflow.com/questions/4675728/redirect-stdout-to-a-file-in-python/22434262#22434262

def _fileno(file_or_fd):
    fd = getattr(file_or_fd, 'fileno', lambda: file_or_fd)()
    if not isinstance(fd, int):
        raise ValueError("Expected a file (`.fileno()`) or a file descriptor")
    return fd

@contextmanager
def _stdout_redirected(to=os.devnull, stdout=None):
    if stdout is None:
        stdout = sys.stdout

    stdout_fd = _fileno(stdout)
    # copy stdout_fd before it is overwritten
    with os.fdopen(os.dup(stdout_fd), 'wb') as copied:
        stdout.flush()  # flush library buffers that dup2 knows nothing about
        try:
            os.dup2(_fileno(to), stdout_fd)  # $ exec >&to
        except ValueError:  # filename
            with open(to, 'ab') as to_file:
                os.dup2(to_file.fileno(), stdout_fd)  # $ exec > to
        try:
            yield stdout # allow code to be run with the redirected stdout
        finally:
            stdout.flush()
            os.dup2(copied.fileno(), stdout_fd)  # $ exec >&copied

