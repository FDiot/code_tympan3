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


#include "Tympan/core/logging.h"
#include "Tympan/models/business/geoacoustic/TYAcousticVolumeNode.h"
#if TY_USE_IHM
  #include "Tympan/gui/widgets/TYAcousticBoxWidget.h"
  #include "Tympan/gui/gl/TYAcousticBoxGraphic.h"
#endif
#include "TYAcousticBox.h"

TY_EXTENSION_INST(TYAcousticBox);
TY_EXT_GRAPHIC_INST(TYAcousticBox);

TYAcousticBox::TYAcousticBox()
{
    _name = TYNameManager::get()->generateName(getClassName());

    float r = 46.0f, g = 4.0f, b = 184.0f;

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "TYAcousticBoxGraphicColorR"))
    {
        TYPreferenceManager::getColor(TYDIRPREFERENCEMANAGER, "TYAcousticBoxGraphicColor", r, g, b);
    }
    else
    {
        TYPreferenceManager::setColor(TYDIRPREFERENCEMANAGER, "TYAcousticBoxGraphicColor", r, g, b);
    }

#endif // TY_USE_IHM

    OColor color;
    color.r = r / 255;
    color.g = g / 255;
    color.b = b / 255;

    setColor(color);

    unsigned int i = 0;
    for (i = 0; i < 6; i++)
    {
        LPTYAcousticRectangleNodeGeoNode pAcRectNode = new TYAcousticRectangleNodeGeoNode(new TYAcousticRectangleNode());
        _faces[i] = pAcRectNode;
        _faces[i]->setParent(this);
        _faces[i]->getElement()->setParent(this);
        _faces[i]->getElement()->setName("Face " + QString(intToStr(i).c_str()));

        // Ajout de la couleur a chaque face.
        TYAcousticRectangleNode::safeDownCast(_faces[i]->getElement())->setColor(getColor());
    }

    // Ajout de la couleur a chaque rectangle de la face non sub.
    TYTabAcousticSurfaceGeoNode tab = acousticFaces();
    for (i = 0; i < tab.size() ; ++i)
    {
        LPTYAcousticSurface pAccSurf = TYAcousticSurface::safeDownCast(tab[i]->getElement());
        if (!pAccSurf->getIsSub())
        {
            pAccSurf->setColor(getColor());
        }
    }

    TYRepere rep;

    rep._vecI = OVector3D(0, 1, 0);
    rep._vecJ = OVector3D(0, 0, 1);
    rep._vecK = OVector3D(1, 0, 0);
    _faces[0]->setRepere(rep);

    rep._vecI = OVector3D(1, 0, 0);
    rep._vecJ = OVector3D(0, 0, 1);
    rep._vecK = OVector3D(0, -1, 0);
    _faces[1]->setRepere(rep);

    rep._vecI = OVector3D(0, -1, 0);
    rep._vecJ = OVector3D(0, 0, 1);
    rep._vecK = OVector3D(-1, 0, 0);
    _faces[2]->setRepere(rep);

    rep._vecI = OVector3D(-1, 0, 0);
    rep._vecJ = OVector3D(0, 0, 1);
    rep._vecK = OVector3D(0, 1, 0);
    _faces[3]->setRepere(rep);

    rep._vecI = OVector3D(1, 0, 0);
    rep._vecJ = OVector3D(0, -1, 0);
    rep._vecK = OVector3D(0, 0, -1);
    _faces[4]->setRepere(rep);

    rep._vecI = OVector3D(1, 0, 0);
    rep._vecJ = OVector3D(0, 1, 0);
    rep._vecK = OVector3D(0, 0, 1);
    _faces[5]->setRepere(rep);

    setDimension(1.0, 1.0, 1.0);

    _volEnglob = volEnglob();
    calculRayonSphere(_volEnglob);
    calculCentreGravite();
}

TYAcousticBox::TYAcousticBox(const TYAcousticBox& other)
{
    *this = other;
}

TYAcousticBox::~TYAcousticBox()
{
}

void TYAcousticBox::setDimension(float larg, float lon, float haut)
{
    LPTYAcousticRectangleNode pElement;

    ORepere3D repere;

    float demiLarg = larg / 2.0;
    float demiLon = lon / 2.0;
    float demiHaut = haut / 2.0;

    pElement = TYAcousticRectangleNode::safeDownCast(_faces[0]->getElement());
    repere = _faces[0]->getORepere3D();
    repere._origin._x = demiLarg;
    _faces[0]->setRepere(repere);
    pElement->setSize(lon, haut);
    _faces[0]->setIsGeometryModified(true);

    pElement = TYAcousticRectangleNode::safeDownCast(_faces[2]->getElement());
    repere = _faces[2]->getORepere3D();
    repere._origin._x = -demiLarg;
    _faces[2]->setRepere(repere);
    pElement->setSize(lon, haut);
    _faces[2]->setIsGeometryModified(true);

    pElement = TYAcousticRectangleNode::safeDownCast(_faces[1]->getElement());
    repere = _faces[1]->getORepere3D();
    repere._origin._y = -demiLon;
    _faces[1]->setRepere(repere);
    pElement->setSize(larg, haut);
    _faces[1]->setIsGeometryModified(true);

    pElement = TYAcousticRectangleNode::safeDownCast(_faces[3]->getElement());
    repere = _faces[3]->getORepere3D();
    repere._origin._y = demiLon;
    _faces[3]->setRepere(repere);
    pElement->setSize(larg, haut);
    _faces[3]->setIsGeometryModified(true);

    pElement = TYAcousticRectangleNode::safeDownCast(_faces[4]->getElement());
    repere = _faces[4]->getORepere3D();
    repere._origin._z = -demiHaut;
    _faces[4]->setRepere(repere);
    pElement->setSize(larg, lon);
    _faces[4]->setIsGeometryModified(true);

    pElement = TYAcousticRectangleNode::safeDownCast(_faces[5]->getElement());
    repere = _faces[5]->getORepere3D();
    repere._origin._z = demiHaut;
    _faces[5]->setRepere(repere);
    pElement->setSize(larg, lon);
    _faces[5]->setIsGeometryModified(true);

    _volEnglob = volEnglob();
    if (_pParent) { TYAcousticVolumeNode::safeDownCast(_pParent)->updateVolEnglob(); } // Informe le parent de se mettre a jour

    calculRayonSphere(_volEnglob);
    calculCentreGravite();

    setIsGeometryModified(true);
}

void TYAcousticBox::getDimension(float& larg, float& lon, float& haut)
{
    LPTYAcousticRectangleNode pElement;


    pElement = TYAcousticRectangleNode::safeDownCast(getFace(1)->getElement());
    larg = pElement->getBoundingRect()->getSizeX();
    pElement = TYAcousticRectangleNode::safeDownCast(getFace(0)->getElement());
    lon  = pElement->getBoundingRect()->getSizeX();
    haut = pElement->getBoundingRect()->getSizeY();
}

void TYAcousticBox::setFace(const LPTYAcousticRectangleNode pAccRect, int numFace)
{
    _faces[numFace]->setElement((LPTYElement)pAccRect);
    _faces[numFace]->setParent(this);

    _volEnglob = volEnglob();
    if (_pParent) { TYAcousticVolumeNode::safeDownCast(_pParent)->updateVolEnglob(); } // Informe le parent de se mettre a jour

    calculRayonSphere(_volEnglob);
    calculCentreGravite();

    setIsGeometryModified(true);
}

TYAcousticBox& TYAcousticBox::operator=(const TYAcousticBox& other)
{
    if (this != &other)
    {
        TYAcousticVolume::operator =(other);
        for (int i = 0; i < 6; i++)
        {
            _faces[i] = other._faces[i];
        }
    }

    _volEnglob = volEnglob();
    calculRayonSphere(_volEnglob);
    calculCentreGravite();

    return *this;
}

bool TYAcousticBox::operator==(const TYAcousticBox& other) const
{
    if (this != &other)
    {
        if (TYAcousticVolume::operator !=(other)) { return false; }
        for (int i = 0; i < 6; i++)
            if (_faces[i] != other._faces[i])
            {
                return false;
            }
    }
    return true;
}

bool TYAcousticBox::operator!=(const TYAcousticBox& other) const
{
    return !operator==(other);
}

bool TYAcousticBox::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYAcousticVolume::deepCopy(pOther, copyId)) { return false; }

    for (int i = 0; i < 6; i++)
    {
        _faces[i]->deepCopy(((TYAcousticBox*) pOther)->getFace(i), copyId);
        _faces[i]->setParent(this);
        _faces[i]->getElement()->setParent(this);
    }

    _volEnglob = volEnglob();
    calculRayonSphere(_volEnglob);
    calculCentreGravite();

    return true;
}

std::string TYAcousticBox::toString() const
{
    return "TYAcousticBox";
}

DOM_Element TYAcousticBox::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYAcousticVolume::toXML(domElement);

    for (int i = 0; i < 6; i++)
    {
        _faces[i]->toXML(domNewElem);
    }

    return domNewElem;
}

int TYAcousticBox::fromXML(DOM_Element domElement)
{
    TYAcousticVolume::fromXML(domElement);

    int faceNb = 0;
    unsigned int i = 0;
    DOM_Element elemCur;
    QDomNodeList childs = domElement.childNodes();

    for (i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();
        if (faceNb < 6)
        {
            // Increment l'indice si la face est trouvee
            if (_faces[faceNb]->callFromXMLIfEqual(elemCur))
            {
                faceNb++;
            }
        }
    }

    // ------- A SUPPRIMER LORSQUE TOUT LES XML AURONT LES INFOS DE COULEUR -------
    //
    // Je sais c'est pas tres beau, mais c'est pour etre compatible avec les vielles versions
    // de l'XML pour la gestion des couleurs... Je teste la couleur pour savoir
    // si elle est blanche, si oui, c'est possible que ce soit un ancien fichier, on applique donc
    // une couleur par default;
    //
    OColor color;
    bool allWhite = true;
    for (faceNb = 0; faceNb < 6; ++faceNb)
    {
        color = TYAcousticSurfaceNode::safeDownCast(_faces[0]->getElement())->getColor();
        if (color.r != 1.0f || color.g != 1.0f || color.b != 1.0f)
        {
            allWhite = false;
            break;
        }

    }

    if (allWhite)
    {
        float r = 46.0f, g = 4.0f, b = 184.0f;

#if TY_USE_IHM
        if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "TYAcousticBoxGraphicColorR"))
        {
            TYPreferenceManager::getColor(TYDIRPREFERENCEMANAGER, "TYAcousticBoxGraphicColor", r, g, b);
        }
        else
        {
            TYPreferenceManager::setColor(TYDIRPREFERENCEMANAGER, "TYAcousticBoxGraphicColor", r, g, b);
        }

#endif // TY_USE_IHM

        OColor color;
        color.r = r / 255;
        color.g = g / 255;
        color.b = b / 255;

        setColor(color);

        for (faceNb = 0; faceNb < 6; ++faceNb)
        {
            TYAcousticSurfaceNode::safeDownCast(_faces[faceNb]->getElement())->setColor(color);
        }

        // Ajout de la couleur a chaque rectangle de la face non sub.
        TYTabAcousticSurfaceGeoNode tab = acousticFaces();
        for (i = 0; i < tab.size() ; ++i)
        {
            LPTYAcousticSurface pAccSurf = TYAcousticSurface::safeDownCast(tab[i]->getElement());
            if (!pAccSurf->getIsSub())
            {
                pAccSurf->setColor(color);
            }
        }
    }
    //
    // ----------------------------------------------------------------------------

    _volEnglob = volEnglob();
    calculRayonSphere(_volEnglob);
    calculCentreGravite();

    if (_forceNormales) { inverseNormales(); }


    return 1;
}

void TYAcousticBox::getChilds(LPTYElementArray& childs, bool recursif /*=true*/)
{
    TYAcousticVolume::getChilds(childs, recursif);

    for (int i = 0; i < 6; i++)
    {
        childs.push_back(_faces[i]);
        childs.push_back(_faces[i]->getElement());
    }

    if (recursif)
    {
        for (int i = 0; i < 6; i++)
        {
            _faces[i]->getChilds(childs, recursif);
        }
    }
}

void TYAcousticBox::setRegime(TYSpectre& Spectre, int regime /*=-1*/, bool recursif /*=false*/)
{
    if (recursif)
    {
        for (int i = 0; i < 6; i++)
        {
            getFaceElement(i)->setRegime(Spectre, regime, recursif);
        }
    }

    TYAcousticVolume::setRegime(Spectre, regime, recursif);
}

void TYAcousticBox::exportCSV(std::ofstream& ofs)
{
    // Export du nom de l'objet
    ofs << getName().toAscii().data() << '\n';

    // Export du type de l'objet
    ofs << toString() << '\n';

    // Export des donnees acoustiques
    TYAcousticInterface::exportCSV(ofs);

    for (int i = 0; i < 6 ; i++)
    {
        getFaceElement(i)->exportCSV(ofs);
    }

    ofs << '\n';
}


bool TYAcousticBox::remRegime(int regime)
{
    for (int i = 0; i < 6; i++)
    {
        getFaceElement(i)->remRegime(regime);
    }

    return TYAcousticVolume::remRegime(regime);
}

void TYAcousticBox::loadRegime(int regimeNb)
{
    for (int i = 0; i < 6; i++)
    {
        getFaceElement(i)->loadRegime(regimeNb);
    }

    TYAcousticVolume::loadRegime(regimeNb);
}

void TYAcousticBox::setCurRegime(int regime)
{
    for (int i = 0; i < 6; i++)
    {
        getFaceElement(i)->setCurRegime(regime);
    }

    TYAcousticInterface::setCurRegime(regime);

    setIsAcousticModified(true);
}
/*
int TYAcousticBox::addRegime(const TYRegime& regime)
{
    for (int i = 0; i < 6; i++)
    {
        getFaceElement(i)->addRegime();
    }

    return TYAcousticInterface::addRegime(regime);
}
*/
int TYAcousticBox::addRegime()
{
    for (int i = 0; i < 6; i++)
    {
        getFaceElement(i)->addRegime();
    }

    return TYAcousticInterface::addRegime();
}

void TYAcousticBox::setNextRegimeNb(const int& next)
{
    _nextRegime = next;

    for (int i = 0; i < 6; i++)
    {
        getFaceElement(i)->setNextRegimeNb(next);
    }

}

void TYAcousticBox::setRegimeName(const QString& name)
{
    bool status = true;
    TYRegime& reg = getRegimeNb(_curRegime, status);
    if (status) { reg.setRegimeName(name); }

    for (int i = 0; i < 6; i++)
    {
        getFaceElement(i)->setRegimeName(name);
    }

}

void TYAcousticBox::propagateAtt(LPTYAttenuateur pAtt)
{
    for (int i = 0; i < 6; i++)
    {
        getFaceElement(i)->propagateAtt(pAtt);
    }

    TYAcousticVolume::propagateAtt(pAtt);
}

TYTabSourcePonctuelleGeoNode TYAcousticBox::getSrcs() const
{
    TYTabSourcePonctuelleGeoNode tab;

    // Pour chaque face
    for (int i = 0; i < 6; i++)
    {
        LPTYAcousticRectangleNode pElement = TYAcousticRectangleNode::safeDownCast(_faces[i]->getElement());

        if (pElement->getIsRayonnant())
        {
            // Recupere l'ensemble des srcs de la face
            TYTabSourcePonctuelleGeoNode tabChild = pElement->getSrcs();

            // Concatenation des matrices
            OMatrix matrix = _faces[i]->getMatrix();
            for (unsigned int j = 0; j < tabChild.size(); j++)
            {
                tabChild[j]->setMatrix(matrix * tabChild[j]->getMatrix());
            }

            //...et ajoute au tableau a retourner
            tab.insert(tab.end(), tabChild.begin(), tabChild.end());
        }
    }

    return tab;
}

TYSourcePonctuelle TYAcousticBox::srcPonctEquiv() const
{
    return TYSourcePonctuelle();
}

void TYAcousticBox::setDensiteSrcsH(double densite, bool recursif /*=true*/)
{
    TYAcousticVolume::setDensiteSrcsH(densite, recursif);

    if (recursif)
    {
        for (int i = 0; i < 6; i++)
        {
            getFaceElement(i)->setDensiteSrcsH(getDensiteSrcsH(), recursif);
        }
    }
}

void TYAcousticBox::setDensiteSrcsV(double densite, bool recursif /*=true*/)
{
    TYAcousticVolume::setDensiteSrcsV(densite, recursif);

    if (recursif)
    {
        for (int i = 0; i < 6; i++)
        {
            getFaceElement(i)->setDensiteSrcsV(getDensiteSrcsV(), recursif);
        }
    }
}

void TYAcousticBox::distriSrcs()
{
    if (_isRayonnant)
    {
        for (int i = 0; i < 6; i++)
        {
            // Si la face est rayonnante et que qu'elle a une densite de source, on lui en met...
            if ((getFaceElement(i)->getIsRayonnant())) // && (getFaceElement(i)->getDensiteSrcs() > 0))
            {
                getFaceElement(i)->distriSrcs();
            }
        }
    }
}

void TYAcousticBox::remAllSrcs()
{
    // Suppression de toutes les sources pour toutes les faces
    for (int i = 0 ; i < 6 ; i++)
    {
        getFaceElement(i)->remAllSrcs();
    }
}

bool TYAcousticBox::setSrcsLw()
{
    bool res = true;

    for (int i = 0; i < 6; i++)
    {
        // Affectation de puissance uniquement si l'element est rayonnant
        if ((getFaceElement(i)->getIsRayonnant()))  //&& (getFaceElement(i)->getDensiteSrcs() > 0))
        {
            res = getFaceElement(i)->setSrcsLw();
        }
    }

    return res;
}

double TYAcousticBox::volume() const
{
    double profondeur = getFaceElement(1)->getSizeX();

    // Vol = aire d'un cote x profondeur
    return (getFaceElement(0)->surface() * profondeur);
}

double TYAcousticBox::surface() const
{
    double ret = 0.0;

    // C'est la somme de toutes les surfaces
    for (int i = 0; i < 6; i++)
    {
        ret += getFaceElement(i)->surface();
    }

    return ret;
}

double TYAcousticBox::activeSurface() const
{
    double ret = 0.0;

    // C'est la somme de toutes les surfaces actives
    for (int i = 0; i < 6; i++)
    {
        if (getFaceElement(i)->getIsRayonnant())
        {
            ret += getFaceElement(i)->activeSurface();
        }
    }

    return ret;
}


double TYAcousticBox::activeSurface()
{
    double ret = 0.0;

    // C'est la somme de toutes les surfaces actives
    for (int i = 0; i < 6; i++)
    {
        if (getFaceElement(i)->getIsRayonnant())
        {
            ret += getFaceElement(i)->activeSurface();
        }
    }

    return ret;
}


TYTabVector TYAcousticBox::normals() const
{
    TYTabVector tab;

    for (int i = 0; i < 6; i++)
    {
        // On recupere la normale de la face
        OVector3D normal = getFaceElement(i)->getBoundingRect()->normal();

        // On convertit la normale dans le repere du vol node
        normal = _faces[i]->getMatrix() * normal;

        // Ajout
        tab.push_back(normal);
    }

    return tab;
}

TYTabPoint TYAcousticBox::sommets() const
{
    TYTabPoint tab;
    TYPoint pt;
    OMatrix matrix0 = _faces[0]->getMatrix();
    OMatrix matrix2 = _faces[2]->getMatrix();

    tab.reserve(8);

    // Les sommets des faces 0 et 2 suffisent
    for (int i = 0; i < 4; i++)
    {
        pt = getFaceElement(0)->getBoundingRect()->_pts[i];
        pt = matrix0 * pt;
        tab.push_back(pt);

        pt = getFaceElement(2)->getBoundingRect()->_pts[i];
        pt = matrix2 * pt;
        tab.push_back(pt);
    }

    return tab;
}

TYTabLPPolygon TYAcousticBox::faces() const
{
    TYTabLPPolygon tab;

    tab.reserve(6);

    for (register int i = 0; i < 6; i++)
    {
        // Polygon representant la face
        TYPolygon* pPolygon = getFaceElement(i)->getBoundingRect()->toPolygon();

        // On convertit le polygon dans le repere de l'acc box
        OMatrix matrix = _faces[i]->getMatrix();

        pPolygon->transform(matrix);
        // Ajout du polygon
        tab.push_back(pPolygon);
    }

    return tab;
}

TYBox TYAcousticBox::volEnglob() const
{
    return TYVolumeInterface::volEnglob();
}

TYPoint TYAcousticBox::centreGravite() const
{
    // Le centre de gravite se trouve au milieu du segment defini par 2 points opposes dans la box
    OPoint3D pt1 = _faces[0]->getMatrix() * getFaceElement(0)->getBoundingRect()->_pts[0];
    OPoint3D pt2 = _faces[2]->getMatrix() * getFaceElement(2)->getBoundingRect()->_pts[3];

    OVector3D vecDiagonale(pt1, pt2);
    OVector3D vecCentreGravite = OVector3D(pt1) + (vecDiagonale * 0.5);

    return TYPoint(vecCentreGravite);
}

int TYAcousticBox::intersects(const OSegment3D& seg, TYTabPoint& ptList) const
{
    return TYVolumeInterface::intersects(seg, ptList);
}

int TYAcousticBox::isInside(const TYPoint& pt) const
{
    return _volEnglob.isInside(pt);
}

bool TYAcousticBox::updateAcoustic(const bool& force) // force = false
{
    /*
       NB : On travaille sur un regime donne.
    */

    int i;
    bool ret = true;

    // Si le volume  n'est pas rayonnant, inutile de tenter de distribuer
    // la puissance, mais ce n'est pas une raison pour bloquer le calcul
    if (!_isRayonnant) { return true ; }


    // 1/ Recuperer la surface totale
    double surfTotale = activeSurface();

    TYSpectre LWc = TYSpectre::getEmptyLinSpectre(); // Spectre cumule de l'ensembles des elements
    TYSpectre LWg = getCurrentSpectre().toGPhy(); // Spectre global du regime courant
    TYSpectre LWv; //Spectre de puissance de la face courante

    LWc.setType(SPECTRE_TYPE_LW);

    for (i = 0 ; i < 6 ; i++) // Les faces ordinaires
    {
        LPTYAcousticSurfaceNode pFace = getFaceElement(i);

        // Si la face est rayonnante
        if (pFace->getIsRayonnant())
        {
            LWv = pFace->setGlobalLW(LWg, surfTotale);
            LWc = LWc.sum(LWv);
            ret &= pFace->updateAcoustic();
        }
        else
        {
            TYSpectre aTYSpectre;
            pFace->setRegime(aTYSpectre);
        }
    }

    // 3 cas le derner ret==false

    if (ret && (_typeDistribution == TY_PUISSANCE_IMPOSEE))
    {
        // Il faut s'assurer que la somme des puissances des volumes egale la puissance de la machine
        double residu = ABS(10 * log10(LWg.sigma()) - 10 * log10(LWc.sigma()));

        // Si l'ecart est inferieur a 1 dB on accepte
        ret = residu < 1 ? true : false;
    }
    else if (ret && (_typeDistribution == TY_PUISSANCE_CALCULEE))
    {
        // Puissance de la machine = puissance cumulee des volumes qui la composent
        LWc = OSpectre::getEmptyLinSpectre();
        LWc.setType(SPECTRE_TYPE_LW);

        for (i = 0 ; i < 6 ; i++)
        {
            LPTYAcousticSurfaceNode pFace = getFaceElement(i);
            if (pFace->getIsRayonnant()) { LWc = LWc.sum(pFace->getCurrentSpectre().toGPhy()); }
        }

        TYSpectre temp = LWc.toDB();
        this->setRegime(temp, -1, false); // Pas de probleme la puissance attribuee au volume peut etre modifiee
    }

    return ret;
}

TYTabAcousticSurfaceGeoNode TYAcousticBox::acousticFaces()
{
    TYTabAcousticSurfaceGeoNode tab;

    for (int i = 0; i < 6; i++)
    {
        // Sous-faces pour cette face
        TYTabAcousticSurfaceGeoNode tabTmp = getFaceElement(i)->getTabAcousticSurf();
        OMatrix matrixFace = _faces[i]->getMatrix();

        for (unsigned int j = 0; j < tabTmp.size(); j++)
        {
            LPTYAcousticSurface pAccSurf = TYAcousticSurface::safeDownCast(tabTmp[j]->getElement());

            // M = M(face) . M(sous-face)
            OMatrix matrix = matrixFace * tabTmp[j]->getMatrix();

            tab.push_back(new TYAcousticSurfaceGeoNode((LPTYElement)pAccSurf, matrix));
        }
    }

    return tab;
}

bool TYAcousticBox::findAcousticSurface(const TYAcousticSurface* pAccSurf, OMatrix* pMatrix /*=0*/)
{
    bool ret = false;

    for (int i = 0; (i < 6) && !ret; i++)
    {
        // Sous-faces pour cette face
        TYTabAcousticSurfaceGeoNode& tabTmp = getFaceElement(i)->getTabAcousticSurf();

        for (unsigned int j = 0; j < tabTmp.size(); j++)
        {
            if (TYAcousticSurface::safeDownCast(tabTmp[j]._pObj->getElement()) == pAccSurf)
            {
                if (pMatrix)
                {
                    // Mise a jour de la matrice
                    *pMatrix = *pMatrix * _faces[i]->getMatrix() * tabTmp[j]->getMatrix();
                }

                // Surf trouvee
                break;
            }
        }
    }

    return ret;
}

TYTabLPAcousticSurface TYAcousticBox::getSubFace()
{
    TYTabLPAcousticSurface tabSubFaces;

    // On balaye toutes les faces
    for (int i = 0; i < 6; i++)
    {
        TYTabAcousticSurfaceGeoNode tabSurf = getFaceElement(i)->getTabAcousticSurf();

        for (unsigned int j = 0; j < tabSurf.size(); j++)
        {
            LPTYAcousticSurface pSurface = TYAcousticSurface::safeDownCast(tabSurf[j]->getElement());

            if (pSurface->getIsSub())
            {
                tabSubFaces.push_back(pSurface);
            }
        }

    }

    return tabSubFaces;
}

void TYAcousticBox::setIsRayonnant(bool rayonnant, bool recursif)
{
    TYAcousticVolume::setIsRayonnant(rayonnant, recursif);
}
