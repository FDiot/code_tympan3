# -*- coding: utf-8 -*-
'''
Created on 6 janv. 2017

@author: t.legall

Graphic interface to do calculations between on a surface
receptor between two computations.
'''
import sys
if sys.version_info[:2] <= (2, 7):
    print("Error! Python 3 only can be used.")
    sys.exit(-1)
import tkinter as tk
from tkinter.filedialog import askopenfile
from tympan.models.project import Project
from _util import mesh_spect_to_ndarray, ndarray_to_mesh_spect, operation_array, MyDialog


class App_rec_surf(tk.Tk):
    """
    Class creating main graphic window
    """
    def __init__(self, parent):
        tk.Tk.__init__(self,parent)
        self.parent = parent
        self.initialize()
        
    def initialize(self):
        # We use a grid to position the widgets
        self.grid
        
        # ============================================================================================================
        
        # LabelFrame for section "choose XML file"
        LF_xml = tk.LabelFrame(self, text=u"Choix du fichier XML", padx=0, pady=0)
        LF_xml.grid(column=0,row=0,columnspan=3,sticky='EW')
        LF_xml.grid_columnconfigure(0,weight=1)
        
        # Label used to show the xml file path
        self.labelVariable = tk.StringVar()
        label_file = tk.Label(LF_xml,textvariable=self.labelVariable,anchor="w",fg="blue",bg="white")
        label_file.grid(column=0,row=1,columnspan=1,in_=LF_xml,sticky='EW')
        self.labelVariable.set(u"Salut ! Charge donc un fichier !")
        
        # button used to charge xml file
        button_charge_xml = tk.Button(LF_xml,text=u"Charger fichier XML", anchor="e", command=self.loadXmlFile)
        button_charge_xml.grid(column=1,row=1,in_=LF_xml,sticky='E')
        
        # ============================================================================================================
        
        # LabelFrame for section "choose surface receptor"
        LF_surf_rec = tk.LabelFrame(self, text=u"Récepteur surfacique", padx=0, pady=0)
        LF_surf_rec.grid(column=0,row=1,columnspan=1,sticky='EW')
        LF_surf_rec.grid_columnconfigure(0,weight=1)
        
        # Label surface receptor chosen
        self.labelRecSurf = tk.StringVar()
        rec_surf_chosen = tk.Label(LF_surf_rec,textvariable=self.labelRecSurf,anchor="w",fg="blue",bg="grey")
        rec_surf_chosen.grid(column=0,row=0,columnspan=2,in_=LF_surf_rec,sticky='EW')
        
        # Listbox with surface receptor names
        self.list_rec_surf = tk.Listbox(LF_surf_rec,fg="blue")
#        self.list_rec_surf.pack(fill=tk.X, expand=1)
        self.list_rec_surf.grid(column=0,row=1,in_=LF_surf_rec,sticky='EW')
        
        # OK button pressed when surface receptor is chosen
        button_rec_surf = tk.Button(LF_surf_rec,text=u"OK",command=self.clickButtonRecSurf)
        button_rec_surf.grid(column=1,row=1,in_=LF_surf_rec)
        
        # ============================================================================================================
        
        # LabelFrame for section "choose computation n°1"
        LF_ref_comp = tk.LabelFrame(self, text=u"Calcul n°1 (L1)", padx=0, pady=0)
        LF_ref_comp.grid(column=1,row=1,columnspan=1,sticky='EW')
        LF_ref_comp.grid_columnconfigure(0,weight=1)
        
        # Label reference computation chosen
        self.labelRefComp01 = tk.StringVar()
        ref_comp_01_chosen = tk.Label(LF_ref_comp,textvariable=self.labelRefComp01,anchor="w",fg="blue",bg="grey")
        ref_comp_01_chosen.grid(column=0,row=0,columnspan=2,sticky='EW')
        
        # First Listbox with computation names
        self.list_comp_01 = tk.Listbox(LF_ref_comp,fg="blue")
#        self.list_comp_01.pack(fill=tk.X, expand=1)
        self.list_comp_01.grid(column=0,row=1,sticky='EW')
        
        # OK button pressed when reference computation is chosen
        button_comp_01 = tk.Button(LF_ref_comp,text=u"OK",command=self.clickButtonComp01)
        button_comp_01.grid(column=1,row=1)
        
        # ============================================================================================================
        
        # LabelFrame for section "choose computation n°2"
        LF_comp = tk.LabelFrame(self, text=u"Calcul n°2 (L2)", padx=0, pady=0)
        LF_comp.grid(column=2,row=1,columnspan=1,sticky='EW')
        LF_comp.grid_columnconfigure(0,weight=1)
        
        # Label other computation chosen
        self.labelRefComp02 = tk.StringVar()
        ref_comp_02_chosen = tk.Label(LF_comp,textvariable=self.labelRefComp02,anchor="w",fg="blue",bg="grey")
        ref_comp_02_chosen.grid(column=0,row=0,columnspan=2,sticky='EW')
        
        # Second Listbox with computation names
        self.list_comp_02 = tk.Listbox(LF_comp,fg="blue")
#        self.list_comp_02.pack(fill=tk.X, expand=1)
        self.list_comp_02.grid(column=0,row=1,sticky='EW')
        
        # OK button pressed when other computation is chosen
        button_comp_02 = tk.Button(LF_comp,text=u"OK",command=self.clickButtonComp02)
        button_comp_02.grid(column=1,row=1)
        
        # ============================================================================================================
        
        # LabelFrame for section "choose operation"
        LF_ope = tk.LabelFrame(self, text=u"Choix opération", padx=0, pady=0)
        LF_ope.grid(column=0,row=4,columnspan=1,sticky='EW')
        LF_ope.grid_columnconfigure(0,weight=1)
        
        # RadioButtons corresponding to the proposed operations
        # initializing choice
        self.v = tk.IntVar()
        self.v.set(0)

        self.operations_name = ['difference', 'emergence', 'bruit ambiant']
        operations = [
            (u"Différence : L2 - L1",0),
            (u"Emergence : L1 \u2295 L2 - L2 ",1),
            (u"Bruit ambiant : L1 \u2295 L2",2)
        ]
        
        for txt, val in operations:
            tk.Radiobutton(LF_ope, 
                text=txt,
                padx = 20, 
                variable=self.v,
                value=val).pack(anchor=tk.W)
        
        
        # ============================================================================================================
        
        # LabelFrame for section "launch calculation"
        LF_calc = tk.LabelFrame(self, text=u"Lancer calcul", padx=0, pady=0)
        LF_calc.grid(column=1,row=4,columnspan=3,sticky='EW')
        LF_calc.grid_columnconfigure(0,weight=1)
        
        # Button pressed to launch calculation between the 2 computation results
        button_launch_calc = tk.Button(LF_calc,text=u"LANCER CALCUL !",command=self.clickLaunchCalc)
        button_launch_calc.grid(column=1,row=0,sticky='W')
        
        # Label progress
        self.labelProg = tk.StringVar()
        prog = tk.Label(LF_calc,textvariable=self.labelProg,anchor="w",fg="red",bg="grey")
        prog.grid(column=0,row=0,sticky='EW')
        
        # ============================================================================================================
                
        # Geometry options of the window
        self.grid_columnconfigure(0,weight=1)
        self.resizable(False,False)

    def loadXmlFile(self):
        """
        Load TYMPAN xml file, set the meshes (récepteurs
        surfaciques)list and the computations list
        """
        # Open dialog window to choose XML file
        self.xml_file = askopenfile(title=u"Ouvrir XML",filetypes=[('xml files','.xml'),('all files','.*')])
        # Set label variable in principal window
        self.labelVariable.set(self.xml_file.name)
        # Set project
        self.project = Project.from_xml(self.xml_file.name)
        
        # Get meshes
        rec_surf = self.project.meshes
        for i in range(len(rec_surf)):
            self.list_rec_surf.insert(i, u"Récepteur surfacique n°"+str(i+1))
        
        # Get computations   
        comps = self.project.computations
        for i in range(len(comps)):
            self.list_comp_01.insert(i, comps[i].name)
        for i in range(len(comps)):
            self.list_comp_02.insert(i, comps[i].name)
        
        # Set computations and mesh labels
        self.labelRecSurf.set(u"Choisis un récepteur surfacique...")
        self.labelRefComp01.set(u"Choisis le calcul n°1...")
        self.labelRefComp02.set(u"Choisis le calcul n°2...")

    def clickButtonRecSurf(self):
        """
        OK button callback function to set chosen mesh
        """
        mhs = self.project.meshes
        num_mesh = self.list_rec_surf.index(tk.ACTIVE)
        self.mesh = mhs[num_mesh]
        nom_mesh = self.list_rec_surf.get(tk.ACTIVE)
        self.labelRecSurf.set(u"RECEPTEUR CHOISI : "+nom_mesh)

    def clickButtonComp01(self):
        """
        OK button callback function to set chosen first computation
        """
        # Get chosen computation
        num_c_01 = self.list_comp_01.index(tk.ACTIVE)
        comps = self.project.computations
        self.comp_01 = comps[num_c_01]
        
        # Set chosen computation name
        nom_comp = self.list_comp_01.get(tk.ACTIVE)
        self.labelRefComp01.set(u"CALCUL N°1 : "+nom_comp)

    def clickButtonComp02(self):
        """
        OK button callback function to set chosen second computation
        """
        # Get chosen computation
        num_c_02 = self.list_comp_02.index(tk.ACTIVE)
        comps = self.project.computations
        self.comp_02 = comps[num_c_02]
        
        # Set chosen computation name
        nom_comp = self.list_comp_02.get(tk.ACTIVE)
        self.labelRefComp02.set(u"CALCUL N°2 : "+nom_comp)


    def clickLaunchCalc(self):
        """
        Launch calculation Once every parameters are set 
        """
        # Set progression label
        self.labelProg.set(u"Calcul en cours...")
        
        # Verify if computations and mesh were chosen
        try:
            self.project.select_computation(self.comp_01)
        except:
            d = MyDialog(self, u"Tu n'as pas choisi le calcul n°1 !")
            self.wait_window(d.top)
            return
        
        try:
            mesh_spectrums_01 = self.comp_01.get_noise_map_spectrums(self.mesh)
        except:
            d = MyDialog(self, u"Tu n'as pas choisi de récepteur surfacique !")
            self.wait_window(d.top)
            return
        else:
            ndarray_01 = mesh_spect_to_ndarray(mesh_spectrums_01)
        
        try:
            self.project.select_computation(self.comp_02)
        except:
            d = MyDialog(self, u"Tu n'as pas choisi le calcul n°2 !")
            self.wait_window(d.top)
            return
            
        try:
            mesh_spectrums_calc = self.comp_02.get_noise_map_spectrums(self.mesh)
        except:
            d = MyDialog(self, u"Tu n'as pas choisi de récepteur surfacique !")
            self.wait_window(d.top)
            return
        else:
            ndarray_02 = mesh_spect_to_ndarray(mesh_spectrums_calc)
        
            # Get the operation value from the radio buttons and calculate the result array
            val_ope = self.v
            ndarray_result = operation_array(ndarray_01, ndarray_02, val_ope.get())
            
            # Create new mesh spectrum with the result array
            mesh_spectrums_result = ndarray_to_mesh_spect(ndarray_result)
        
            # Add a new computation
            self.project.add_computation()
            calc_list =  [calc for calc in self.project.computations]
            last_calc = calc_list[-1]
        
            # Add a new noise map to the new computation
            last_calc.add_noise_map(self.mesh)
        
            # Set the current computation to the last one and set the noise map spectrums
            self.project.select_computation(last_calc)
            result_name = 'Resultat de ' + self.operations_name[val_ope.get()] + " on " + self.comp_01.name + " and " + self.comp_02.name
            self.project.current_computation.set_name(result_name)
            self.project.current_computation.set_noise_map_spectrums(self.mesh, mesh_spectrums_result)

            # Create xml file and save result
            output_xml = 'Resultat.xml'

            # Set progress label to done (Terminé !)
            #self.labelProg.set(u"Terminé !")
            self.labelProg.set(u" "+result_name + " sauve dans " + output_xml)

if __name__ == "__main__":
    app = App_rec_surf(None)
    app.title(u'Code_TYMPAN : Calculs sur les récepteurs surfaciques')
    app.mainloop()