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

    # Build a project from a site XML description (update site infrastructure altimetry)
    project = Project.from_xml("my_project.xml")
    # Build a simplified model from the project (the new infrastructure
    # altitudes will be taken into account)
    model = Model.from_project(project)
    # Load and configure the acoustic solver
    solver = Solver.from_project(project)
    # Run the simulation
    result = solver.solve(model)
    # Update the project with the results of the computation
    project.import_result(model, result)

This API can be used to do other types of computations, here goes an example of how to do a computation
with user-defined sources:

.. code-block:: python

    # Build a project from a XML model
    project = Project.from_xml("my_project.xml")
    # build solver model, but don't use the project sources
    model = Model.from_project(project, set_sources=False)
    # Manually define the sources (position and spectrum) depending on your needs
    model.add_source(Source((0, 0, 0), np.array([100.0] * 31, dtype=float)))
    model.add_source(Source((100, 50, 0), np.array([150.0] * 31, dtype=float)))
    solver = Solver.from_project(project)
    result = solver.solve(model)
    # retrieve combined spectra per receptor
    combined_spectra = result.combined_spectra()
"""

