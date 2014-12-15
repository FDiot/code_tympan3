"""Generate solver.pxd/solver.pyx from solver.pxd.in/solver.pyx.in and by adding
generated code to handle SolverConfiguration class
"""
from tympan import SOLVER_CONFIG_ATTRIBUTES

with open('_solver.pxd', 'w') as output_stream:
    output_stream.write('''"""THIS FILE IS GENERATED, DON'T EDIT IT"""
''')
    with open('_solver.pxd.in') as input_stream:
        output_stream.write(input_stream.read())
    output_stream.write('''
cdef extern from "Tympan/models/solver/config.h" namespace "tympan":
    cdef cppclass SolverConfiguration:
''')
    for attrtype, attrname in SOLVER_CONFIG_ATTRIBUTES:
        output_stream.write('        %s %s\n' % (attrtype, attrname))


with open('_solver.pyx', 'w') as output_stream:
    output_stream.write('''"""THIS FILE IS GENERATED, DON'T EDIT IT"""
''')
    with open('_solver.pyx.in') as input_stream:
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
    for attrtype, attrname in SOLVER_CONFIG_ATTRIBUTES:
        output_stream.write('''
    def get%(attr)s(self):
        return self.thisptr.getRealPointer().%(attr)s
    def set%(attr)s(self, value):
        self.thisptr.getRealPointer().%(attr)s = value
    %(attr)s = property(get%(attr)s, set%(attr)s)''' % {'attr': attrname})
