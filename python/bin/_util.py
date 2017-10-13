# -*- coding: utf-8 -*-

'''
Methods used in Code_TYMPAN Scripts
'''
import os, sys
import numpy as np

from functools import cmp_to_key
from tympan.models.project import Project
from tympan.altimetry import AltimetryMesh
from tympan.models.solver import Model, Solver
from tympan.models._common import Spectrum
from tympan.models._business import Element_array, Engine, Building, Site
from tkinter import Toplevel, Label, Button

# Environment variables
_HERE = os.path.realpath(os.path.dirname(__file__))
PROJECT_BASE = os.path.abspath(os.path.join(_HERE, '..'))
# Tests directory:
TEST_DATA_DIR = os.path.join(PROJECT_BASE, 'tests', 'data')
# Toolbox tests data:
TOOLBOX_DATA_DIR = os.path.join(TEST_DATA_DIR,"toolbox")
# Solver directory:
ty_solverdir = os.environ.get('TYMPAN_SOLVERDIR')
if not ty_solverdir:
    for d in ('pluginsd', 'plugins'):
        ty_solverdir = os.path.join(PROJECT_BASE, d)
        if os.path.isdir(ty_solverdir) and os.listdir(ty_solverdir):
            break
    else:
        raise RuntimeError("The test solver plugins dir wasn't found")


def run_calculations(input_xml, run_first=True):
    """
    Run all calculations
    """

    # load project
    project = Project.from_xml(input_xml)

    for i, calc in enumerate(project.computations):
        if run_first or i > 0:
            print('Select calculation '+str(i)+' :', calc.name)
            project.select_computation(calc)

            print('Update altimetry')
            altim = AltimetryMesh.from_site(project.site)
            project.update_site_altimetry(altim)

            print('Get Solver object from Tympan project located at %s' % ty_solverdir)
            solver = Solver.from_project(project, ty_solverdir)

            print('Build model')
            model = Model.from_project(project, set_sources=True,
                                       set_receptors=True)
            print('Launch solver')
            result = solver.solve(model)

            # Import results to project
            print('Import results')
            project.import_result(model, result)

            # result must be destroyed otherwise it crashes
            del result

    return project


def load_elements(elements_xml):
    """
       Load elements defined in the file elements_xml
    """
    # Load project from XML file and get elements
    project_elements = Project.from_xml(elements_xml)
    elements = project_elements.site

    return elements


def mesh_spect_to_ndarray(mesh_spectrums):
    """
        Get values (third octave band values) of
        each Spectrum of a noise_map and put them into
        a ndarray.
        Method used to facilitate operations between
        spectrums.
    """
    # Create ndarray with first spectrum
    ndarray_spec = mesh_spectrums[0].values
    ndarray_spec = [ndarray_spec]

    # Each iteration, append new spectrum to ndarray
    for i in range(len(mesh_spectrums) - 1):
        ndarray_spec = np.append(ndarray_spec, [mesh_spectrums[i + 1].values], axis=0)

    return ndarray_spec


def ndarray_to_mesh_spect(ndarray_spec):
    """
        Get the values of a ndarray (result of a calculation for
        example) and put them into a new noise_map.
    """
    # Create list of mesh_spectrums
    mesh_spectrums = list()

    # Transform ndarray to mesh_spectrum and append the list
    for i in range(len(ndarray_spec)):
        sp = Spectrum(ndarray_spec[i])
        mesh_spectrums.append(sp)
        del (sp)

    return mesh_spectrums


def moyenne_mesh(list_ndarray_spec):
    """
        Calculation of mean value of different noise_maps.
        Return ndarray of mean values.
    """
    ndarray_moy = list_ndarray_spec[0] / len(list_ndarray_spec)
    for i in range(len(list_ndarray_spec) - 1):
        ndarray_moy += list_ndarray_spec[i + 1] / len(list_ndarray_spec)

    return ndarray_moy


def operation_array(ndarray_ref, ndarray_calc, num_ope):
    """
        Calculate result of different operations :
        0- Difference
        1- Emergence
        2- Bruit ambiant
    """
    ndarray_result = None
    if num_ope == 0:
        ndarray_result = ndarray_calc - ndarray_ref
    elif num_ope == 1:
        ndarray_result = 10 * np.log10(10 ** (ndarray_calc / 10) + 10 ** (ndarray_ref / 10)) - ndarray_ref
    elif num_ope == 2:
        ndarray_result = 10 * np.log10(10 ** (ndarray_calc / 10) + 10 ** (ndarray_ref / 10))

    return ndarray_result


def input_string(message):
    """
        String input
    """
    return input(message)


def input_int(message):
    """
        Integer input
    """
    waiting = True
    while waiting:
        try:
            i = int(input_string(message))
            waiting = False
            return i
        except Exception:
            print("Error! You must enter an integer.")


def ask_confirm(message):
    return True if input_string(message+" (y/n) ?") == "y" else False


def ask_result_file(result_xml = "Resultat.xml"):
    """
        Ask for result filename
    """
    loop = True
    while loop:
        filename = input_string("Enter result file name (with xml extension) or press enter to keep default one ("+result_xml+"): ")
        if filename!="": result_xml = filename
        directory = os.path.dirname(result_xml)
        if directory!="" and not os.path.isdir(directory):
            print("Error! Directory "+directory+" doesn't exist.")
        elif os.path.isfile(result_xml):
            print("Warning, result file ",result_xml," is an existing file!")
            if (input_string("Confirm the result file could be deleted (y/n) ?")=="y"):
                loop = False
        else:
            loop = False

    return result_xml


def ask_xml_file(message, object_type=""):
    """ Ask for an XML file and do some checks """
    verif_choix = False
    while not verif_choix:
        filename = ask_input_file(message)
        verif_choix = filename.endswith(".xml")
        if not verif_choix:
            print(filename+" is not a XML file !")
    # Some checks
    if object_type == "engine" or object_type == "building":
        # Build an empty project
        Project.create()
        import_infra(filename, object_type)
    return filename


def ask_input_file(message):
    """ Ask for an input file """
    # Check path
    verif_choix = False
    while not verif_choix:
        filename = input_string(message)
        abs_path = os.path.abspath(filename)
        verif_choix = os.path.isfile(abs_path)
        if not verif_choix:
            print("File "+abs_path+" does not exist. Choose again.")
    return filename


def line_count(filename):
    """ Count the lines number of a file """
    with open(filename) as f:
        lines = sum(1 for _ in f)
    f.close()
    return lines


def import_infra(object_file, object_type):
    """
    Read the single infrastructure element from object_file
    object_type = "engine" or "building"
    and return the element
    """
    elements = Element_array()
    elements.from_xml(object_file)
    # Check there is one and only one element
    if object_type == "engine":
        number_elements = len(elements.engines)
    elif object_type == "building":
        number_elements = len(elements.buildings)
    else:
        print("Error in import_infra, unknown object_type=",object_type)
        sys.exit(-1)
    if number_elements != 1:
        print("Error: " + object_file + " should contain only one engine or building and not " + str(number_elements))
        sys.exit(-1)
    # Check here the radiant status of building
    if object_type == "building" and elements.buildings[0].getIsRayonnant == False:
         print("Building should have radiant status.")
         sys.exit(-1)
    return elements


def check_positions(xyz, project):
    """
        Check if points are inside the domain
    """
    xmin, xmax, ymin, ymax = project.site.getEmprise()
    for x in xyz[:, 0]:
        if x > xmax or x < xmin:
            print("Error in the CSV file, x=", x, " position is outside the domain! [xmin, xmax]=[", xmin, ",", xmax, "]")
            sys.exit(-1)
    for y in xyz[:, 1]:
        if y > ymax or y < ymin:
            print("Error in the CSV file, y=", y, " position is outside the domain! [ymin, ymax]=[", ymin, ",", ymax, "]")
            sys.exit(-1)


def import_xyz_csv(csv_file, project):
    """
    Import and return coordinates from a csv file
    """
    print("Reading "+csv_file+" ...")
    csv = np.genfromtxt(csv_file, delimiter=",")
    if csv.shape[1]!=3:
        print("Error! Should read 3 columns with x,y,z in "+csv_file)
        sys.exit(-1)

    # Check positions is inside the domain:
    check_positions(csv, project)

    x = csv[:, 0]
    y = csv[:, 1]
    z = csv[:, 2]
    return x, y, z


def import_xyz_angle_csv(csv_file, project):
    """
    Import and return coordinates from a csv file
    """
    print("Reading "+csv_file+" ...")
    csv = np.genfromtxt(csv_file, delimiter=",")
    if csv.shape[1] != 4:
        print("Error! Should read 4 columns with x,y,z,angle in "+csv_file)
        sys.exit(-1)

    # Check positions is inside the domain:
    check_positions(csv, project)

    x = csv[:, 0]
    y = csv[:, 1]
    z = csv[:, 2]
    angle = csv[:, 3]
    return x, y, z, angle


class MyDialog:
    """
    Class creating popup window when user error is committed
    """

    def __init__(self, parent, mess):

        top = self.top = Toplevel(parent)
        label = Label(top,text=mess,anchor="w",fg="red")
        label.pack()

        b = Button(top, text="OK", command=self.ok_close)
        b.pack(pady=5)

    def ok_close(self):

        self.top.destroy()


def compare_txt_results(actual_file, expected_file):
    """ Compare equality of csv or txt files """
    actual_x, actual_y = read_results(actual_file)
    expected_x, expected_y = read_results(expected_file)
    decimal=6
    np.testing.assert_almost_equal(actual_x, expected_x,decimal)
    np.testing.assert_almost_equal(actual_y, expected_y,decimal)


def read_results(f_name):
    csv = np.genfromtxt(f_name, delimiter=",")
    freq = csv[:, 0]
    result = csv[:, 1]
    return freq, result


# ToDo merge with utils.py later during merge
def compare_xml_results(result_xml, reference_xml, testobj):
    """
    Compare the results to expected results extracted from XML files
    """
    project = Project.from_xml(result_xml)
    expected_project = Project.from_xml(reference_xml)
    compare_project_results(project, expected_project, testobj)


def compare_project_results(project, expected_project, testobj):
    """
    Compare the results to expected results extracted from projects
    """
    # Compare results
    current_result = project.current_computation.result
    expected_result = expected_project.current_computation.result
    # Check we have the same number of receptors
    testobj.assertEqual(current_result.nreceptors, expected_result.nreceptors)
    # Check if there is a control point (TYPointControl) among the receptors
    # in the project.
    # If not, we are not interested in checking the sources since the
    # control points are the only receptors able to take into account the
    # individual contributions of the sources.
    # The sources here can be user sources, the machines and the buildings
    # (TYUserSourcePonctuelle, TYMachine, TYBatiment)
    check_nsources  = False
    for i in range(current_result.nreceptors):
        if current_result.receptor(i).is_control_point():
            testobj.assertTrue(expected_result.receptor(i).is_control_point())
            check_nsources = True
    if check_nsources:
        testobj.assertEqual(current_result.nsources, expected_result.nsources)

    current_spectra = np.array(list(current_result.spectrum(current_result.receptors[i], current_result.sources[j]).values
                                    for i in range(current_result.nreceptors)
                                    for j in range(current_result.nsources)))
    expected_spectra = np.array(list(expected_result.spectrum(expected_result.receptors[i], expected_result.sources[j]).values
                                    for i in range(expected_result.nreceptors)
                                    for j in range(expected_result.nsources)))

    if current_result.nsources + current_result.nreceptors > 1:
        # Order the two spectra lists because spectra are not always kept in the same order
        current_spectra = sorted(current_spectra, key=cmp_to_key(compare_floats))
        expected_spectra = sorted(expected_spectra, key=cmp_to_key(compare_floats))
    for i in range(len(current_spectra)):
        # All spectra must have the same number of elements
        testobj.assertEqual(current_spectra[i].size, expected_spectra[i].size)
        np.testing.assert_almost_equal(current_spectra[i],
                                       expected_spectra[i], decimal=1)

def compare_floats(x, y):
    """ Compare x and y which are float arrays:
        Go through them, considering x[i] and y[i] equal up to a 3 decimal
        precision. Then when they sufficiently differ, return -1 if x[i] < y[i]
        and 1 otherwise. 0 is returned if the arrays are equal.
    """
    for xi, yi in zip(x, y):
        if not np.allclose(xi, yi, atol=1e-03):
            if xi < yi:
                return -1
            else:
                return 1
    return 0  # arrays are equal


def list_sites(item):
    """ Build recursively the list of sites """
    sites = []
    if isinstance(item, Project):
        site = item.site
    elif isinstance(item, Site):
        site = item
    else:
        print("Error.")
        sys.exit(-1)
    sites.append(site)
    for subsite in site.subsites:
        sites.extend(list_sites(subsite))
    return sites


def build_dict(project):
    """ Build dict to convert Id to name for acoustic sources """
    dict_id_name = {}
    # Loop on sites
    for site in list_sites(project):
        # Update dictionnary for punctual sources
        for src in site.user_sources:
            dict_id_name[src.getID] = src.name
        # Update dictionnary for acoustic surfaces
        for ac in site.acoustic_surfaces:
            volume_name = ac.volume_name()
            element_name = ac.element_name()
            face_name = ac.surface_node_name()
            if face_name is not None:
                dict_id_name[ac.surface_node_id()] = element_name + "-" + volume_name + "-" + face_name
            dict_id_name[ac.volume_id()] = element_name + "-" + volume_name
    return dict_id_name
