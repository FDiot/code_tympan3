"""Generate a c++ header file defining a char* that contains the solver parameters original
definition (1 line = "param name=value").
"""
import json
import os
import argparse


def _get_solver_config(solver_config):
    """Generate ini formatted lines from json `solver_config` dict"""
    for category, options in sorted(solver_config.items()):
        yield r'"[%s]\n"' % category
        for option in sorted(options):
            yield r'"%s=%s\n"' % (option, options[option]['default'])


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--params-filepath', '-p',
                        help='path to the json dump containing the solver parameters data model')
    parser.add_argument('--output-filepath', '-o',
                        help='path to the output file where to write header content')
    args = parser.parse_args()
    with open(args.params_filepath) as stream:
        solver_config = json.load(stream)
    with open(args.output_filepath, 'w') as stream:
        stream.write(os.linesep.join([
            "/* WARNING: I am an auto-generated header file, don't modify me !! */",
            '',
            '#ifndef TY_MODELS_BUSINESS_DEFAULT_SOLVER_CONFIG',
            '#define TY_MODELS_BUSINESS_DEFAULT_SOLVER_CONFIG',
            'const char * DEFAULT_SOLVER_CONFIG =']))
        for config_line in _get_solver_config(solver_config):
            stream.write(config_line + os.linesep)
        stream.write(';%s#endif' % os.linesep)
