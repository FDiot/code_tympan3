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

#ifndef __TY_XML_TOOLS__
#define __TY_XML_TOOLS__

#include "Tympan/models/business/QT2DOM.h"

#include "Tympan/core/macros.h"


/**
 * Methodes outils pour XML.
 */
class TYXMLTools
{
    // Methodes
public:
    /**
     * Ajoute un nouvel element avec une valeur de type <code>string</code> a un element parent.
     *
     * @param parentElem    L'element XML parent.
     * @param nodeName      Le nom du nouvel element XML a ajouter.
     * @param nodeValue     La valeur (CDATA) du nouvel element.
     */
    static void addElementStringValue(DOM_Element& parentElem, DOMString nodeName, DOMString nodeValue);

#if TY_ARCH_TYPE == TY_ARCHITECTURE_64
    /**
     * Ajoute un nouvel element avec une valeur de type <code>unsigned int</code> a un element parent.
     *
     * @param parentElem    L'element XML parent.
     * @param nodeName      Le nom du nouvel element XML a ajouter.
     * @param nodeValue     La valeur (CDATA) du nouvel element.
     */
    static void addElementUIntValue(DOM_Element& parentElem, DOMString nodeName, unsigned int nodeValue);

    /**
     * Ajoute un nouvel element avec une valeur de type <code>size_t</code> a un element parent.
     *
     * @param parentElem    L'element XML parent.
     * @param nodeName      Le nom du nouvel element XML a ajouter.
     * @param nodeValue     La valeur (CDATA) du nouvel element.
     */
    static void addElementUIntValue(DOM_Element& parentElem, DOMString nodeName, size_t nodeValue);
#if TY_COMPILER == TY_COMPILER_MSVC
    /**
     * Ajoute un nouvel element avec une valeur de type <code>unsigned long</code> a un element parent.
     *
     * @param parentElem    L'element XML parent.
     * @param nodeName      Le nom du nouvel element XML a ajouter.
     * @param nodeValue     La valeur (CDATA) du nouvel element.
     */
    static void addElementUIntValue(DOM_Element& parentElem, DOMString nodeName, unsigned long nodeValue);
#endif
#else
    /**
     * Ajoute un nouvel element avec une valeur de type <code>size_t</code> a un element parent.
     *
     * @param parentElem    L'element XML parent.
     * @param nodeName      Le nom du nouvel element XML a ajouter.
     * @param nodeValue     La valeur (CDATA) du nouvel element.
     */
    static void addElementUIntValue(DOM_Element& parentElem, DOMString nodeName, size_t nodeValue);

    /**
     * Ajoute un nouvel element avec une valeur de type <code>unsigned long</code> a un element parent.
     *
     * @param parentElem    L'element XML parent.
     * @param nodeName      Le nom du nouvel element XML a ajouter.
     * @param nodeValue     La valeur (CDATA) du nouvel element.
     */
    static void addElementUIntValue(DOM_Element& parentElem, DOMString nodeName, unsigned long nodeValue);
#endif

    /**
     * Ajoute un nouvel element avec une valeur de type <code>int</code> a un element parent.
     *
     * @param parentElem    L'element XML parent.
     * @param nodeName      Le nom du nouvel element XML a ajouter.
     * @param nodeValue     La valeur (CDATA) du nouvel element.
     */
    static void addElementIntValue(DOM_Element& parentElem, DOMString nodeName, int nodeValue);

    /**
     * Ajoute un nouvel element avec une valeur de type <code>bool</code> a un element parent.
     *
     * @param parentElem    L'element XML parent.
     * @param nodeName      Le nom du nouvel element XML a ajouter.
     * @param nodeValue     La valeur (CDATA) du nouvel element.
     */
    static void addElementBoolValue(DOM_Element& parentElem, DOMString nodeName, bool nodeValue);

    /**
     * Ajoute un nouvel element avec une valeur de type <code>float</code> a un element parent.
     *
     * @param parentElem    L'element XML parent.
     * @param nodeName      Le nom du nouvel element XML a ajouter.
     * @param nodeValue     La valeur (CDATA) du nouvel element.
     */
    static void addElementFloatValue(DOM_Element& parentElem, DOMString nodeName, float nodeValue);

    /**
     * Ajoute un nouvel element avec une valeur de type <code>double</code> a un element parent.
     *
     * @param parentElem    L'element XML parent.
     * @param nodeName      Le nom du nouvel element XML a ajouter.
     * @param nodeValue     La valeur (CDATA) du nouvel element.
     */
    static void addElementDoubleValue(DOM_Element& parentElem, DOMString nodeName, double nodeValue);

    /**
     * Verifie si le noeud passe correspond au nom passe et en extrait la donnee associee
     * sous forme de <code>string</code>.
     *
     * @param parentElem    L'element XML parent.
     * @param nodeName      Le nom de l'element XML a chercher.
     * @param nodeValue     La valeur a remplir.
     *
     * @return <code>true</code> si le noeud a ete trouve et la valeur attribuee.
     */
    static bool getElementStringValue(DOM_Element parentElem, DOMString nodeName, QString& nodeValue);
    /**
     * Verifie si le noeud passe correspond au nom passe et en extrait la donnee associee
     * sous forme de <code>string</code>.
     *
     * @param parentElem    L'element XML parent.
     * @param nodeName      Le nom de l'element XML a chercher.
     * @param nodeValue     La valeur a remplir.
     * @param ok            Si ok vaut <code>false</code> le noeud n'est pas parcouru.
     *                      Sinon, si le noeud a ete trouve, il est mis a <code>true</code>.
     */
    static void getElementStringValue(DOM_Element parentElem, DOMString nodeName, QString& nodeValue, bool& ok);

    /**
     * Verifie si le noeud passe correspond au nom passe et en extrait la donnee associee
     * sous forme de <code>int</code>.
     *
     * @param parentElem    L'element XML parent.
     * @param nodeName      Le nom de l'element XML a chercher.
     * @param nodeValue     La valeur a remplir.
     *
     * @return <code>true</code> si le noeud a ete trouve et la valeur attribuee.
     */
    static bool getElementIntValue(DOM_Element parentElem, DOMString nodeName, int& nodeValue);
    /**
     * Verifie si le noeud passe correspond au nom passe et en extrait la donnee associee
     * sous forme de <code>int</code>.
     *
     * @param parentElem    L'element XML parent.
     * @param nodeName      Le nom de l'element XML a chercher.
     * @param nodeValue     La valeur a remplir.
     * @param ok            Si ok vaut <code>false</code> le noeud n'est pas parcouru.
     *                      Sinon, si le noeud a ete trouve, il est mis a <code>true</code>.
     */
    static void getElementIntValue(DOM_Element parentElem, DOMString nodeName, int& nodeValue, bool& ok);

    /**
     * Verifie si le noeud passe correspond au nom passe et en extrait la donnee associee
     * sous forme de <code>bool</code>.
     *
     * @param parentElem    L'element XML parent.
     * @param nodeName      Le nom de l'element XML a chercher.
     * @param nodeValue     La valeur a remplir.
     *
     * @return <code>true</code> si le noeud a ete trouve et la valeur attribuee.
     */
    static bool getElementBoolValue(DOM_Element parentElem, DOMString nodeName, bool& nodeValue);
    /**
     * Verifie si le noeud passe correspond au nom passe et en extrait la donnee associee
     * sous forme de <code>bool</code>.
     *
     * @param parentElem    L'element XML parent.
     * @param nodeName      Le nom de l'element XML a chercher.
     * @param nodeValue     La valeur a remplir.
     * @param ok            Si ok vaut <code>false</code> le noeud n'est pas parcouru.
     *                      Sinon, si le noeud a ete trouve, il est mis a <code>true</code>.
     */
    static void getElementBoolValue(DOM_Element parentElem, DOMString nodeName, bool& nodeValue, bool& ok);

    /**
     * Verifie si le noeud passe correspond au nom passe et en extrait la donnee associee
     * sous forme de <code>float</code>.
     *
     * @param parentElem    L'element XML parent.
     * @param nodeName      Le nom de l'element XML a chercher.
     * @param nodeValue     La valeur a remplir.
     *
     * @return <code>true</code> si le noeud a ete trouve et la valeur attribuee.
     */
    static bool getElementFloatValue(DOM_Element parentElem, DOMString nodeName, float& nodeValue);
    /**
     * Verifie si le noeud passe correspond au nom passe et en extrait la donnee associee
     * sous forme de <code>float</code>.
     *
     * @param parentElem    L'element XML parent.
     * @param nodeName      Le nom de l'element XML a chercher.
     * @param nodeValue     La valeur a remplir.
     * @param ok            Si ok vaut <code>false</code> le noeud n'est pas parcouru.
     *                      Sinon, si le noeud a ete trouve, il est mis a <code>true</code>.
     */
    static void getElementFloatValue(DOM_Element parentElem, DOMString nodeName, float& nodeValue, bool& ok);

    /**
     * Verifie si le noeud passe correspond au nom passe et en extrait la donnee associee
     * sous forme de <code>double</code>.
     *
     * @param parentElem    L'element XML parent.
     * @param nodeName      Le nom de l'element XML a chercher.
     * @param nodeValue     La valeur a remplir.
     *
     * @return <code>true</code> si le noeud a ete trouve et la valeur attribuee.
     */
    static bool getElementDoubleValue(DOM_Element parentElem, DOMString nodeName, double& nodeValue);
    /**
     * Verifie si le noeud passe correspond au nom passe et en extrait la donnee associee
     * sous forme de <code>double</code>.
     *
     * @param parentElem    L'element XML parent.
     * @param nodeName      Le nom de l'element XML a chercher.
     * @param nodeValue     La valeur a remplir.
     * @param ok            Si ok vaut <code>false</code> le noeud n'est pas parcouru.
     *                      Sinon, si le noeud a ete trouve, il est mis a <code>true</code>.
     */
    static void getElementDoubleValue(DOM_Element parentElem, DOMString nodeName, double& nodeValue, bool& ok);

    /**
     * Retourne l'attribut portant le nom passe du noeud passe sous forme de <code>string</code>.
     *
     * @param parentElem    L'element XML parent.
     * @param nodeName      Le nom de l'element XML a chercher.
     *
     * @return La valeur de l'attribut s'il a ete trouve, sinon une chaine de caractere vide.
     */
    static QString getElementAttributeToString(DOM_Element parentElem, DOMString attName);

    /**
     * Retourne l'attribut portant le nom passe du noeud passe sous forme de <code>double</code>.
     *
     * @param parentElem    L'element XML parent.
     * @param nodeName      Le nom de l'element XML a chercher.
     *
     * @return La valeur de l'attribut s'il a ete trouve, sinon 0.0.
     */
    static double getElementAttributeToDouble(DOM_Element parentElem, DOMString attName, bool* ok = NULL);

    /**
     * Retourne l'attribut portant le nom passe du noeud passe sous forme de <code>int</code>.
     *
     * @param parentElem    L'element XML parent.
     * @param nodeName      Le nom de l'element XML a chercher.
     *
     * @return La valeur de l'attribut s'il a ete trouve, sinon 0.
     */
    static int getElementAttributeToInt(DOM_Element parentElem, DOMString attName, bool* ok = NULL);

    static unsigned getElementAttributeToUnsigned(DOM_Element parentElem, DOMString attName, bool* ok = NULL);
};


#endif // __TY_XML_TOOLS__
