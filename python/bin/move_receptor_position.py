"""How to simulate a moving receptor"""

from __future__ import print_function
from tympan.models.project import Project
from tympan.models._common import Point3D
from run_all_calculations import run_calculations
from _util import ask_input_file, ask_xml_file, input_string, import_xyz_csv, ask_result_file
from tympan.models.solver import Model, Solver

import os, sys
import numpy as np

def get_receptors_list(project, calculations_namelist):
    """
    Build a list of receivers for the calculations in the calculations_list
    """
    receptors_namelist = []
    receptors = []
    for calc in project.computations:
        if calc.name in calculations_namelist:
            result = calc.result
            for rec in result.receptors:
                if rec.name not in receptors_namelist:
                    receptors_namelist.append(rec.name)
                    receptors.append(rec)
    
    return receptors


def get_sources_list(project, calculations_namelist):
    """
    Build a list of sources for the calculations in the calculations_list
    """
    sources_namelist =[]
    sources = []
    for calc in project.computations:
        if calc.name in calculations_namelist:
            result =  calc.result
            for src in result.sources:
                # print('Calcul:',calc.name,'Source:',src.name)
                if src.name not in sources_namelist :
                    sources_namelist.append(src.name)
                    sources.append(src)
    
    return sources


def get_rec_spec(project, sources, receptors):
    """
    Get all point receptors spectrums in the project
    """
    valeurs = np.empty((0, 31))
    noms_spectres = list()
    for calc in project.computations:
        result = calc.result
        for rec in receptors:
            val_sum = np.zeros(31)
            for src in sources:
                spectre = result.spectrum(rec, src)
                sp_val = spectre.values
                val_sum = 10*np.log10(10**(val_sum/10)+10**(sp_val/10))
                del sp_val
#               val_sum = np.append(val_sum, [spectre.values], axis=0)
#           val_sum = np.sum(val_sum, axis=0)
            valeurs = np.append(valeurs, [val_sum], axis=0)
            nom_spectre = rec.name
            noms_spectres.append(nom_spectre)
    
    return valeurs, noms_spectres


def write_results(valeurs, noms_spectres, f_name):
    """
    Write results in a txt file
    """
    freq_tiers_oct = np.array([[16.0, 20.0, 25.0, 31.5, 40.0,
                               50.0, 63.0, 80.0, 100.0, 125.0,
                               160.0, 200.0, 250.0, 315.0, 400.0,
                               500.0, 630.0, 800.0, 1000.0, 1250.0, 1600.0,
                               2000.0, 2500.0, 3150.0, 4000.0, 5000.0,
                               6300.0, 8000.0, 10000.0, 12500.0, 16000.0]])
    noms = np.array(noms_spectres)[:, np.newaxis]
    freq = np.array(['Frequence'])[:, np.newaxis]
    
    result_array = np.hstack((noms, [valeurs]))
    freq_array = np.hstack((freq, freq_tiers_oct))
    final_array = np.append(freq_array, result_array, axis=0)
    final_array = final_array.transpose()
    with open(f_name, 'wb') as f:
        np.savetxt(f, final_array, delimiter=', ', fmt='%s')


def main(tympan_xml, receptors_csv, output_xml, output_txt='Niveau_sonore_moyen.txt'):
    """
        Main
    """
    # load project
    project = Project.from_xml(tympan_xml)

    # Case receptors are read in a CSV file
    # ToDo create a _util/add_receptor_from_csv instead ?
    if receptors_csv != "":
        # Import and add receptor positions
        x, y, z = import_xyz_csv(receptors_csv, project)
        for i in range(len(x)):
            height = z[i]
            position = Point3D()
            position.set_x(x[i])
            position.set_y(y[i])
            position.set_z(height)
            project.add_user_receptor(position, height, "Position_"+str(i))

    # Run the calculations
    run_calculations(project)

    # Choose current computation, get receptors list and sources list
    list_calc = []
    list_calc.append(project.current_computation.name)
    list_src = get_sources_list(project, list_calc)
    list_rec = get_receptors_list(project, list_calc)
    
    # Get receptor number
    N = len(list_rec)

    # Get spectrums
    valeurs, nom_spectres = get_rec_spec(project, list_src, list_rec)
    del nom_spectres

    # Calculate mean value and set spectrum name
    val_result = np.sum(valeurs, axis=0)/N
    nom_spectre = ['Niveau sonore moyen sur parcours']
    
    # Write results to a .txt file
    print('\nFile created: '+output_txt)
    write_results(val_result, nom_spectre, output_txt)

    # Save to output_xml
    if output_xml != "":
        project.to_xml(output_xml)
        print('Result saved to', output_xml)

    return project
   
    
if __name__ == '__main__':

    # Get project file name and verify user input
    tympan_xml = ask_xml_file("Enter XML file name for the project (with .xml extension) : ")

    # Possible to read CSV file for receptors positions
    receptors_csv = ""
    verif_choix = False
    while not verif_choix:
        receptors_csv = input_string("Enter CSV file name (with .csv extension) to add receptor positions or press enter to continue: ")
        if receptors_csv == "":
            verif_choix = True
        else:
            abs_path = os.path.abspath(receptors_csv)
            verif_choix = os.path.isfile(abs_path)
            if not verif_choix:
                print("File does not exist. Choose again.")

    # Output file
    output_xml = ask_result_file()

    # Launch main
    main(tympan_xml, receptors_csv, output_xml)
