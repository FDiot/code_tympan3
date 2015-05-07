import sys
import logging
import os

# open file in unbuffered mode so it get written asap, in case of later crash
# due to underlying C code
stream = open('tympan.log', 'a', 0)
logging.basicConfig(stream=stream, level=logging.DEBUG,
                    format='%(levelname)s:%(asctime)s - %(name)s - %(message)s')

multithreading_on = True
interactive=False
tympan_debug = os.environ.get('TYMPAN_DEBUG')
if tympan_debug is not None:
    if 'monothread' in tympan_debug.lower():
        print("bin/solve_tympan_project.py : le multithreading est desactive pour ce calcul.")
        logging.debug("Multithreading will be disabled during this computation")
        multithreading_on = False
    if 'interactive' in tympan_debug.lower():
        process_id = os.getpid()
        print ("bin/solve_tympan_project.py : l'application est prete pour le debuggage. Entrez 'run' une fois que "
               "ce processus python (id: %d) aura ete rattache au debugger.\n"
               "Attention, ce mode de debuggage n'est disponible que si "
               "solve_tympan_project.py est appele directement depuis une console interactive." % process_id)
        logging.debug("Running solve_project.py in debug mode...")
        interactive =True

try:
    import tympan.solve_project as tysolve
except ImportError:
    err = "bin/solve_tympan_project.py couldn't find tympan.solve_project module."
    logging.critical("%s Check PYTHONPATH and path to Tympan libraries.", err)
    sys.stderr.write('Error: ' + err + '. Check PYTHONPATH and path to Tympan libraries.')


if __name__ == '__main__':
    if len(sys.argv) != 5:
        err = "bin/solve_tympan_project.py called with bad arguments"
        logging.error("%s Couldn't solve acoustic problem.", err)
        stream.close()
        sys.exit(-1) # XXX to be improved
    # read command-line arguments
    input_proj = sys.argv[1]
    output_proj = sys.argv[2]
    output_mesh = sys.argv[3]
    solverdir = sys.argv[4]
    # solve problem
    try:
        tysolve.solve(input_project=input_proj,
                      output_project=output_proj,
                      output_mesh=output_mesh,
                      solverdir=solverdir,
                      multithreading_on=multithreading_on,
                      interactive=interactive, verbose=True)
    except Exception as exc:
        sys.stderr.write('Error: ' + exc.message)
        logging.exception("tympan.solve_project.py couldn't solve the acoustic problem:\n%s", exc)
        stream.close()
        sys.exit(-1)
    stream.close()
    sys.exit(0)
