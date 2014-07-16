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
 */





TYBox TYVolumeInterface::volEnglob() const
{
    TYBox res;
    OPoint3D ptMin, ptMax;

    TYTabPoint tabPts = sommets();
    size_t nbPts = tabPts.size();

    if (nbPts <= 0)
    {
        return res;
    }

    OPoint3D* pts = new OPoint3D[nbPts];
    for (size_t i = 0; i < tabPts.size(); i++)
    {
        pts[i] = tabPts[i];
    }

    OGeometrie::boundingBox(pts, static_cast<int>(nbPts), ptMin, ptMax);

    delete [] pts;

    res._sizeX = ptMax._x - ptMin._x;
    res._sizeY = ptMax._y - ptMin._y;
    res._sizeZ = ptMax._z - ptMin._z;

    res.setPosition(OVector3D(ptMin) + (OVector3D(ptMin, ptMax) * 0.5));

    return res;
}

void TYVolumeInterface::calculRayonSphere(const TYBox& volEnglob)
{
    _rayonSphere = 1.0 / 2.0 * sqrt(volEnglob._sizeX * volEnglob._sizeX +
                                    volEnglob._sizeY * volEnglob._sizeY +
                                    volEnglob._sizeZ * volEnglob._sizeZ);
}

int TYVolumeInterface::intersects(const OSegment3D& seg, TYTabPoint& ptList) const
{
    int res = INTERS_NULLE;
    OPoint3D pt;
    int nbIntersects = 0;

    // Pour eviter des calculs inutiles...
    TYTabPoint pointTest;
    if (volEnglob().intersects(seg, pointTest) == INTERS_NULLE)
    {
        return res;
    }

    // Recherche de l'intersection du segment avec chacune des faces
    // composant ce volume
    TYTabLPPolygon tabFaces = faces();

    for (unsigned int i = 0; i < tabFaces.size(); i++)
    {
        LPTYPolygon pFace = tabFaces[i];
        assert(pFace);

        if (pFace->intersects(seg, pt) != INTERS_NULLE)
        {
            // Ajout du pt d'intersection trouve a la liste
            ptList.push_back(pt);

            // Increment du nb d'intersections trouvees
            nbIntersects++;
        }
    }

    // Une intersection a-t-elle ete trouvee ?
    if (nbIntersects > 0)
    {
        res = INTERS_OUI;
    }

    return res;
}
