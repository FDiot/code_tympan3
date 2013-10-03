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
 
#ifndef _TY_ACOUSTIC_EDGE_
#define _TY_ACOUSTIC_EDGE_

#include "Tympan/MetierSolver/DataManagerCore/TYElement.h"
#include "Tympan/MetierSolver/DataManagerCore/TYDefines.h"          //Importation des TYPoint et TYTabPoint

/*!
* \class TYAcousticEdge
* \brief TYAcoustisEdge represente une arete de diffraction au sens du lancer de rayon. Elle est representee par un cylindre d'axe AB avec une epaisseur.
* \note Il est possible que la forme evolue au profit d'un Oriented Bounding Box pour alleger les calculs et faciliter le portage vers le GPU
*/
class TYAcousticEdge
{

public:

    /*!
    * \fn TYAcousticEdge()
    * \brief Constructeur par defaut. L'epaisseur est nulle et les point sont initialises par TYPoint();
    */
    TYAcousticEdge();

    /*!
    * \fn TYAcousticEdge(const TYPoint &_A, const TYPoint &_B, const double _epaisseur)
    * \brief Constructeur complet qui set entierement les attributs de l'arete.
    * \param _A : Premier point du segment decrivant l'axe de l'arete.
    * \param _B : Deuxieme point du segment decrivant l'axe de l'arete.
    * \param _epaisseur : epaisseur du cylindre.
    */
    TYAcousticEdge(const TYPoint& _A, const TYPoint& _B, const double _epaisseur);

    /*!
    * \fn TYPoint getFirstPoint()
    * \brief Renvoie le premier point du segment decrivant l'axe de l'arete.
    */
    TYPoint getFirstPoint() { return A;}

    /*!
    * \fn void setFirstPoint(const TYPoint &_A)
    * \brief Set le premier point du segment decrivant l'axe de l'arete.
    * \param _A : Nouveau premier point du segment.
    */
    void setFirstPoint(const TYPoint& _A) { A = TYPoint(_A);}

    /*!
    * \fn TYPoint getSecondPoint()
    * \brief Renvoie le deuxieme point du segment decrivant l'axe de l'arete.
    */
    TYPoint getSecondPoint() { return B;}

    /*!
    * \fn void setSecondPoint(const TYPoint &_B)
    * \brief Set le second point du segment decrivant l'axe de l'arete.
    * \param _B : Nouveau second point du segment.
    */
    void setSecondPoint(const TYPoint& _B) { B = TYPoint(_B);}

    /*!
    * \fn double getEpaisseur()
    * \brief Renvoie l'epaisseur du cylindre.
    */
    double getEpaisseur() { return epaisseur;}

    /*!
    * \fn void setEpaisseur(double _epaisseur)
    * \brief Set une nouvelle epaisseur pour le cylindre.
    * \param _epaisseur : Nouvelle epaisseur du cylindre.
    */
    void setEpaisseur(double _epaisseur) { epaisseur = _epaisseur;}

protected:
    TYPoint A;          /*!< Premier point du segment decrivant l'axe du cylindre */
    TYPoint B;          /*!< Second point du segment decrivant l'axe du cylindre */
    double epaisseur;   /*!< Epaisseur du cylindre modelisant l'arete de diffraction */
};

typedef std::vector<TYAcousticEdge> TYTabAcousticEdge;

#endif