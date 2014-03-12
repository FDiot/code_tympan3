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


#ifndef __TYINPROFACESELECTOR__
#define __TYINPROFACESELECTOR__

class TYANIME3DFaceSelector
{
public:
    TYANIME3DFaceSelector(const TYSiteNode& Site);
    virtual ~TYANIME3DFaceSelector();

    /*!
     * \fn bool exec(TYStructSurfIntersect* tabPolygon, unsigned int& tabPolygonSize);
     * \brief buil list of faces used by the ray tracer and the acoustic solver
     */
    bool exec(TYStructSurfIntersect *&tabPolygon, size_t& tabPolygonSize);

private :
    /*!
    * \fn bool buildCalcStruct(TYStructSurfIntersect* _tabPolygon)
    * \brief Construit un tableau contenant la structure regroupant l'ensemble des informations relatives a une face/triangle du site.
    * Correspond a la fonction du meme nom dans le SolveurHarmonoise avec l'ajout des normales et des faces triangulees dans la structure.
    * \param site Reference vers le site de la scene
    */
    bool buildCalcStruct(TYStructSurfIntersect *&tabPolygon, size_t& tabPolygonSize);

    /*!
    * \fn bool triangulateConcavePolygon(TYStructSurfIntersect* _tabPolygon)
    * \brief Effectue une triangulation de l'ensemble des faces de la topographie et des infrastructures.
    * \return Renvoie vrai si les triangulations se sont bien passees.
    */
    bool triangulateConcavePolygon(TYStructSurfIntersect* tabPolygon, size_t tabPolygonSize);

protected :
    const TYSiteNode& _site;
};

#endif // __TYANIME3DFACESELECTOR__
