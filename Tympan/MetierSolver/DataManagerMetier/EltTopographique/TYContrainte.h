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

#ifndef __TY_CONTRAINTE__
#define __TY_CONTRAINTE__

#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYGeometryNode.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYPolygon.h"
#include "Tympan/MetierSolver/CommonTools/OPoint3D.h"

typedef std::pair<unsigned int, unsigned int> Edge;

/**
* Classe donnant l'interface pour donner une contrainte a l'altimetrie. Celle ci permet de donner des points fixes pour l'altimetrie par rapport � un calcul initial de la topographie
*/
class TYContrainte
{

public :
    /**
    * Constructeur
    */
    TYContrainte() { };

    /**
    * Constructeur par copie
    */
    TYContrainte(const TYContrainte& other);

    /**
    * Destructeur
    */
    virtual ~TYContrainte() { };

    /**
    * Reset de la contrainte
    */
    void purge() { pointsAltimetrie.clear(); enveloppes.clear();}

    /**
    * Ajoute un point fixe pour l'altimetrie
    */
    void addPoint(TYPoint& point)
    {
        double coords[3];
        point.getCoords(coords);
        pointsAltimetrie.push_back(OPoint3D(coords[0], coords[1], coords[2]));
    }

    /**
    * Ajoute un point fixe pour l'altimetrie
    */
    void addPoint(OPoint3D& point) { pointsAltimetrie.push_back(point); }

    /**
    * Ajoute un polygone decrivant l'enveloppe concave de la contrainte
    */
    void addEnveloppe(TYTabPoint& enveloppe) { enveloppes.push_back(enveloppe); }

    /**
    * Renvoie l'ensemble des points fixes pour l'altimetrie
    */
    std::vector<OPoint3D>& getPointsAltimetrie() { return pointsAltimetrie; }

    /**
    * Renvoie l'ensemble des polygones d�crivant l'enveloppe de la contrainte
    */
    std::vector<TYTabPoint>& getEnveloppe() { return enveloppes; }

    /**
    * Adjoute un segment � la Contrainte bas�e sur les vertex d�j� d�finis
    * warning : Les indices de vertex doivent �tre inf�rieur au nombre de vertex definis
    */
    bool addConstraint(Edge& edge)
    {
        if (edge.first >= pointsAltimetrie.size() ||
            edge.second >= pointsAltimetrie.size()) { return false; }
        contraintes.push_back(edge);
        return true;
    }

    /**
    * Renvoie l'ensemble des segments contraignant la topographie
    */
    std::vector<Edge>& getConstraints() { return contraintes; }

    /**
    * Fonction principale de la classe de contrainte. Celle ci calcul l'enveloppe de la contrainte ainsi que les points d'altitude.
    * Elle doit �tre r�impl�ment�e par chaque objet souhaitant contraindre la topographie.
    */
    virtual bool computeConstraint() = 0;

protected :

    std::vector<OPoint3D> pointsAltimetrie;      /*!< Ensemble de points fixant l'altimetrie */
    std::vector<Edge> contraintes;              /*!< Ensemble de segments d�finissant des contraintes bas�es sur les vertices d�finit par l'objet */
    std::vector<TYTabPoint> enveloppes;         /*!< Pour le moment non utilise par la triangulation. Ensemble de polygones decrivant l'enveloppe concave de la contrainte. Tous les points pr�sents dans l'altimetrie et inclus dans l'enveloppe devront etre supprimes */

};

#endif
