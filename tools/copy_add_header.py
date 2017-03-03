"""Create an auto-generated header file
"""
import sys
import argparse
import fileinput
from shutil import copyfile

parser = argparse.ArgumentParser()
parser.add_argument('--root-tympan', '-r',
                    help='path to Tympan root sources')
parser.add_argument('--input-filepath', '-i',
                    help='path to the input file')
parser.add_argument('--output-filepath', '-o',
                    help='path to the output file')
args = parser.parse_args()
# Copy the file to the destination
copyfile(args.input_filepath, args.output_filepath)
# Add 
header = "/* WARNING: I am an auto-generated header file, don't modify me !! */\n/* Modify "+args.input_filepath.replace(args.root_tympan+"/","")+" instead */\n"
for line in fileinput.input(files=args.output_filepath, inplace=True):
    if fileinput.isfirstline():
        sys.stdout.write(header)
    sys.stdout.write(line)
