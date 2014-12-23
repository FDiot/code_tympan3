"""Code_TYMPAN: acoustic simulations based on C++ libraries

Allows to:

- Build a project from a XML file describing an industrial site
- Compute and export the altimetry from the topography and infrastructures of the site
- Create an abstract model from the "business" description, describing the
  topography as a mesh, and with elementary sources and receptors
- Run an acoustic simulation on an abstract model
- Update the business model with the results of the altimetry computation or
  with the result of the acoustic computation

Here goes an example of the typical use that can be done of this API:

.. code-block:: python

    # Build a project from a site XML description
    project = Project.from_xml("my_project.xml")
    # Compute altimetry from the site
    altimetry_mesh = AltimetryMesh.from_site(project.site)
    # Update the site infrastructure
    project.update_site_altimetry(altimetry_mesh)
    # Build a simplified model from the project (the new infrastructure
    # altitudes will be taken into account)
    model = Model.from_project(project)
    # Load and configure the acoustic solver
    solver = Solver.from_project(project, solverdir)
    # Run the simulation
    result = solver.solve(model)
    # Update the project with the results of the computation
    project.import_result(model, result)

"""

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


