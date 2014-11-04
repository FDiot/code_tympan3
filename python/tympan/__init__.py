from ._simulation import Simulation

# solver parameters description
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


