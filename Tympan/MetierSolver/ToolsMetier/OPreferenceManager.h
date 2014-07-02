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
#ifdef _MSC_VER
#pragma warning(disable : 4503)
#pragma warning(disable : 4786)
#endif

#ifndef __O_PREFERENCE_MANAGER__
#define __O_PREFERENCE_MANAGER__

#include "Tympan/core/defines.h"
#include <map>
#include <QString>

/**
 * \struct OPrefMap
 * \brief Le type utilise pour stocker les preferences et leur valeur. C'est un tableau associatif string/string.
 */
//typedef std::map<QString, QString> OPrefMap;
struct OPrefMap
{
    typedef std::map<QString, QString>::iterator OPrefMapIt;
    std::map<QString, QString> _prefMap;
};

/**
 * \struct ODirMap
 * \brief Le type utilise pour stocker une categorie de preferences. C'est un tableau associatif string/OPrefMap.
 */
//typedef std::map<QString, OPrefMap> ODirMap;
struct ODirMap
{
    typedef std::map<QString, OPrefMap>::iterator ODirMapIt;
    std::map<QString, OPrefMap> _dirMap;
};


/**
 * \file OPreferenceManager.h
 * \class OPreferenceManager
 * \brief Systeme de gestion des preferences.
 *
 * Il offre un moyen pour conserver et recuperer des informations
 * quelconques (a base de string tout de meme).
 *
 * Son principe est d'associer simplement une chaine de caractere,
 * le nom du parametre ou la cle, a une autre chaine, qui est la valeur
 * associee au parametre ou a la cle.
 *
 * Pendant l'execution, toutes ces valeurs sont stockees dans un
 * tableau associatif de type map<string, string>.
 * Actuellement le format de sauvegarde est le XML (par les methodes
 * readXML et writeXML) mais il est tout a fait envisageable d'ajouter
 * d'autres formats de sauvegarde, par exemple en fichier INI ou dans
 * la base de registre de Windows.
 *
 * Ce code devrait etre portable sans trop de modification.
 *
 * Remarque : Cette implementation utilise l'API XML Xerces-C du
 * XML Apache Project (http://xml.apache.org/).
 *
 * \author Projet_Tympan
 */
class OPreferenceManager
{
public:
    /**
     * Constructeur par defaut.
     */
    OPreferenceManager();
    /**
     * Constructeur.
     */
    OPreferenceManager(const QString& currentDirectory);
    /**
     * Constructeur.
     */
    OPreferenceManager(const QString& rootNodeName,
                       const QString& paramNodeName);
    /**
     * Constructeur.
     */
    OPreferenceManager(const QString& rootNodeName,
                       const QString& paramNodeName,
                       const QString& currentDirectory);
    /**
     * Destructeur.
     */
    virtual ~OPreferenceManager();


    /**
     * \brief Charge le tableau associatif preference/valeur avec les valeurs par defaut.
     *
     * \defaultParams Un tableau de couples preference/valeur.
     * \nbParams Le nombre de couples contenu dans le tableau passe.
     */
    void loadDefault(const char* defaultParams[][2], int nbParams);

    /**
     * \brief Vide le tableau associatif preference/valeur.
     */
    void reset();

    /**
     * \brief Change le nom du noeud root du fichier XML.
     *
     * \param name Le nouveu nom du noeud.
     */
    void setXMLRootNodeName(QString name) { _xmlRootNodeName = name; }

    /**
     * \brief Change le nom des noeuds de parametres dans le fichier XML.
     *
     * \param name Le nouveu nom du noeud.
     */
    void setXMLParamNodeName(QString name) { _xmlParamNodeName = name; }

    /**
     * \brief Ouvre un fichier de preference en XML et le parcours pour mettre a jour la tableau de preferences.
     *
     * \param fileName Le chemin du fichier de preferences a ouvrir.
     *
     * \return <code>false</code> si le fichier n'a pas pu etre ouvert.
     */
    bool readXML(QString fileName);

    /**
     * \brief Enregistre le fichier de preference au format XML.
     *
     * \param fileName Le chemin du fichier de preferences a enregistrer.
     *
     * \return <code>false</code> si le fichier n'a pas pu etre enregistre.
     */
    bool writeXML(QString fileName);

    /**
     * \brief Test si une preference est presente dans le tableau de preferences. Utilise la categorie courante.
     *
     * \param pref Le nom de la preference a tester.
     *
     * \return <code>true</code> si la preference existe;
     *         <code>false</code> sinon.
     */
    bool exists(const QString& pref);

    /**
     * \brief Test si une preference est presente dans le tableau de preferences. Utilise la categorie specifiee.
     *
     * \param dir Le nom de la categorie.
     * \param pref Le nom de la preference a tester.
     *
     * \return <code>true</code> si la preference existe;
     *         <code>false</code> sinon.
     */
    bool exists(const QString& dir, const QString& pref);

    /**
     * \brief Recherche le nom de la categorie pour une preference donnee.
     *
     * \param pref Le nom de la preference a tester.
     *
     * \return Le nom de la 1ere categorie dans laquelle se trouve
     *         le parametre recherche.
     */
    QString getDirectory(const QString& pref);

    /**
     * \brief Defini la categorie courante pour la lecture et l'ecriture de parametres.
     *
     * \param dir Le nom de la categorie, si elle n'existe pas elle sera creee.
     */
    void setCurrentDirectory(QString dir);
    /**
     * \brief Retourne la categorie courante pour la lecture et l'ecriture de parametres.
     *
     * \return Le nom de la categorie courante.
     */
    QString getCurrentDirectory() { return _curDir; }

    /**
     * \brief Met a jour la valeur associee a une preference.
     *
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie courante.
     *
     * \param pref Le nom de la preference.
     * \param value La valeur a associer a la preference.
     */
    void setString(const QString& pref, const QString& value);

    /**
     * \brief Met a jour la valeur associee a une preference.
     *
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie specifiee.
     *
     * \param dir Le nom de la categorie.
     * \param pref Le nom de la preference.
     * \param value La valeur a associer a la preference.
     */
    void setString(const QString& dir, const QString& pref, const QString& value);

    /**
     * \brief Recherche une ressource et retourne sa valeur associee. Utilise la categorie courante.
     *
     * \param pref Le nom de la preference a rechercher.
     *
     * \return La valeur associee a la preference.
     */
    QString getString(const QString& pref);

    /**
     * \brief Recherche une ressource et retourne sa valeur associee. Utilise la categorie specifiee.
     *
     * \param dir Le nom de la categorie.
     * \param pref Le nom de la preference a rechercher.
     *
     * \return La valeur associee a la preference.
     */
    QString getString(const QString& dir, const QString& pref);

    /**
     * \brief Met a jour un tableau de valeurs associees a une preference.
     *
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie courante.
     *
     * \param pref Le nom de la preference.
     * \param valueArray Le tableau de valeurs a associer a la preference.
     * \param sizeArray Le nombre de valeurs contenues dans le tableau passe.
     */
    void setStringArray(const QString& pref, const QString* valueArray, const int& sizeArray);

    /**
     * \brief Met a jour un tableau de valeurs associees a une preference.
     *
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie specifiee.
     *
     * \param dir Le nom de la categorie.
     * \param pref Le nom de la preference.
     * \param valueArray Le tableau de valeurs a associer a la preference.
     * \param sizeArray Le nombre de valeurs contenues dans le tableau passe.
     */
    void setStringArray(const QString& dir, const QString& pref, const QString* valueArray, const int& sizeArray);

    /**
     * \brief Recherche une ressource et retourne son tableau de valeurs associees. Utilise la categorie courante.
     *
     * \param pref Le nom de la preference a rechercher.
     * \param sizeArray Cet entier contiendra la taille du tableau retourne.
     *
     * \return Un tableau contenant les valeurs associees a la preference.
     *         L'appelant de cette methode est responsable d'effacer ce tableau.
     */
    QString* getStringArray(const QString& pref, int& sizeArray);

    /**
     * \brief Recherche une ressource et retourne son tableau de valeurs associees. Utilise la categorie specifiee.
     *
     * \param dir Le nom de la categorie.
     * \param pref Le nom de la preference a rechercher.
     * \param sizeArray Cet entier contiendra la taille du tableau retourne.
     *
     * \return Un tableau contenant les valeurs associees a la preference.
     *         L'appelant de cette methode est responsable d'effacer ce tableau.
     */
    QString* getStringArray(const QString& dir, const QString& pref, int& sizeArray);

#if TY_ARCH_TYPE == TY_ARCHITECTURE_64
    /**
     * Mets a jour la valeur associee a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie courante.
     *
     * \param pref Le nom de la preference.
     * \param value La valeur a associer a la preference.
     */
    void setUInt(const QString& pref, const unsigned int& value);
    /**
     * Mets a jour la valeur associee a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie specifiee.
     *
     * \param dir Le nom de la categorie.
     * \param dir Le nom de la categorie.
     * \param pref Le nom de la preference.
     * \param value La valeur a associer a la preference.
     */
    void setUInt(const QString& dir, const QString& pref, const unsigned int& value);

    /**
     * Mets a jour la valeur associee a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie courante.
     *
     * \param pref Le nom de la preference.
     * \param value La valeur a associer a la preference.
     */
    void setUInt(const QString& pref, const size_t& value);
    /**
     * Mets a jour la valeur associee a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie specifiee.
     *
     * \param dir Le nom de la categorie.
     * \param dir Le nom de la categorie.
     * \param pref Le nom de la preference.
     * \param value La valeur a associer a la preference.
     */
    void setUInt(const QString& dir, const QString& pref, const size_t& value);

#if TY_COMPILER == TY_COMPILER_MSVC
    /**
     * Mets a jour la valeur associee a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie courante.
     *
     * \param pref Le nom de la preference.
     * \param value La valeur a associer a la preference.
     */
    void setUInt(const QString& pref, const unsigned long& value);
    /**
     * Mets a jour la valeur associee a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie specifiee.
     *
     * \param dir Le nom de la categorie.
     * \param dir Le nom de la categorie.
     * \param pref Le nom de la preference.
     * \param value La valeur a associer a la preference.
     */
    void setUInt(const QString& dir, const QString& pref, const unsigned long& value);
#endif
#else
    /**
     * \brief Met a jour la valeur associee a une preference.
     *
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie courante.
     *
     * \param pref Le nom de la preference.
     * \param value La valeur a associer a la preference.
     */
    void setUInt(const QString& pref, const size_t& value);

    /**
     * \brief Met a jour la valeur associee a une preference.
     *
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie specifiee.
     *
     * \param dir Le nom de la categorie.
     * \param dir Le nom de la categorie.
     * \param pref Le nom de la preference.
     * \param value La valeur a associer a la preference.
     */
    void setUInt(const QString& dir, const QString& pref, const size_t& value);

    /**
     * \brief Met a jour la valeur associee a une preference.
     *
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie courante.
     *
     * \param pref Le nom de la preference.
     * \param value La valeur a associer a la preference.
     */
    void setUInt(const QString& pref, const unsigned long& value);

    /**
     * \brief Met a jour la valeur associee a une preference.
     *
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie specifiee.
     *
     * \param dir Le nom de la categorie.
     * \param dir Le nom de la categorie.
     * \param pref Le nom de la preference.
     * \param value La valeur a associer a la preference.
     */
    void setUInt(const QString& dir, const QString& pref, const unsigned long& value);

#endif
    /**
     * \brief Met a jour la valeur associee a une preference.
     *
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie courante.
     *
     * \param pref Le nom de la preference.
     * \param value La valeur a associer a la preference.
     */
    void setInt(const QString& pref, const int& value);

    /**
     * \brief Met a jour la valeur associee a une preference.
     *
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie specifiee.
     *
     * \param dir Le nom de la categorie.
     * \param dir Le nom de la categorie.
     * \param pref Le nom de la preference.
     * \param value La valeur a associer a la preference.
     */
    void setInt(const QString& dir, const QString& pref, const int& value);

    /**
     * \brief Recherche une ressource et retourne sa valeur associee.Utilise la categorie courante.
     *
     * \param pref Le nom de la preference a rechercher.
     *
     * \return La valeur associee a la preference.
     */
    int getInt(const QString& pref);

    /**
     * \brief Recherche une ressource et retourne sa valeur associee.
     * Utilise la categorie courante.
     * Utilise la categorie specifiee.
     *
     * \param dir Le nom de la categorie.
     * \param pref Le nom de la preference a rechercher.
     *
     * \return La valeur associee a la preference.
     */
    int getInt(const QString& dir, const QString& pref);

    /**
     * \brief Met a jour la valeur associee a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie courante.
     *
     * \param pref Le nom de la preference.
     * \param value La valeur a associer a la preference.
     */

    void setBool(const QString& pref, const bool& value);
    /**
     * \brief Mets a jour la valeur associee a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie specifiee.
     *
     * \param dir Le nom de la categorie.
     * \param pref Le nom de la preference.
     * \param value La valeur a associer a la preference.
     */
    void setBool(const QString& dir, const QString& pref, const bool& value);

    /**
     * \brief Recherche une ressource et retourne sa valeur associee. Utilise la categorie courante.
     *
     * \param pref Le nom de la preference a rechercher.
     *
     * \return La valeur associee a la preference.
     */
    bool getBool(const QString& pref);

    /**
     * \brief Recherche une ressource et retourne sa valeur associee. Utilise la categorie specifiee.
     *
     * \param dir Le nom de la categorie.
     * \param pref Le nom de la preference a rechercher.
     *
     * \return La valeur associee a la preference.
     */
    bool getBool(const QString& dir, const QString& pref);

    /**
     * \brief Met a jour la valeur associee a une preference.
     *
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie courante.
     *
     * \param pref Le nom de la preference.
     * \param value La valeur a associer a la preference.
     */
    void setFloat(const QString& pref, const float& value);

    /**
     * \brief Met a jour la valeur associee a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie specifiee.
     *
     * \param dir Le nom de la categorie.
     * \param pref Le nom de la preference.
     * \param value La valeur a associer a la preference.
     */
    void setFloat(const QString& dir, const QString& pref, const float& value);

    /**
     * \brief Recherche une ressource et retourne sa valeur associee. Utilise la categorie courante.
     *
     * \param pref Le nom de la preference a rechercher.
     *
     * \return La valeur associee a la preference.
     */
    float getFloat(const QString& pref);

    /**
     * \brief Recherche une ressource et retourne sa valeur associee. Utilise la categorie specifiee.
     *
     * \param dir Le nom de la categorie.
     * \param pref Le nom de la preference a rechercher.
     *
     * \return La valeur associee a la preference.
     */
    float getFloat(const QString& dir, const QString& pref);

    /**
     * \brief Met a jour la valeur associee a une preference.
     *
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie courante.
     *
     * \param pref Le nom de la preference.
     * \param value La valeur a associer a la preference.
     */
    void setDouble(const QString& pref, const double& value);

    /**
     * \brief Met a jour la valeur associee a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie specifiee.
     *
     * \param dir Le nom de la categorie.
     * \param pref Le nom de la preference.
     * \param value La valeur a associer a la preference.
     */
    void setDouble(const QString& dir, const QString& pref, const double& value);

    /**
     * \brief Recherche une ressource et retourne sa valeur associee. Utilise la categorie courante.
     *
     * \param pref Le nom de la preference a rechercher.
     *
     * \return La valeur associee a la preference.
     */
    double getDouble(const QString& pref);
    /**
     * \brief Recherche une ressource et retourne sa valeur associee. Utilise la categorie specifiee.
     *
     * \param dir Le nom de la categorie.
     * \param pref Le nom de la preference a rechercher.
     *
     * \return La valeur associee a la preference.
     */
    double getDouble(const QString& dir, const QString& pref);

    /**
     * \brief Sauvegarde la position et la taille d'une fenetre.
     *
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie courante.
     *
     * \param pref Le nom de la preference.
     * \param posX La position en X.
     * \param posY La position en Y.
     * \param sizeX La taille en X.
     * \param sizeY La taille en Y.
     */
    void setFrame(const QString& pref, const int& posX, const int& posY, const int& sizeX, const int& sizeY);

    /**
     * \brief Sauvegarde la position et la taille d'une fenetre.
     *
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie specifiee.
     *
     * \param dir Le nom de la categorie.
     * \param pref Le nom de la preference.
     * \param posX La position en X.
     * \param posY La position en Y.
     * \param sizeX La taille en X.
     * \param sizeY La taille en Y.
     */
    void setFrame(const QString& dir, const QString& pref, const int& posX, const int& posY, const int& sizeX, const int& sizeY);

    /**
     * \brief Charge la position et la taille d'une fenetre. Utilise la categorie courante.
     *
     * \param pref Le nom de la preference a rechercher.
     * \param posX La position en X.
     * \param posY La position en Y.
     * \param sizeX La taille en X.
     * \param sizeY La taille en Y.
     */
    void getFrame(const QString& pref, int& posX, int& posY, int& sizeX, int& sizeY);

    /**
     * \brief Charge la position et la taille d'une fenetre. Utilise la categorie specifiee.
     *
     * \param dir Le nom de la categorie.
     * \param pref Le nom de la preference a rechercher.
     * \param posX La position en X.
     * \param posY La position en Y.
     * \param sizeX La taille en X.
     * \param sizeY La taille en Y.
     */
    void getFrame(const QString& dir, const QString& pref, int& posX, int& posY, int& sizeX, int& sizeY);

    /**
     * \brief Sauvegarde les coordonnees en pixel d'un point.
     *
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie courante.
     *
     * \param pref Le nom de la preference.
     * \param posX La coordonnee X.
     * \param posX La coordonnee Y.
     */
    void setPoint(const QString& pref, const int& x, const int& y);

    /**
     * \brief Sauvegarde les coordonnees en pixel d'un point.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie specifiee.
     *
     * \param dir Le nom de la categorie.
     * \param pref Le nom de la preference.
     * \param posX La coordonnee X.
     * \param posX La coordonnee Y.
     */
    void setPoint(const QString& dir, const QString& pref, const int& x, const int& y);

    /**
     * \brief Charge les coordonnees en pixel d'un point. Utilise la categorie courante.
     *
     * \param pref Le nom de la preference a rechercher.
     * \param posX La coordonnee X.
     * \param posX La coordonnee Y.
     */
    void getPoint(const QString& pref, int& x, int& y);

    /**
     * \brief Charge les coordonnees en pixel d'un point. Utilise la categorie specifiee.
     *
     * \param dir Le nom de la categorie.
     * \param pref Le nom de la preference a rechercher.
     * \param posX La coordonnee X.
     * \param posX La coordonnee Y.
     */
    void getPoint(const QString& dir, const QString& pref, int& x, int& y);

    /**
     * \brief Sauvegarde les composantes RGB d'une couleur, en float.Utilise la categorie courante.
     *
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     *
     * \param pref Le nom de la preference.
     * \param r La composante rouge.
     * \param g La composante verte.
     * \param b La composante bleue.
     */
    void setColor(const QString& pref, const float& r, const float& g, const float& b);

    /**
     * \brief Sauvegarde les composantes RGB d'une couleur, en float.Utilise la categorie specifiee.
     *
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     *
     * \param dir Le nom de la categorie.
     * \param pref Le nom de la preference.
     * \param r La composante rouge.
     * \param g La composante verte.
     * \param b La composante bleue.
     */
    void setColor(const QString& dir, const QString& pref, const float& r, const float& g, const float& b);

    /**
     * \brief Charge les composantes RGB d'une couleur, en float.Utilise la categorie courante.
     *
     * \param pref Le nom de la preference.
     * \param r La composante rouge.
     * \param g La composante verte.
     * \param b La composante bleue.
     */
    void getColor(const QString& pref, float& r, float& g, float& b);

    /**
     * \brief Charge les composantes RGB d'une couleur, en float. Utilise la categorie specifiee.
     *
     * \param dir Le nom de la categorie.
     * \param pref Le nom de la preference.
     * \param r La composante rouge.
     * \param g La composante verte.
     * \param b La composante bleue.
     */
    void getColor(const QString& dir, const QString& pref, float& r, float& g, float& b);

#if TY_ARCH_TYPE == TY_ARCHITECTURE_64
    /**
     * Methode utilitaire.
     * Convertit un <code>unsigned int</code> en <code>string</code>.
     */
    static QString uintToString(const unsigned int& val);

    /**
     * Methode utilitaire.
     * Convertit un <code>size_t</code> en <code>string</code>.
     */
    static QString uintToString(const size_t& val);
#if TY_COMPILER == TY_COMPILER_MSVC
    /**
     * Methode utilitaire.
     * Convertit un <code>unsigned long</code> en <code>string</code>.
     */
    static QString uintToString(const unsigned long& val);
#endif
#else
    /**
     * Methode utilitaire.
     * Convertit un <code>size_t</code> en <code>string</code>.
     */
    static QString uintToString(const size_t& val);

    /**
     * Methode utilitaire.
     * Convertit un <code>unsigned long</code> en <code>string</code>.
     */
    static QString uintToString(const unsigned long& val);
#endif

    /**
     * Methode utilitaire.
     * Convertit un <code>int</code> en <code>string</code>.
     */
    static QString intToString(const int& val);

    /**
     * Methode utilitaire.
     * Convertit un <code>float</code> en <code>string</code>.
     */
    static QString floatToString(const float& val);

    /**
     * Methode utilitaire.
     * Convertit un <code>double</code> en <code>string</code>.
     */
    static QString doubleToString(const double& val);

    /**
     * Renvoie un n° de version permettant de savoir si des valeurs ont ete modifiees
     */
    int getLastModificationCode() { return _nPreferenceModificationStamp;}

protected:
    ///Tableau associatif parametre/valeur.
    ODirMap _prefDirs;

    ///Le nom de la categorie courante.
    QString _curDir;

    ///Nom du noeud root du fichier XML.
    QString _xmlRootNodeName;
    ///Nom des noeuds des parametres du fichier XML.
    QString _xmlParamNodeName;
private:
    int _nPreferenceModificationStamp;
};


#endif // __O_PREFERENCE_MANAGER__
