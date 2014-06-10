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




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/CommonTools/OSegment3D.h"

#include "Tympan/Tools/OMessageManager.h"

// nombre de valeurs de ka dans le tableau
#ifdef NB_KA
#   undef NB_KA
#   define NB_KA 9
#else
#   define NB_KA 9
#endif

// nombre de valeurs de theta dans le tableau
#ifdef NB_THETA
#   undef NB_THETA
#   define NB_THETA 41
#else
#   define NB_THETA 41
#endif


const double TYSourceBafflee::_tabKa[NB_KA] = { 0.05, 0.1, 0.25, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0 };

const double TYSourceBafflee::_tabQ[NB_KA][NB_THETA] =
{
    {
        /*ka=0,05*/ 2.0382E+00, 2.0382E+00, 1.7752E+00, 1.7752E+00, 2.0382E+00, 2.0382E+00, 1.6953E+00,
        1.4101E+00, 1.2860E+00, 1.1729E+00, 8.1142E-01, 6.0152E-01, 4.4591E-01, 3.3056E-01,
        2.4505E-01, 1.8165E-01, 1.3466E-01, 9.9827E-02, 7.4003E-02, 5.4859E-02, 4.0667E-02,
        5.4859E-02, 7.4003E-02, 9.9827E-02, 1.3466E-01, 1.8165E-01, 2.4505E-01, 3.3056E-01,
        4.4591E-01, 6.0152E-01, 8.1142E-01, 1.1729E+00, 1.2860E+00, 1.4101E+00, 1.6953E+00,
        2.0382E+00, 2.0382E+00, 1.7752E+00, 1.7752E+00, 2.0382E+00, 2.0382E+00
    },
    {
        /*ka=0,1*/  1.0215E+00, 1.0215E+00, 1.0215E+00, 1.1201E+00, 1.2860E+00, 1.2860E+00, 1.2860E+00,
        1.4765E+00, 1.6190E+00, 1.6190E+00, 1.6190E+00, 1.1201E+00, 7.7490E-01, 5.3610E-01,
        3.7089E-01, 2.5659E-01, 1.7752E-01, 1.2281E-01, 8.4966E-02, 5.8782E-02, 4.0667E-02,
        5.8782E-02, 8.4966E-02, 1.2281E-01, 1.7752E-01, 2.5659E-01, 3.7089E-01, 5.3610E-01,
        7.7490E-01, 1.1201E+00, 1.6190E+00, 1.6190E+00, 1.6190E+00, 1.4765E+00, 1.2860E+00,
        1.2860E+00, 1.2860E+00, 1.1201E+00, 1.0215E+00, 1.0215E+00, 1.0215E+00
    },
    {
        /*ka=0,25*/ 2.1534E+00, 1.7911E+00, 1.4898E+00, 1.3587E+00, 1.4227E+00, 1.7105E+00, 1.7105E+00,
        1.4898E+00, 1.3587E+00, 1.3587E+00, 1.3587E+00, 9.6189E-01, 6.8096E-01, 4.8209E-01,
        3.4129E-01, 2.4162E-01, 1.7105E-01, 1.2109E-01, 8.5728E-02, 6.0691E-02, 4.2966E-02,
        6.0691E-02, 8.5728E-02, 1.2109E-01, 1.7105E-01, 2.4162E-01, 3.4129E-01, 4.8209E-01,
        6.8096E-01, 9.6189E-01, 1.3587E+00, 1.3587E+00, 1.3587E+00, 1.4898E+00, 1.7105E+00,
        1.7105E+00, 1.4227E+00, 1.3587E+00, 1.4898E+00, 1.7911E+00, 2.1534E+00
    },
    {
        /*ka=0,5*/  1.4385E+00, 1.4385E+00, 1.4385E+00, 1.5773E+00, 1.8109E+00, 1.8109E+00, 2.1772E+00,
        1.9857E+00, 1.5063E+00, 1.0912E+00, 9.0762E-01, 6.7283E-01, 4.9877E-01, 3.6975E-01,
        2.7410E-01, 2.0319E-01, 1.5063E-01, 1.1166E-01, 8.2776E-02, 6.1363E-02, 4.5489E-02,
        6.1363E-02, 8.2776E-02, 1.1166E-01, 1.5063E-01, 2.0319E-01, 2.7410E-01, 3.6975E-01,
        4.9877E-01, 6.7283E-01, 9.0762E-01, 1.0912E+00, 1.5063E+00, 1.9857E+00, 2.1772E+00,
        1.8109E+00, 1.8109E+00, 1.5773E+00, 1.4385E+00, 1.4385E+00, 1.4385E+00
    },
    {
        /*ka=1*/    3.5678E+00, 4.2895E+00, 4.4916E+00, 3.7360E+00, 2.7065E+00, 2.2511E+00, 1.2954E+00,
        8.5586E-01, 7.8055E-01, 8.5586E-01, 7.1187E-01, 5.4001E-01, 4.0964E-01, 3.1074E-01,
        2.3572E-01, 1.7881E-01, 1.3564E-01, 1.0290E-01, 7.8055E-02, 5.9211E-02, 4.4916E-02,
        5.9211E-02, 7.8055E-02, 1.0290E-01, 1.3564E-01, 1.7881E-01, 2.3572E-01, 3.1074E-01,
        4.0964E-01, 5.4001E-01, 7.1187E-01, 8.5586E-01, 7.8055E-01, 8.5586E-01, 1.2954E+00,
        2.2511E+00, 2.7065E+00, 3.7360E+00, 4.4916E+00, 4.2895E+00, 3.5678E+00
    },
    {
        /*ka=2*/    1.1891E+01, 9.8907E+00, 7.1652E+00, 4.5209E+00, 2.6015E+00, 1.4970E+00, 1.2452E+00,
        7.8565E-01, 5.4353E-01, 4.5209E-01, 3.7603E-01, 3.0565E-01, 2.4844E-01, 2.0194E-01,
        1.6414E-01, 1.3342E-01, 1.0845E-01, 8.8151E-02, 7.1652E-02, 5.8241E-02, 4.7340E-02,
        5.8241E-02, 7.1652E-02, 8.8151E-02, 1.0845E-01, 1.3342E-01, 1.6414E-01, 2.0194E-01,
        2.4844E-01, 3.0565E-01, 3.7603E-01, 4.5209E-01, 5.4353E-01, 7.8565E-01, 1.2452E+00,
        1.4970E+00, 2.6015E+00, 4.5209E+00, 7.1652E+00, 9.8907E+00, 1.1891E+01
    },
    {
        /*ka=5*/    4.5941E+00, 4.5941E+00, 4.5941E+00, 4.1899E+00, 3.1784E+00, 1.8290E+00, 1.2653E+00,
        1.0051E+00, 8.3600E-01, 6.9535E-01, 5.7837E-01, 4.4896E-01, 3.4850E-01, 2.7052E-01,
        2.0999E-01, 1.6301E-01, 1.2653E-01, 9.8221E-02, 7.6244E-02, 5.9184E-02, 4.5941E-02,
        5.9184E-02, 7.6244E-02, 9.8221E-02, 1.2653E-01, 1.6301E-01, 2.0999E-01, 2.7052E-01,
        3.4850E-01, 4.4896E-01, 5.7837E-01, 6.9535E-01, 8.3600E-01, 1.0051E+00, 1.2653E+00,
        1.8290E+00, 3.1784E+00, 4.1899E+00, 4.5941E+00, 4.5941E+00, 4.5941E+00
    },
    {
        /*ka=10*/   3.8007E+00, 3.1613E+00, 3.0190E+00, 2.7534E+00, 2.3981E+00, 2.3981E+00, 1.9946E+00,
        1.4450E+00, 1.0961E+00, 8.7070E-01, 6.0237E-01, 4.6759E-01, 3.6297E-01, 2.8175E-01,
        2.1871E-01, 1.6977E-01, 1.3179E-01, 1.0230E-01, 7.9408E-02, 6.1641E-02, 4.7848E-02,
        6.1641E-02, 7.9408E-02, 1.0230E-01, 1.3179E-01, 1.6977E-01, 2.1871E-01, 2.8175E-01,
        3.6297E-01, 4.6759E-01, 6.0237E-01, 8.7070E-01, 1.0961E+00, 1.4450E+00, 1.9946E+00,
        2.3981E+00, 2.3981E+00, 2.7534E+00, 3.0190E+00, 3.1613E+00, 3.8007E+00
    },
    {
        /*ka=20*/   2.3330E+00, 2.3330E+00, 2.3330E+00, 2.3330E+00, 2.3330E+00, 2.3330E+00, 2.3330E+00,
        1.7698E+00, 1.2244E+00, 8.4708E-01, 5.8603E-01, 4.5491E-01, 3.5312E-01, 2.7411E-01,
        2.1278E-01, 1.6517E-01, 1.2821E-01, 9.9523E-02, 7.7254E-02, 5.9968E-02, 4.6550E-02,
        5.9968E-02, 7.7254E-02, 9.9523E-02, 1.2821E-01, 1.6517E-01, 2.1278E-01, 2.7411E-01,
        3.5312E-01, 4.5491E-01, 5.8603E-01, 8.4708E-01, 1.2244E+00, 1.7698E+00, 2.3330E+00,
        2.3330E+00, 2.3330E+00, 2.3330E+00, 2.3330E+00, 2.3330E+00, 2.3330E+00
    }
};
TYSourceBafflee::TYSourceBafflee()
{
    _name = TYNameManager::get()->generateName(getClassName());

    _type = TypeBouche;

    //  _pAcousticRectangle = new TYAcousticRectangle();
    _pAcousticRectangle = new TYRectangle();
}

TYSourceBafflee::TYSourceBafflee(const TYSourceBafflee& other)
{
    *this = other;
}

TYSourceBafflee::~TYSourceBafflee()
{
}

TYSourceBafflee& TYSourceBafflee::operator=(const TYSourceBafflee& other)
{
    if (this != &other)
    {
        TYSourcePonctuelle::operator =(other);
        _pAcousticRectangle = other._pAcousticRectangle;
    }
    return *this;
}

bool TYSourceBafflee::operator==(const TYSourceBafflee& other) const
{
    if (this != &other)
    {
        if (TYSourcePonctuelle::operator !=(other)) { return false; }
        if (_pAcousticRectangle != other._pAcousticRectangle) { return false; }
    }
    return true;
}

bool TYSourceBafflee::operator!=(const TYSourceBafflee& other) const
{
    return !operator==(other);
}

bool TYSourceBafflee::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYSource::deepCopy(pOther, copyId)) { return false; }

    TYSourceBafflee* pOtherSrc = (TYSourceBafflee*) pOther;

    if (_pAcousticRectangle && pOtherSrc->_pAcousticRectangle)
    {
        _pAcousticRectangle->deepCopy(pOtherSrc->_pAcousticRectangle, copyId);
    }
    else
    {
        _pAcousticRectangle = NULL;
    }

    return true;
}

std::string TYSourceBafflee::toString() const
{
    return "TYSourceBafflee";
}

DOM_Element TYSourceBafflee::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYSourcePonctuelle::toXML(domElement);

    if (_pAcousticRectangle)
    {
        // Mise en commentaire DT20061228 - car cette ligne devrait etre inutile
        //      TYXMLTools::addElementStringValue(domNewElem, "accRectId", _pAcousticRectangle->getID().toString().c_str());
    }

    return domNewElem;
}

int TYSourceBafflee::fromXML(DOM_Element domElement)
{
    TYSourcePonctuelle::fromXML(domElement);

    _pAcousticRectangle = NULL;

    QString id;
    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();

    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        if (TYXMLTools::getElementStringValue(elemCur, "accRectId", id))
        {
            _pAcousticRectangle = TYRectangle::safeDownCast(TYElement::getInstance(id));
        }
    }

    return 1;
}

OSpectre TYSourceBafflee::lwApparenteSrcDest(const OSegment3D& seg, const TYAtmosphere& atmos, const int& expGeo /*=0*/, const int& regime/*=-1*/) const
{
    OSpectre s = OSpectre::getEmptyLinSpectre();

    if (!_pAcousticRectangle)
    {
        return s;
    }

    //    OSpectre spectre_Ka = atmos.getKAcoust().mult(_pAcousticRectangle->getBoundingRect()->getCircleEqDiameter() / 2.0); // 1/2 longueur de la diagonale de la bouche
    OSpectre spectre_Ka = atmos.getKAcoust().mult(_pAcousticRectangle->getCircleEqDiameter() / 2.0); // 1/2 longueur de la diagonale de la bouche

    OVector3D v3D = seg.toVector3D();
    OVector3D normale = _pAcousticRectangle->normal();
    double theta = v3D.angle(normale); // Angle du segment par rapport a la normale a la source

    // Angle compris entre 0 et 2pi;
    if (theta < 0) { theta = theta + M_2PI; }
    if (theta > M_2PI) { theta = theta - M_2PI; }

    int indice_theta = (int)(20.0 * theta / M_PI);

    indice_theta = indice_theta > (NB_THETA - 2) ? NB_THETA - 2 : indice_theta; // Eviter les depassement de tableau

    for (unsigned int i = 0 ; i < s.getNbValues(); i++)
    {
        double ka = spectre_Ka.getTabValReel()[i];
        ka = ka > 20.0 ? 20.0 : ka ;

        int indice_Ka = getIndiceKa(ka);
        indice_Ka = indice_Ka > (NB_KA - 2) ? NB_KA - 2 : indice_Ka; // Eviter les depassement de tableau

        s.getTabValReel()[i] = normeQBaffle(indice_Ka, indice_theta, ka, theta);
    }

    return s.racine();
}

int TYSourceBafflee::getIndiceKa(const double& ka) const
{
    int indice = 0;

    while ((_tabKa[indice] < ka) && (indice < (NB_KA - 1)))
    {
        indice++;
    }

    return indice = indice > 0 ? indice - 1 : 0;
}

double TYSourceBafflee::normeQBaffle(const int& indice_Ka, const int& indice_theta, const double& ka, const double& theta) const
{
    /* ====================================================================================
        Recherche par interpolation lineaire dans le tableau _tabQ
        de la valeur de Q la plus proche
       ===================================================================================*/
    double tabQ1_1 = _tabQ[indice_Ka]  [indice_theta];
    double tabQ1_2 = _tabQ[indice_Ka]  [indice_theta + 1];
    double tabQ2_1 = _tabQ[indice_Ka + 1][indice_theta];
    double tabQ2_2 = _tabQ[indice_Ka + 1][indice_theta + 1];

    double ka1 = _tabKa[indice_Ka];
    double ka2 = _tabKa[indice_Ka + 1];

    double theta1 =  indice_theta    * M_PI / 20;
    double theta2 = (indice_theta + 1) * M_PI / 20;

    //  double pos_theta = (theta - theta1) / (theta2 - theta1);

    double val1 = tabQ1_1 + (theta - theta1) * (tabQ1_2 - tabQ1_1) / (theta2 - theta1);
    double val2 = tabQ2_1 + (theta - theta1) * (tabQ2_2 - tabQ2_1) / (theta2 - theta1);

    double val = val1 + (ka - ka1) * (val2 - val1) / (ka2 - ka1);


    return val;
}

//void TYSourceBafflee::setAcousticRectangle(LPTYAcousticRectangle pAcousticRectangle)
void TYSourceBafflee::setAcousticRectangle(LPTYRectangle pAcousticRectangle)
{

    _pAcousticRectangle = pAcousticRectangle;
    _pAcousticRectangle->setParent(this);
    setIsAcousticModified(true);
}

