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



#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHCore.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/Tools/OMessageManager.h"
#include "TYXMLManager.h"
#include "Tympan/Tools/TYProgressManager.h"

#include <qfile.h>

#include "Tympan/MetierSolver/ToolsMetier/DOMSave.h"


#if defined(WIN32)
#include <crtdbg.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif

QString TYXMLManager::_savedFileName = QString("");


TYXMLManager::TYXMLManager()
{
}

TYXMLManager::~TYXMLManager()
{
}

int TYXMLManager::createDoc(QString docName, QString version)
{
    QDomImplementation domImpl;
    // Reset doc
    resetDoc();
    // Declaration du type de document (DTD)
    QString qualifiedName = "Tympan";       // Nom du noeud root
    QString systemId = "Tympan.dtd";        // URI de la DTD
    QDomDocumentType docType = domImpl.createDocumentType(qualifiedName, QString(), systemId);

    // Creation d'un Document XML vide (Precision: document != fichier)
    _domDocument = domImpl.createDocument(
                       QString(),                  // root element namespace URI.
                       docName,        // root element name
                       docType);           // document type object (DTD).

    // Recuperation du noeud root du document
    _rootElement = _domDocument.documentElement();

    TYXMLTools::addElementStringValue(_rootElement, "Version", version);

    return 1;
}

int TYXMLManager::load(const QString& fileName, TYElementCollection& eltCollection)
{
    OMessageManager::get()->info("Charge le fichier %s.", fileName.toAscii().data());

    QDomDocument doc;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        std::cerr << "\nError when open '" << fileName.toAscii().data() << "'\n";
        return -1;
    }
    QString  errorMsg ;
    int errorLine ;
    int errorColumn ;
    if (!doc.setContent(&file, &errorMsg, &errorLine, &errorColumn))
    {
        std::cerr << "\nError during parsing: '" << fileName.toAscii().data() << "'\n"
                  << "error message is:  \n"
                  <<  errorMsg.toAscii().constData() << "\n"
                  << "in line:" << errorLine << " in column:" << errorColumn << std::endl;
        file.close();
        return -2;
    }
    file.close();
    // On recupere le noeud Document
    _domDocument = doc;

    // On recupere l'element XML root du document
    _rootElement = _domDocument.documentElement();

    // Recense tous les elements enfants du root
    QDomNodeList nodeList = _rootElement.childNodes();

    TYElement* pElt = NULL;
    QString str;

    // On active la sauvegarde des instances de type TYElement et derivees
    TYElement::purgeInstances();
    TYElement::setLogInstances(true);

    int nodecount = nodeList.length();

    int readOk = 1; // Indicateur de probleme de relecture dans les calculs

    // Pour chaque noeud enfant du noeud root
    for (int i = 0; i < nodecount; i++)
    {
        // Utilisation de la Prototype Factory pour creer un nouvel element a partir
        // du nom du noeud enfant trouve

        OMessageManager::get()->info("Charge element du fichier %d/%d.", i + 1, nodecount);

        // Les donnees metiers ne sont pas prefixees par TY...
        str = "TY";
        str += nodeList.item(i).nodeName();

        pElt = (TYElement*) TYElement::findAndClone((char*)str.toAscii().data());

        // Si l element a ete trouve
        if (pElt != NULL)
        {
            // Auto chargement des parametres par l'element
            readOk = pElt->fromXML(nodeList.item(i).toElement());

            // Ajout de l'element a la collection
            eltCollection.add(pElt);
        }
    }

    // On desactive la sauvegarde des instances de type TYElement et derivees
    TYElement::setLogInstances(false);
    TYElement::purgeInstances();

    if (readOk == -1) // La relecture du calcul s'est mal effectuee
    {
        OMessageManager::get()->info("\n\n\t\t\t*** PROBLEME DE RELECTURE DES RESULTATS DU CALCUL ***\
									   \n\t\t*** CAUSE PROBABLE : MACHINE OU BATIMENT EN DOUBLON (MEME ID) ***\
									   \n\t\t\t\t*** INTEGRITE DES RESULTATS COMPROMISE ***\
									   \n\t\t\t\t\t\t*** REFAIRE LES CALCULS ***\n\n");
    }

    OMessageManager::get()->info("Fin du chargement.");

    return 1;
}

int TYXMLManager::save(QString fileName)
{
    // Sauvegarde du document
    return DOMSave::saveNodeToFile(_domDocument, fileName.toAscii().data());
}

int TYXMLManager::loadFromString(const QString& xmlString, TYElementCollection& eltCollection)
{
    QDomDocument doc;
    QString  errorMsg ;
    int errorLine ;
    int errorColumn ;
    if (!doc.setContent(xmlString, &errorMsg, &errorLine, &errorColumn))
    {
        std::cerr << "\nError during parsing\n"
                  << "error message is:  \n"
                  <<  errorMsg.toAscii().constData() << "\n"
                  << "in line:" << errorLine << " in column:" << errorColumn << std::endl;
        return -2;
    }

    // On recupere le noeud Document
    _domDocument = doc;

    // On recupere l'element XML root du document
    _rootElement = _domDocument.documentElement();

    // Recense tous les elements enfants du root
    QDomNodeList nodeList = _rootElement.childNodes();

    TYElement* pElt = NULL;
    QString str;

    // On active la sauvegarde des instances de type TYElement et derivees
    TYElement::purgeInstances();
    TYElement::setLogInstances(true);

    int nodecount = nodeList.length();

    int readOk = 1; // Indicateur de probleme de relecture dans les calculs

    // Pour chaque noeud enfant du noeud root
    for (int i = 0; i < nodecount; i++)
    {
        // Utilisation de la Prototype Factory pour creer un nouvel element a partir
        // du nom du noeud enfant trouve

        // Les donnees metiers ne sont pas prefixees par TY...
        str = "TY";
        str += nodeList.item(i).nodeName();

        pElt = (TYElement*) TYElement::findAndClone((char*)str.toAscii().data());

        // Si l'element a ete trouve
        if (pElt != NULL)
        {
            // Auto chargement des parametres par l'element
            readOk = pElt->fromXML(nodeList.item(i).toElement());

            // Ajout de l'element a la collection
            eltCollection.add(pElt);
        }
    }

    // On desactive la sauvegarde des instances de type TYElement et derivees
    TYElement::setLogInstances(false);
    TYElement::purgeInstances();

    return 1;
}

QString TYXMLManager::saveToString()
{
    // Sauvegarde du document
    return DOMSave::saveNodeToString(_domDocument);
}

int TYXMLManager::getEltType(const QString& fileName, QString& eltType)
{
    QDomDocument doc;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        std::cerr << "\nError when open '" << fileName.toAscii().data() << "'\n";
        return -1;
    }
    QString  errorMsg ;
    int errorLine ;
    int errorColumn ;
    if (!doc.setContent(&file, &errorMsg, &errorLine, &errorColumn))
    {
        std::cerr << "\nError during parsing: '" << fileName.toAscii().data() << "'\n"
                  << "error message is:  \n"
                  <<  errorMsg.toAscii().constData() << "\n"
                  << "in line:" << errorLine << " in column:" << errorColumn << std::endl;
        file.close();
        return -2;
    }
    file.close();
    // On recupere le noeud Document
    _domDocument = doc;

    // On recupere l'element XML root du document
    _rootElement = _domDocument.documentElement();

    // On recupere le premier elements
    DOM_Node node = _rootElement.firstChild();

    // Si null, on retourne zero
    if (node.isNull())
    {
        return 0;
    }

    // Si il correspond a la version de tympan, on passe au suivant
    if (node.nodeName() == "Version")
    {
        node = node.nextSibling();
    }

    // Si null, on retourne zero
    if (node.isNull())
    {
        return 0;
    }

    eltType = "TY";
    eltType += node.nodeName();

    return 1;
}

int TYXMLManager::addElements(TYElementCollection& eltCollection)
{
    // Pour chaque element
    for (unsigned int i = 0; i < eltCollection.getCount(); i++)
    {
        // Serialisation a partir du noeud root
        eltCollection.getAt(i)->toXML(_rootElement);
    }

    return true;
}

int TYXMLManager::addElement(TYElement* pElt)
{
    if (pElt == NULL) { return false; }

    // Serialisation a partir du noeud root
    pElt->toXML(_rootElement);

    return true;
}

void TYXMLManager::resetDoc()
{
    if (_rootElement.isNull() && _domDocument.isNull())
    {
        return;
    }
    // Recense tous les elements enfants du root
    QDomNodeList nodeList = _rootElement.childNodes();

    // Pour chaque noeud enfant du noeud root
    for (unsigned int i = 0; i < nodeList.length(); i++)
    {
        _rootElement.removeChild(nodeList.item(i));
    }
}
