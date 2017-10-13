"""Generate solver.pxd/solver.pyx from solver.pxd.in/solver.pyx.in and by adding
generated code to handle SolverConfiguration class
"""

import json
import os
from os import path


ROOT_DIR = path.join(path.dirname(path.abspath(__file__)), os.pardir)

_CONFIG_MODEL_FILE = path.join(ROOT_DIR,
                               'resources', 'solver_config_datamodel.json')
with open(_CONFIG_MODEL_FILE) as stream:
    _CONFIG_MODEL = json.load(stream)
_SOLVER_CONFIG_ATTRIBUTES = []
for options in _CONFIG_MODEL.values():
    for option in options:
        _SOLVER_CONFIG_ATTRIBUTES.append((options[option]['type'], option))

def python_model_file(fname):
    return path.join(ROOT_DIR, 'python', 'tympan', 'models', fname)

with open(python_model_file('_solver.pxd'), 'w') as output_stream:
    output_stream.write('''"""THIS FILE IS GENERATED, DON'T EDIT IT"""
''')
    with open(python_model_file('_solver.pxd.in')) as input_stream:
        output_stream.write(input_stream.read())
    output_stream.write('''
cdef extern from "Tympan/models/solver/config.h" namespace "tympan":
    cdef cppclass SolverConfiguration:
''')
    for attrtype, attrname in _SOLVER_CONFIG_ATTRIBUTES:
        output_stream.write('        %s %s\n' % (attrtype, attrname))


with open(python_model_file('_solver.pyx'), 'w') as output_stream:
    output_stream.write('''"""THIS FILE IS GENERATED, DON'T EDIT IT"""
''')
    with open(python_model_file('_solver.pyx.in')) as input_stream:
        output_stream.write(input_stream.read())
    output_stream.write('''
cdef class Configuration:
    thisptr = cy.declare(SmartPtr[SolverConfiguration])

    @staticmethod
    def get():
        config = Configuration()
        config.thisptr = get()
        return config
''')
    for attrtype, attrname in _SOLVER_CONFIG_ATTRIBUTES:
        output_stream.write('''
    def get%(attr)s(self):
        return self.thisptr.getRealPointer().%(attr)s
    def set%(attr)s(self, value):
        self.thisptr.getRealPointer().%(attr)s = value
    %(attr)s = property(get%(attr)s, set%(attr)s)''' % {'attr': attrname})
