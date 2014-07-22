import sys
import logging

# open file in unbuffered mode so it get written asap, in case of later crash
# due to underlying C code
stream = open('tympan.log', 'a', 0)
logging.basicConfig(stream=stream, level=logging.DEBUG,
                    format='%(levelname)s:%(asctime)s - %(name)s - %(message)s')

try:
    import tympan.solve_project as tysolve
except ImportError:
    err = "bin/solve_tympan_project.py script couldn't find tympan.solve_project module"
    logging.critical("%s Check PYTHONPATH and path to Tympan libraries.", err)
    raise ImportError(err)


if __name__ == '__main__':
    if len(sys.argv) != 4:
        err = "bin/solve_tympan_project.py called with bad arguments"
        logging.error("%s Couldn't solve acoustic problem.", err)
        stream.close()
        sys.exit(-1) # XXX to be improved
    # read command-line arguments
    input_proj = sys.argv[1]
    output_proj = sys.argv[2]
    solverdir = sys.argv[3]
    # solve problem
    try:
        tysolve.solve(input_project=input_proj,
                      output_project=output_proj,
                      solverdir=solverdir)
    except:
        logging.exception("tympan.solve_project.py module couldn't solve the acoustic problem")
        stream.close()
        sys.exit(-1)
    stream.close()
    sys.exit(0)
