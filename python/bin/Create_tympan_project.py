# -*- coding: utf-8 -*-


import os

from tympan.models.project import Project

if __name__ == "__main__":
	print("Tentative de création d'un projet TYMPAN ex nihilo")
	proj = Project(Project.create())
	proj.create()
	proj.to_xml("toto.xml")
	print ("That's all folks !")
