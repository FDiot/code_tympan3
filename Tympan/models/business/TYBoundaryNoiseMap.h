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

/*
 *
 *
 *
 *
 */

#ifndef __TY_BOUNDARY_NOISE_MAP__
#define __TY_BOUNDARY_NOISE_MAP__


#include "Tympan/models/business/geometry/TYRectangle.h"
#include "TYMaillage.h"


/**
 * \file TYBoundaryNoiseMap.h
 * \class TYBoundaryNoiseMap
 * \brief This class represents a polyline with a thickness. Acoustic receptors are sampled inside this region.
 * \version v 1.1
 * \date 2013/05/06
 * \author Projet_Tympan
 *
 */
class TYBoundaryNoiseMap: public TYMaillage
{
    OPROTODECL(TYBoundaryNoiseMap)
    TY_EXT_GRAPHIC_DECL_ONLY(TYBoundaryNoiseMap)
    TY_EXTENSION_DECL_ONLY(TYBoundaryNoiseMap)

    // Methods
public:
    /**
     * \brief Constructor.
     * TYBoundaryNoiseMap constructor
     */
    TYBoundaryNoiseMap();

    /**
     * \brief Copy constructor.
     * TYBoundaryNoiseMap copy constructor
     */
    TYBoundaryNoiseMap(const TYBoundaryNoiseMap& other);

    /**
     * \brief Destructor.
     * TYBoundaryNoiseMap destructor
     */
    virtual ~TYBoundaryNoiseMap();

    ///Operator =.
    TYBoundaryNoiseMap& operator=(const TYBoundaryNoiseMap& other);

    ///Operator ==.
    bool operator==(const TYBoundaryNoiseMap& other) const;

    ///Operator !=.
    bool operator!=(const TYBoundaryNoiseMap& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    bool toXML(const std::string& sFilePath);

    bool fromXML(const std::string& sFilePath);

    std::string toXMLString();

    bool fromXMLString(const std::string& sXMLString);


    /**
     * \brief Clear result.
     */
    virtual void clearResult();

    /**
     * \fn virtual void make(const TYTabPoint &tabPoints, double thickness, bool closed, double density);
     * \brief Build the table of \c TYPointCalcul around the polyline.
     * \param tabPoints Array of points.
     * \param thickness The thickness of the polyline.
     * \param closed If the polyline is closed.
     * \param density Density of points (number of points per meter).
     */
    virtual void make(const TYTabPoint& tabPoints,
                      double thickness,
                      bool closed,
                      double density = TY_MAILLAGE_DEFAULT_DENSITE
                     );

    /**
     * \fn void computeBoundingBox(double &box_x_min, double &box_x_max, double &box_y_min, double &box_y_max) const;
     * \brief Compute the bounding box of the polyline.
     * \param box_x_min lower left coordinate.
     * \param box_x_max lower right coordinate.
     * \param box_y_min higher left coordinate.
     * \param box_y_max higher right coordinate.
     */
    void computeBoundingBox(double& box_x_min, double& box_x_max, double& box_y_min, double& box_y_max) const;

    /**
     * \fn void computePoints(double box_x_min, double box_x_max, double box_y_min, double box_y_max);
     * \brief Compute the calcul points.
     * \param box_x_min lower left coordinate.
     * \param box_x_max lower right coordinate.
     * \param box_y_min higher left coordinate.
     * \param box_y_max higher right coordinate.
     */
    void computePoints(double box_x_min, double box_x_max, double box_y_min, double box_y_max);

    /**
     * \fn double getDensity()
     * \brief Return the density of points.
     * \return _density
     */
    double getDensity() const { return _density; }

    /**
     * \fn double getThickness()
     * \brief Return the thickness of the polyline.
     * \return _thickness
     */
    double getThickness() const { return _thickness; }

    //! Is polyline closed?
    /*!
      \return True if the polyline is closed.
     */
    bool isClosed() const { return _closed; }

    /**
     * \brief Return the position that will be used for the legend/label display
     * \return A const reference to a point.
     */
    const TYPoint& getPosLabel() { return _posLabel; }

    /**
     * \brief Return the points defining the polyline
     * \return A const reference to the points.
     */
    const TYTabPoint& getTabPoint() const { return _tabPoint; }

    /**
     * \brief Return the points defining the polyline
     * \return A reference to the points.
     */
    TYTabPoint& getTabPoint() { return _tabPoint; }

    /**
     * \brief Return the indices of calcul points
     * \return A const reference to the points.
     */
    const std::vector<int>& getPtsIndices() const { return _ptsIndices; }

    /**
     * \brief Return the indices of calcul points
     * \return A reference to the points.
     */
    std::vector<int>& getPtsIndices() { return _ptsIndices; }

    /**
     * \brief Return number of points in column, for the 2D array
     * \return The number of points in y.
     */
    int getNbPointsY() const { return _nbPointsY; }

    /**
     * \brief Return the minimum density to get a correct sampling
     * \param thickness The thickness to test
     * \return The minimum density.
     */
    static double computeMinimumDensity(double thickness);

    /**
     * \brief Return the index
     * \param x Row value
     * \param y Column value
     * \return The index of point calcul
     */
    virtual int getIndexPtCalcul(int x, int y) const;

    /**
     * \brief Return the dimensions in x and y
     * \param x Dimension x
     * \param y Dimension y
     */
    virtual void getDimensions(int& x, int& y) const;

    /**
     * \brief Getter of _canBeClosed.
     * \return _canBeClosed
     */
    bool getCanBeClosed() const { return _canBeClosed; }

    /**
     * \brief Setter of _canBeClosed.
     * \param canBeClosed If the poyline can be closed or not.
     */
    void setCanBeClosed(bool canBeClosed) { _canBeClosed = canBeClosed; }

    // Members
protected:
    /// The position of the label.
    TYPoint _posLabel;

    /// Density of points (number of points per meter).
    double _density;

    /// Thickness of the polyline (in meters).
    double _thickness;

    /// If the polyline is closed.
    bool _closed;

    /// The tab point defining the polyline.
    TYTabPoint _tabPoint;

    /// Array of points indices : -1 means the point doesn't exist.
    std::vector<int> _ptsIndices;

    /// Number of points in column y (rectangular bounding box).
    int _nbPointsY;

    /// If the polyline can be closed.
    bool _canBeClosed;
};

///Smart Pointer on TYBoundaryNoiseMap.
typedef SmartPtr<TYBoundaryNoiseMap> LPTYTYBoundaryNoiseMap;

///TYBoundaryNoiseMap geometry node.
typedef TYGeometryNode TYBoundaryNoiseMapGeoNode;

///Smart Pointer on TYBoundaryNoiseMapGeoNode.
typedef SmartPtr<TYBoundaryNoiseMapGeoNode> LPTYBoundaryNoiseMapGeoNode;

///Vector of geometry nodes of TYBoundaryNoiseMap.
typedef std::vector<LPTYBoundaryNoiseMapGeoNode> TYTabBoundaryNoiseMapGeoNode;


#endif // __TY_BOUNDARY_NOISE_MAP__
