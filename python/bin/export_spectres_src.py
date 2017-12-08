# -*- coding: utf-8 -*-
"""
Created on 15 nov. 2016

@author: t.legall

Export spectrums from all sources in the tympan project, included machines and buildings.
"""
import sys
import numpy as np
if sys.version_info[:2] <= (2, 7):
    print("Error! Python 3 only can be used.")
    sys.exit(-1)
from tympan.models.solver import Model
from _util import input_string, run_calculations, ask_xml_file
from _util import list_sites, build_dict, power2db


def write_results(valeurs, src_ids, dict_id_name, fichier_txt):
    """
    Write third octave spectrum values to a txt file.
    Objects used to handle the values are numpy arrays.
    
    valeurs type : ndarray
    src_ids type : list
    """
    # numpy array containing frequency values
    freq_tiers_oct = np.array([[16.0, 20.0, 25.0, 31.5, 40.0,
                               50.0, 63.0, 80.0, 100.0, 125.0,
                               160.0, 200.0, 250.0, 315.0, 400.0,
                               500.0, 630.0, 800.0, 1000.0, 1250.0, 1600.0,
                               2000.0, 2500.0, 3150.0, 4000.0, 5000.0,
                               6300.0, 8000.0, 10000.0, 12500.0, 16000.0]])

    # Convert IDs to Names:
    names = []
    for i in range(len(src_ids)):
        names.append(dict_id_name[src_ids[i]] if src_ids[i] != "" else src_ids[i])
    # Sort the arrays by names:
    sorted_names = []
    sorted_valeurs = np.empty([len(names), 31])
    ranks = [i for i in range(len(names))]
    for i, (name, rank) in enumerate(sorted(zip(names, ranks))):
        sorted_names.append(name)
        sorted_valeurs[i, :] = valeurs[rank]
    # Create final array to be written in the txt file
    noms = np.array(sorted_names)[:, np.newaxis]
    result_array = np.hstack((noms, sorted_valeurs))
    freq = np.array(['Frequence'])[:, np.newaxis]
    freq_array = np.hstack((freq, freq_tiers_oct))
    final_array = np.append(freq_array, result_array, axis=0)
    final_array = final_array.transpose()
    
    # save in txt file
    with open(fichier_txt, 'wb') as f:
        np.savetxt(f, final_array, delimiter=', ', fmt='%s')


def convert(indices_fin, list_src):
    """ Convert indices_fin to a numpy array """
    temp_array = np.asarray(indices_fin)
    del (indices_fin)
    indices_fin = temp_array
    del (temp_array)

    # Define indices_debut and finalize the lists
    indices_debut = np.append([0], indices_fin + 1)
    indices_fin = np.append(indices_fin, len(list_src) - 1)

    spectrum_values = None
    for i in range(len(list_src)):
        values = [list_src[i].spectrum.values]
        spectrum_values = values if i == 0 else np.append(spectrum_values, values, axis=0)

    # Creation of n-dimensional array for each real "surface" source, not each point source
    # There is a different calculus if the "surface" source is a point source or a surface
    values_final = np.empty([len(indices_debut), 31])

    for i in range(len(indices_debut)):
        if indices_fin[i] - indices_debut[i] == 0:
            resultat = spectrum_values[indices_debut[i], :]
        else:
            resultat = spectrum_values[indices_debut[i]] * ((indices_fin[i] - indices_debut[i]) + 1)

        values_final[i, :] = resultat

    return values_final


def calc_surf_spectrum(list_src):
    """
    Get sources list contained in the model (list_src = model.sources) and return surface
    spectrum for sources that are not a point source. If the source is a point source return
    spectrum of the point source only.
    """
    # Definition of variables
    src_ids = list()
    indices_fin = list()

    # Add the face_id (id of the source) to the list src_ids
    # Add the end indices to indices_fin
    src_ids.append(list_src[0].face_id)
    
    for i in range(len(list_src)-1):
        if list_src[i+1].face_id != list_src[i].face_id:
            src_ids.append(list_src[i+1].face_id)
            indices_fin.append(i)

    values_final = convert(indices_fin, list_src)
    return values_final, src_ids


def calc_vol_spectrum(list_src):
    """
    Get the sources list contained in the model (list_src = model.sources) and return volume
    spectrum for sources that are not a point source. If the source is a point source return
    spectrum of the point source only.
    """
    # Definition of variables
    src_ids = list()
    indices_fin = list()

    # Add the volume_id (id of the source) to the list src_ids
    # Add the end indices to indices_fin
    src_ids.append(list_src[0].volume_id)
    
    for i in range(len(list_src)-1):
        if list_src[i+1].volume_id != list_src[i].volume_id:
            src_ids.append(list_src[i+1].volume_id)
            indices_fin.append(i)

    values_final = convert(indices_fin, list_src)
    return values_final, src_ids


def main(fichier_xml, fichier_txt):
    # Get model from project
    project = run_calculations(fichier_xml)
    model = Model.from_project(project)
    
    # Get sources list
    list_src = model.sources

    # Build a dictionnary to convert ID to name
    dict_id_name = build_dict(project)

    # Loop on acoustic surfaces to print infos
    print("----------------:")
    print("Punctual sources:")
    print("----------------:")
    sites = list_sites(project)
    for site in sites:
        for src in site.user_sources:
            print(src.name)

    print("")
    print("-----------------:")
    print("Acoustic surfaces:")
    print("-----------------:")
    for site in sites:
        for ac in site.acoustic_surfaces:
            face_name = ac.surface_node_name()
            ac_name = "Face \""+face_name+"\" of volume " if face_name is not None else "Volume "
            volume_name = ac.volume_name()
            ac_name += "\""+volume_name+"\""
            element_name = ac.element_name()
            ac_name += " from element \""+element_name+"\""
            print(ac_name+" is","radiant." if ac.getIsRayonnant else "non radiant.")

    # Get spectrums from the different sources PER SURFACE
    (values_final, src_ids) = calc_surf_spectrum(list_src)
    # Transform the power value into a dB value
    values_final_db = power2db(values_final)

    # Write the values into a file using write_results
    write_results(values_final_db, src_ids, dict_id_name, fichier_txt+'_parSurface.txt')

    # Get spectrums from the different sources PER VOLUME
    (values_final, src_ids) = calc_vol_spectrum(list_src)
    # Transform the power value into a dB value
    values_final_db = power2db(values_final)

    # Write the values into a file using write_results
    write_results(values_final_db, src_ids, dict_id_name, fichier_txt+'_parVolume.txt')

    # IMPORTANT COMMENT : The result values present some round problems or precision problems
    # moreover, in the "surface" case it seems better to multiply the spectrum value by :
    # ((indices_fin[i]-indices_debut[i])+1)
    # Whereas in the "volume" case the results seem better when multiplied by :
    # (indices_fin[i]-indices_debut[i])


if __name__ == '__main__':
    # Get xml file name and verify user input
    fichier_xml = ask_xml_file("Enter xml file name (with xml extension) : ")

    # Get txt file name where results have to be written
    fichier_txt = input_string("Enter txt file name (without extension) : ")
     
    # launch main 
    main(fichier_xml, fichier_txt)
