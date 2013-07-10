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

#include <qdir.h>

#include "Tympan/Tools/OMessageManager.h"
#include "Tympan/MetierSolver/DataManagerCore/TYXMLManager.h"

OPROTOINST(TYSpectre);

// Precision d'ecriture des resultats dans les fichiers XML
int TYSpectre::_XMLprecision = 1;

TYSpectre::TYSpectre(const double &defaultValue) : OSpectre(defaultValue)
{
    _name = TYNameManager::get()->generateName(getClassName());

    _remarque = "";
    _isReadOnly = false;
}

TYSpectre::TYSpectre(const TYSpectre& other)
{
    *this = other;
}

TYSpectre::TYSpectre(const OSpectre& spectre)
{
    _name = TYNameManager::get()->generateName(getClassName());

    OSpectre::operator = (spectre);

    _remarque = "";
    _isReadOnly = false;
}

TYSpectre::TYSpectre(const OSpectreComplex& spectre)
{
    _name = TYNameManager::get()->generateName(getClassName());

    OSpectre::operator = (spectre);

    _remarque = "";
    _isReadOnly = false;
}

TYSpectre::~TYSpectre()
{
}

TYSpectre& TYSpectre::operator=(const TYSpectre& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        OSpectre::operator =(other);
        _isReadOnly = other._isReadOnly;
        _remarque = other._remarque;
    }

    return *this;
}


TYSpectre& TYSpectre::operator=(const OSpectre& other)
{
    if (this != &other)
    {
        OSpectre::operator=(other);
        _isReadOnly = false;
        _remarque = "";
    }
    return *this;
}

bool TYSpectre::operator==(const TYSpectre& other) const
{
    if (this != &other)
    {
        if (_valid != other._valid) { return false; }
        if (TYElement::operator !=(other)) { return false; }
        if (OSpectre::operator != (other)) { return false; }
        if (_isReadOnly != other._isReadOnly) { return false; }
        if (_remarque != other._remarque) { return false; }
    }

    return true;
}

bool TYSpectre::operator!=(const TYSpectre& other) const
{
    return !operator==(other);
}

bool TYSpectre::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    TYSpectre* pOtherSpectre = (TYSpectre*) pOther;

    OSpectre::operator =(*pOtherSpectre);

    _isReadOnly = pOtherSpectre->_isReadOnly;
    _remarque = pOtherSpectre->_remarque;

    return true;
}

std::string TYSpectre::toString() const
{
    return "TYSpectre";
}


DOM_Element TYSpectre::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);

    TYXMLTools::addElementIntValue(domNewElem, "type", _type);

    if (!_remarque.isEmpty())
    {
        TYXMLTools::addElementStringValue(domNewElem, "rmq", _remarque);
    }

    TYXMLTools::addElementDoubleValue(domNewElem, "fMin", _fMin);
    TYXMLTools::addElementDoubleValue(domNewElem, "fMax", _fMax);

    DOM_Document domDoc = domElement.ownerDocument();

    // Cree un nouveau noeud pour l'ensemble des frequence
    DOM_Element freqTabElem = domDoc.createElement("freqTab");
    domNewElem.appendChild(freqTabElem);

    // Ajout de la forme du spectre
    freqTabElem.setAttribute("form", _form);

    // Formatage des donnees
    std::ostringstream oss;
    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT - 1; ++i)
    {
        oss << doubleToStrPre(_module[i], _XMLprecision) << ' ';
    }
    oss << doubleToStrPre(_module[TY_SPECTRE_DEFAULT_NB_ELMT - 1], _XMLprecision);
    DOMString tab(oss.str().c_str());

    // Ajout des donnees
    QDomText data = domDoc.createTextNode(tab);

    freqTabElem.appendChild(data);

    return domNewElem;
}


int TYSpectre::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);

    setDefaultValue();

    DOM_Element elemCur;
    bool typeOk = false;
    bool fMinOk = false;
    bool fMaxOk = false;
    bool fDomain = false;
    bool keepInBDDOk = false;
    bool rmqOk = false;
    int type;
    float fMin;
    float fMax;
    float freq;
    double reel;
    double imag;

    QDomNodeList childs = domElement.childNodes();

    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        TYXMLTools::getElementIntValue(elemCur, "type", type, typeOk);
        if (typeOk)
        {
            _type = (TYSpectreType) type;
        }

        TYXMLTools::getElementStringValue(elemCur, "rmq", _remarque, rmqOk);
        TYXMLTools::getElementFloatValue(elemCur, "fMin", fMin, fMinOk);
        TYXMLTools::getElementFloatValue(elemCur, "fMax", fMax, fMaxOk);

        if (fMinOk && fMaxOk && !fDomain)
        {
            setDefaultValue();
            fDomain = true;
        }

        if (elemCur.nodeName() == "freqTab")
        {
            _form = (TYSpectreForm)TYXMLTools::getElementAttributeToInt(elemCur, "form");

            // On recupere les valeurs
            QString tab;
            TYXMLTools::getElementStringValue(elemCur, "freqTab", tab);
            std::istringstream iss(tab.toAscii().data());

            // On extrait les valeurs suivants la forme du spectres
            switch (_form)
            {
                case SPECTRE_FORM_TIERS:
                {
                    float reel;
                    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; ++i)
                    {
                        iss >> reel;
                        _module[i] = reel;
                    }
                }
                break;
                // Rajout possible dans le futur...
                default:
                    break;

            }
        }

        // Ancienne version
        if (elemCur.nodeName() == "freqCplx")
        {
            freq = TYXMLTools::getElementAttributeToDouble(elemCur, "f");
            reel = TYXMLTools::getElementAttributeToDouble(elemCur, "re");
            imag = TYXMLTools::getElementAttributeToDouble(elemCur, "im");

            setValue(freq, reel);
        }

    }

    return 1;
}

bool TYSpectre::toXML(const std::string& sFilePath)
{
    bool bRet = false;
    QString fileName = QString(sFilePath.c_str());
    int i = fileName.lastIndexOf('/');
    QDir fileDirectory = QDir(fileName.mid(0, i));
    if (!fileDirectory.exists())
    {
        fileDirectory.mkdir(fileName.mid(0, i));
    }

    if (!fileName.isEmpty())
    {
        if (!fileName.endsWith(".xml"))
        {
            fileName += ".xml";
        }
    }
    else
    {
        return false;
    }

    TYXMLManager xmlManager;

    xmlManager.createDoc(TY_PRODUCT_XMLTAG_, TY_PRODUCT_VERSION_);
    xmlManager.addElement(this);

    if (xmlManager.save(fileName) == 0)
    {
        bRet = true;
    }

    return bRet;
}

bool TYSpectre::fromXML(const std::string& sFilePath)
{
    bool bRet = false;

    QString fileName = QString(sFilePath.c_str());

    if (fileName.isEmpty())
    {
        return false;
    }

    TYXMLManager xmlManager;
    TYElementCollection elements;
    if (xmlManager.load(fileName, elements))
    {
        if (elements.size() == 1)
        {
            LPTYElement elt = elements[0];
            if (strcmp(elt->getClassName(), "TYSpectre") == 0)
            {
                TYSpectre* pTmpSpectre = static_cast<TYSpectre*>(elt.getRealPointer());
                TYSpectre::operator=(*pTmpSpectre);
                bRet = true;
            }
        }
    }

    return bRet;
}

std::string TYSpectre::toXMLString()
{
    TYXMLManager xmlManager;

    xmlManager.createDoc(TY_PRODUCT_XMLTAG_, TY_PRODUCT_VERSION_);
    xmlManager.addElement(this);

    QString retString = xmlManager.saveToString();

    return retString.toStdString();
}

bool TYSpectre::fromXMLString(const std::string& sXMLString)
{
    bool bRet = false;

    TYXMLManager xmlManager;
    TYElementCollection elements;
    if (xmlManager.loadFromString(QString(sXMLString.c_str()), elements))
    {
        if (elements.size() == 1)
        {
            LPTYElement elt = elements[0];
            if (strcmp(elt->getClassName(), "TYSpectre") == 0)
            {
                TYSpectre* pTmpSpectre = static_cast<TYSpectre*>(elt.getRealPointer());
                TYSpectre::operator=(*pTmpSpectre);
                bRet = true;
            }
        }
    }

    return bRet;
}

bool TYSpectre::equivTo(const TYSpectre& other) const
{
    if (*this == other) { return true; }
    return false;
}

TYSpectre TYSpectre::toTOct() const
{
    TYSpectre s = TYSpectre::getEmptyLinSpectre();

    if (_form == SPECTRE_FORM_TIERS) // Si on est deja en tiers d'octave
    {
        s = *this;
        return s;
    }
    else if (_form != SPECTRE_FORM_OCT) // Si le spectre n'est pas en octave
    {
        s._valid = false;
        return s;
    }

    s._type = _type;

    if (_type != SPECTRE_TYPE_ABSO)
    {
        TYSpectre travail = this->toGPhy();

        short indice = 2;
        short nbOctValue = 9;

        double valeur = 0.0;
        double coef = 3.0;

        if (_type == SPECTRE_TYPE_ATT) { coef = 1.0; }

        for (short i = 0 ; i < nbOctValue ; i++, indice += 3)
        {
            valeur = travail._module[i] / coef;

            s._module[indice] = valeur;
            s._module[indice + 1] = valeur;
            s._module[indice + 2] = valeur;
        }

        if (_type == SPECTRE_TYPE_ATT)
        {
            s._module[0]  = 1;
            s._module[1]  = 1;
            s._module[29] = 1;
            s._module[30] = 1;
        }

        s = s.toDB();
    }
    else
    {
        TYSpectre travail = *this;

        short indice = 2;
        short nbOctValue = 9;

        double valeur = 0.0;

        for (short i = 0 ; i < nbOctValue ; i++, indice += 3)
        {
            valeur = travail._module[i];

            s._module[indice] = valeur;
            s._module[indice + 1] = valeur;
            s._module[indice + 2] = valeur;
        }

        s._module[0]  = 1;
        s._module[1]  = 1;
        s._module[29] = 1;
        s._module[30] = 1;

        s._etat = SPECTRE_ETAT_DB;
    }

    s._form = SPECTRE_FORM_TIERS; // indication explicite de la forme tiers d'octave

    return s;
}

TYSpectre TYSpectre::toOct() const
{
    TYSpectre s = TYSpectre::makeOctSpect();

    if (_form == SPECTRE_FORM_OCT)  // Si le spectre est deja en octave
    {
        s = *this;
        return s;
    }
    else if (_form != SPECTRE_FORM_TIERS)  // Si le spectre n'est pas en tiers d'octave
    {
        s._valid = false;
        return s;
    }

    s._etat = SPECTRE_ETAT_LIN; // s aussi est en lin a ce moment
    s._type = _type;

    if (_type != SPECTRE_TYPE_ABSO)
    {
        TYSpectre travail = this->toGPhy();

        unsigned int indiceDepart = 2;
        unsigned int indice = 0;

        double valeur = 0.0;
        double coef = 1.0;

        if (_type == SPECTRE_TYPE_ATT) { coef = 3.0; }

        for (unsigned int i = indiceDepart ; i < TY_SPECTRE_DEFAULT_NB_ELMT - 2 ; i += 3, indice++)
        {
            valeur = (travail._module[i] + travail._module[i + 1] + travail._module[i + 2]) / coef;

            s._module[indice] = valeur;
        }

        s = s.toDB();
    }
    else
    {
        TYSpectre travail = *this;

        unsigned int indiceDepart = 2;
        unsigned int indice = 0;

        double valeur = 0.0;

        for (unsigned int i = indiceDepart ; i < TY_SPECTRE_DEFAULT_NB_ELMT - 2 ; i += 3, indice++)
        {
            valeur = (travail._module[i] + travail._module[i + 1] + travail._module[i + 2]) / 3; // Moyenne des absos

            s._module[indice] = valeur;
        }

        s._etat = SPECTRE_ETAT_DB;
    }

    s._form = SPECTRE_FORM_OCT; // indication explicite de la forme octave

    return s;

}

void TYSpectre::exportCSV(const std::string& filename)
{
    std::ofstream ofs;
    ofs.open(filename.c_str(), std::ios_base::out);
    if (ofs.is_open())
    {
        exportCSV(ofs);
        ofs.close();
    }
}

void TYSpectre::exportCSV(std::ofstream& ofs)
{
    TYTabFreq tabFreq = TYSpectre::getTabFreqNorm(getForm());

    ofs.setf(std::ios::fixed, std::ios::floatfield);
    ofs.precision(2);
    ofs << "freq" << ";" << "dB\n";

    for (int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        ofs << tabFreq[i] << ";" << getTabValReel()[i] << "\n";
    }

    ofs << "Global (Lin)" << ";" << valGlobDBLin() << "\n";
    ofs << "Global (A)" << ";" << valGlobDBA() << "\n";

    ofs << '\n';
}

TYSpectre TYSpectre::makeOctSpect()
{
    TYSpectre s;
    s._form = SPECTRE_FORM_OCT;

    return s;
}

const TYTabFreq TYSpectre::getTabFreqNorm(TYSpectreForm form)
{
    TYTabFreq tabFreq;
    short offset = 0;
    short facteur = 1;
    short nbOfVal = TY_SPECTRE_DEFAULT_NB_ELMT;

    if (form == SPECTRE_FORM_OCT)
    {
        offset = 3;
        facteur = 3;
        nbOfVal = 9;
    }

    for (short i = 0; i < nbOfVal; i++)
    {
        tabFreq.push_back(_freqNorm[(i * facteur) + offset]);
    }

    return tabFreq;
}

const float TYSpectre::getFreqNorm(int freqIndex)
{
    if ((freqIndex >= 0) && (freqIndex < TY_SPECTRE_DEFAULT_NB_ELMT))
    {
        return _freqNorm[freqIndex];
    }

    return -1.0;
}

TYSpectre TYSpectre::getEmptyLinSpectre(const double& valInit/*= 1.0E-20*/)
{
    return OSpectre::getEmptyLinSpectre(valInit);
}

/// Construit un spectre de ponderation A.
TYSpectre TYSpectre::pondA()
{
    return OSpectre::pondA();
}

/// Construit un spectre de ponderation B.
TYSpectre TYSpectre::pondB()
{
    return OSpectre::pondB();
}

/// Construit un spectre de ponderation C
TYSpectre TYSpectre::pondC()
{
    return OSpectre::pondC();
}

/**
 * Retourne un spectre representant la longueur d'onde associee a chaque frequence
 * @param double Vitesse de propagation de l'onde
 * @return un TYSpectre
 */
TYSpectre TYSpectre::getLambda(const double& c)
{
    return OSpectre::getLambda(c);
}
