# -*- coding: utf-8 -*-
"""
Created on 20 dec. 2016

@author: t.legall

Do calculations between on a surface receptor
and between two computations
"""
import sys
if sys.version_info[:2] <= (2, 7):
    print("Error! Python 3 only can be used.")
    sys.exit(-1)
from tympan.models.project import Project
from _util import ask_xml_file, input_int, ask_result_file, input_string
from _util import mesh_spect_to_ndarray, ndarray_to_mesh_spect, operation_array

def load_tympan_xml(tympan_xml):
    """
    Open xml file and return project
    """
    print('Opening Code_TYMPAN project from %s' % tympan_xml)
    project = Project.from_xml(tympan_xml, verbose=False)
    return project


def select_calc(project):
    """ 
    Select a computation from a tympan project
    """
    # Get and print computations list
    calcs = project.computations
    index = 0
    print('Select computation in the list below :')
    for c in calcs:
        index+=1
        print('{}.'.format(index) + c.name)
    
    # Verify user input
    verif_choix = False
    while not verif_choix:
        numero_calcul = input_int('Enter chosen computation number : ')
        if numero_calcul <= 0 or numero_calcul > len(calcs):
            verif_choix = False
            print('You did not enter the right number, choose again.')
        else:
            verif_choix = True
    
    
    # Get current computation
    calcul_courant = calcs[numero_calcul-1]
    print('Selected computation is : ' + calcul_courant.name)
    return calcul_courant


def select_maillage(project):
    """
    Select a mesh from a tympan project
    """
    # Get and print meshes list
    mhs = project.meshes

    size = len(mhs)
    print('Choose mesh in the list below :')
    for index in range(size):
        print('{}. maillage {}'.format(index, index))
    
    # Verify user input
    verif_choix = False
    while not verif_choix:
        numero_maillage = input_int('Enter chosen mesh number : ')
        if numero_maillage < 0 or numero_maillage > len(mhs)-1:
            verif_choix = False
            print('You did not enter the right number, choose again.')
        else:
            verif_choix = True
    
    # Get chosen mesh
    maillage = mhs[numero_maillage-1]
    return maillage


def main(fichier_xml, output_xml):

    # Load project
    projet = load_tympan_xml(fichier_xml)

    # Select mesh
    mh = select_maillage(projet)

    # Select calculation number 1
    print('\nSelect computation n°1 : ')
    calc = select_calc(projet)
    projet.select_computation(calc)
    name_calc = calc.name
    mesh_spectrums_ref = calc.get_noise_map_spectrums(mh)
    ndarray_ref = mesh_spect_to_ndarray(mesh_spectrums_ref)

    # Select calculation number 2
    print('\nSelect computation n°2 : ')
    calc = select_calc(projet)
    name_ref = calc.name
    projet.select_computation(calc)
    mesh_spectrums_calc = calc.get_noise_map_spectrums(mh)
    ndarray_calc = mesh_spect_to_ndarray(mesh_spectrums_calc)

    # Loop until the user save and exit
    operations_name = ['difference', 'emergence', 'ambient noise']
    # Big trouble on windows console to print "\u2295" so:
    circed_plus = "+"
    operations = ['0.Différence : L2 - L1', '1.Emergence L1'+circed_plus+'L2 - L2', '2.Bruit ambiant : L1'+circed_plus+'L2', '3.Exit']
    while True:
        # Select operation, verify input and create mesh spectrum result
        print('\nSelect operation : ')
        for op in operations:
            print(op)
        verif_choix = False
        while not verif_choix:
            num_ope = input_int('Enter chosen operation : ')
            verif_choix = True
            if num_ope == len(operations)-1:
                print('OK, exiting.')
                return
            if num_ope < 0 or num_ope >= len(operations):
                verif_choix = False
                print('You did not enter the right number, choose again.')

        # Compute operation
        ndarray_result = operation_array(ndarray_ref,ndarray_calc,num_ope)

        # The result is converted to a mesh_spectrum
        mesh_spectrums_result = ndarray_to_mesh_spect(ndarray_result)

        # Name the result
        calc_name = 'Result of '+operations_name[num_ope]+" on "+name_calc+" and "+name_ref

        # Check if a computation exist with this name
        calc_exist = False
        for calc in projet.computations:
            if calc.name == calc_name:
                calc_exist = True
                break

        # Add a new noise map to the new computation
        if calc_exist == True and input_string("This computation exists. Confirm deleting (y/n) ?")=="y":
            last_calc = calc
        else:
            # Add a new computation
            projet.add_computation()
            last_calc = projet.computations[-1]
            last_calc.add_noise_map(mh)

        # Set the current computation to the last one and set the noise map spectrums
        projet.select_computation(last_calc)
        projet.current_computation.set_name(calc_name)
        projet.current_computation.set_noise_map_spectrums(mh, mesh_spectrums_result)

        projet.to_xml(output_xml)
        print(calc_name + ' saved to ' + output_xml)

if __name__ == '__main__':

    # Get file name and verify user input
    fichier_xml = ask_xml_file("Enter xml file name (with xml extension) : ")

    # Result file
    output_xml = ask_result_file()

    # Launch main
    main(fichier_xml, output_xml)
