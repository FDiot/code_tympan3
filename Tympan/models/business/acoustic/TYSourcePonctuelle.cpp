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
#include "Tympan/gui/widgets/TYSourcePonctuelleWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYSourcePonctuelleGraphic.h"
#endif


#include <typeinfo>

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/models/business/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/models/common/3d.h"

#include "Tympan/core/logging.h"

#if defined(WIN32)
#include <crtdbg.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif


TY_EXTENSION_INST(TYSourcePonctuelle);
TY_EXT_GRAPHIC_INST(TYSourcePonctuelle);

/*static*/ const double TYSourcePonctuelle::_tabRA[] = {  1.0,
                                                          2.0,
                                                          3.0,
                                                          4.0,
                                                          5.0,
                                                          10.0,
                                                          20.0,
                                                          30.0,
                                                          40.0,
                                                          50.0,
                                                          100.0,
                                                          200.0,
                                                          300.0,
                                                       };
/// Tableau des facteurs de correction
/*static*/ const double TYSourcePonctuelle::_tabCor[] = {0.608,
                                                         0.817,
                                                         0.879,
                                                         0.909,
                                                         0.928,
                                                         0.964,
                                                         0.982,
                                                         0.988,
                                                         0.991,
                                                         0.993,
                                                         0.996,
                                                         0.998,
                                                         1.000
                                                        };

TYSourcePonctuelle::TYSourcePonctuelle():
    _type(TypeSurface),
    _object(0),
    _typeRaynt(SPHERIC),
    _hauteur(0.0)
{
    _name = TYNameManager::get()->generateName(getClassName());

    float r = 255.0f, g = 0.0f, b = 0.0f;

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "SrcPonctGraphicColorR"))
    {
        TYPreferenceManager::getColor(TYDIRPREFERENCEMANAGER, "SrcPonctGraphicColor", r, g, b);
    }
    else
    {
        TYPreferenceManager::setColor(TYDIRPREFERENCEMANAGER, "SrcPonctGraphicColor", r, g, b);
    }

#endif

    // Couleur de preference.
    setColor(OColor(r / 255, g / 255, b / 255));

    _pPos = new TYPoint();
}

TYSourcePonctuelle::TYSourcePonctuelle(const TYSourcePonctuelle& other)
{
    *this = other;
}

TYSourcePonctuelle::~TYSourcePonctuelle()
{
}

TYSourcePonctuelle& TYSourcePonctuelle::operator=(const TYSourcePonctuelle& other)
{
    if (this != &other)
    {
        TYSource::operator =(other);
        TYColorInterface::operator =(other);
        _pPos = other._pPos;
        _orientation = other._orientation;
        _type = other._type;
        _typeRaynt = other._typeRaynt;
        _hauteur = other._hauteur;
        _object = other._object;
    }
    return *this;
}

bool TYSourcePonctuelle::operator==(const TYSourcePonctuelle& other) const
{
    if (this != &other)
    {
        if (TYSource::operator !=(other)) { return false; }
        if (TYColorInterface::operator !=(other)) { return false; }
        if (_pPos != other._pPos) { return false; }
        if (_orientation != other._orientation) { return false; }
        if (_type != other._type) { return false; }
        if (_typeRaynt != other._typeRaynt) { return false; }
        if (_hauteur != other._hauteur) { return false; }
        if (_object != other._object) { return false; }
    }
    return true;
}

bool TYSourcePonctuelle::operator!=(const TYSourcePonctuelle& other) const
{
    return !operator==(other);
}

bool TYSourcePonctuelle::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYSource::deepCopy(pOther, copyId)) { return false; }

    TYSourcePonctuelle* pOtherSrc = (TYSourcePonctuelle*) pOther;
    TYColorInterface::deepCopy((TYColorInterface*)pOtherSrc, copyId);

    _orientation = pOtherSrc->_orientation;
    _type = pOtherSrc->_type;
    _typeRaynt = pOtherSrc->_typeRaynt;
    _pPos->deepCopy(pOtherSrc->_pPos, copyId);
    _hauteur = pOtherSrc->_hauteur;
    _object = pOtherSrc->_object;

    return true;
}

std::string TYSourcePonctuelle::toString() const
{
    return "TYSourcePonctuelle";
}

DOM_Element TYSourcePonctuelle::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYSource::toXML(domElement);
    TYColorInterface::toXML(domNewElem);

    _pPos->toXML(domNewElem);

    DOM_Document domDoc = domElement.ownerDocument();

    DOM_Element vectorXElem = domDoc.createElement("Vector");
    domNewElem.appendChild(vectorXElem);
    vectorXElem.setAttribute("x", doubleToStrPre(_orientation._x, 3).data());
    vectorXElem.setAttribute("y", doubleToStrPre(_orientation._y, 3).data());
    vectorXElem.setAttribute("z", doubleToStrPre(_orientation._z, 3).data());

    TYXMLTools::addElementIntValue(domNewElem, "type", _type);

    if (_type == TypeUser)
    {
        TYXMLTools::addElementStringValue(domNewElem, "hauteur", doubleToStrPre(_hauteur, 3).data());
    }

    TYXMLTools::addElementIntValue(domNewElem, "typeRaynt", _typeRaynt);
    TYXMLTools::addElementIntValue(domNewElem, "formeObjet", _object);

    return domNewElem;
}

int TYSourcePonctuelle::fromXML(DOM_Element domElement)
{
    TYSource::fromXML(domElement);
    TYColorInterface::fromXML(domElement);

    // Reset
    bool typeOk = false;
    bool hauteurOk = false;
    bool typeRayntOk = false;
    bool formeObjetOk = false;

    DOM_Element elemCur;
    QDomNodeList childs = domElement.childNodes();

    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        _pPos->callFromXMLIfEqual(elemCur);
        if (elemCur.nodeName() == "Vector")
        {
            _orientation._x = TYXMLTools::getElementAttributeToDouble(elemCur, "x");
            _orientation._y = TYXMLTools::getElementAttributeToDouble(elemCur, "y");
            _orientation._z = TYXMLTools::getElementAttributeToDouble(elemCur, "z");
        }

        TYXMLTools::getElementIntValue(elemCur, "type", _type, typeOk);
        TYXMLTools::getElementDoubleValue(elemCur, "hauteur", _hauteur, hauteurOk);
        TYXMLTools::getElementIntValue(elemCur, "typeRaynt", _typeRaynt, typeRayntOk);
        TYXMLTools::getElementIntValue(elemCur, "formeObjet", _object, formeObjetOk);

    }

    return 1;
}

OSpectre TYSourcePonctuelle::lwApparenteSrcDest(const OSegment3D& seg, const TYAtmosphere& Atmo, const int& expGeo /*=0*/, const int& regime /*=-1*/) const
{
    OSpectre s = OSpectre::getEmptyLinSpectre();

    TYAcousticSurface* pSupport = NULL;
    TYAcousticVolume* pVolParent = NULL;
    OMatrix matrix; // Matrice de changement de repere entre le volume parent et cette source

    OVector3D v3D;
    double coef = 0.0;
    TYElement* pParent = NULL;

    // NOTA : On prend la racine du coefficient de directivite car celui-ci est eleve au carre lors du calcul


    // Type de directivite de la source
    switch (_typeRaynt)
    {
        case SPHERIC :
            coef = sqrt(1.0 + expGeo);
            s.setDefaultValue(coef); //Gain 1 (expansion geometrique 4pi) ou 2 (expansion geometrique 2pi)
            break;
        case HEMISPHERIC:
            coef = sqrt(2.0);
            s.setDefaultValue(coef);
            break;
        case SOLID:
            coef = 1;
            s.setDefaultValue(coef);
            break;
        case FORCED:
            coef = 1;
            s.setDefaultValue(coef);
            break;
        case CALCULATED:
            // Recherche en "paternite"
            pParent = getParent();

            // Recherche du support (de type AcousticSurface) de la source
            // Rmq : la position de la source est exprime dans le repere du support
            while (pParent)
            {
                pSupport = dynamic_cast<TYAcousticSurface*>(pParent);
                if (pSupport) { break; }

                pParent = pParent->getParent();
            }

            // Le support a ete trouve
            if (pSupport)
            {
                // On repart du parent du support
                pParent = pSupport->getParent();
            }
            else
            {
                // On repart du parent de la source
                pParent = getParent();
            }

            // Recherche du volume (de type AcousticVolume) parent (direct ou non)
            while (pParent)
            {
                pVolParent = dynamic_cast<TYAcousticVolume*>(pParent);
                if (pVolParent) { break; }

                pParent = pParent->getParent();
            }

            // Le volume parent a ete trouve
            if (pVolParent)
            {
                // Matrice de changement de repere entre le volume parent et cette source (son support)
                pVolParent->findAcousticSurface(pSupport, &matrix);
                matrix.invert();

                // Traitement selon cas
                TYElement* pElement = pVolParent->getParent();
                TYMachine* pMachine = NULL;
                TYEtage* pEtage = NULL;
                if (pElement)  { pMachine = dynamic_cast<TYMachine*>(pElement); }
                if (!pMachine) { pEtage = dynamic_cast<TYEtage*>(pElement); }
                if (pMachine)
                {
                    s = (calcDirectiviteMachine(pVolParent, pSupport, matrix, seg, Atmo)).racine();
                }
                else if (pEtage)
                {
                    s = (calcDirectiviteEtage(pVolParent, pSupport, matrix, seg, Atmo)).racine();
                }
                else // Cas du bâtiment cylindrique
                {
                    s = (calcDirectiviteMachine(pVolParent, pSupport, matrix, seg, Atmo)).racine();
                }
            }

            break;
        default:
            break;
    }

    return s;
}

OSpectre TYSourcePonctuelle::calcDirectiviteMachine(const TYAcousticVolume* pVolume, const TYAcousticSurface* pSupport, const OMatrix& matrix, const OSegment3D& seg, const TYAtmosphere& Atmo) const
{
    OSpectre s = OSpectre::getEmptyLinSpectre();

    if (!pVolume) { return s; }

    double q = 0.0; // Coefficient de directivite
    double ka = 0.0;

    // Rayon de la sphere englobant le volume
    double a = 1.0; //pVolume->getRayonSphere();

    // Centre de gravite du volume, passage dans le repere de la source
    //    const TYPoint& centre = matrix * pVolume->getCentreGravite();
    const OPoint3D centre = matrix * pVolume->getCentreGravite();

    // On teste si pSupport existe (tout sauf enveloppe cylindre et 1/2 cylindre)
    OVector3D normale;

    // On construit le vecteur indiquant l'exterieur du volume
    OVector3D extVolume(centre, seg._ptA);

    // Tentative pour eviter le mecanisme de RTTI de Code_TYMPAN (OPrototype)
    TYAcousticVolume* pVolTemp = const_cast<TYAcousticVolume*>(pVolume);
    TYAcousticCylinder* pCyl = dynamic_cast<TYAcousticCylinder*>(pVolTemp);
    TYAcousticSemiCylinder* pSCyl = NULL;
    if (!pCyl) { pSCyl = dynamic_cast<TYAcousticSemiCylinder*>(pVolTemp); }

    if ((pSupport == NULL) || (pCyl || pSCyl))
    {
        normale = OVector3D(centre, seg._ptA);

        if (pCyl)
        {
            a = pCyl->getDiameter();
        }
        else if (pSCyl)
        {
            double diag = pSCyl->getDiameter();
            a = sqrt(diag * diag + (diag / 2) * (diag / 2));
        }
    }
    else if (pSupport != NULL)
    {
        //      normale = pSupport->normal() * (-1); // DT20070724 : on multiplie par -1 car la normale a change de sens
        normale = pSupport->normal(); // DT20110128 : on est revenu a la normale ...

        // On change le sens de la normale s'il n'est pas dirige dans le meme sens que l'axe centre du volume/source
        if (extVolume.scalar(normale) < 0.0) { normale = normale * (-1); }

        // Si pVolume est un cylindre ou un semicylindre
        if (pCyl)
        {
            a = pCyl->getDiameter();
        }
        else if (pSCyl)
        {
            double diag = pSCyl->getDiameter();
            a = sqrt(diag * diag + (diag / 2) * (diag / 2));
        }
        else
        {
            TYAcousticSurface* pTempSurf = const_cast<TYAcousticSurface*>(pSupport);
            TYAcousticRectangle* pRectangle = dynamic_cast<TYAcousticRectangle*>(pTempSurf);
            if (pRectangle) { a = pRectangle->getBoundingRect()->getDiagSize(); }
        }
    }
    else // pSupport == NULL
    {
        return s;
    }

    // Angle de la normale a la source par rapport au vecteur Centre du volume -> Point de reception
    OVector3D evalPhi(seg._ptA, seg._ptB); //evalPhi(centre, seg._ptB); DT 11-02-04
    double cosPhi = cos(normale.angle(evalPhi));

    double C = calculC(seg.longueur(), a);  //Coefficient de directivite

    for (unsigned int i = 0 ; i < s.getNbValues() ; i++)
    {
        ka = Atmo.getKAcoust().getTabValReel()[i] * a ;

        q = (1 + (ka / (ka + 0.4)) * cosPhi) * C;// * 2.0;

        s.getTabValReel()[i] = q;
    }

    return s;
}

OSpectre TYSourcePonctuelle::calcDirectiviteEtage(const TYAcousticVolume* pVolume, const TYAcousticSurface* pSupport, const OMatrix& matrix, const OSegment3D& seg, const TYAtmosphere& Atmo) const
{

    OSpectre s = OSpectre::getEmptyLinSpectre();

    if (!pVolume) { return s; } // remplace le assert

    double q = 0.0 ;// Coefficient de directivite
    double ka = 0.0;

    double a = 1.0;

    // Centre de gravite du volume, passage dans le repere de la source
    const OPoint3D centre = matrix * pVolume->getCentreGravite();

    // On teste si pSupport existe (tout sauf enveloppe cylindre et 1/2 cylindre)
    OVector3D normale;

    // On construit le vecteur indiquant l'exterieur du volume
    OVector3D extVolume(centre, seg._ptA);

    // Tentative pour eviter le mecanisme de RTTI de Code_TYMPAN (OPrototype)
    TYAcousticVolume* pVolTemp = const_cast<TYAcousticVolume*>(pVolume);
    TYAcousticCylinder* pCyl = dynamic_cast<TYAcousticCylinder*>(pVolTemp);
    TYAcousticSemiCylinder* pSCyl = NULL;
    if (!pCyl) { pSCyl = dynamic_cast<TYAcousticSemiCylinder*>(pVolTemp); }

    if ((pSupport == NULL) && (pCyl || pSCyl))
    {
        normale = OVector3D(centre, seg._ptA);

        if (pCyl)
        {
            a = pCyl->getDiameter();
        }
        else if (pSCyl)
        {
            double diag = pSCyl->getDiameter();
            a = sqrt(diag * diag + (diag / 2) * (diag / 2));
        }
    }
    else if (pSupport != NULL)
    {
        normale = pSupport->normal();
        // On change le sens de la normale s'il n'est pas dirige dans le meme sens que l'axe centre du volume/source
        if (extVolume.scalar(normale) < 0.0) { normale = normale * (-1); }

        // Si pVolume est un cylindre ou un semicylindre
        if (pCyl)
        {
            if (pCyl) { a = pCyl->getDiameter(); }
        }
        else if (pSCyl)
        {
            double diag = pSCyl->getDiameter();
            a = sqrt(diag * diag + (diag / 2) * (diag / 2));
        }
        else
        {
            TYAcousticSurface* pTempSurf = const_cast<TYAcousticSurface*>(pSupport);
            TYAcousticRectangle* pRectangle = dynamic_cast<TYAcousticRectangle*>(pTempSurf);
            if (pRectangle) { a = pRectangle->getBoundingRect()->getDiagSize(); }
        }
    }
    else // pSupport == NULL
    {
        return s;
    }


    // Angle de la normale a la source par rapport au vecteur Centre du volume -> Point de reception
    OVector3D evalPhi(seg._ptA, seg._ptB); //evalPhi(centre, seg._ptB); DT 11-02-04
    double cosPhi = cos(normale.angle(evalPhi));

    double C = calculC(seg.longueur(), a);  //Coefficient de directivite

    for (unsigned int i = 0 ; i < s.getNbValues() ; i++)
    {
        ka = Atmo.getKAcoust().getTabValReel()[i] * a ;

        q = (1 + (ka / (ka + 0.4)) * cosPhi) * C; //* 2.0;

        s.getTabValReel()[i] = q;
    }

    return s;
}

inline double TYSourcePonctuelle::calculC(const double& r, const double& a) const
{
    //#define STAGIAIRE 1
    //#define PERSO 1
#define TYMPAN2 1

    double C, X;
#ifdef PERSO
    X = (r / a);

    X = X < 1 ? 1 : X ;

    X = X - 0.178;

    C = X / (1 + X) + X / (2 * (1 + X * X)) - X / (6 * (1 + X * X * X)) ;
#endif

#ifdef STAGIAIRE
    C = r / (r + (2.0 * a / 3.0));
#endif

#ifdef TYMPAN2
    X = r / a;

    if (X < 1.0) { return _tabCor[0]; }
    if (X > 300.0) { return 1.0; }

    int i = 1;
    while (_tabRA[i] < X && i < 12) { i++ ; } // Recherche de l'indice le plus proche

    // Calcul de la valeur de c par interpolation lineaire
    C = (_tabCor[i] - _tabCor[i - 1]) / (_tabRA[i] - _tabRA[i - 1]) * (X - _tabRA[i - 1]) + _tabCor[i - 1];

#endif
    return C;
}

void TYSourcePonctuelle::setType(int type)
{
    _type = type;

    // Si c'est une source utilisateur, il faut lui affecter une orientation
    if (_type == TYSourcePonctuelle::TypeUser)
    {
        _orientation = OVector3D(1, 0, 0);
    }
}

