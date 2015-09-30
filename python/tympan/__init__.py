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

