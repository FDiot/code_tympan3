#!/usr/bin/env python
"""usage: ./tools/find_unused_file.py  Tympan/

(working directory matters)
"""
import sys
import os
from os.path import join, basename

h_files = set()
included_files = set()

for directory, dirs, files in os.walk(sys.argv[1]):
    for fname in files:
        if fname.endswith(('.h', '.hpp')):
            h_files.add(join(directory, fname))
            #print 'H file', join(directory, fname)
        if fname.endswith(('.h', '.hpp', '.cpp')):
            include_h = False
            for line in open(join(directory, fname)):
                line = line.strip()
                if line.startswith('#include "'):
                    included = line.split('"')[1]
                    if fname.split('.', 1)[0] != included.split('.', 1)[0]:
                        included_files.add(included)
                        #print 'include', included
                    else:
                        include_h = True
            if fname.endswith('.cpp') and not include_h:
                print "file don't include its header file", join(directory, fname)

not_found = set()
for file_path in included_files:
    try:
        h_files.remove(file_path)
    except KeyError:
        not_found.add(file_path)

for file_path in tuple(h_files):
    if basename(file_path) in not_found:
        h_files.remove(file_path)

for file_path in sorted(h_files):
    print file_path, 'seems not used'
