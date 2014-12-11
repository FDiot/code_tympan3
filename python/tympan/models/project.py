from tympan.models._business import Project as cyProject

class Project(object):
    """Acoustic project describing an industrial site"""

    cyclass = cyProject

    def __init__(self, project):
        """Build a project from a project cython object)"""
        self._project = project

    def __getattr__(self, name):
        return getattr(self._project, name)

    @classmethod
    def from_xml(cls, fpath):
        """Create a project from a XML file path"""
        cyproject = cls.cyclass.from_xml(fpath)
        return cls(cyproject)

    def import_result(self, model, solver_result):
        """Update project's site acoustic according to solver result"""
        model._converter.postprocessing(model._model, solver_result)
