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


#include <fstream>
#include <iomanip>
#include <cassert>

#include "Tympan/core/logging.h"
#include "Tympan/models/business/TYProgressManager.h"
#include "Tympan/models/business/TYCalcul.h"
#include "Tympan/models/business/geoacoustic/TYAcousticLine.h"
#include "Tympan/models/business/geoacoustic/TYAcousticVolumeNode.h"
#include "TYResultat.h"

#if TY_USE_IHM
  #include "Tympan/gui/widgets/TYResultatWidget.h"
  #include "Tympan/gui/gl/TYResultatGraphic.h"
#endif


TY_EXTENSION_INST(TYResultat);
TY_EXT_GRAPHIC_INST(TYResultat);


TYResultat::TYResultat() : _bPartial(false), _hideLW(false)
{
    _name = TYNameManager::get()->generateName(getClassName());

    setIsAcousticModified(true);
}

TYResultat::TYResultat(const TYResultat& other)
{
    *this = other;
}

TYResultat::~TYResultat()
{
    purge();
}

TYResultat& TYResultat::operator=(const TYResultat& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        _matrix = other._matrix;
        _hideLW = other._hideLW;
    }
    return *this;
}

bool TYResultat::operator==(const TYResultat& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        if (_hideLW != other._hideLW) { return false; }
        // if (!(_matrix == other._matrix)) { return false; } // XXX
    }
    return true;
}

bool TYResultat::operator!=(const TYResultat& other) const
{
    return !operator==(other);
}

std::string TYResultat::toString() const
{
    return "TYResultat";
}

DOM_Element TYResultat::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);
    DOM_Document domDoc = domElement.ownerDocument();
    TYXMLTools::addElementBoolValue(domNewElem, "hide_lw", _hideLW);

    DOM_Element listSrc = domDoc.createElement("ListSources");
    domNewElem.appendChild(listSrc);

    TYMapElementIndex ::iterator iteSrc;
    for (iteSrc = _sources.begin(); iteSrc != _sources.end(); iteSrc++)
    {
        DOM_Element resultEntry = domDoc.createElement("Source");
        listSrc.appendChild(resultEntry);

        // ID de la Source
        resultEntry.setAttribute("srcId", (*iteSrc).first->getID().toString());
        resultEntry.setAttribute("index", QString(intToStr((*iteSrc).second).c_str()));
    }

    DOM_Element listRec = domDoc.createElement("ListRecepteurs");
    domNewElem.appendChild(listRec);

    TYMapElementIndex::iterator iteRec;
    for (iteRec = _recepteurs.begin(); iteRec != _recepteurs.end(); iteRec++)
    {
        DOM_Element resultEntry = domDoc.createElement("Recepteur");
        listRec.appendChild(resultEntry);

        // ID de la Source
        resultEntry.setAttribute("recId", (*iteRec).first->getID().toString());
        resultEntry.setAttribute("index", QString(intToStr((*iteRec).second).c_str()));
    }

    DOM_Element listSp = domDoc.createElement("ListSpectres");
    domNewElem.appendChild(listSp);

    size_t nbMatrixRcpts = _matrix.nb_receptors();
    size_t nbMatrixSrcs = _matrix.nb_sources();

    for (unsigned int i = 0; i < nbMatrixRcpts; i++)
    {
        for (unsigned int j = 0; j < nbMatrixSrcs; j++)
        {
            DOM_Element resultEntry = domDoc.createElement("SpectreInd");
            listSp.appendChild(resultEntry);

            // ID de la Source
            resultEntry.setAttribute("indexRec", intToStr(i).data());
            resultEntry.setAttribute("indexSrc", intToStr(j).data());

            TYSpectre spectre = _matrix(i,j).toDB();
            spectre.toXML(resultEntry);
        }
    }

    // Export des spectres de puissance de chaque source associees au calcul
    DOM_Element listLw = domDoc.createElement("ListLw");
    domNewElem.appendChild(listLw);

    std::map<TYElement*, LPTYSpectre>::iterator it;
    for (it = _mapElementSpectre.begin(); it != _mapElementSpectre.end(); it++)
    {
        DOM_Element lwEntry = domDoc.createElement("SourceLw");
        listLw.appendChild(lwEntry);

        // Ajout de l'id de la source comme attribut
        lwEntry.setAttribute("srcId", (*it).first->getID().toString());

        // Ajout du spectre
        (*it).second->toXML(lwEntry);
    }

    return domNewElem;
}

int TYResultat::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);

    purge();
    _bPartial = false; // Les donnees partielles n'ont pas ete enregistrees
	_hideLW = false;

    unsigned int i;

    DOM_Element elemCur;
    QDomNodeList childs = domElement.childNodes();
    for (i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();
        TYXMLTools::getElementBoolValue(elemCur, "hide_lw", _hideLW);
        if (elemCur.nodeName() == "ListSources")
        {
            // Source
            DOM_Element elemCur2;
            QDomNodeList childs2 = elemCur.childNodes();
            for (unsigned int j = 0; j < childs2.length(); j++)
            {
                elemCur2 = childs2.item(j).toElement();
                if (elemCur2.nodeName() == "Source")
                {
                    QString srcId = TYXMLTools::getElementAttributeToString(elemCur2, "srcId");
                    int index = TYXMLTools::getElementAttributeToInt(elemCur2, "index");

                    TYElement* pSrc = TYElement::getInstance(srcId);

                    if (pSrc)
                    {
                        _sources[pSrc] = index;
                    }
                }
            }
        }
        else if (elemCur.nodeName() == "ListRecepteurs")
        {
            // Source
            DOM_Element elemCur2;
            QDomNodeList childs2 = elemCur.childNodes();
            for (unsigned int j = 0; j < childs2.length(); j++)
            {
                elemCur2 = childs2.item(j).toElement();
                if (elemCur2.nodeName() == "Recepteur")
                {
                    QString srcRec = TYXMLTools::getElementAttributeToString(elemCur2, "recId");
                    int index = TYXMLTools::getElementAttributeToInt(elemCur2, "index");

                    TYPointCalcul* pPtCalcul = TYPointCalcul::safeDownCast(TYElement::getInstance(srcRec));

                    if (pPtCalcul)
                    {
                        _recepteurs[pPtCalcul] = index;
                    }
                    else
                    {
                        QString message = "One or more receptor(s) not found";
                        OMessageManager::get()->error(message);
                    }
                }
            }
        }
    }

    buildMatrix();


    TYElement::setLogInstances(false);

    bool loadOk = true;

    for (i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        if (elemCur.nodeName() == "ListSpectres")
        {

            // Spectre resultat
            DOM_Element elemCur2;
            QDomNodeList childs2 = elemCur.childNodes();

            for (unsigned int j = 0; j < childs2.length(); j++)
            {
                elemCur2 = childs2.item(j).toElement();
                int indSrc, indRec;
                if (elemCur2.nodeName() == "SpectreInd")
                {
                    indRec = TYXMLTools::getElementAttributeToInt(elemCur2, "indexRec");
                    indSrc = TYXMLTools::getElementAttributeToInt(elemCur2, "indexSrc");


                    DOM_Element elemCur3;
                    QDomNodeList childs3 = elemCur2.childNodes();
                    for (unsigned int k = 0; k < childs3.length(); k++)
                    {
                        elemCur3 = childs3.item(k).toElement();
                        LPTYSpectre pSpectre = new TYSpectre;

                        if (pSpectre->callFromXMLIfEqual(elemCur3))
                        {
                            loadOk &= setSpectre(indRec, indSrc, *pSpectre);
                        }
                    }
                }
            }
        }
        else if (elemCur.nodeName() == "ListLw")
        {
            // Recuperation des spectres de puissance acoustique des sources
            DOM_Element elemCur3;
            QDomNodeList childs3 = elemCur.childNodes();
            for (unsigned int j = 0; j < childs3.length(); j++)
            {
                elemCur3 = childs3.item(j).toElement();

                if (elemCur3.nodeName() == "SourceLw")
                {
                    QString srcId = TYXMLTools::getElementAttributeToString(elemCur3, "srcId");
                    TYElement* pSrc = TYElement::getInstance(srcId);
                    if (pSrc)
                    {
                        DOM_Element elemCur4;
                        QDomNodeList childs4 = elemCur3.childNodes();
                        for (unsigned int k = 0; k < childs4.length(); k++)
                        {
                            elemCur4 = childs4.item(k).toElement();
                            LPTYSpectre pSpectre = new TYSpectre();

                            if (pSpectre->callFromXMLIfEqual(elemCur4))
                            {
                                _mapElementSpectre[pSrc] = pSpectre;
                            }
                        }
                    }
                }
            }
        }
    }

    TYElement::setLogInstances(true);

    if (!loadOk) { return -1; }
    return 1;
}

void TYResultat::purge()
{
    _matrix.clear();
    _sources.clear();
    _recepteurs.clear();
    _mapEmetteurSources.clear();
    _backupSources.clear();
    _backupMatrix.clear();
    _mapElementSpectre.clear();

    setIsAcousticModified(true);
}

void TYResultat::addSource(TYElement* pSource)
{
    // use two instructions to avoid order problem between compilers
    // G. Andrade
    int idx = static_cast<int>(_sources.size());
    _sources[pSource] = idx;
}

void TYResultat::buildSources(const TYTabSourcePonctuelleGeoNode& sources)
{
    TYSourcePonctuelle* pSource = NULL;
    for (unsigned int i = 0 ; i < sources.size(); i++)
    {
        pSource = TYSourcePonctuelle::safeDownCast(sources[i]->getElement());
        addSource(pSource);
    }
}

std::vector<LPTYElement> TYResultat::getSources()
{
    std::vector< SmartPtr<TYElement> > listSources;
    listSources.reserve(_sources.size());
    std::map<TYElement*, int>::iterator it;
    for (it=_sources.begin(); it!=_sources.end(); it++)
    {
        listSources.push_back(  SmartPtr<TYElement> ( (*it).first ) );
    }
    
    return listSources;
}

std::vector<LPTYElement> TYResultat::getReceptors()
{
    std::vector< SmartPtr<TYElement> > listReceptors;
    listReceptors.reserve(_recepteurs.size());
    std::map<TYElement*, int>::iterator it;
    for (it=_recepteurs.begin(); it!=_recepteurs.end(); it++)
    {
        listReceptors.push_back(  SmartPtr<TYElement> ( (*it).first ) );
    }
    
    return listReceptors;
}

void TYResultat::buildRecepteurs(const TYTabPointCalculGeoNode& recepteurs)
{
    TYPointCalcul* pRecepteur = NULL;
    for (unsigned int j = 0; j < recepteurs.size() ; j++)
    {
        pRecepteur = TYPointCalcul::safeDownCast(recepteurs[j]->getElement());
        addRecepteur(pRecepteur);
    }
}

bool TYResultat::addRecepteur(TYElement* pRecepteur)
{
    assert(pRecepteur);

    bool need_to_rebuild(false);
    // use two instructions to avoid order problem between compilers
    // G. Andrade
    TYMapElementIndex::iterator it = _recepteurs.find(pRecepteur);
    if (it == _recepteurs.end())
    {
        int idx = static_cast<int>(_recepteurs.size());
        _recepteurs[pRecepteur] = idx;
        need_to_rebuild = true;
    }

    // set the receptor ready for the TYCalcul (if needed)
    dynamic_cast<TYPointCalcul*>(pRecepteur)->setEtat( true );

    return need_to_rebuild;
}

bool TYResultat::remRecepteur(TYPointCalcul* pRecepteur)
{
    assert(pRecepteur);

    bool need_to_rebuild(false);
    TYMapElementIndex::iterator it = _recepteurs.find(pRecepteur);
    if (it != _recepteurs.end()) 
    { 
        _recepteurs.erase(it);
        need_to_rebuild = true;
    }

    // Boucle sur les recepteurs pour les renumeroter
    unsigned int index = 0;
    for (it = _recepteurs.begin(), index = 0; it != _recepteurs.end(); it++, index++)
    {
        (*it).second = index;
    }

    return need_to_rebuild;
}

void TYResultat::buildMatrix()
{
    _matrix = tympan::SpectrumMatrix(_recepteurs.size(), _sources.size());
}

bool TYResultat::setSpectre(TYElement* pRecepteur, TYElement* pSource, OSpectre& Spectre)
{

    assert(pSource);
    assert(pRecepteur);

    // Index source
    int indexSource = _sources[pSource];
    // Index recepteur
    int indexRecepteur = _recepteurs[pRecepteur];

    return setSpectre(indexRecepteur, indexSource, Spectre);
}

bool TYResultat::setSpectre(int indexRecepteur, int indexSource, OSpectre& Spectre)
{
    return setSpectre(indexRecepteur, indexSource, Spectre, _matrix);
}

bool TYResultat::setSpectre(int indexRecepteur, int indexSource, OSpectre& Spectre, tympan::SpectrumMatrix& matrix)
{
    matrix(indexRecepteur, indexSource) = Spectre;
    return true; // TODO return kept for compatibility reasons : to be changed to void later 
}

OSpectre TYResultat::getSpectre(TYElement* pRecepteur, TYElement* pSource)
{
    OSpectre spectre;
    spectre.setValid(false);
    if (_sources.find(pSource) == _sources.end()            ||
        _recepteurs.find(pRecepteur) == _recepteurs.end()) { return spectre; }

    // Index source
    int indexSource = _sources[pSource];
    // Index recepteur
    int indexRecepteur = _recepteurs[pRecepteur];

    return getSpectre(indexRecepteur, indexSource);
}

const OSpectre& TYResultat::getSpectre(int indexRecepteur, int indexSource) const
{
    return _matrix(indexRecepteur, indexSource);
}

const OSpectre& TYResultat::getElementSpectre(int indexRecepteur, int indexSource) const
{
    return _backupMatrix(indexRecepteur, indexSource);
}

OTabSpectre TYResultat::getSpectres(TYPointCalcul* pRecepteur)
{
    // Index recepteur
    int indexRecepteur = _recepteurs[pRecepteur];
    return getSpectres(indexRecepteur);
}

OTabSpectre TYResultat::getSpectres(const int& indexRecepteur) const
{
    return _matrix.by_receptor(indexRecepteur);
}


void TYResultat::remSpectres(TYPointCalcul* pRecepteur)
{
    int indexRecepteur = _recepteurs[pRecepteur];
    _matrix.clearReceptor(indexRecepteur);
    _recepteurs.erase(pRecepteur);

}

LPTYElement TYResultat::getSource(const int& indexSource)
{
    LPTYElement pSrc = NULL;

    TYMapElementIndex::iterator iter;

    iter = _sources.begin();

    while (((*iter).second != indexSource) && (iter != _sources.end())) { iter++; }

    if (iter != _sources.end()) { pSrc = (TYElement*)((*iter).first); }

    return pSrc;
}

LPTYElement TYResultat::getElementSource(const int& indexSource)
{
    LPTYElement pSrc = NULL;

    TYMapElementIndex::iterator iter;

    iter = _backupSources.begin();

    while (((*iter).second != indexSource) && (iter != _backupSources.end())) { iter++; }

    if (iter != _backupSources.end()) { pSrc = (TYElement*)((*iter).first); }

    return pSrc;
}

LPTYPointCalcul TYResultat::getRecepteur(const int& indexRecepteur)
{
    LPTYPointCalcul pPoint = NULL;

    TYMapElementIndex::iterator iter;

    iter = _recepteurs.begin();

    while ((iter != _recepteurs.end()) && ((*iter).second != indexRecepteur)) { iter++; }

    if (iter != _recepteurs.end()) { pPoint = (TYPointCalcul*)(*iter).first; }

    return pPoint;
}

void TYResultat::saveSpectre(const std::string& filename, TYCalcul* pSubstCalcul/*=NULL*/)
{
    const TYTabFreq tabFreq = TYSpectre::getTabFreqNorm(SPECTRE_FORM_TIERS);

    std::ofstream ofs;
    ofs.open(filename.c_str(), std::ios_base::out);
    if (ofs.is_open())
    {
        ofs.setf(std::ios::fixed, std::ios::floatfield);
        ofs.precision(2);

        int nbSources = static_cast<int>(getNbOfSources());
        int nbRecepteurs = static_cast<int>(getNbOfRecepteurs());

        int nbSpectre = nbSources;

        std::vector<OSpectre> tabSpectre;
        tabSpectre.reserve(nbSpectre);

        TYCalcul* pCalcul = TYCalcul::safeDownCast(getParent());

        for (int col = 0; col < nbRecepteurs; ++col)
        {
            // On s'assure que le tableau est vide
            tabSpectre.clear();

            // On decale d'une case (affichage des frequences)
            ofs << ";";

            // Entete pour synthese au point
            ofs << "Synthese-" << getRecepteur(col)->getName().toAscii().data() << ";";

            // On recupere le spectre au point
            TYPointCalcul* pPtCalc = getRecepteur(col);
            OSpectre spectre = *pCalcul->getSpectre( pPtCalc->getID() );//*pPtCalc->getSpectre(pCalcul);
            OSpectre otherSpectum = *pSubstCalcul->getSpectre(pPtCalc->getID());
            if (pSubstCalcul != NULL)
            {
                spectre = getEmergence( spectre, otherSpectum ); //*pPtCalc->getSpectre(pSubstCalcul));
            }
            spectre.setType(SPECTRE_TYPE_LP);
            spectre.toDB();
            tabSpectre.push_back(spectre);

            // Puis on boucle sur les contributions par source
            for (int row = 0; row < nbSources; ++row)
            {
                ofs << getSource(row)->getName().toAscii().data() << "/" << getRecepteur(col)->getName().toAscii().data() << ";";

                // On profite de la boucle pour remplir le tableau
                OSpectre spectre = getSpectre(col, row);
                spectre.setType(SPECTRE_TYPE_LP);
                spectre = spectre.toDB();

                tabSpectre.push_back(spectre);
            }

            // On fait ensuite une boucle pour chaque frequence sur chaque tableau
            for (int i = 0; i < 31 ; ++i)
            {
                ofs << "\n";
                ofs << tabFreq[i] << ";";

                for (unsigned int j = 0; j < tabSpectre.size() ; j++)
                {
                    ofs << tabSpectre[j].getTabValReel()[i] << ";" ;

                }
            }

            ofs << "\n";
        }

        ofs.close();
    }
}

struct SortElement
{
    int indice;
    float value;
    bool operator < (const SortElement& rhs)
    {
        return value > rhs.value;
    }
};

void TYResultat::saveParamValue(std::ofstream& ofs, TYCalcul* pCalcul)
{
    if (pCalcul != NULL)
    {
        ofs << "Calcul" << ';';
        ofs << '\n';
        ofs << pCalcul->getName().toStdString() << ';';
        ofs << '\n';
        ofs << '\n';
    }
}


OSpectre TYResultat::getEmergence(OSpectre& spectre, OSpectre& substSpectre)
{
    if (!substSpectre.isValid()) { return substSpectre; }

    // Emergence = (Bruit particulier (+) Bruit de fond) - Bruit de Fond
    return getAmbiant(spectre, substSpectre).subst(substSpectre);
}

OSpectre TYResultat::getAmbiant(OSpectre& spectre, OSpectre& substSpectre)
{
    return spectre.sumdB(substSpectre);
}

double TYResultat::getEmergence(const double& val1, const double& val2)
{
    const double& BdF = val2; // Bruit de fond (pour la lisibilite)
    const double& bP = val1;  // Bruit particulier (pour la lisibilite)
    double bruitAmbiant = getAmbiant(bP, BdF);

    return bruitAmbiant - BdF;
}

double TYResultat::getAmbiant(const double& val1, const double& val2)
{
    const double& BdF = val2; // Bruit de fond (pour la lisibilite)
    const double& bP = val1;  // Bruit particulier (pour la lisibilite)
    return 10.0 * ::log10(::pow(10.0, (bP / 10.0)) + ::pow(10.0, (BdF / 10.0)));
}

void TYResultat::saveValue(const std::string& filename, const int& affichage, double freq /*=100*/)
{
    std::ofstream ofs;
    ofs.open(filename.c_str(), std::ios_base::out);

    if (ofs.is_open())
    {
        ofs.setf(std::ios::fixed, std::ios::floatfield);
        ofs.precision(2);

        unsigned int nbSources = static_cast<int>(getNbOfSources());
        unsigned int nbRecepteurs = static_cast<int>(getNbOfRecepteurs());


        // CLM-NT33 - Ajout des parametres du calcul
        TYCalcul* pCalcul = TYCalcul::safeDownCast(getParent());
        saveParamValue(ofs, pCalcul);

        switch (affichage)
        {
            case 1 :
                ofs << "dBZ" << ';';
                break;
            case 2 :
                ofs << "dB(" << freq << "Hz)" << ';';
                break;
            case 0 :
            default :
                ofs << "dBA" << ';';
        }

        ofs << "LW" << ';';

        // Sauvegarde des valeurs
        int col;
        for (col = 0; col < nbRecepteurs ; ++col)
        {
            if (getRecepteur(col) == NULL)
            {
                continue;
            }
            ofs << getRecepteur(col)->getName().toAscii().data() << ';';
        }

        // Saut de ligne
        ofs << '\n';

        // Ligne de synthese
        ofs << "Synthese" << ';' << ';';
        for (col = 0; col < nbRecepteurs ; ++col)
        {
            TYPointCalcul* pPtCalc = getRecepteur(col);
            LPTYSpectre spectre = pCalcul->getSpectre( pPtCalc->getID() );

            spectre->setType(SPECTRE_TYPE_LP);
            spectre->toDB();

            switch (affichage)
            {
                case 1 :
                    ofs << spectre->valGlobDBLin() << ';';
                    break;
                case 2 :
                    ofs << spectre->getValueReal(freq) << ';';
                    break;
                case 0 :
                default :
                    ofs << spectre->valGlobDBA() << ';';
            }
        }

        // Saut de ligne
        ofs << '\n';

        // On fait un tri decroissant selon les valeurs LW
        std::list<SortElement> sortArray;
        for (int row = 0; row < nbSources ; ++row)
        {
            LPTYSpectre puissance = 0;
            LPTYElement pElement = getSource(row);
            TYUserSourcePonctuelle* pSource = dynamic_cast<TYUserSourcePonctuelle*>(pElement._pObj);
            if (pSource != nullptr)
            {
                puissance = pSource->getRealPowerSpectrum();
            }
            else
            {
                TYAcousticVolumeNode* pVolNode =
                    dynamic_cast<TYAcousticVolumeNode*>(pElement._pObj);
                if (pVolNode != nullptr)
                {
                    puissance = pVolNode->getRealPowerSpectrum();
                }
                else
                {
                    puissance = TYAcousticLine::safeDownCast(pElement)->getRealPowerSpectrum();
                }
            }

            // Spectre de puissance
            OSpectre spectre = *puissance;
            spectre.setType(SPECTRE_TYPE_LW);

            SortElement pair;
            pair.indice = row;

            switch (affichage)
            {
                case 1 :
                    pair.value = spectre.valGlobDBLin();
                    break;
                case 2 :
                    pair.value = spectre.getValueReal(freq);
                    break;
                case 0 :
                default :
                    pair.value = spectre.valGlobDBA();
            }

            sortArray.push_back(pair);
        }
        sortArray.sort();


        // On traite chaque source avec chaque recepteur
        for (std::list<SortElement>::iterator it = sortArray.begin(); it != sortArray.end(); ++it)
        {
            ofs << getSource(it->indice)->getName().toAscii().data() << ';';
            ofs << it->value << ';';
            for (col = 0; col < nbRecepteurs ; ++col)
            {
                OSpectre spectre = getSpectre(col, it->indice);
                spectre.setType(SPECTRE_TYPE_LP);
                spectre = spectre.toDB();

                switch (affichage)
                {
                    case 1 :
                        ofs << spectre.valGlobDBLin() << ';';
                        break;
                    case 2 :
                        ofs << spectre.getValueReal(freq) << ';';
                        break;
                    case 0 :
                    default :
                        ofs << spectre.valGlobDBA() << ';';
                }
            }

            ofs << '\n';
        }

        ofs.close();
    }
}

void TYResultat::setPartialState(const bool& bPartial)
{
    _bPartial = bPartial;
    _backupSources.clear();
    _backupMatrix.clear();
}

void TYResultat::buildMapSourceSpectre()
{
    LPTYSpectre puissance = new TYSpectre();

	std::map<TYElement*,int>::iterator it;
    for (it = _sources.begin(); it != _sources.end(); it++)
    {
        TYElement* pElement = (*it).first;
        TYUserSourcePonctuelle* pSource = dynamic_cast<TYUserSourcePonctuelle*>(pElement);
        if (pSource != nullptr)
        {
            puissance = pSource->getRealPowerSpectrum();
        }
        else
        {
            TYAcousticVolumeNode* pVolNode = dynamic_cast<TYAcousticVolumeNode*>(pElement);
            if (pVolNode != nullptr) { puissance = pVolNode->getRealPowerSpectrum(); }
            else // Source lineique
            {
                TYAcousticLine* pLine = TYAcousticLine::safeDownCast(pElement);
                if (pLine) { puissance = pLine->getRealPowerSpectrum(); }
            }
        }

        puissance->setType(SPECTRE_TYPE_LW);
        _mapElementSpectre[pElement] = puissance;
    }
}
