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


#ifdef _MSC_VER
  #pragma warning(disable : 4786)
#endif

#include <qfile.h>
#include <iostream>

#include "Tympan/core/logging.h"
#include "Tympan/core/exceptions.h"
#include "Tympan/models/business/DOMSave.h"
#include "TYXMLManager.h"



QString TYXMLManager::_savedFileName = QString("");

/**
 * Write XML parsing error on std::err and throw a tympan::invalid_data
 * exception
 */
void handle_xml_parsing_error(std::string cause, std::string msg, int err_line, int err_col)
{
    std::string error_msg = "Error during parsing" + cause +
        + "\nerror message is:\n" + msg + "\nat line:"
        + std::to_string(static_cast<long long>(err_line)) + " at column:"
        + std::to_string(static_cast<long long>(err_col));
    throw tympan::invalid_data(error_msg);
}

/**
 * Parse XML from file `filepath` and return the corresponding QDomDocument.
 *
 */
QDomDocument parse_xml_file(const QString & filepath)
{
    QDomDocument doc;
    QString msg;
    int err_line, err_col;
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly))
    {
        std::string error_msg = "Error on opening " + filepath.toStdString();
        throw tympan::invalid_data(error_msg);
    }
    file.close();
    if ( !doc.setContent(&file, &msg, &err_line, &err_col))
    {
        std::string cause = " of: " + filepath.toStdString();
        handle_xml_parsing_error(cause, msg.toStdString(), err_line, err_col);
    }
    return doc;
}

/**
 * Parse XML content from `xml_content` and return the corresponding QDomDocument.
 *
 */
QDomDocument parse_xml_content(const QString & xml_content)
{
    QDomDocument doc;
    QString msg;
    int err_line, err_col;
    if (!doc.setContent(xml_content, &msg, &err_line, &err_col))
    {
        handle_xml_parsing_error(std::string("QString content"), msg.toStdString(), err_line, err_col);
    }
    return doc;
}


TYXMLManager::TYXMLManager()
{
}

TYXMLManager::~TYXMLManager()
{
}

void TYXMLManager::createDoc(QString docName, QString version)
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
}

int TYXMLManager::load(const QString& fileName, LPTYElementArray& eltCollection)
{
    OMessageManager::get()->info("Charge le fichier %s.", fileName.toAscii().data());

    try
    {
        _domDocument = parse_xml_file(fileName);
    }
    catch(tympan::invalid_data& exc)
    {
        OMessageManager::get()->error(exc.what());
        return -2;
    }

    // If the document contains any external elements, replace them by the
    // content of the external file before elements creation
    QDomNodeList ext_elements = _domDocument.documentElement().elementsByTagName(
            QString("ExternalElement"));
    int processed = 0;
    while(processed < ext_elements.length())
    {
        QDomNode ext_element = ext_elements.item(processed);
        QDomNamedNodeMap attrs = ext_element.attributes();
        if (attrs.contains(QString("filename")))
        {
            QString ext_filepath = attrs.namedItem(QString("filename")).nodeValue();
            // External file must be in the same directory as the main file
            QDir dir = QFileInfo(fileName).absoluteDir();
            ext_filepath = dir.filePath(ext_filepath);
            QDomDocument extdoc;
            try
            {
                extdoc = parse_xml_file(ext_filepath);
            }
            catch(tympan::invalid_data& exc)
            {
                OMessageManager::get()->error(exc.what());
                processed++;
                continue;
            }
            QDomNodeList elements = extdoc.documentElement().childNodes();
            if (elements.isEmpty())
            {
                OMessageManager::get()->error("External XML project %s seems empty.",
                         ext_filepath.toStdString().c_str());
                processed++;
                continue;
            }
            QDomNode parent = ext_element.parentNode();
            // The first element of the external file will replace the ExternalElement markup.
            // Then the following ones if any will be added to the parent of the
            // ExternalElement markup.
            // replaceChild removes element from "elements" list
            parent.replaceChild(elements.item(0), ext_element);
            for (int j = 0; j < elements.length(); j++)
            {
                parent.appendChild(elements.item(j));
            }
        }
        else
        {
            // Si pas de fichier spécifié on ne peut pas lire l'entité
            OMessageManager::get()->error("XML project contains an external element defined without a file name");
            processed++;
        }
        ext_elements = _domDocument.documentElement().elementsByTagName(QString("ExternalElement"));
    }
    create_tyelements(eltCollection);
    return 1;
}

void TYXMLManager::create_tyelements(LPTYElementArray& eltCollection)
{
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

        try
        {
            pElt = dynamic_cast<TYElement*>(TYElement::findAndClone((char*)str.toAscii().data()));
        }
        catch(tympan::invalid_data& exc) {pElt = nullptr;}
        if(pElt != nullptr)
        {
            // Si l element a ete trouve
            // Auto chargement des parametres par l'element
            readOk = pElt->fromXML(nodeList.item(i).toElement());

            // Ajout de l'element a la collection
            eltCollection.push_back(pElt);
        }
    }

    // On desactive la sauvegarde des instances de type TYElement et derivees

    if (readOk == -1) // La relecture du calcul s'est mal effectuee
    {
        OMessageManager::get()->info("\n\n\t\t\t*** PROBLEME DE RELECTURE DES RESULTATS DU CALCUL ***\
									   \n\t\t*** CAUSE PROBABLE : MACHINE OU BATIMENT EN DOUBLON (MEME ID) ***\
									   \n\t\t\t\t*** INTEGRITE DES RESULTATS COMPROMISE ***\
									   \n\t\t\t\t\t\t*** REFAIRE LES CALCULS ***\n\n");
    }

    OMessageManager::get()->info("Fin du chargement.");
}

int TYXMLManager::save(QString fileName)
{
    // Sauvegarde du document
    return DOMSave::saveNodeToFile(_domDocument, fileName.toAscii().data());
}

int TYXMLManager::loadFromString(const QString& xmlString, LPTYElementArray& eltCollection)
{
    try
    {
        // On recupere le noeud Document
        _domDocument = parse_xml_content(xmlString);
    }
    catch(tympan::invalid_data& exc)
    {
        OMessageManager::get()->error(exc.what());
        return -2;
    }
    create_tyelements(eltCollection);
    return 1;
}

QString TYXMLManager::saveToString()
{
    // Sauvegarde du document
    return DOMSave::saveNodeToString(_domDocument);
}

int TYXMLManager::getEltType(const QString& fileName, QString& eltType)
{
    try
    {
        // On recupere le noeud Document
        _domDocument = parse_xml_file(fileName);
    }
    catch(tympan::invalid_data& exc)
    {
        OMessageManager::get()->error(exc.what());
        return -2;
    }

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

int TYXMLManager::addElements(LPTYElementArray& eltCollection)
{
    // Pour chaque element
    for (unsigned int i = 0; i < eltCollection.size(); i++)
    {
        // Serialisation a partir du noeud root
        eltCollection.at(i)->toXML(_rootElement);
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
