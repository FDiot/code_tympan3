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

#if TY_USE_IHM
#include "Tympan/gui/widgets/TYSegmentWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYSegmentGraphic.h"
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/models/business/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/models/business/TYPreferenceManager.h"
#include <math.h>

#include "Tympan/core/logging.h"

#include "Tympan/core/defines.h"

TY_EXTENSION_INST(TYSegment);
TY_EXT_GRAPHIC_INST(TYSegment);

TYSegment::TYSegment()
{
    _name = TYNameManager::get()->generateName(getClassName());

    _ptA._x = 0; _ptA._y = 0; _ptA._z = 0;
    _ptB._x = 0; _ptB._y = 1; _ptB._z = 0;
}

TYSegment::TYSegment(const TYSegment& other)
{
    *this = other;
}

TYSegment::TYSegment(TYPoint ptA, TYPoint ptB)
{
    _name = TYNameManager::get()->generateName(getClassName());

    _ptA = ptA;
    _ptB = ptB;
}

TYSegment::~TYSegment()
{
}

TYSegment& TYSegment::operator=(const TYSegment& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        TYColorInterface::operator =(other);
        _ptA = other._ptA;
        _ptB = other._ptB;
    }
    return *this;
}

bool TYSegment::operator==(const TYSegment& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        if (TYColorInterface::operator !=(other)) { return false; }
        if (_ptA != other._ptA) { return false; }
        if (_ptB != other._ptB) { return false; }
    }
    return true;
}

bool TYSegment::operator!=(const TYSegment& other) const
{
    return !operator==(other);
}

bool TYSegment::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    TYColorInterface::deepCopy((TYColorInterface*)pOther, copyId);

    TYSegment* pOtherSeg = (TYSegment*) pOther;

    _ptA.deepCopy(&pOtherSeg->_ptA, copyId);
    _ptB.deepCopy(&pOtherSeg->_ptB, copyId);

    return true;
}

std::string TYSegment::toString() const
{
    std::string str = "TYSegment\n\tPtA=" + _ptA.toString() + "\n\tPtB=" + _ptB.toString();
    return str;
}

DOM_Element TYSegment::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);

    TYColorInterface::toXML(domNewElem);

    _ptA.toXML(domNewElem);
    _ptB.toXML(domNewElem);

    return domNewElem;
}

int TYSegment::fromXML(DOM_Element domElement)
{
    int ptNb = 0;
    DOM_Element elemCur;

    TYElement::fromXML(domElement);

    TYColorInterface::fromXML(domElement);

    QDomNodeList childs = domElement.childNodes();

    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();
        if (ptNb == 0)
        {
            // Increment l'indice si le pt A est trouve
            _ptA.callFromXMLIfEqual(elemCur) ? ptNb++ : ptNb;
        }
        else if (ptNb == 1)
        {
            // Increment l'indice si le pt B est trouve
            _ptB.callFromXMLIfEqual(elemCur) ? ptNb++ : ptNb;
        }
    }

    return 1;
}

TYSegment TYSegment::operator*(const OMatrix& matrix) const
{
    return TYSegment(matrix * _ptA, matrix * _ptB);
}

double TYSegment::longueur() const
{
    return OVector3D(_ptA, _ptB).norme();
}

int TYSegment::symetrieOf(const TYPoint& pt, TYPoint& ptSym) const
{
    int res = 0;

    // Calcul le pt image a ptSym (pt source) par rapport a la droite definie passant
    // par ce segment
    double k = OGeometrie::symPointDroite(_ptA, _ptB, pt, ptSym);

    // Le coefficient k indique si le segment [Point source / Point image] coupe
    // ce segment, pour cela il faut : 0 < k < 1.
    if ((0.0 <= k) && (k <= 1.0))
    {
        res = 1;
    }

    return res;
}

int TYSegment::projection(const TYPoint& pt, TYPoint& ptProj) const
{
    int res = 0;
    TYPoint ptSym;

    res = symetrieOf(pt, ptSym);
    res = res && intersects(TYSegment(pt, ptSym), ptProj);

    return res;
}

int TYSegment::intersects(const TYSegment& seg, TYPoint& pt) const
{
    int res = INTERS_NULLE;

    TYPoint ptA, ptB;
    double mua, mub;

    double seuilConfondus = TYSEUILCONFONDUS;

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "SeuilConfondus"))
    {
        seuilConfondus = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "SeuilConfondus");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "SeuilConfondus", seuilConfondus);
    }
#endif

    // Calcul le segment le plus court entre les 2 segments dont on recherche l'eventuelle intersection
    if (OGeometrie::shortestSegBetween2Lines(this->_ptA, this->_ptB, seg._ptA, seg._ptB, ptA, ptB, &mua, &mub))
    {
        // Les coefs mua et mub doivent etre compris entre 0 et 1 pour que les points constituant
        // le segment trouve se trouve sur les segments respectifs a tester
        if ((0.0 <= mua) && (mua <= 1.0) && (0.0 <= mub) && (mub <= 1.0))
        {
            // La norme du segment trouve doit etre inferieur a un seuil pour que l'on puisse admettre
            // que les 2 points constituant ce segment sont quasi confondus
            OVector3D vecSeg(ptA, ptB);

            if (vecSeg.norme() <= seuilConfondus)
            {
                res = INTERS_OUI;

                // Arbitrairement on prend le 1er point du segment trouve comme point d'intersection.
                pt = ptA;
            }
        }
    }

    return res;
}

TYPoint TYSegment::centreOf() const
{
    TYPoint point;
    double x, y, z;

    x = ((_ptB._x - _ptA._x) / 2.0) + _ptA._x;
    y = ((_ptB._y - _ptA._y) / 2.0) + _ptA._y;
    z = ((_ptB._z - _ptA._z) / 2.0) + _ptA._z;

    point.set(x, y , z);

    return point;
}

TYPoint TYSegment::centerOfCurvedPath(const double& R) const
{
    TYPoint point;
    TYPoint milieu = centreOf() ;
    double demiLongueur = longueur() / 2;
    double d, x, y, z, q;
    x = milieu._x; y = milieu._y; z = milieu._z;

    if (R > demiLongueur)
    {
        d = sqrt(R * R - demiLongueur * demiLongueur);

        if (_ptA._z != _ptB._z)
        {
            d = d * SIGNE(_ptA._z - _ptB._z);
            q = (_ptB._y - _ptA._y) / (_ptB._z - _ptA._z);
            y = milieu._y + d / sqrt(1 + q * q);
        }
        z = milieu._z + sqrt(ABS(d * d - (y - milieu._y) * (y - milieu._y)));
    }

    point.setCoords(x, y, z);
    return point;
}

double TYSegment::lengthOfCurvedPath(const double& R)
{
    // Longueur du cote du triangle rectangle
    double cote = longueur() / 2.0;
    // Angle d'ouverture du segment
    double angle = 2 * asin(cote / R);

    return angle * R;
}

bool TYSegment::unorderedIsEqual(const TYSegment& s1, const TYSegment& s2)
{
    if (&s1 == &s2)
    {
        return true;
    }
    if (s1._ptA == s2._ptA && s1._ptB == s2._ptB)
    {
        return true;
    }
    if (s1._ptA == s2._ptB && s1._ptB == s2._ptA)
    {
        return true;
    }
    return false;
}

