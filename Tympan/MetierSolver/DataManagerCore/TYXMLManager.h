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
 *
 *
 *
 */

#ifndef __TY_XMLMANAGER__
#define __TY_XMLMANAGER__


#include "TYElementCollection.h"
#include "Tympan/MetierSolver/ToolsMetier/QT2DOM.h"


/**
 * Classe pour la gestion de l'import et l'export des elements Tympan en XML.
 */
class TYXMLManager
{
    // Methodes
public:
    /**
     * Constructeur.
     */
    TYXMLManager();
    /**
     * Destructeur.
     */
    ~TYXMLManager();

    /**
     * Creer un nouveau document XML.
     * Remarque : un document n'est pas un fichier.
     *
     * @param docName Le nom du noeud racine du document XML.
     * @param version le numero de version
     *
     * @return L'etat de reussite.
     */
    int createDoc(QString docName, QString version);

    /**
     * Ouvre un fichier XML et parse le document.
     * Une collection d'element est peuple par les noeuds Tympan valides trouves.
     *
     * @param fileName Le chemin du fichier XML a lire.
     * @param eltCollection La collection d'element a peupler.
     *
     * @return L'etat de reussite.
     */
    int load(const QString& fileName, LPTYElementArray& eltCollection);

    /**
     * Sauvegarde le document XML en cours dans un fichier XML.
     * Si le fichier existe, celui-ci sera ecrase.
     *
     * @param fileName Le chemin du fichier XML a ecrire.
     *
     * @return L'etat de reussite.
     */
    int save(QString fileName);

    /**
     * Ouvre uns string XML et parse le document.
     * Une collection d'element est peuple par les noeuds Tympan valides trouves.
     *
     * @param xmlString La string XML a lire.
     * @param eltCollection La collection d'element a peupler.
     *
     * @return L'etat de reussite.
     */
    int loadFromString(const QString& xmlString, LPTYElementArray& eltCollection);

    /**
     * Sauvegarde le document XML en cours dans une string XML.
     *
     * @return La string XML.
     */
    QString saveToString();

    /**
     * Indique le type d'element auquel correspond le fichier XML
     *
     * @param fileName Le chemin du fichier XML a lire.
     * @param eltType Le type d'element que contient le fichier.
     *
     * @return L'etat de reussite.
     */
    int getEltType(const QString& fileName, QString& eltType);

    /**
     * Ajoute une collection de nouveaux elements au document XML.
     *
     * @param eltCollection La collection d'elements a ajouter.
     *
     * @return L'etat de reussite.
     */
    int addElements(LPTYElementArray& eltCollection);
    /**
     * Ajoute un nouvel element au document XML.
     *
     * @param pElt L'element a ajouter.
     *
     * @return L'etat de reussite.
     */
    int addElement(TYElement* pElt);

    /**
     * Enleve tous les elements du document DOM courant.
     */
    void resetDoc();

    /**
     * Permet le stockage du fichier de sauvegarde
     *
     * @param savedFileName Le path de fichier de sauvegarde.
     */
    static void setSavedFileName(QString savedFileName) { _savedFileName = savedFileName; }
    /**
     * Permet le stockage du fichier de sauvegarde
     *
     * @return savedFileName Le path de fichier de sauvegarde.
     */
    static QString getSavedFileName() { return _savedFileName; }

    // Membres
protected:
    ///Le document XML.
    DOM_Document _domDocument;

    ///L'element racine du document XML.
    DOM_Element _rootElement;

    ///Le nom de fichier pdt la sauvegarde (util pour l'enregistrement du topofile en relatif
    static QString _savedFileName;
};


#endif // __TY_XMLMANAGER__
