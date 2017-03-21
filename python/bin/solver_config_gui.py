import argparse
import configparser
import json

from tympan.config_gui import ConfigWidget, _update_config_with_user_values, _update_user_config

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--data-model-filepath', '-m',
                        help='path to the json dump containing the solver parameters data model')
    parser.add_argument('--input-filepath', '-i',
                        help='path to the input file containing solver original parameters')
    parser.add_argument('--output-filepath', '-o',
                        help='path to the output file where to write the updated parameters')
    args = parser.parse_args()
    with open(args.data_model_filepath) as stream:
        config_model = json.load(stream)
    user_config = configparser.ConfigParser()
    user_config.optionxform = str  # keep param names case
    with open(args.input_filepath) as stream:
        user_config.readfp(stream)
    _update_config_with_user_values(config_model, user_config)
    widget = ConfigWidget(config_model)
    widget.display_config()
    widget.mainloop()
    _update_user_config(widget.config, user_config)
    with open(args.output_filepath, 'w') as stream:
        user_config.write(stream)
