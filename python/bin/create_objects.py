# -*- coding: utf-8 -*-
"""
Automatic creation of sources or receptors with TYMPAN coordinates
"""
import sys
import numpy as np
from tympan.models.project import Project
from tympan.models._common import Point3D
from tympan.models._business import User_source, UserReceptor, Element_array
from _util import ask_input_file, ask_xml_file, input_int, input_string, ask_result_file, import_infra
if sys.version_info[:2] <= (2, 7):
    print("Error! Python 3 only can be used.")
    sys.exit(-1)


def main(object_file_list, object_positions_file_list, object_type_list, object_name_list, input_xml, output_xml):
    """
    Import csv file with TYMPAN coordinates and create new project with objects
    Or add objects to an existing project
    """
    # Create new project if no project entered by user
    project = Project.create() if input_xml == '' else Project.from_xml(input_xml)

    # Loop on objects
    for object_file, object_positions_file, object_type, object_name in zip(object_file_list, object_positions_file_list, object_type_list, object_name_list):
        # Import the object read in the xml file:
        if object_file != "":
            # Import the elements from XML file:
            elements = import_infra(object_file, object_type)

        # Create numpy array with csv file containing TYMPAN coordinates of the objects
        my_data = np.genfromtxt(object_positions_file, dtype='float', delimiter=';', skip_header=1)
        if my_data.shape[1] < 3:
            print("Error! Should read x,y,z or x,y,z,angle in "+object_positions_file)
            sys.exit(-1)
        # Add object to project site for each position
        site = project.site
        rotation = Point3D(0, 0, 0)
        for i in range(len(my_data[:, 0])):
            position = Point3D()
            height = my_data[i, 2]
            position.set_x(my_data[i, 0])
            position.set_y(my_data[i, 1])
            position.set_z(height)
            # Add rotation if found
            if my_data.shape[1] == 4:
                rotation.set_z(my_data[i, 3])
            name = object_name+str(i)
            # Add object:
            if object_type == "source":
                # Source
                src = User_source(height, name, position)
                site.add_user_source(src, position, height)
            elif object_type == "receptor":
                # Receptor
                project.add_user_receptor(position, height, name)
            elif object_type == "engine":
                # Engine
                site.add_engine(elements.engines[0], position, rotation, height)
            elif object_type == "building":
                # Building
                site.add_building(elements.buildings[0], position, rotation, height)
            else:
                print("Error: Unknown object type: ", object_type)
                sys.exit(-1)

        # Add project user_receptors to the last computation
        if object_type == "receptor":
            calcul = project.computations[-1]
            for rec in project.user_receptors:
                calcul.addReceptor(rec)

    # Write the project
    project.to_xml(output_xml)
    print('Project saved to ', output_xml)


if __name__ == '__main__':

    # Add or not to an existing project ?
    ask_projet = input_string("Do you want to add one or several objects to an existing project ? (y or n) : ")
    if ask_projet == 'y':
        input_xml = ask_xml_file("Enter XML file name containing project (with .xml extension) : ")
        # Name result xml file
        output_xml = ask_result_file(input_xml)
    else:
        input_xml = ''
        # Name project xml file
        output_xml = ask_result_file("Project.xml")

    # Initialize
    object_file_list = []
    object_positions_file_list = []
    object_type_list = []
    object_name_list = []

    while True:
        # Select object type
        print('Select one or several objects to add:')
        print('0. Source')
        print('1. Receptor')
        print('2. Engine defined in a XML file')
        print('3. Building defined in a XML file')
        choice = input_int("Enter object type (0,1,2,3) or -1 to exit: ")
        if choice <= -1:
            break
        elif choice <= 1:
            object_type = "source" if choice == 0 else "receptor"
            object_file = ""
            object_positions_file = ask_input_file("Enter CSV file containing TYMPAN coordinates of the "+object_type+" (with csv extension) : ")
        elif choice <= 3:
            object_type = "engine" if choice == 2 else "building"
            object_file = ask_xml_file("Enter XML file containing the "+object_type+" (with xml extension) : ", object_type)
            object_positions_file = ask_input_file("Enter CSV file containing positions of the "+object_type+" (with csv extension) : ")
        else:
            print("Unknown choice.")
            object_type = ""
            object_file = ""
            object_positions_file = ""

        if object_type != "":
            # Name the object
            object_name = input_string("Enter "+object_type+" name : ")
            # Add to list:
            object_file_list.append(object_file)
            object_positions_file_list.append(object_positions_file)
            object_type_list.append(object_type)
            object_name_list.append(object_name)

    # Call to main
    main(object_file_list, object_positions_file_list, object_type_list, object_name_list, input_xml, output_xml)
