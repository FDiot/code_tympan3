"""Main script for altimetry processing from Code_TYMPAN GUI"""

import sys
import logging

from tympan.models.business import Project
from tympan.altimetry import export_to_ply, builder


def set_logger(fpath='tympan.log'):
    """Configure logging"""
    # open file in unbuffered mode so it get written asap, in case of later crash
    # due to underlying C code
    stream = open(fpath, 'a', 0)
    logging.basicConfig(stream=stream, level=logging.DEBUG,
                        format='%(levelname)s:%(asctime)s - %(name)s - %(message)s')


def main(input_project, result_file):
    """Process altimetry from `input_project` and save to `result_file` (PLY
    format).
    """
    try:
        try:
            project = Project.from_xml(input_project)
        except RuntimeError:
            logging.exception("Couldn't load the acoustic project from %s file", input_project)
            raise
        asite = builder.build_sitenode(project.site)
        _, mesh, feature_by_face = builder.build_altimetry(
            asite, allow_features_outside_mainsite=True)
        material_by_face = builder.material_by_face(feature_by_face)
        export_to_ply(mesh, material_by_face, result_file)
    except Exception as exc:
        sys.stderr.write('Error: %s' % exc)
        logging.exception("Error processing the altimetry:\n%s", exc)


if __name__ == '__main__':
    set_logger()
    if len(sys.argv) != 3:
        err = "%s called with bad arguments." % __file__
        logging.error("%s Couldn't process altimetry.", err)
        sys.stderr.write('Error: ' + err)
        sys.exit(-1) # XXX to be improved
    input_project = sys.argv[1]
    result_file = sys.argv[2]
    main(input_project, result_file)
