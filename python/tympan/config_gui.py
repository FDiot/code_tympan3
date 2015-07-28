import ConfigParser
from Tkinter import Label, Entry, Frame, Tk, Button
import ttk
import tkFont


class ConfigWidget(Frame):
    """Tabbed widget allowing to handle a configuration"""

    def __init__(self, config, **kwargs):
        """Create config widget

        `config` should have the following structure:
            {category: {param1: {'type': type,
                                 'value': initial value,
                                 'help': information about param1},
                        param2: {....}}}
        """
        Frame.__init__(self, kwargs)
        self.config = config
        self.master.title('Solver parameters')

    def display_config(self):
        """Display the tabbed frame (one tab per category -- `self.config` keys, and one text field
        per parameter)
        """
        # Tabbed frame with the parameters organized by category
        notebook = ttk.Notebook(self.master)
        help_font = tkFont.Font(slant='italic', size=10)
        entry_variables = {}
        for tab, options in self.config.items():
            entry_variables[tab] = {}
            tab_frame = Frame(notebook)
            notebook.add(tab_frame, text=tab)
            for row, option in enumerate(options):
                Label(tab_frame, text=option).grid(row=row)
                param = Entry(tab_frame)
                param.grid(row=row, column=1)
                help_msg = ''
                if 'help' in options[option]:
                    help_msg = options[option]['help'] + ' '
                help_msg += '(type: %s)' % options[option]['type']
                Label(tab_frame, text=help_msg, font=help_font).grid(row=row, column=2)
        notebook.pack(expand=True, fill='both')
        # Buttons
        buttons_frame = Frame(self.master)
        self.cancel = Button(buttons_frame)
        self.cancel['text'] = 'Annuler'
        self.cancel['command'] = self.quit
        self.cancel.grid(row=0, column=0)
        buttons_frame.pack()


def _update_config_with_user_values(config, user_config_parser):
    """Update `config`: add a 'value' key to the param dicts, containing the value read from
    the user config parser `user_config_parser`

    If user config is erroneous or absent, use default value instead.
    """
    converters = {
        'double': user_config_parser.getfloat,
        'float': user_config_parser.getfloat,
        'bool': user_config_parser.getboolean,
        'int': user_config_parser.getint
    }
    for section, options in config.items():
        for option in options:
            try:
                value = converters[options[option]['type']](section, option)
            except (ValueError, ConfigParser.NoSectionError, ConfigParser.NoOptionError):
                options[option]['value'] = options[option]['default']
            else:
                options[option]['value'] = value
