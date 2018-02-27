"""
Script for launching a Tympan calculation, taking as input parameters:
- an XML Tympan model
- a CSV road file or a directory that contains one or several CSV road files
- the target directory
"""

import argparse
import numpy as np
import os
from pathlib import Path

from tympan.altimetry import (
    builder,
    datamodel,
    export_to_ply,
)
from tympan.models.project import Project
from tympan.models.solver import (
    Model,
    Solver,
)


def parse_args():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("project", help="xml project file")
    parser.add_argument("road", help="csv road file or directory"
                        " that contains csv road files")
    parser.add_argument("-t", "--target-dir", help="target repository",
                        default=os.getcwd())
    args = parser.parse_args()
    # check arguments
    project_path = Path(args.project)
    if not project_path.exists():
        raise IOError("Project {} doesn't exists".format(project_path))
    road_path = Path(args.road)
    if not road_path.exists():
        raise IOError("Road(s) {} doesn't exists".format(road_path))
    if road_path.is_dir():
        road_files = road_path.glob('*.csv')
    else:
        road_files = [road_path]
    target_dir = Path(args.target_dir)
    if not target_dir.exists():
        raise IOError("Directory {} doesn't exists".format(target_dir))
    return project_path, road_files, target_dir


if __name__ == '__main__':
    # parse arguments
    project_file, road_files, target_dir = parse_args()
    # load project
    project = Project.from_xml(str(project_file))
    # build site
    asite = builder.build_sitenode(project.site)
    # create and add road(s)
    for road_file in road_files:
        road = datamodel.Road(
            datamodel.load_csv_road(str(road_file)),
            id=road_file.name,
            parent_site=asite,
        )

    # build altimetry
    merged_site, mesh, feature_by_face = builder.build_altimetry(asite)

    # update project site
    material_by_face = builder.material_by_face(feature_by_face)
    project.site.update_altimetry(mesh, material_by_face)

    # save mesh (PLY)
    export_to_ply(mesh, material_by_face, str(target_dir.joinpath("mesh.ply")))

    # call solver
    solver = Solver.from_project(project)
    model = Model.from_project(project)
    result = solver.solve(model)

    # save results
    np.savetxt(target_dir.joinpath("results.txt"), result.spectrum(0, 0).values)
    # save project
    project.import_result(model, result)
    project.to_xml(str(target_dir.joinpath("results.xml")))
