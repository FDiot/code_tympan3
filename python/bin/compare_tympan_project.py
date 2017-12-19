# -*- coding: utf-8 -*-
"""
Created on 23 august 2017

@author: P. LEDAC (pierre.ledac@c-s.fr)

Graphic interface to compare Tympan projects.
"""
import sys

if sys.version_info[:2] <= (2, 7):
    print("Error! Python 3 only can be used.")
    sys.exit(-1)
import pandas as pd
import numpy as np
import tkinter as tk
from operator import itemgetter
from tkinter.filedialog import askopenfile
from tkinter.filedialog import asksaveasfile
from run_all_calculations import run_calculations
from tympan.models.project import Project
from tympan.models.solver import Model
from tympan.models._common import Vector3D
from tympan.models._business import Site, elemen2receptor
from _util import MyDialog, list_sites, build_dict, power2db
from xlsxwriter.utility import xl_range, xl_rowcol_to_cell


class App(tk.Tk):
    """
    Class creating main graphic window
    """

    def __init__(self, parent, gui=True):
        default_compare = 2         # Compare results and elements by default
        default_relaunch = 0        # Do not relaunch by default
        default_tolerance = 1.e-2   # Tolerance by default
        default_output_file = "Rapport.xlsx" # Output file by default
        self.gui = gui
        if self.gui:
            tk.Tk.__init__(self, parent)
            self.parent = parent
            self.initialize()
            self.compare.set(default_compare)
            self.relaunch.set(default_relaunch)
            self.tolerance.set(default_tolerance)
            self.output_file.set(default_output_file)
        else:
            self.compare = default_compare
            self.relaunch = default_relaunch
            self.tolerance = default_tolerance
            self.output_file = default_output_file

        self.ExcelWriter = None
        self.project_reference = None
        self.project_compared = None

    def initialize(self):
        # We use a grid to position the widgets
        self.grid

        # ============================================================================================================

        # LabelFrame for section "choose XML file"
        lf_xml = tk.LabelFrame(self, text=u"Sélection des fichiers", padx=0, pady=0)
        lf_xml.grid(column=0, row=0, columnspan=3, sticky='EW')
        lf_xml.grid_columnconfigure(0, weight=1)

        # Labels used to show the xml file path
        self.labelVariable = tk.StringVar()
        label_file = tk.Label(lf_xml, textvariable=self.labelVariable, anchor="w", fg="blue", bg="white")
        label_file.grid(column=0, row=1, columnspan=1, in_=lf_xml, sticky='EW')
        self.labelVariable.set(u"Fichier XML du projet de référence")

        self.label2Variable = tk.StringVar()
        label2_file = tk.Label(lf_xml, textvariable=self.label2Variable, anchor="w", fg="blue", bg="white")
        label2_file.grid(column=0, row=2, columnspan=1, in_=lf_xml, sticky='EW')
        self.label2Variable.set(u"Fichier XML du projet")

        self.output_file = tk.StringVar()
        label3_file = tk.Label(lf_xml, textvariable=self.output_file, anchor="w", fg="blue", bg="white")
        label3_file.grid(column=0, row=3, columnspan=1, in_=lf_xml, sticky='EW')

        # buttons used to charge xml files
        button_charge_xml = tk.Button(lf_xml, text=u"Charger le fichier projet de référence", anchor="e",
                                      command=self.load_reference_xml_file)
        button_charge_xml.grid(column=1, row=1, in_=lf_xml, sticky='E')

        button2_charge_xml = tk.Button(lf_xml, text=u"Charger le fichier projet à comparer", anchor="e",
                                       command=self.load_compared_xml_file)
        button2_charge_xml.grid(column=1, row=2, in_=lf_xml, sticky='E')

        button3_charge_xml = tk.Button(lf_xml, text=u"Nommer le fichier de sortie (rapport)", anchor="e",
                                       command=self.select_output_file)
        button3_charge_xml.grid(column=1, row=3, in_=lf_xml, sticky='E')

        # ============================================================================================================

        # LabelFrame for section "what to compare"
        lf_ope = tk.LabelFrame(self, text=u"Comparer", padx=0, pady=0)
        lf_ope.grid(column=0, row=4, columnspan=1, sticky='EW')
        lf_ope.grid_columnconfigure(0, weight=1)

        # RadioButtons corresponding to the proposed operations
        # initializing choice
        self.compare = tk.IntVar()
        self.operations_name = ['Les modèles', 'Les résultats', 'Les modèles et les résultats']
        operations = [
            (u"" + self.operations_name[0], 0),
            (u"" + self.operations_name[1], 1),
            (u"" + self.operations_name[2], 2)
        ]

        for txt, val in operations:
            tk.Radiobutton(lf_ope,
                           text=txt,
                           padx=20,
                           variable=self.compare,
                           value=val).pack(anchor=tk.W)

        # ============================================================================================================

        # LabelFrame for section "calculation relaunched or not"
        lf_relaunch = tk.LabelFrame(self, text=u"Relancer le calcul pour le projet ?", padx=0, pady=0)
        lf_relaunch.grid(column=1, row=4, columnspan=1, sticky='EWN')
        lf_relaunch.grid_columnconfigure(0, weight=1)

        # RadioButtons corresponding to the proposed choices
        # initializing choice
        self.relaunch = tk.IntVar()

        operations = [
            (u"Non", 0),
            (u"Oui", 1)
        ]

        for txt, val in operations:
            tk.Radiobutton(lf_relaunch,
                           text=txt,
                           padx=20,
                           variable=self.relaunch,
                           value=val).pack(anchor=tk.W)

        # ============================================================================================================

        # LabelFrame for section "Spectre tolerance comparison"
        lf_tolerance = tk.LabelFrame(self, text=u"Tolérance pour la comparaison des spectres", padx=0, pady=0)
        lf_tolerance.grid(column=2, row=4, columnspan=1, sticky='EWN')
        lf_tolerance.grid_columnconfigure(0, weight=1)

        self.tolerance = tk.DoubleVar()
        tk.Entry(lf_tolerance, textvariable=self.tolerance).pack(anchor=tk.W)

        # ============================================================================================================

        # LabelFrame for section "launch comparison"
        lf_calc = tk.LabelFrame(self, text=u"Statut", padx=0, pady=0)
        lf_calc.grid(column=0, row=5, columnspan=3, sticky='EW')
        lf_calc.grid_columnconfigure(0, weight=1)

        # Button pressed to launch comparison
        button_launch_calc = tk.Button(lf_calc, text=u"Comparer les projets", command=self.click_launch_compare)
        button_launch_calc.grid(column=1, row=0, sticky='W')

        # Label progress
        self.labelProg = tk.StringVar()
        self.prog = tk.Label(lf_calc, textvariable=self.labelProg, anchor="w", fg="red", bg="grey")
        self.prog.grid(column=0, row=0, sticky='EW')

        # ============================================================================================================

        # Geometry options of the window
        self.grid_columnconfigure(0, weight=1)
        self.resizable(False, False)

    def select_output_file(self):
        # Open dialog window to choose output file
        output_file = asksaveasfile(mode='w', defaultextension='.xlsx',
                                         title=u"Donner le nom du fichier Excel (nouveau ou existant)",
                                         filetypes=[('fichiers xlsx', '.xlsx'), ('tous les fichiers', '.*')])
        self.output_file.set(output_file.name)

    def load_reference_xml_file(self, xml_file=""):
        """
        Load TYMPAN xml file and set reference project
        """
        if xml_file == "":
            # Open dialog window to choose XML file
            xml_file = askopenfile(title=u"Ouvrir le fichier XML", filetypes=[('fichiers xml', '.xml'), ('tous les fichiers', '.*')]).name
            # Set label variable in principal window
            self.labelVariable.set(xml_file)

        # Set project
        try:
            self.project_reference = Project.from_xml(xml_file)
        except:
            self.labelVariable.set("")
            d = MyDialog(self, u"Erreur, ce n'est pas un projet Code_TYMPAN.")
            self.wait_window(d.top)
            return

    def load_compared_xml_file(self, xml_file=""):
        """
        Load TYMPAN xml file and set compared project
        """
        if xml_file == "":
            # Open dialog window to choose XML file
            xml_file = askopenfile(title=u"Ouvrir le fichier XML", filetypes=[('fichiers xml', '.xml'), ('tous les fichiers', '.*')]).name
            # Set label variable in principal window
            self.label2Variable.set(xml_file)

        # Set project
        try:
            self.project_compared = Project.from_xml(xml_file)
        except:
            self.label2Variable.set("")
            d = MyDialog(self, u"Erreur, ce n'est pas un projet Code_TYMPAN.")
            self.wait_window(d.top)
            return

    def click_launch_compare(self):
        """
        Launch comparison once every parameters are set
        """
        # Checks
        if self.project_reference is None:
            d = MyDialog(self, u"Projet de référence non spécifié!")
            self.wait_window(d.top)
            return
        if self.project_compared is None:
            d = MyDialog(self, u"Projet à comparer non spécifié!")
            self.wait_window(d.top)
            return

        # Set progression label
        self.labelProg.set(u"Attendre...")

        # Launch comparison
        is_same = self.launch_compare()

        # Set progress label to done (Terminé !)
        message = u"Comparaison sauvée dans le fichier " + self.get_output_filename()
        if is_same:
            self.prog.config(fg="green")
        else:
            self.prog.config(fg="red")
        self.labelProg.set(message)

        status = "Aucun écart." if is_same else "Attention, écarts constatés!"
        print(message+"\n"+status)

    def get_output_filename(self):
        return str(self.output_file.get()) if self.gui else self.output_file

    def launch_compare(self):
        """
        Launch comparison once every parameters are set
        """
        relaunch = int(self.relaunch.get()) if self.gui else self.relaunch
        compare = int(self.compare.get()) if self.gui else self.compare
        # Relaunch calculation
        if relaunch == 1:
            if self.compare == 0:
                d = MyDialog(self, u"Calcul relancé mais seule la comparaison de modèles est activée!")
                self.wait_window(d.top)
                return
            print("Calcul du projet relancé...")
            run_calculations(self.project_compared)
            # We write the Tympan project once calculation is relaunched
            self.project_compared.to_xml("relaunched.xml")

        # Open to write the output
        self.ExcelWriter = pd.ExcelWriter(self.get_output_filename(), engine='xlsxwriter')

        # Comparisons
        is_same = True
        if compare != 1:
            is_same = self.compare_elements() and is_same    # Infrastructure elements
            is_same = self.compare_topography() and is_same  # Topography
            is_same = self.compare_model() and is_same       # Model
        if compare != 0:
            is_same = self.compare_solvers() and is_same     # Solver parameters
            is_same = self.compare_results() and is_same     # Results

        # Save and close the ouput
        self.ExcelWriter.save()

        # Return if different or not
        return is_same

    def compare_elements(self):
        """ Compare Infrastructure """
        # Check same number of sites (main site + subsites)
        reference_sites_number = len(list_sites(self.project_reference))
        compared_sites_number = len(list_sites(self.project_compared))
        if reference_sites_number != compared_sites_number:
            print("Erreur, pas de comparaison possible. Pas le même nombre de sites au total!")
            print(reference_sites_number, " sites dans le projet de référénce.")
            print(compared_sites_number, " sites dans le projet à comparer.")
            sys.exit(-1)

        # Get data
        data_reference = self.extract_from_infrastructure(self.project_reference)
        data_compared = self.extract_from_infrastructure(self.project_compared)

        # Write
        return self.export_to_excel("Eléments", data_reference, data_compared, False)

    def compare_topography(self):
        """ Compare Topography """
        # Get data
        data_reference = self.extract_from_topography(self.project_reference)
        data_compared = self.extract_from_topography(self.project_compared)

        # Write
        return self.export_to_excel("Topographies", data_reference, data_compared, False)

    def compare_model(self):
        """ Compare model """
        # Get data
        data_reference = self.extract_from_model(self.project_reference)
        data_compared = self.extract_from_model(self.project_compared)

        # Write
        return self.export_to_excel("SourcesAcoustiques", data_reference, data_compared, False)

    def compare_solvers(self):
        """ Compare solver and its parameters """
        # Get data
        data_reference = self.extract_from_solver_parameters(self.project_reference)
        data_compared = self.extract_from_solver_parameters(self.project_compared)

        # Write
        return self.export_to_excel("Solveurs", data_reference, data_compared, False)

    def compare_results(self):
        """ Compare results from two projects """
        # Extract results from the last computation of the two projects
        data_reference = self.extract_from_result(self.project_reference)
        data_compared = self.extract_from_result(self.project_compared)

        # Write
        return self.export_to_excel("Résultats", data_reference, data_compared, True)

    def append_xyz(self, data, name, position):
        """ Function to format the x,y,z print of a position """
        if isinstance(position, Vector3D):
            x, y, z = position.vx, position.vy, position.vz
        else:
            x, y, z = position.x, position.y, position.z
        # 3 digits only (as in the GUI):
        data.append([name + " (x,y,z)", "(" + "%.3f" % x + "," + "%.3f" % y + "," + "%.3f" % z + ")"])

    def append_spectrum(self, data, values):
        """ Function to format the print of the spectrum """
        freq_tiers_oct = np.array([16.0, 20.0, 25.0, 31.5, 40.0,
                                   50.0, 63.0, 80.0, 100.0, 125.0,
                                   160.0, 200.0, 250.0, 315.0, 400.0,
                                   500.0, 630.0, 800.0, 1000.0, 1250.0, 1600.0,
                                   2000.0, 2500.0, 3150.0, 4000.0, 5000.0,
                                   6300.0, 8000.0, 10000.0, 12500.0, 16000.0])
        # One digit for value
        data.append(["Frequence", "Db"])
        data.extend([freq_tiers_oct[i], round(values[i], 1)] for i in range(len(values)))
        data.append(["[+]Spectre...", " "])

    def append_points(self, data, points):
        """ Function to format print of vertices """
        data.append(["Nombre de points", len(points)])
        for idx, point in enumerate(points):
            self.append_xyz(data, "Point " + str(idx), point)
        data.append(["[+]Liste de points...", " "])

    def append_vegetation(self, data, material_area):
        """ Function to format the print of vegetation if needed """
        if material_area.has_vegetation():
            vegetation = material_area.vegetation
            data.append(["Nom", vegetation.name()])
            data.append(["Hauteur", vegetation.height])
            data.append(["Foliage:", "" if vegetation.foliage else "Non"])

    def append_blank_line(self, data):
        """ Append blank line """
        data.append(["", ""])  # Blank line

    def extract_from_infrastructure(self, project):
        """ Extract from infrastructure """
        data = ([])
        data.append(["Nombre de sites:", len(list_sites(project))])
        self.append_blank_line(data)
        # Loop on sites:
        for site in list_sites(project):
            # Site name:
            data.append(["Site:", ""])
            data.append(["Nom", site.name])
            data.append(["Nombre de sources ponctuelles", len(site.user_sources)])
            data.append(["Nombre de machines", len(site.engines)])
            data.append(["Nombre de bâtiments", len(site.buildings)])
            self.append_blank_line(data)

            # UserSources ranked by name
            names = list(item.name for item in site.user_sources)
            for _, user_source in sorted(zip(names, site.user_sources), key=itemgetter(0)):
                data.append(["Source ponctuelle:", ""])
                data.append(["Nom", user_source.name])
                self.append_xyz(data, "Position", user_source.position)
                self.append_blank_line(data)

            # Engines ranked by name
            names = list(item.name for item in site.engines)
            for _, engine in sorted(zip(names, site.engines), key=itemgetter(0)):
                data.append(["Machine:", ""])
                data.append(["Nom", engine.name])
                data.append(["Altitude", engine.hauteur])
                self.append_xyz(data, "Position", engine.position)
                self.append_xyz(data, "Rotation", engine.rotation)
                self.append_points(data, engine.sommets)
                self.append_blank_line(data)

            # Buildings ranked by name
            names = list(item.name for item in site.buildings)
            for _, building in sorted(zip(names, site.buildings), key=itemgetter(0)):
                data.append(["Bâtiment:", ""])
                data.append(["Nom", building.name])
                data.append(["estRayonnant?", "Oui" if building.getIsRayonnant == 1 else "Non"])
                data.append(["Altitude", building.hauteur])
                self.append_xyz(data, "Position", building.position)
                self.append_xyz(data, "Rotation", building.rotation)
                self.append_points(data, building.sommets)
                self.append_blank_line(data)

        return data

    def extract_from_topography(self, project):
        """ Extract from topography """
        data = ([])
        data.append(["Nombre de sites:", len(list_sites(project))])
        self.append_blank_line(data)
        # Loop on sites:
        for site in list_sites(project):
            # Site name:
            data.append(["Site:", ""])
            data.append(["Nom", site.name])
            data.append(["Nombre de lacs", len(site.lakes)])
            data.append(["Nombre de terrains", len(site.material_areas)])
            data.append(["Nombre de courbes de niveau", len(site.level_curves)])
            self.append_blank_line(data)
            # Lakes ranked by name:
            names = list(item.name for item in site.lakes)
            for _, lake in sorted(zip(names, site.lakes), key=itemgetter(0)):
                data.append(["Plan d'eau:", ""])
                data.append(["Nom", lake.name])
                data.append(["Altitude", lake.altitude])
                data.append(["Courbe de niveau", lake.level_curve.name])
                self.append_blank_line(data)
            # Grounds ranked by name
            names = list(item.name for item in site.material_areas)
            for _, material_area in sorted(zip(names, site.material_areas), key=itemgetter(0)):
                data.append(["Terrain:", ""])
                data.append(["Nom", material_area.name])
                data.append(["Végétation?", "Oui" if material_area.has_vegetation() else "Non"])
                self.append_vegetation(data, material_area)
                ground = material_area.ground_material
                data.append(["Sol:", ""])
                data.append(["Nom", ground.name()])
                data.append(["Résistivité", ground.resistivity])
                data.append(["Epaisseur", ground.width])
                data.append(["Ecart type", ground.deviation])
                data.append(["Longueur", ground.length])
                self.append_blank_line(data)
            # LevelCurves ranked by name
            names = list(item.name for item in site.level_curves)
            for _, level_curve in sorted(zip(names, site.level_curves), key=itemgetter(0)):
                data.append(["Courbe de niveau:", ""])
                data.append(["Nom", level_curve.name])
                data.append(["Altitude", level_curve.altitude])
                self.append_points(data, level_curve.points)
                self.append_blank_line(data)
        return data

    def extract_from_model(self, project):
        """ Extract from model """
        data = ([])
        model = Model.from_project(project)
        # Build dict
        dict_id_name = build_dict(project)

        data.append(["Nombre de sources acoustiques:", len(model.sources)])
        self.append_blank_line(data)
        # SourceSolvers ranked by names/then positions:
        source_names = list([dict_id_name[src.face_id if src.face_id != "" else src.volume_id]] for src in model.sources)
        source_positions = list([str(src.position.x) + "," + str(src.position.y) + "," + str(src.position.z)] for src in model.sources)
        for name, _, source in sorted(zip(source_names, source_positions, model.sources), key=itemgetter(0, 1)):
            data.append(["Source acoustique:", ""])
            data.append(["Nom", name])
            self.append_xyz(data, "Position", source.position)
            self.append_spectrum(data, power2db(source.spectrum.values))
            self.append_blank_line(data)

        return data

    def extract_from_solver_parameters(self, project):
        """ Extract from solver parameters """
        data = ([])
        calc = project.computations[-1]
        # Solver:
        data.append(["Solver:", ""])
        data.append(["Id", calc.solver_id])
        self.append_blank_line(data)
        # Parameters:
        data.append(["Solver parameters:", ""])
        solver_parameters = calc.get_solver_parameters()
        for line in solver_parameters.decode().split("\n"):
            if '=' not in line:
                data.append([line, ""])
            else:
                param, value = line.split("=")
                data.append([param, value.replace('.', ',')])
        return data

    def extract_from_result(self, project):
        """ Extract from the results """
        data = ([])  # List
        result = project.computations[-1].result
        data.append(["Nombre de récepteurs:", len(result.receptors)])
        self.append_blank_line(data)
        # Rank receptors by name:
        names = list(item.name for item in result.receptors)
        for _, rec in sorted(zip(names, result.receptors), key=itemgetter(0)):
            data.append(["Récepteur:", ""])
            data.append(["Nom", rec.name])
            # result.receptors is a list of Element
            # Tip: convert to receptor to have more access:
            receptor = elemen2receptor(rec)
            self.append_xyz(data, "Position", receptor.position)
            # Loop on each source for contribution
            val_sum = np.zeros(31)
            # As the precision of value in xml file is one 1 digit,
            # we should round to 1 digit the computed values:
            rounder = np.vectorize(lambda x: round(x, 1))
            # Rank src by name:
            names = list(src.name for src in result.sources)
            for _, source in sorted(zip(names, result.sources), key=itemgetter(0)):
                spectre = result.spectrum(rec, source)
                sp_val = rounder(spectre.values)
                # Contribution globale
                val_sum = 10 * np.log10(10 ** (val_sum / 10) + 10 ** (sp_val / 10))
                data.append(["Contribution de", source.name])
                self.append_spectrum(data, sp_val)
                del sp_val
            # Print global
            data.append(["Spectre global", ""])
            self.append_spectrum(data, val_sum)
            self.append_blank_line(data)
        return data

    def export_to_excel(self, sheet_name, data_reference, data_compared, compute_difference=False):
        """
        Export into a sheet of the Excel file.
        Data is written on several columns.
        Return if data are different or not (is_same)
        """
        # Number of columns and rows:
        nb_col = 0
        nb_row1 = len(data_reference)
        nb_row2 = len(data_compared)
        nb_row = max(nb_row1, nb_row2)
        if nb_row1 != nb_row2:
            # We loop on the longest list:
            long_list = data_reference if nb_row1 > nb_row2 else data_compared
            short_list = data_reference if nb_row1 < nb_row2 else data_compared
            # We insert blank lines in the shortest lines:
            row = 0
            while row < nb_row:
                label = str(long_list[row][0])
                # Get an object, compare names:
                if ":" in label:
                    name = str(long_list[row+1][1])
                    other_name = str(short_list[row+1][1]) if row < len(short_list) else "empty"
                    if name != other_name:
                        while row+1 < nb_row and other_name != str(long_list[row+1][1]):
                            short_list.insert(row, [long_list[row][0], ""])
                            row += 1
                row += 1
        # Initialize data:
        row = 0
        tolerance = float(self.tolerance.get()) if self.gui else self.tolerance
        red_rows = ([])
        green_rows = ([])
        hidden_rows = ([])
        data_status = (['OK' for i in range(nb_row)])
        same_object = True
        object_row = 1
        object_label = ""
        for (r, c) in zip(data_reference, data_compared):
            row += 1
            label = str(r[0])
            # Detect object:
            if ":" in label:
                object_row = row
                object_label = label
                green_rows.append([object_row, object_label])
                same_list = True
            # Detect differences:
            same_label = (r[0] == c[0])
            same_value = (abs(float(r[1])-float(c[1]))<tolerance) if compute_difference and isinstance(r[1], float) else (r[1] == c[1])
            same_line = same_label and same_value

            if not same_line and object_row not in red_rows:
                same_object = False
                red_rows.append([object_row, object_label])

            tmp_label = label #if label != "" else str(c[0])
            tmp_value = r[0] #if label != "" else c[0]
            value_is_coordinate = True if "Sommet" in tmp_label or "Point" in tmp_label else False
            value_is_frequency = True if isinstance(tmp_value, float) or "Frequence" in tmp_label else False
            # Hide list of coordinate or frequency
            hide = value_is_coordinate or value_is_frequency
            if hide:
                hidden_rows.append([row, same_value])
                if not same_value:
                    same_list = False
            # Color list of point or frequencies ...
            if "..." in label and not same_list:
                same_object = False
                red_rows.append([row, label])
                same_list = True

            # Detect end of object, we fill data_status
            if label == "" or row == len(data_reference):
                if not same_object:
                    for r in range(object_row-1, row):
                        data_status[r] = "KO"
                    same_object = True

        # Change Excel default format (align to left):
        workbook = self.ExcelWriter.book
        format_align = workbook.add_format({'align': 'left'})
        # Defines 3 color formats:
        format_blank = workbook.add_format({'right': 1})
        format_red = workbook.add_format({'bg_color': 'red', 'right': 1})
        format_green = workbook.add_format({'bg_color': 'green', 'right': 1})

        # Status column
        status_name = ""
        status_col = 0
        status = pd.DataFrame(data_status, None, columns=[status_name])
        status.to_excel(self.ExcelWriter, index=False, sheet_name=sheet_name, startcol=nb_col)
        nb_col += 1

        # Reference project
        reference_name = "Projet de référence"
        reference_col = nb_col
        reference = pd.DataFrame(data_reference, None, columns=["", reference_name])
        reference.to_excel(self.ExcelWriter, index=False, sheet_name=sheet_name, startcol=nb_col)
        nb_col += 2

        # Compared project
        compared_name = "Projet comparé"
        compared_col = nb_col
        compared = pd.DataFrame(data_compared, None, columns=["", compared_name])
        compared.to_excel(self.ExcelWriter, index=False, sheet_name=sheet_name, startcol=nb_col)
        nb_col += 2

        # Conditional format:
        worksheet = self.ExcelWriter.sheets[sheet_name]
        cell_range = xl_range(1, 0, nb_row-1, compared_col)
        worksheet.conditional_format(cell_range, {'type': 'no_errors', 'format': format_blank})

        is_same = True
        # Add difference column
        if compute_difference:
            delta_name = "Différences (tol=" + str(tolerance) + ")"
            delta_col = nb_col
            data_delta = ["" if isinstance(x1, str) else x1 - x2 if x1 != x2 else "" for (x1, x2) in
                          zip(reference.get(reference_name).tolist(), compared.get(compared_name).tolist())]
            delta = pd.DataFrame(data_delta, index=None, columns=[delta_name])
            delta.to_excel(self.ExcelWriter, index=False, sheet_name=sheet_name, startcol=nb_col)
            # Check if results are different:
            for value in data_delta:
                if isinstance(value, float) and abs(value) > tolerance:
                    is_same = False
            worksheet.conditional_format(1, nb_col, nb_row - 1, nb_col, {'type': 'cell',
                                                                         'criteria': '>',
                                                                         'value': tolerance,
                                                                         'format': format_red})
            worksheet.conditional_format(1, nb_col, nb_row - 1, nb_col, {'type': 'cell',
                                                                         'criteria': '<',
                                                                         'value': -tolerance,
                                                                         'format': format_red})
            worksheet.conditional_format(1, nb_col, nb_row - 1, nb_col, {'type': 'cell',
                                                                         'criteria': '=',
                                                                         'value': 0,
                                                                         'format': format_blank})
            nb_col += 1
        else:
            # Check if the data matches:
            is_same = (data_reference == data_compared)

        # If differences found in a worksheet:
        if not is_same:
            # Change colortab:
            worksheet.set_tab_color('red')

        # Green row
        for row, label in green_rows:
            worksheet.write(row, reference_col, label, format_green)

        # Color cell for different object
        for row, label in red_rows:
            worksheet.write(row, reference_col, label, format_red)

        width = 30  # Different width for some columns
        for col in range(reference_col, nb_col):
            if col == compared_col:
                # Hide label compared column:
                worksheet.set_column(col, col, width, format_align,
                                     {'level': 1, 'hidden': 1, 'collapsed': False})
            else:
                worksheet.set_column(col, col, width, format_align)

        # Hide row
        for item in hidden_rows:
            row = item[0]
            worksheet.set_row(row, None, None, {'level': 1, 'hidden': 1, 'collapsed': False})

        # Red color on cells where values are different
        for row in range(1, nb_row):
            cell1 = xl_rowcol_to_cell(row, reference_col+1)
            cell2 = xl_rowcol_to_cell(row, compared_col+1)
            worksheet.conditional_format(cell2, {'type': 'cell',
                                                 'criteria': '!=',
                                                 'value': cell1,
                                                 'format': format_red})
            worksheet.conditional_format(cell2, {'type': 'cell',
                                                 'criteria': '=',
                                                 'value': cell1,
                                                 'format': format_blank})

        # Top border to finish
        format_border_top = workbook.add_format({'top': 1})
        for col in range(nb_col):
            worksheet.write(nb_row, col, "", format_border_top)

        print(self.get_output_filename() + ":" + sheet_name + " " + ("identiques" if is_same else "différents!"))
        return is_same


if __name__ == "__main__":
    app = App(None)
    app.title(u'Code_TYMPAN : Outil de comparaison de projets')
    app.mainloop()
