/*! \file site_functions.hpp

   Propose functions to get specific \c TYElement from a site, i.e. \c
   TYSiteNode. These \c TYElement will be used to fill entities related to the
   solver data model.

   Created on: 9 nov. 2012
       Author: Damien Garaud <damien.garaud@logilab.fr>
 */


#include <vector>

#include "Tympan/MetierSolver/DataManagerMetier/Site/TYSiteNode.h"

namespace tympan
{

//! Get the total number of points from a \c TYSiteNode.
inline unsigned int total_point_number(LPTYSiteNode site_ptr)
{
    return site_ptr->getTopographie()->number_of_vertices();
};

//! Get all points into a vector.
/*!
 \c TYTabPoint is a typedef of \c std::vector<TYPoint>.
 */
inline std::auto_ptr<std::deque<OPoint3D> > all_point(LPTYSiteNode site_ptr)
{
    // TYTabPoint tab_points = site_ptr->collectPointsForAltimetrie();
	std::auto_ptr<std::deque<OPoint3D> > p_points( new std::deque<OPoint3D>() );
	std::deque<OTriangle> triangles;
    site_ptr->getTopographie()->exportMesh(*p_points, triangles);
    return p_points;
};


//! Get all acoustic surfaces from a \c TYSiteNode.
/*! Note that \c TYTabAcousticSurfaceGeoNode is a typedef of \c
    std::vector<LPTYAcousticSurfaceGeoNode>. Moreover, \c
    TYAcousticSurfaceGeoNode is also a typdef of \c TYGeometryNode.
 */
std::vector<LPTYAcousticSurfaceGeoNode> get_acoustic_surfaces(LPTYSiteNode site_ptr,
                                                              bool with_ecran = false);


//! Get the number of acoustic surfaces.
inline unsigned int get_acoustic_surface_number(LPTYSiteNode site_ptr)
{
    return get_acoustic_surfaces(site_ptr).size();
};


//! Get the number of acoustic surfaces with 'ecran'.
inline unsigned int get_acoustic_surface_number_with_ecran(LPTYSiteNode site_ptr)
{
    return get_acoustic_surfaces(site_ptr, true).size();
};


//! Get \c TYMateriauConstruction from a \c TYGeometryNode (or a \c TYAcousticSurface).
/*! XXX \todo does not work well. Don't know if we use the right object.
 */
TYMateriauConstruction* get_material(TYTabAcousticSurfaceGeoNode surfaces);
} // namespace 'tympan'
