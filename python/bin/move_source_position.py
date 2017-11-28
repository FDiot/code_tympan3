# -*- coding: utf-8 -*-
"""How to simulate a moving source"""
import os, sys
if sys.version_info[:2] <= (2, 7):
    print("Error! Python 3 only can be used.")
    sys.exit(-1)
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.colors import LinearSegmentedColormap
from tympan.models.project import Project
from tympan.models.solver import Model
from tympan.models._common import Point3D
from tympan.altimetry import AltimetryMesh
from _util import input_int, import_xyz_angle_csv, import_infra, line_count, ask_xml_file, ask_input_file, moyenne_mesh, mesh_spect_to_ndarray, ndarray_to_mesh_spect, load_elements, run_calculations, ask_result_file


def create_calculations(fichier_xml, objects, output_xml):
    """
        Place the objects at the positions
        and save the project in output_xml file
    """

    # load project
    project = Project.from_xml(fichier_xml, verbose=False)
    
    # altimetry must be updated to get right the source position.z
    altim = AltimetryMesh.from_site(project.site)
    project.update_site_altimetry(altim)
    
    # Get the calculation
    Model.from_project(project, set_sources=True, set_receptors=True)

    # Get the initial number of computations in the project:
    nb_computations = len(project.computations)
    nb_points = 0
    # Get data from the list of objects
    for object_type, object_xml, object_positions in objects:
        # Import element (engine or building) from XML file:
        elements = import_infra(object_xml, object_type)
        element = elements.engines[0] if object_type == "engine" else elements.buildings[0]
        element_name = element.name

        # Get the mesh
        meshes = project.meshes

        # Retrieve positions from CSV file and direction vect and
        # add the element and do the calculation at each step
        position_x, position_y, position_z, position_angle = import_xyz_angle_csv(object_positions, project)

        # Loop on points:
        point = 0
        print ("Number of points in "+object_positions+": "+str(len(position_x)))
        if nb_points != 0 and nb_points != len(position_x):
            print("Error, the number of points in CSV files should be the same !")
            sys.exit(-1)
        nb_points = len(position_x)
        for point in range(nb_points):

            x = position_x[point]
            y = position_y[point]
            z = position_z[point]
            angle = position_angle[point]

            # Create the position for the element on the current position:
            pos_element = Point3D()
            pos_element.set_x(x)
            pos_element.set_y(y)
            pos_element.set_z(z)

            rot_element = Point3D()
            rot_element.set_z(angle) # rotation sur l'axe z

            # Add a computation if necessary
            num_computation = nb_computations + point
            if num_computation >= len(project.computations):
                # New computation needed
                project.add_computation()
                print("Add computation number ", num_computation)
                last_calc = project.computations[num_computation]
                last_calc.add_noise_map(meshes[0])
                project.select_computation(last_calc)
                project.current_computation.set_name('Position {0}'.format(point))
            else:
                last_calc = project.computations[num_computation]
                project.select_computation(last_calc)
                print("Select computation number ",)

            # select receptors in the current calculation
            for rec in project.user_receptors:
                last_calc.addReceptor(rec)

            # Add a element at the current position pos_element
            site = project.site
            # Change name
            element.setName(element_name+" at position "+str(point))
            if object_type == "engine":
                print("Adding an engine...")
                site.add_engine(element, pos_element, rot_element, 0.)
            elif object_type == "building":
                print("Adding a building...")
                site.add_building(element, pos_element, rot_element, 0.)
            else:
                print("Error: Unknown object type: ", object_type)
                sys.exit(-1)
            print("Point ",point," :", x, y, angle)

    project.to_xml(output_xml)
    print('Project saved to', output_xml)


def get_list_mesh_spectrums(project):
    """
        Return a list of the noise_maps associated to a mesh from
        different calculus of the same project
    """
    list_of_mesh_spectrums = list()
    meshes = project.meshes
    # Check the number of meshes
    if len(meshes) > 1:
        print('Error, more than one mesh detected!')
        sys.exit(-1)

    for calc in project.computations:
        list_of_mesh_spectrums.append(calc.get_noise_map_spectrums(meshes[0]))
    
    return list_of_mesh_spectrums


def main(fichier_xml, objects, output_xml, gui=True):

    # Place the objects at the defined positions in different calculations
    # and save the new project to output_xml
    create_calculations(fichier_xml, objects, output_xml)
        
    # Run all calculations
    project = run_calculations(output_xml)
    
    # Retrieve the noise maps of the project and put the spectrum value into a ndarray
    list_mesh_spec = get_list_mesh_spectrums(project)
    list_array_spec = list()
    for m in list_mesh_spec:
        list_array_spec.append(mesh_spect_to_ndarray(m))
    
    # Calculation of mean value and transform it into a spectrum noise map
    array_moy = moyenne_mesh(list_array_spec)
    list_spec_finale = ndarray_to_mesh_spect(array_moy)
    
    # Retrieve the meshes
    meshes = project.meshes
    
    # Add a new computation
    project.add_computation()
    # Add a new noise map to the new computation
    last_calc = project.computations[-1]
    last_calc.add_noise_map(meshes[0])
    
    # Set the current computation to the last one and set the noise map spectrums
    project.select_computation(last_calc)
    project.current_computation.set_name('Average')
    project.current_computation.set_noise_map_spectrums(meshes[0], list_spec_finale)

    # select receptors in the current calculation
    for rec in project.user_receptors:
        last_calc.addReceptor(rec)

    # Save to output_xml                   
    project.to_xml(output_xml)
    print('Result saved to', output_xml)

    # Plot (by default)
    if gui == True:
        plot(output_xml)


def tympan(output_xml):
    """
        Extract results for the plot
    """
    global calc_number
    project = Project.from_xml(output_xml)
    max_calc_number = len(project.computations)
    calc_name = ""
    while not calc_name.startswith("Position") and not calc_name.startswith("Average"):
        calc_number += 1
        # Reset
        if calc_number >= max_calc_number:
            calc_number = 1
        calc = project.computations[calc_number]
        calc_name = calc.name
    # Select the computation
    project.select_computation(calc)
    csv_file="map"+str(calc_number)+".csv"
    mesh = project.meshes[0]
    # Export the map:
    mesh.export_csv(csv_file)

    # Read the map:
    x,y,z,values = np.loadtxt(csv_file, skiprows=1, delimiter=";").T # Transposed for easier unpacking
    nrows, ncols = 38, 37
    data = values.reshape((nrows, ncols))
    # Show sources:
    src_x = []
    src_y = []
    model = Model.from_project(project)
    for src in model.sources:
        src_x.append(src.position.x)
        src_y.append(src.position.y)
    # Select unit:
    unit = 'dBA' if mesh.getDataType==0 else 'dBZ'
    print("calc_number ",calc_number)
    print(calc.name)
    print("Spectre min=", values.min(), " max=", values.max())
    #print(data)
    return x,y,z,data,src_x,src_y,"Calculation: "+calc.name,unit


def update(calc_number, im, title, srcs, output_xml):
    """ Function to update the map """
    x,y,z,data,src_x,src_y,title_name,unit = tympan(output_xml)
    im.set_data(data)
    title.set_text(title_name)
    srcs.set_data(src_x, src_y)
    return im,title,


def plot(output_xml):
    """ Plot animation
    Use matplotlib to show noise map
    """
    global calc_number
    calc_number = -1
    x, y, z, data, src_x, src_y, title_name, unit = tympan(output_xml)
    calc_number -= 1
    fig, points = plt.subplots()
    srcs, = points.plot(src_x, src_y, "bo")
    # Colormap:
    colors = [(0, 1, 0), (1, 1, 0), (1, 0, 0)]  # R -> G -> B
    cmap = LinearSegmentedColormap.from_list("TympanColorMap", colors, 10)
    ax = plt.axes()
    plt.xlabel('x (m)')
    plt.ylabel('y (m)')
    title = ax.text(.35, 1.05, '', transform=ax.transAxes, va='center')
    title.set_text(title_name)
    im = plt.imshow(data, extent=(x.min(), x.max(), y.min(), y.max()), interpolation='bicubic', animated=True, cmap=cmap,
                    vmin=40, vmax=80)
    # Colorbar
    plt.colorbar().ax.set_title(unit)
    # Animation
    ani = animation.FuncAnimation(fig, update, 1, fargs=(im, title, srcs, output_xml), interval=500)

    # Mpeg (optional)
    mpeg = False
    if mpeg == True:
        ani.save('Tympan.mpeg', writer='avconv')

    # Show the plot
    plt.show()


if __name__ == '__main__':

    # Get project file name and verify user input
    fichier_xml = ask_xml_file("Enter project file name (with .xml extension) : ")

    # List of added moving objects:
    objects = []
    positions_count = 0
    choice = -1
    while choice != 2:
        print('Select choice in the list below :')
        print('0. Add a moving engine (defined in a XML file)')
        print('1. Add a moving building (defined in a XML file)')
        print('2. End adding object')
        choice = input_int("Enter choice (0,1,2) : ")
        if choice <= 1:
            object_type = "engine" if choice == 0 else "building"
            object_file = ask_xml_file("Enter XML file containing the "+object_type+" (with xml extension) : ", object_type)
            # Ask for trajectory and check the number of positions
            # Check the number of lines in object_positions_file are the same
            positions_file = ask_input_file("Enter CSV file containing positions of the " + object_type + " (with csv extension) : ")
            new_positions_count = line_count(positions_file)
            if len(objects) == 0 or new_positions_count == positions_count:
                valid = True
                positions_count = new_positions_count
                objects.append([object_type, object_file, positions_file])
            else:
                print("Error! Number of positions in the file (",str(new_positions_count),") is not the same than expected: ",str(positions_count))
                print("Object "+object_type+" is not added...")

    # Name result xml file
    output_xml = ask_result_file()
    
    # Launch main
    main(fichier_xml, objects, output_xml)
