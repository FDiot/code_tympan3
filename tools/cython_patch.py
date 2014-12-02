# -*- coding: utf-8 -*-

import sys, shutil

SUBSITUTIONS = {
    '#include "pyconfig.h"' : '#include "tympan_wrap_pyconfig.h"\n',
    '#include "Python.h"' : '#include "tympan_wrap_python.h"\n',
}

def main(args):
    if len(args) != 3:
        sys.stderr.write("Usage:\t cythonpatch.py <orig_file> <output_file>")
        return -1
    source_fname = args[1]
    output_fname = args[2]
    with open(source_fname) as source, open(output_fname, 'w') as output:
        for line in source:
            stripped_line = line.strip()
            if stripped_line in SUBSITUTIONS:
                out_line = SUBSITUTIONS[stripped_line]
            else :
                out_line = line
            output.write(out_line)

if __name__ == "__main__":
    sys.exit(main(sys.argv))
