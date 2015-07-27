import argparse
import ConfigParser
import json

from tympan.config_gui import ConfigWidget, _update_config_with_user_values

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--data-model-filepath', '-m',
                        help='path to the json dump containing the solver parameters data model')
    parser.add_argument('--input-filepath', '-i',
                        help='path to the input file containing solver original parameters')
    args = parser.parse_args()
    with open(args.data_model_filepath) as stream:
        config_model = json.load(stream)
    user_config = ConfigParser.ConfigParser()
    user_config.optionxform = str  # keep param names case
    with open(args.input_filepath) as stream:
        user_config.readfp(stream)
    _update_config_with_user_values(config_model, user_config)
    widget = ConfigWidget(config_model)
    widget.display_config()
    widget.mainloop()
