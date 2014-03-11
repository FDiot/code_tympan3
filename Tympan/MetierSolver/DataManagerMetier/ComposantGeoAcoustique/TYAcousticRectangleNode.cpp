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
#include "Tympan/GraphicIHM/DataManagerIHM/TYAcousticRectangleNodeWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYAcousticRectangleNodeGraphic.h"
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/Tools/OMessageManager.h"



OPROTOINST(TYAcousticRectangleNode);
TY_EXTENSION_INST(TYAcousticRectangleNode);
TY_EXT_GRAPHIC_INST(TYAcousticRectangleNode);

TYAcousticRectangleNode::TYAcousticRectangleNode()
{
    _name = TYNameManager::get()->generateName(getClassName());

    TYAcousticRectangle* pAccRect = new TYAcousticRectangle();
    pAccRect->getBoundingRect()->setSize(getSizeX(), getSizeY());
    addAcousticSurf(pAccRect);
}

TYAcousticRectangleNode::TYAcousticRectangleNode(const TYAcousticRectangleNode& other)
{
    *this = other;
}

TYAcousticRectangleNode::~TYAcousticRectangleNode()
{
}

TYAcousticRectangleNode& TYAcousticRectangleNode::operator=(const TYAcousticRectangleNode& other)
{
    if (this != &other)
    {
        TYAcousticSurfaceNode::operator =(other);
    }
    return *this;
}

bool TYAcousticRectangleNode::operator==(const TYAcousticRectangleNode& other) const
{
    if (this != &other)
    {
        if (TYAcousticSurfaceNode::operator !=(other)) { return false; }
    }
    return true;
}

bool TYAcousticRectangleNode::operator!=(const TYAcousticRectangleNode& other) const
{
    return !operator==(other);
}

std::string TYAcousticRectangleNode::toString() const
{
    return "TYAcousticRectangleNode";
}

bool TYAcousticRectangleNode::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    return TYAcousticSurfaceNode::deepCopy(pOther, copyId);
}

DOM_Element TYAcousticRectangleNode::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYAcousticSurfaceNode::toXML(domElement);
    return domNewElem;
}

int TYAcousticRectangleNode::fromXML(DOM_Element domElement)
{
    TYAcousticSurfaceNode::fromXML(domElement);

    updateGrid();

    return 1;
}

TYTabSourcePonctuelleGeoNode TYAcousticRectangleNode::getSrcs() const
{
    return TYAcousticSurfaceNode::getSrcs();
}

TYSourcePonctuelle TYAcousticRectangleNode::srcPonctEquiv() const
{
    return TYSourcePonctuelle();
}

void TYAcousticRectangleNode::distriSrcs()
{
    TYAcousticSurfaceNode::distriSrcs();
}

bool TYAcousticRectangleNode::setSrcsLw()
{
    return TYAcousticSurfaceNode::setSrcsLw(); // Appel de la methode generale
}

void TYAcousticRectangleNode::setSizeX(float sizeX)
{
    float factorX = sizeX / getSizeX();

    if ((factorX == 0) || (factorX == 1))
    {
        // Inutile d'appliquer un scale = 0 ou = 1
        return;
    }

    scale(factorX, 1.0f);
}

void TYAcousticRectangleNode::setSizeY(float sizeY)
{
    float factorY = sizeY / getSizeY();

    if ((factorY == 0) || (factorY == 1))
    {
        // Inutile d'appliquer un scale = 0 ou = 1
        return;
    }

    scale(1.0f, factorY);
}

void TYAcousticRectangleNode::setSize(float sizeX, float sizeY)
{
    setSizeX(sizeX);
    setSizeY(sizeY);
}

void TYAcousticRectangleNode::getSize(float& sizeX, float& sizeY)
{
    sizeX = getSizeX();
    sizeY = getSizeY();
}

float TYAcousticRectangleNode::getSizeX()
{
    return _pBoundingRect->getSizeX();
}

float TYAcousticRectangleNode::getSizeY()
{
    return _pBoundingRect->getSizeY();
}

void TYAcousticRectangleNode::scale(float factorX, float factorY)
{
    getBoundingRect()->scale(factorX, factorY);

    // On applique les memes facteurs aux sous-rect
    for (unsigned int i = 0; i < _tabAcousticSurf.size(); i++)
    {
        LPTYAcousticSurface pSurface = TYAcousticSurface::safeDownCast(_tabAcousticSurf[i]->getElement());

        ORepere3D repere;

        pSurface->getBoundingRect()->scale(factorX, factorY);
        repere = _tabAcousticSurf[i]->getRepere();
        repere._origin._x *= factorX;
        repere._origin._y *= factorY;
        _tabAcousticSurf[i]->setRepere(repere);

        pSurface->setIsGeometryModified(true);
        _tabAcousticSurf[i]->setIsGeometryModified(true);
    }

    setIsGeometryModified(true);
}

bool TYAcousticRectangleNode::makeGrid(int nbCol, int nbRow)
{
    return makeGrid(nbCol, nbRow, "TYAcousticRectangle");
}

bool TYAcousticRectangleNode::makeGrid(int nbCol, int nbRow, const char* childType)
{
    assert(TYElement::findPrototype(childType) != -1);

    if ((nbCol <= 0) || (nbRow <= 0) || (childType == NULL))
    {
        return false;
    }

    // Nombre de case pour la grille
    int nbCase = nbCol * nbRow;

    // Reset tab
    purge();

    // Calcul des dimensions de chaque rect
    OVector3D vecBoundingRectPt0(_pBoundingRect->_pts[0]);
    OVector3D vecCol, vecRow;
    vecCol = OVector3D(_pBoundingRect->_pts[0], _pBoundingRect->_pts[1]) * (1 / (double)nbCol);
    vecRow = OVector3D(_pBoundingRect->_pts[0], _pBoundingRect->_pts[3]) * (1 / (double)nbRow);

    TYAcousticRectangle* pAccRect = NULL;

    // Populate tab
    for (int i = 0; i < nbCase; i++)
    {
        // Nouveau rectangle
        pAccRect = (TYAcousticRectangle*) TYElement::findAndClone(childType);
        pAccRect->setParent(this);

        double curRow = (int)(i / nbCol);
        double curCol = (int)(i % nbCol);

        OVector3D vec0 = (vecCol * curCol) + (vecRow * curRow);
        OVector3D vec1 = (vecCol * (curCol + 1)) + (vecRow * curRow);
        OVector3D vec3 = (vecRow * (curRow + 1)) + (vecCol * curCol);
        OVector3D vec2 = (vecRow * (curRow + 1)) + (vecCol * (curCol + 1));

        // Dimension & position
        pAccRect->getShape()->_pts[0] = vecBoundingRectPt0 + vec0;
        pAccRect->getShape()->_pts[1] = vecBoundingRectPt0 + vec1;
        pAccRect->getShape()->_pts[2] = vecBoundingRectPt0 + vec2;
        pAccRect->getShape()->_pts[3] = vecBoundingRectPt0 + vec3;

        // Ajout au tableau
        _tabAcousticSurf.push_back(new TYAcousticSurfaceGeoNode(pAccRect));
    }

    setIsGeometryModified(true);

    return true;
}

bool TYAcousticRectangleNode::addSubRect(LPTYAcousticRectangleGeoNode pAccRectGeoNode)
{
    assert(pAccRectGeoNode);
    assert(pAccRectGeoNode->getElement());

    LPTYAcousticRectangle pRectangle = TYAcousticRectangle::safeDownCast(pAccRectGeoNode->getElement());

    pRectangle->setIsSub(true);

    TYRectangle* pRect = pRectangle->getBoundingRect();
    TYRepere repRect = pAccRectGeoNode->getRepere();

    // Test si le rect passe se trouve a l'interieur du bounding rect de cette face
    if (((pRect->_pts[3]._x + repRect._origin._x) < _pBoundingRect->_pts[3]._x) ||
        ((pRect->_pts[3]._y + repRect._origin._y) < _pBoundingRect->_pts[3]._y) ||
        ((pRect->_pts[1]._x + repRect._origin._x) > _pBoundingRect->_pts[1]._x) ||
        ((pRect->_pts[1]._y + repRect._origin._y) > _pBoundingRect->_pts[1]._y))
    {
        return false;
    }

    // Test si le rect passe ne coupe pas un rect sub deja present
    for (unsigned int i = 0; i < _tabAcousticSurf.size(); i++)
    {
        LPTYAcousticSurface pSurface = TYAcousticSurface::safeDownCast(_tabAcousticSurf[i]->getElement());

        if ((pSurface->getIsSub()) &&
            (pRect->intersectRect(pSurface->getBoundingRect(), repRect._origin, _tabAcousticSurf[i]->getRepere()._origin)))
        {
            return false;
        }
    }

    // On peut ajouter le rect
    addAcousticSurf((LPTYAcousticSurfaceGeoNode&) pAccRectGeoNode);

    // Recalcul de la grille
    updateGrid();

    return true;
}

bool TYAcousticRectangleNode::addSubRect(LPTYAcousticRectangle pAccRect, TYRepere repRect)
{
    return addSubRect(new TYAcousticRectangleGeoNode(repRect, (LPTYElement)pAccRect));
}

bool TYAcousticRectangleNode::remAcousticSurf(const LPTYAcousticSurfaceGeoNode pAccSurfGeoNode)
{
    bool bVal = TYAcousticSurfaceNode::remAcousticSurf(pAccSurfGeoNode);
    updateGrid();

    return bVal;
}

bool TYAcousticRectangleNode::remAcousticSurf(const LPTYAcousticSurface pAccSurf)
{
    bool bVal = TYAcousticSurfaceNode::remAcousticSurf(pAccSurf);
    updateGrid();

    return bVal;
}
bool TYAcousticRectangleNode::remAcousticSurf(QString idAccSurf)
{
    bool bVal = TYAcousticSurfaceNode::remAcousticSurf(idAccSurf);
    updateGrid();

    return bVal;
}

void TYAcousticRectangleNode::updateGrid()
{
    updateGrid("TYAcousticRectangle");
}

void TYAcousticRectangleNode::updateGrid(const char* childType)
{
    assert(TYElement::findPrototype(childType) != -1);

    unsigned int i, j;
    TYTabLPRectangle tabGlobal, tabSub;

    LPTYElementArray childs;
    getChilds(childs);

    for (i = 0; i < childs.size(); i++)
    {
        childs[i]->setInCurrentCalcul(isInCurrentCalcul());
    }

    // On teste si la mise a jour est vraiment necessaire
    TYTabAcousticSurfaceGeoNode::iterator iter;
    iter = _tabAcousticSurf.begin();

    // On supprime les sous-faces de "construction"
    while (iter != _tabAcousticSurf.end())
    {
        if (!TYAcousticSurface::safeDownCast((*iter)->getElement())->getIsSub())
        {
            iter = _tabAcousticSurf.erase(iter);
        }
        else
        {
            iter++;
        }
    }

    // S'il n'y a aucune face 'Sub', on cree une face de construction
    // de la taille du rectangle englobant
    if (!_tabAcousticSurf.size())
    {
        // Construction de la face
        TYAcousticRectangle* pAccRect = (TYAcousticRectangle*) TYElement::findAndClone(childType);

        // Taille max
        pAccRect->getBoundingRect()->setSize(getSizeX(), getSizeY());

        // Ajout de la face
        pAccRect->setColor(getColor());
        addAcousticSurf(pAccRect);

        return;
    }

    // Il ne reste plus dans le tableau de faces que les sous-face marques "isSub"
    // On recupere la geometrie (rectangles) de ses sous-faces que l'on ramene dans le
    // repere du rectangle node
    for (i = 0; i < _tabAcousticSurf.size(); i++)
    {
        // Tableau des rectangles des sous-faces pour l'expansion dans le repere du rectangle node.
        LPTYRectangle pRectGlobal = new TYRectangle();
        // Tableau des rectangles des sous-faces dans le repere du rectangle node.
        LPTYRectangle pRectSub = new TYRectangle();

        for (j = 0; j < 4; j++)
        {
            LPTYAcousticSurface pSurface = TYAcousticSurface::safeDownCast(_tabAcousticSurf[i]->getElement());

            pRectGlobal->_pts[j]._x = pSurface->getBoundingRect()->_pts[j]._x + _tabAcousticSurf[i]->getRepere()._origin._x;
            pRectGlobal->_pts[j]._y = pSurface->getBoundingRect()->_pts[j]._y + _tabAcousticSurf[i]->getRepere()._origin._y;
            pRectGlobal->_pts[j]._z = 0.0;

            pRectSub->_pts[j] = pRectGlobal->_pts[j];
        }

        tabGlobal.push_back(pRectGlobal);
        tabSub.push_back(pRectSub);
    }

    bool intersect;
    TYRectangle rect;  // Rectangle trouve

    // Construction pour chaque sous-face du rectangle englobant de taille max
    for (i = 0; i < tabGlobal.size(); i++)
    {
        intersect = false;

        // Recherche en x-
        if (findNearestRect(tabGlobal[i], tabGlobal, -1, 0, rect)) // Un rectangle de construction a ete trouve
        {
            tabGlobal[i]->_pts[0]._x = rect._pts[1]._x;
            tabGlobal[i]->_pts[3]._x = rect._pts[1]._x;
        }
        else // On prend le rectangle englobant
        {
            tabGlobal[i]->_pts[0]._x = _pBoundingRect->_pts[0]._x;
            tabGlobal[i]->_pts[3]._x = _pBoundingRect->_pts[0]._x;
        }

        // Recherche en x+
        if (findNearestRect(tabGlobal[i], tabGlobal, 1, 0, rect)) // Un rectangle de construction a ete trouve
        {
            tabGlobal[i]->_pts[1]._x = rect._pts[0]._x;
            tabGlobal[i]->_pts[2]._x = rect._pts[0]._x;
        }
        else // On prend le rectangle englobant
        {
            tabGlobal[i]->_pts[1]._x = _pBoundingRect->_pts[1]._x;
            tabGlobal[i]->_pts[2]._x = _pBoundingRect->_pts[1]._x;
        }

        // Recherche en y-
        if (findNearestRect(tabGlobal[i], tabGlobal, -1, 1, rect)) // Un rectangle de construction a ete trouve
        {
            tabGlobal[i]->_pts[3]._y = rect._pts[0]._y;
            tabGlobal[i]->_pts[2]._y = rect._pts[0]._y;
        }
        else // On prend le rectangle englobant
        {
            tabGlobal[i]->_pts[3]._y = _pBoundingRect->_pts[2]._y;
            tabGlobal[i]->_pts[2]._y = _pBoundingRect->_pts[2]._y;
        }

        // Recherche en y+
        if (findNearestRect(tabGlobal[i], tabGlobal, 1, 1, rect)) // Un rectangle de construction a ete trouve
        {
            tabGlobal[i]->_pts[0]._y = rect._pts[2]._y;
            tabGlobal[i]->_pts[1]._y = rect._pts[2]._y;
        }
        else // On prend le rectangle englobant
        {
            tabGlobal[i]->_pts[0]._y = _pBoundingRect->_pts[0]._y;
            tabGlobal[i]->_pts[1]._y = _pBoundingRect->_pts[0]._y;
        }

    }

    // Decoupage de chaque rectangle max en grille de 5 cases ou moins
    // a partir du sous-rectangle correspondant
    for (i = 0; i < tabGlobal.size(); i++)
    {
        LPTYRectangle pGlobalRect = tabGlobal[i];
        LPTYRectangle pSubRect = tabSub[i];
        TYRepere rep;

        double lMax = pGlobalRect->getSizeX();
        double hRect = pSubRect->getSizeY();

        // Sous-rectangle de dessus
        LPTYAcousticRectangle pAccRect_1 = (TYAcousticRectangle*) TYElement::findAndClone(childType);
        double longueur = lMax;
        double hauteur = ABS(pGlobalRect->_pts[0]._y - pSubRect->_pts[0]._y);

        if (hauteur != 0)
        {
            pAccRect_1->getBoundingRect()->setDimension(longueur, hauteur);
            rep._origin._x = pGlobalRect->_pts[0]._x + longueur / 2.0;
            rep._origin._y = pGlobalRect->_pts[0]._y - hauteur / 2.0;
            addAcousticSurf(pAccRect_1, rep);
        }

        // Sous-rectangle droit
        LPTYAcousticRectangle pAccRect_2 = (TYAcousticRectangle*) TYElement::findAndClone(childType);
        longueur = ABS(pGlobalRect->_pts[3]._x - pSubRect->_pts[3]._x);
        hauteur = hRect;

        if (longueur != 0)
        {
            pAccRect_2->getBoundingRect()->setDimension(longueur, hauteur);
            rep._origin._x = pSubRect->_pts[3]._x - longueur / 2.0;
            rep._origin._y = pSubRect->_pts[3]._y + hauteur / 2.0;
            addAcousticSurf(pAccRect_2, rep);
        }

        // Sous-rectangle de dessous
        LPTYAcousticRectangle pAccRect_3 = (TYAcousticRectangle*) TYElement::findAndClone(childType);
        longueur = lMax;
        hauteur = ABS(pGlobalRect->_pts[3]._y - pSubRect->_pts[3]._y);

        if (hauteur != 0)
        {
            pAccRect_3->getBoundingRect()->setDimension(longueur, hauteur);
            rep._origin._x = pGlobalRect->_pts[3]._x + longueur / 2.0;
            rep._origin._y = pGlobalRect->_pts[3]._y + hauteur / 2.0;
            addAcousticSurf(pAccRect_3, rep);
        }

        // Sous-rectangle gauche
        LPTYAcousticRectangle pAccRect_4 = (TYAcousticRectangle*) TYElement::findAndClone(childType);
        longueur = ABS(pGlobalRect->_pts[2]._x - pSubRect->_pts[2]._x);
        hauteur = hRect;

        if (longueur != 0)
        {
            pAccRect_4->getBoundingRect()->setDimension(longueur, hauteur);
            rep._origin._x = pSubRect->_pts[2]._x + longueur / 2.0;
            rep._origin._y = pSubRect->_pts[2]._y + hauteur / 2.0;
            addAcousticSurf(pAccRect_4, rep);
        }
    }

    // Concerve la couleur du mur
    for (iter = _tabAcousticSurf.begin(); iter != _tabAcousticSurf.end(); ++iter)
        if (!TYAcousticSurface::safeDownCast((*iter)->getElement())->getIsSub())
        {
            TYAcousticRectangle::safeDownCast((*iter)->getElement())->setColor(getColor());
        }

    setIsGeometryModified(true);
}

bool TYAcousticRectangleNode::findNearestRect(const LPTYRectangle currentRect, const TYTabLPRectangle tabGlobal, const short signe, const short dir, TYRectangle& rect)
{
    bool ret = false;
    unsigned int i;
    const double epsilon = 1E-3;
    double longueur = 0.0;
    double shortest = 1E6; // point le plus proche a 1000 km au depart (je pense que ca devrait aller ...)
    TYRectangle test;

    if (dir == 0) // Direction horizontale
    {
        if (signe == -1) // A gauche
        {
            for (i = 0 ; i < tabGlobal.size() ; i++)
            {
                if (tabGlobal[i] != currentRect) // Eviter de comparer le rectangle a lui meme
                {
                    longueur = currentRect->_pts[0]._x - tabGlobal[i]->_pts[1]._x;

                    if ((longueur >= 0.0) && (ABS(longueur) < shortest)) // Recherche du plus proche dans cette direction
                    {
                        // Construction d'un rectangle un peu plus grand pour tester l'intersection
                        test = *currentRect;  // Pour gagner du temps
                        test._pts[0]._x = tabGlobal[i]->_pts[1]._x - epsilon;
                        test._pts[3]._x = tabGlobal[i]->_pts[2]._x - epsilon;

                        if (test.intersectRect(tabGlobal[i])) // rectangle valable si intersection
                        {
                            shortest = ABS(longueur);
                            rect = *tabGlobal[i];
                            ret = true;
                        }
                    }
                }
            }
        }
        else // A droite
        {
            for (i = 0 ; i < tabGlobal.size() ; i++)
            {
                if (tabGlobal[i] != currentRect)
                {
                    longueur = currentRect->_pts[1]._x - tabGlobal[i]->_pts[0]._x;

                    if ((longueur <= 0.0) && (ABS(longueur) < shortest))
                    {
                        // Construction d'un rectangle un peu plus grand pour tester l'intersection
                        test = *currentRect;  // Pour gagner du temps
                        test._pts[1]._x = tabGlobal[i]->_pts[0]._x + epsilon;
                        test._pts[2]._x = tabGlobal[i]->_pts[3]._x + epsilon;

                        if (test.intersectRect(tabGlobal[i])) // rectangle valable si intersection
                        {
                            shortest = ABS(longueur);
                            rect = *tabGlobal[i];
                            ret = true;
                        }
                    }
                }
            }
        }
    }
    else // Direction verticale
    {
        if (signe == -1) // Au dessous
        {
            for (i = 0 ; i < tabGlobal.size() ; i++)
            {
                if (tabGlobal[i] != currentRect)
                {
                    longueur = currentRect->_pts[2]._y - tabGlobal[i]->_pts[1]._y;

                    if ((longueur >= 0.0) && (ABS(longueur) < shortest))
                    {
                        // Construction d'un rectangle un peu plus grand pour tester l'intersection
                        test = *currentRect;  // Pour gagner du temps
                        test._pts[3]._y = tabGlobal[i]->_pts[0]._y - epsilon;
                        test._pts[2]._y = tabGlobal[i]->_pts[1]._y - epsilon;

                        if (test.intersectRect(tabGlobal[i])) // rectangle valable si intersection
                        {
                            shortest = ABS(longueur);
                            rect = *tabGlobal[i];
                            ret = true;
                        }
                    }
                }
            }
        }
        else // Au dessus
        {
            for (i = 0 ; i < tabGlobal.size() ; i++)
            {
                if (tabGlobal[i] != currentRect)
                {
                    longueur = currentRect->_pts[1]._y - tabGlobal[i]->_pts[2]._y;

                    if ((longueur <= 0.0) && (ABS(longueur) < shortest))
                    {
                        // Construction d'un rectangle un peu plus grand pour tester l'intersection
                        test = *currentRect;  // Pour gagner du temps
                        test._pts[0]._y = tabGlobal[i]->_pts[3]._y + epsilon;
                        test._pts[1]._y = tabGlobal[i]->_pts[2]._y + epsilon;

                        if (test.intersectRect(tabGlobal[i])) // rectangle valable si intersection
                        {
                            shortest = ABS(longueur);
                            rect = *tabGlobal[i];
                            ret = true;
                        }
                    }
                }
            }
        }
    }

    return ret;
}

double TYAcousticRectangleNode::activeSurface()
{
    return TYAcousticSurfaceNode::activeSurface();
}

void TYAcousticRectangleNode::remAllSrcs()
{
    TYAcousticSurfaceNode::remAllSrcs();
}
