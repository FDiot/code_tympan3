/*
 * Copyright (C) <2012> <EDF-R&D> <FRANCE>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef TYANIME3D_SETUP_H
#define TYANIME3D_SETUP_H

/**
* \namespace TYANIME3DSetUp
* \brief Functions to load solver parameters
*/
namespace ANIME3DSetup
{
    /*!
     * \brief set values
     */
    void exec();

    /*!
    * \fn virtual void initGlobalValues()
    * \brief  Parametres par defaut du lancer de rayons pour le solveur ANIME3D
    */
    void initGlobalValues();

    /*!
    * \fn virtual void loadParameters()
    * \brief  Lecture des parametres lancer de rayons pour le solveur ANIME3D dans le fichier ANIME3DRayTracerParameters.txt
    * \return Renvoie vrai si l'ensemble des operations se sont bien deroulees.
    */
    bool loadParameters();
	
    double getParam(char* ligne);
}

#endif //TYANIME3DRAYTRACERSETUP_H
