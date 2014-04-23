/*! \file site_functions.cpp

   Propose functions to get specific \c TYElement from a site, i.e. \c
   TYSiteNode. These \c TYElement will be used to fill entities related to the
   solver data model.

   Created on: 9 nov. 2012
       Author: Damien Garaud <damien.garaud@logilab.fr>
 */

#include "site_functions.hpp"

namespace tympan
{

std::vector<LPTYAcousticSurfaceGeoNode> get_acoustic_surfaces(LPTYSiteNode site_ptr)
{
    TYTabAcousticSurfaceGeoNode tab_faces;
    unsigned int nb_faces_infra = 0;
    std::vector<bool> is_face_ecran_index;
    site_ptr->getListFaces(tab_faces, nb_faces_infra, is_face_ecran_index);
    return tab_faces;
};

TYMateriauConstruction* get_material(TYTabAcousticSurfaceGeoNode surfaces)
{
    // Try to get all surfaces (i.e. triangle).
    TYTabAcousticSurfaceGeoNode::const_iterator it_surface = surfaces.begin();
    int count = 0;
    for (; it_surface != surfaces.end(); ++it_surface, ++count)
    {
        cout << "Surface  no. " << count << endl;
        // Recuperation du tableau des points formant le contour
        TYAcousticPolygon* pPoly =
            TYAcousticPolygon::safeDownCast((*it_surface)->getElement());

        // Get points which define the polygon.
        TYTabPoint tab_p = pPoly->getContour();
        // Normal vector.
        OVector3D vec = pPoly->normal();
        TYTabPoint::const_iterator it_point = tab_p.begin();
        for (; it_point != tab_p.end(); ++it_point)
        {
            // Get (x, y, z) coordinates.
            cout << "  ("
                 << it_point->_x << ", "
                 << it_point->_y << ", "
                 << it_point->_z << ")" << endl;
        }
        // cout << "vec.getCoords(): " << vec.getCoords() << endl;

        // Code, in DefaultSolver, of 'TYAcousticModel::getMateriauFace'.
        TYMateriauConstruction* mat;
        TYMurElement* pMurElem = TYMurElement::safeDownCast(pPoly);

        cout << "try to get Materiau" << endl;
        if (pMurElem != NULL) // Cas d'une face de batiment ou d'un ecran
        {
            cout << "  # if pMurElem != NULL" << endl;

            OVector3D normale = pPoly->normal();

        }
        else // Cas d'une machine ou d'un acousticVolume quelconque
        {
            cout << "  # else pMurElem != NULL" << endl;
            TYElement* pParentSurface = pPoly->getParent();
            TYAcousticVolume* pVolParentSurface = NULL;
            while (pParentSurface && !(pParentSurface->inherits("TYAcousticVolume")))
            {
                pParentSurface = pParentSurface->getParent();
            }

            pVolParentSurface = (TYAcousticVolume*)pParentSurface;
            mat = pVolParentSurface->getMateriau();
        }
        // cout << "mat.getMasseVol(): " << mat.getMasseVol() << endl;
        // Spectrum.
        // OSpectre SpectreAbso = mat.getSpectreAbsoEyring();
        return mat;
    }
    return NULL;
};
} // namespace 'tympan'
