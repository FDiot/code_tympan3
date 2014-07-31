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

#ifndef __TY_ALTIMETRIE__
#define __TY_ALTIMETRIE__

#include <deque>

#include <gtest/gtest_prod.h>

#include "Tympan/models/business/geometry/TYGeometryNode.h"
#include "Tympan/models/business/geometry/TYPolygon.h"
#include "Tympan/models/business/exceptions.h"
#include "Tympan/models/common/delaunay_maker.h"
#include "Tympan/models/common/3d.h"

/**
 * \class TYAltimetrie
 * \brief Assigne une altitude a chaque point de l'espace.
 * \author Projet_Tympan
 */
class TYAltimetrie: public TYElement
{
    OPROTODECL(TYAltimetrie)
    TY_EXTENSION_DECL_ONLY(TYAltimetrie)
    TY_EXT_GRAPHIC_DECL_ONLY(TYAltimetrie)

    // Methodes
public:
    static const double invalid_altitude;
    /**
     * Constructeur.
     */
    TYAltimetrie();

    /**
     * Constructeur par copie.
     */
    TYAltimetrie(const TYAltimetrie& other);

    /**
     * Destructeur.
     */
    virtual ~TYAltimetrie();

    ///Operateur =.
    TYAltimetrie& operator=(const TYAltimetrie& other);

    ///Operateur ==.
    bool operator==(const TYAltimetrie& other) const;

    ///Operateur !=.
    bool operator!=(const TYAltimetrie& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    //  XXX is being refactored to enable plugin back AltimetryBuilder result.
    //    /**
    //     * Calcul l'altimetrie a partir d'une collection de points.
    //     * L'altimetrie est le resultat de la triangulation de Delaunay calculee
    //     * a partir des points passes.
    //     *
    //     * @param points Les points pour calculer l'altimetrie.
    //     */
    //    virtual void compute(const TYTabPoint& points, const double& delaunay);

    //  XXX Is being replaced by the new triangulation builder
    //      from the SolverDataModel component
    //    /**
    //     * Calcul l'altimetrie a partir d'une collection de points et de segments.
    //     * L'altimetrie est le resultat de la triangulation de Delaunay contrainte calculee
    //     * a partir des points passes et des segments.
    //     *
    //     */
    //    void computeWithConstraint(OConstraintDelaunayMaker& oConstraintDelaunayMaker);

    /**
     * @brief plug back triangulation providfed by the TYTopographie
     *
     * This function expect to be passed the deques of points and triangles
     * computed by TYTopographie::computeAltimetricTriangulation, which itself
     * calls the new tympan::AltimetryBuilder.
     *
     * @param points the vertices of the triangulation
     * @param triangles the faces of the triangulation
     */
    void plugBackTriangulation(
        const std::deque<OPoint3D>& points,
        std::deque<OTriangle>& triangles);

    /**
     * Set/Get de la liste des faces.
     */
    TYTabLPPolygon& getListFaces() { return _listFaces; }

    /**
     * Set/Get de la liste des faces.
     */
    const TYTabLPPolygon& getListFaces() const { return _listFaces; }

    /**
     * Set/Get de la liste des faces.
     */
    void setListFaces(const TYTabLPPolygon& list) { _listFaces = list; }

    /**
     * Ajoute une face a la liste des faces.
     */
    bool addFace(LPTYPolygon pFace);

    /**
     * Supprime une face a la liste des faces.
     */
    bool remFace(const LPTYPolygon pFace);

    /**
     * Supprime une face a la liste des faces a partir de son ID.
     */
    bool remFace(QString idFace);

    /**
     * Retourne une face de la liste des faces.
     */
    LPTYPolygon getFace(int index) { return _listFaces[index]; }
    const TYPolygon* getFace(int index) const { return _listFaces[index]; }

    /**
     * Calcule l'altitude d'un point de l'espace.
     * La coordonee Z du point est mise a jour si le point est situe dans
     * la zone de definition de l'altimetrie.
     *
     * @return <code>false</code> si l'altitude du point n'a pu etre
     *         determinee; <code>false</code> sinon.
     */
    double altitude(const OPoint3D& pt);

    /**
     * \brief Modifie l'altitude d'un point donn�. Si le point est hors de la zone dans laquelle l'altim�trie est d�finie, la valeur z du point est mise a \c TYAltimetry::invalid_altitude
     *
     *
     * \return false si l'altitude du point n'a pu etre determinee
     */
    bool updateAltitude(OPoint3D& pt) const;

    /**
     * \brief Calcule les coordonnees de la projection au sol d'un point de l'espace
     * \return les coordonnees du pt d'intersection
     */
    OPoint3D projection(const OPoint3D& pt) const;

    /**
     * Retourne la hauteur moyenne des points passes en arguments.
     */
    double HauteurMoyenne(TYTabPoint& pts);

    /**
     * Retourne la premiere (ordre de parsing du tableau passes en arguments)
     * hauteur positive ou nulle des points ptsIn.
     */
    double PremiereHauteurPositiveOuNulle(TYTabPoint& ptsIn);

    /**
     * \brief Return the face under a point
     * \fn LPTYPolygon getFaceUnder(OPoint3D pt);
     */
    LPTYPolygon getFaceUnder(OPoint3D pt);

    /**
     * \brief find a list of triangle partialy or totaly included in a box
     * \brief A return value equal 0 means all triangle near the boxes are included
     * \fn unsigned int getFacesInBox(OBox2 box, TYTabLPPolygon& tabPolygon)
     */
    unsigned int getFacesInBox(const OBox2& box, TYTabLPPolygon& tabPolygon);

    /**
     * \brief find a list of point included in a box defined by four points
     * \brief Value returned is the number of point
     * \fn unsigned int getPointsInBox(const OPoint3D& pt0, const OPoint3D& pt1, const OPoint3D& pt2, const OPoint3D& pt3, TYTabPoint& tabPolygon)
     */
    unsigned int getPointsInBox(const OPoint3D& pt0, const OPoint3D& pt1, const OPoint3D& pt2, const OPoint3D& pt3, TYTabPoint& tabPolygon);

    /** \brief Integer coordinates into the grid */
    struct grid_index { unsigned pi, qi; } ;

protected:
    FRIEND_TEST(TYAltimetryTest, dummy_grid);
    FRIEND_TEST(TYAltimetryTest, simple_grid);
    FRIEND_TEST(TYAltimetryTest, simple_terrain);

    /**
     * \brief Select indices of faces to test
     * \fn bool getGridIndices(const OPoint3D& pt, int* indXY)
     */
    bool getGridIndices(const OPoint3D& pt, grid_index& indXY) const;

    /**
     * \brief Select indices of faces to test
     * \fn bool getGridIndices(const OPoint3D* pts, int* indXY)
     */
    bool getGridIndices(const OPoint3D* pts, unsigned int* iMinMax) const;

    /**
     * \brief Select indices of faces to test
     * \fn bool getGridIndices(const OPoint3D& pt, int* indXY)
     */
    bool getGridIndices(const OBox2& box, unsigned int* iMinMax) const;

    /**
     * \brief Select faces in the interval minX, maxX, minY, maxY
     * \fn void getFacesinIndices(unsigned int& minX, unsigned int&maxX, unsigned int&minY, unsigned int&maxY, TYTabLPPolygon& faces);
     */
    void getFacesinIndices(unsigned int& minX, unsigned int& maxX, unsigned int& minY, unsigned int& maxY, TYTabLPPolygon& faces);

private :
    inline bool IsInsideFace(const TYTabPoint& pts, OPoint3D& pt) const;

    // Membres
protected:
    ///Liste des polygones correspondant aux faces de cet altimetrie.
    TYTabLPPolygon _listFaces;

    /// Bounding Box 2D de l'altimetrie
    OBox             _bbox;


    // The members below handle the accelerating grid : this could / should be a disctinc class

    /// \brief Initilise the grid related attributes for a null grid
    void initNullGrid();

    /// \brief clean the accelerating structure
    void clearAcceleratingGrid();

    /// \brief initialise the accelerating structure given current _bbox and _gridS{XY}
    void initAcceleratingGrid(unsigned to_be_reserved = 0);

    /// \brief Clear the grid and reinitialise it as a copy of \c other
    void copyAcceleratingGrid(const TYAltimetrie& other);

    // Grille accélératrice contenant une liste de polygones pour chaque case
    TYTabLPPolygon** _pSortedFaces;

    /// Size along each dimension of the accelerating grid
    unsigned _gridSX;
    unsigned _gridSY;

    /// Step along each dimension of the accelerating grid
    double _gridDX;
    double _gridDY;
};


///Noeud geometrique de type TYAltimetrie.
typedef TYGeometryNode TYAltimetrieGeoNode;


#endif // __TY_ALTIMETRIE__
