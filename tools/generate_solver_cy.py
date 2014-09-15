"""Generate solver.pxd/solver.pyx from solver.pxd.in/solver.pyx.in and by adding
generated code to handle SolverConfiguration class
"""

SOLVER_CONFIG_ATTRIBUTES = [
        ('double', 'AtmosPressure'),
        ('double', 'AtmosTemperature'),
        ('double', 'AtmosHygrometry'),
        ('double', 'AnalyticC0'),
        ('double', 'WindDirection'),
        ('double', 'AnalyticGradC'),
        ('double', 'AnalyticGradV'),

        ('int', 'RayTracingOrder'),
        ('int', 'Discretization'),
        ('int', 'NbRaysPerSource'),
        ('float', 'MaxLength'),
        ('float', 'SizeReceiver'),
        ('int', 'Accelerator'),
        ('int', 'MaxTreeDepth'),
        ('float', 'AngleDiffMin'),
        ('float', 'CylindreThick'),
        ('int', 'MaxProfondeur'),
        ('bool', 'UseSol'),
        ('int', 'MaxReflexion'),
        ('int', 'MaxDiffraction'),
        ('bool', 'DiffractionUseRandomSampler'),
        ('int', 'NbRayWithDiffraction'),
        ('bool', 'DiffractionDropDownNbRays'),
        ('bool', 'DiffractionFilterRayAtCreation'),
        ('bool', 'UsePathDifValidation'),
        ('float', 'MaxPathDifference'),
        ('bool', 'DiffractionUseDistanceAsFilter'),
        ('bool', 'KeepDebugRay'),
        ('bool', 'UsePostFilters'),

        ('bool', 'EnableFullTargets'),
        ('float', 'TargetsDensity'),

        ('float', 'InitialAngleTheta'),
        ('float', 'InitialAnglePhi'),
        ('int', 'AnalyticNbRay'),
        ('double', 'AnalyticTMax'),
        ('double', 'AnalyticH'),
        ('double', 'AnalyticDMax'),

        ('int', 'AnalyticTypeTransfo'),

        ('int', 'NbThreads'),
        ('bool', 'UseRealGround'),
        ('bool', 'UseLateralDiffraction'),
        ('bool', 'UseReflection'),
        ('bool', 'PropaConditions'),
        ('float', 'H1parameter'),
        ('bool', 'ModSummation'),

        ('bool', 'UseMeteo'),
        ('float', 'OverSampleD'),
        ('bool', 'UseFresnelArea'),
        ('float', 'Anime3DSigma'),
        ('int', 'Anime3DForceC'),

        ('bool', 'DebugUseCloseEventSelector'),
        ('bool', 'DebugUseDiffractionAngleSelector'),
        ('bool', 'DebugUseDiffractionPathSelector'),
        ('bool', 'DebugUseFermatSelector'),
        ('bool', 'DebugUseFaceSelector'),
]

with open('solver.pxd', 'w') as output_stream:
    output_stream.write('''"""THIS FILE IS GENERATED, DON'T EDIT IT"""
''')
    with open('solver.pxd.in') as input_stream:
        output_stream.write(input_stream.read())
    output_stream.write('''
cdef extern from "Tympan/models/solver/config.h" namespace "tympan":
    cdef cppclass SolverConfiguration:
''')
    for attrtype, attrname in SOLVER_CONFIG_ATTRIBUTES:
        output_stream.write('        %s %s\n' % (attrtype, attrname))


with open('solver.pyx', 'w') as output_stream:
    output_stream.write('''"""THIS FILE IS GENERATED, DON'T EDIT IT"""
''')
    with open('solver.pyx.in') as input_stream:
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
