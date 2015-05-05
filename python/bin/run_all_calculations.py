# coding: utf-8
"""How to run all the calculations xml file"""
from __future__ import print_function
#
import argparse
import os
import sys
from tympan.models.project import Project
from tympan.models.solver import Model, Solver
from tympan.altimetry import AltimetryMesh


def load_tympan_xml(tympan_xml):
    '''
    Return data read from Code_Tympan XML file and built using Code_Tympan
    data model and altimetry
    tympan_xml: filepath to xml tympan project file
    '''
    print('Building Tympan model from %s' % tympan_xml)
    project = Project.from_xml(tympan_xml, verbose=False)

    return project


def run_calculations(project):
    ''' Run all calculations
    '''
    ty_solverdir = os.environ.get('TYMPAN_SOLVERDIR')
    if not ty_solverdir:
        raise RuntimeError(
            'Please set the TYMPAN_SOLVERDIR environment variable')

    for calc in project.computations:
        print('Select calculation:', calc.name)
        project.set_current_computation(calc)

        print('Update altimetry')
        altim = AltimetryMesh.from_site(project.site)
        project.update_site_altimetry(altim)

        print('Get Solver object from Tympan project located at %s' % ty_solverdir)
        solver = Solver.from_project(project, ty_solverdir)

        print('Build model')
        model = Model.from_project( project, set_sources=True,
                                    set_receptors=True )
        print('Launch solver')
        result = solver.solve(model)

        # print('Save results')
        # Import results to project
        project.import_result(model, result)

        # result must be destroyed otherwise it crashes
        del result


def main(tympan_xml, debug=0):
    '''
    load & run
    '''
    process_id = None
    if debug != 0:
        process_id = os.getpid()
        print ("bin/solve_tympan_project.py : l'application est prete pour le debuggage. Entrez 'run' une fois que "
               "ce processus python (id: %d) aura ete rattache au debugger.\n"
               "Attention, ce mode de debuggage n'est disponible que si "
               "solve_tympan_project.py est appele directement depuis une console interactive." % process_id)
        import pdb
        pdb.set_trace()

    # Load project
    project = load_tympan_xml(tympan_xml)

    # Execute all computations
    run_calculations(project)

    # Save project to a temp xml file
    print('Save project to new project file named tempall.xml')
    project.to_xml('tempall.xml')
    
    #  To avoid console output due to the loading of the xml  file (verbose=False not functional)
    sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 0)
    sys.exit('End of calculation(s) ') 


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Load  Code_TYMPAN XML file ')
    parser.add_argument(
        'tympan_xml_file',
        help='the Code_TYMPAN XML file')
    parser.add_argument('--debug',metavar='D', default=0, type=int, help='is debugging on ? D=1 : Yes / D=0 otherwise (default)')
    args = parser.parse_args()
    main(args.tympan_xml_file, args.debug)
