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

#ifndef __TY_PREFERENCE_MANAGER__
#define __TY_PREFERENCE_MANAGER__


#include "Tympan/core/defines.h"
#if TY_USE_IHM

#include "Tympan/models/business/OPreferenceManager.h"

class QWidget;
class QString;
class TYSpectre;


/**
 * Classe statique pour la gestion des preferences dans Tympan.
 * Cette classe statique instancie un objet de type OPreferenceManager
 * et offre une interface statique pour une utilisation simple.
 */
class TYPreferenceManager
{
public:
    /**
     * Retourne l'instance unique qui gere les preferences.
     */
    static OPreferenceManager& getInstance() ;

    /**
     * Initialise le manager de preference.
     * Le tableau associatif preference/valeur est charge avec les valeurs
     * par defaut.
     *
     * @param fileName Le chemin du fichier de preferences a utiliser.
     *                 Si ce fichier n'est pas specifie, ce sont les parametres
     *                 par defaut qui sont utilises.
     */
    static void init(const QString& fileName = QString::null);

    /**
     * Vide le tableau de preferences.
     */
    static void reset();

    /**
     * Defini le chemin du fichier de preferences.
     *
     * @param fileName Le chemin du fichier de preferences a utiliser.
     */
    static void setFileName(const QString& fileName);

    /**
     * Ouvre le fichier de preference en XML et le parcours pour mettre a
     * jour la tableau de preferences.
     *
     * @return <code>false</code> si le fichier n'a pas pu etre ouvert.
     *
     * @see setFileName()
     */
    static bool read();

    /**
     * Enregistre le fichier de preference au format XML.
     *
     * @return <code>false</code> si le fichier n'a pas pu etre enregistre.
     *
     * @see setFileName()
     */
    static bool write();

    /**
     * Test si une preference est presente dans le tableau de
     * preferences.
     *
     * @param pref Le nom de la preference a tester.
     *
     * @return <code>true</code> si la preference existe;
     *         <code>false</code> sinon.
     */
    static bool exists(const QString& pref);

    /**
     * Defini la categorie courante pour la lecture et l'ecriture de parametres.
     *
     * @param dir Le nom de la categorie, si elle n'existe pas elle sera creee.
     */
    static void setCurrentDirectory(QString dir) { getInstance().setCurrentDirectory(dir); }
    /**
     * Retourne la categorie courante pour la lecture et l'ecriture de parametres.
     *
     * @return Le nom de la categorie courante.
     */
    static QString getCurrentDirectory() { return getInstance().getCurrentDirectory(); }

    /**
     * Mets a jour la valeur associee a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     *
     * @param pref Le nom de la preference.
     * @param value La valeur a associer a la preference.
     */
    static void setString(const QString& pref, const QString& value)
    {
        getInstance().setString(pref, value);
    }

    /**
     * Recherche une ressource et retourne sa valeur associee.
     *
     * @param pref Le nom de la preference a rechercher.
     *
     * @return La valeur associee a la preference.
     */
    static QString getString(const QString& pref)
    {
        return getInstance().getString(pref);
    }

    /**
     * Mets a jour un tableau de valeurs associees a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     *
     * @param pref Le nom de la preference.
     * @param valueArray Le tableau de valeurs a associer a la preference.
     * @param sizeArray Le nombre de valeurs contenues dans le tableau passe.
     */
    static void setStringArray(const QString& pref, const QString* valueArray, const int& sizeArray)
    {
        getInstance().setStringArray(pref, valueArray, sizeArray);
    }

    /**
     * Recherche une ressource et retourne son tableau de valeurs
     * associees.
     *
     * @param pref Le nom de la preference a rechercher.
     * @param sizeArray Cet entier contiendra la taille du tableau retourne.
     *
     * @return Un tableau contenant les valeurs associees a la preference.
     *         L'appelant de cette methode est responsable d'effacer ce tableau.
     */
    static QString* getStringArray(const QString& pref, int& sizeArray)
    {
        return getInstance().getStringArray(pref, sizeArray);
    }

#if TY_ARCH_TYPE == TY_ARCHITECTURE_64
    /**
     * Mets a jour la valeur associee a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     *
     * @param pref Le nom de la preference.
     * @param value La valeur a associer a la preference.
     */
    static void setUInt(const QString& pref, const unsigned int& value)
    {
        getInstance().setUInt(pref, value);
    }

    /**
     * Mets a jour la valeur associee a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     *
     * @param pref Le nom de la preference.
     * @param value La valeur a associer a la preference.
     */
    static void setUInt(const QString& pref, const size_t& value)
    {
        getInstance().setUInt(pref, value);
    }
#if TY_COMPILER == TY_COMPILER_MSVC
    /**
     * Mets a jour la valeur associee a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     *
     * @param pref Le nom de la preference.
     * @param value La valeur a associer a la preference.
     */
    static void setUInt(const QString& pref, const unsigned long& value)
    {
        getInstance().setUInt(pref, value);
    }
#endif
#else
    /**
     * Mets a jour la valeur associee a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     *
     * @param pref Le nom de la preference.
     * @param value La valeur a associer a la preference.
     */
    static void setUInt(const QString& pref, const size_t& value)
    {
        getInstance().setUInt(pref, value);
    }

    /**
     * Mets a jour la valeur associee a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     *
     * @param pref Le nom de la preference.
     * @param value La valeur a associer a la preference.
     */
    static void setUInt(const QString& pref, const unsigned long& value)
    {
        getInstance().setUInt(pref, value);
    }
#endif

    /**
     * Mets a jour la valeur associee a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     *
     * @param pref Le nom de la preference.
     * @param value La valeur a associer a la preference.
     */
    static void setInt(const QString& pref, const int& value)
    {
        getInstance().setInt(pref, value);
    }

    /**
     * Recherche une ressource et retourne sa valeur associee.
     *
     * @param pref Le nom de la preference a rechercher.
     *
     * @return La valeur associee a la preference.
     */
    static int getInt(const QString& pref)
    {
        return getInstance().getInt(pref);
    }

    /**
     * Mets a jour la valeur associee a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     *
     * @param pref Le nom de la preference.
     * @param value La valeur a associer a la preference.
     */
    static void setBool(const QString& pref, const bool& value)
    {
        getInstance().setBool(pref, value);
    }

    /**
     * Recherche une ressource et retourne sa valeur associee.
     *
     * @param pref Le nom de la preference a rechercher.
     *
     * @return La valeur associee a la preference.
     */
    static bool getBool(const QString& pref)
    {
        return getInstance().getBool(pref);
    }

    /**
     * Mets a jour la valeur associee a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     *
     * @param pref Le nom de la preference.
     * @param value La valeur a associer a la preference.
     */
    static void setFloat(const QString& pref, const float& value)
    {
        getInstance().setFloat(pref, value);
    }

    /**
     * Recherche une ressource et retourne sa valeur associee.
     *
     * @param pref Le nom de la preference a rechercher.
     *
     * @return La valeur associee a la preference.
     */
    static float getFloat(const QString& pref)
    {
        return getInstance().getFloat(pref);
    }

    /**
     * Mets a jour la valeur associee a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     *
     * @param pref Le nom de la preference.
     * @param value La valeur a associer a la preference.
     */
    static void setDouble(const QString& pref, const double& value)
    {
        getInstance().setDouble(pref, value);
    }

    /**
     * Recherche une ressource et retourne sa valeur associee.
     *
     * @param pref Le nom de la preference a rechercher.
     *
     * @return La valeur associee a la preference.
     */
    static double getDouble(const QString& pref)
    {
        return getInstance().getDouble(pref);
    }

    /**
     * Sauvegarder la geometrie (position et taille) d'un widget.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     *
     * @param pref Le nom de la preference.
     * @param pWidget Le widget dont on souhaite conserve la geometrie.
     */
    static void saveGeometryToPreferences(const QString& pref, const QWidget* pWidget);

    /**
     * Charge et applique la position et la taille d'une fenetre.
     *
     * @param pref Le nom de la preference a rechercher.
     * @param pWidget Le widget dont on souhaite recupere la geometrie.
     */
    static void loadGeometryFromPreferences(const QString& pref, QWidget* pWidget);

    /**
     * Sauvegarder les coordonnees en pixel d'un point.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     *
     * @param pref Le nom de la preference.
     * @param posX La coordonnee X.
     * @param posX La coordonnee Y.
     */
    static void setPoint(const QString& pref, const int& x, const int& y)
    {
        getInstance().setPoint(pref, x, y);
    }

    /**
     * Charge les coordonnees en pixel d'un point.
     *
     * @param pref Le nom de la preference a rechercher.
     * @param posX La coordonnee X.
     * @param posX La coordonnee Y.
     */
    static void getPoint(const QString& pref, int& x, int& y)
    {
        getInstance().getPoint(pref, x, y);
    }

    /**
     * Sauvegarder les composantes RGB d'une couleur, en float.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     *
     * @param pref Le nom de la preference.
     * @param red La composante rouge.
     * @param green La composante verte.
     * @param blue La composante bleue.
     */
    static void setColor(const QString& pref, const float& r, const float& g, const float& b)
    {
        getInstance().setColor(pref, r, g, b);
    }

    /**
     * Charge les composantes RGB d'une couleur, en float.
     *
     * @param pref Le nom de la preference.
     * @param red La composante rouge.
     * @param green La composante verte.
     * @param blue La composante bleue.
     */
    static void getColor(const QString& pref, float& r, float& g, float& b)
    {
        getInstance().getColor(pref, r, g, b);
    }

    /**
     * Mets a jour la valeur associee a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     *
     * @param pref Le nom de la preference.
     * @param value La valeur a associer a la preference.
     */
    static void setSpectre(const QString& pref, const TYSpectre*);

    /**
     * Recherche une ressource et retourne sa valeur associee.
     *
     * @param pref Le nom de la preference a rechercher.
     *
     * @return La valeur associee a la preference.
     */
    static TYSpectre* getSpectre(const QString& pref);

    /**
     * Test si une preference est presente dans le tableau de
     * preferences.
     * Utilise la categorie specifiee.
     *
     * @param dir Le nom de la categorie.
     * @param pref Le nom de la preference a tester.
     *
     * @return <code>true</code> si la preference existe;
     *         <code>false</code> sinon.
     */
    static bool exists(const QString& dir, const QString& pref)
    {
        return getInstance().exists(dir, pref);
    }

    /**
     * Mets a jour la valeur associee a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie specifiee.
     *
     * @param dir Le nom de la categorie.
     * @param pref Le nom de la preference.
     * @param value La valeur a associer a la preference.
     */
    static void setString(const QString& dir, const QString& pref, const QString& value)
    {
        getInstance().setString(dir, pref, value);
    }

    /**
     * Recherche une ressource et retourne sa valeur associee.
     * Utilise la categorie specifiee.
     *
     * @param dir Le nom de la categorie.
     * @param pref Le nom de la preference a rechercher.
     *
     * @return La valeur associee a la preference.
     */
    static QString getString(const QString& dir, const QString& pref)
    {
        return getInstance().getString(dir, pref);
    }

    /**
     * Mets a jour un tableau de valeurs associees a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie specifiee.
     *
     * @param dir Le nom de la categorie.
     * @param pref Le nom de la preference.
     * @param valueArray Le tableau de valeurs a associer a la preference.
     * @param sizeArray Le nombre de valeurs contenues dans le tableau passe.
     */
    static void setStringArray(const QString& dir, const QString& pref, const QString* valueArray, const int& sizeArray)
    {
        getInstance().setStringArray(dir, pref, valueArray, sizeArray);
    }

    /**
     * Recherche une ressource et retourne son tableau de valeurs
     * associees.
     * Utilise la categorie specifiee.
     *
     * @param dir Le nom de la categorie.
     * @param pref Le nom de la preference a rechercher.
     * @param sizeArray Cet entier contiendra la taille du tableau retourne.
     *
     * @return Un tableau contenant les valeurs associees a la preference.
     *         L'appelant de cette methode est responsable d'effacer ce tableau.
     */
    static QString* getStringArray(const QString& dir, const QString& pref, int& sizeArray)
    {
        return getInstance().getStringArray(dir, pref, sizeArray);
    }

#if TY_ARCH_TYPE == TY_ARCHITECTURE_64
    /**
     * Mets a jour la valeur associee a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie specifiee.
     *
     * @param dir Le nom de la categorie.
     * @param pref Le nom de la preference.
     * @param value La valeur a associer a la preference.
     */
    static void setUInt(const QString& dir, const QString& pref, const unsigned int& value)
    {
        getInstance().setUInt(dir, pref, value);
    }

    /**
     * Mets a jour la valeur associee a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie specifiee.
     *
     * @param dir Le nom de la categorie.
     * @param pref Le nom de la preference.
     * @param value La valeur a associer a la preference.
     */
    static void setUInt(const QString& dir, const QString& pref, const size_t& value)
    {
        getInstance().setUInt(dir, pref, value);
    }
#if TY_COMPILER == TY_COMPILER_MSVC
    /**
     * Mets a jour la valeur associee a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie specifiee.
     *
     * @param dir Le nom de la categorie.
     * @param pref Le nom de la preference.
     * @param value La valeur a associer a la preference.
     */
    static void setUInt(const QString& dir, const QString& pref, const unsigned long& value)
    {
        getInstance().setUInt(dir, pref, value);
    }
#endif
#else
    /**
     * Mets a jour la valeur associee a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie specifiee.
     *
     * @param dir Le nom de la categorie.
     * @param pref Le nom de la preference.
     * @param value La valeur a associer a la preference.
     */
    static void setUInt(const QString& dir, const QString& pref, const size_t& value)
    {
        getInstance().setUInt(dir, pref, value);
    }

    /**
     * Mets a jour la valeur associee a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie specifiee.
     *
     * @param dir Le nom de la categorie.
     * @param pref Le nom de la preference.
     * @param value La valeur a associer a la preference.
     */
    static void setUInt(const QString& dir, const QString& pref, const unsigned long& value)
    {
        getInstance().setUInt(dir, pref, value);
    }
#endif

    /**
     * Mets a jour la valeur associee a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie specifiee.
     *
     * @param dir Le nom de la categorie.
     * @param pref Le nom de la preference.
     * @param value La valeur a associer a la preference.
     */
    static void setInt(const QString& dir, const QString& pref, const int& value)
    {
        getInstance().setInt(dir, pref, value);
    }

    /**
     * Recherche une ressource et retourne sa valeur associee.
     * Utilise la categorie specifiee.
     *
     * @param dir Le nom de la categorie.
     * @param pref Le nom de la preference a rechercher.
     *
     * @return La valeur associee a la preference.
     */
    static int getInt(const QString& dir, const QString& pref)
    {
        return getInstance().getInt(dir, pref);
    }

    /**
     * Mets a jour la valeur associee a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie specifiee.
     *
     * @param dir Le nom de la categorie.
     * @param pref Le nom de la preference.
     * @param value La valeur a associer a la preference.
     */
    static void setBool(const QString& dir, const QString& pref, const bool& value)
    {
        getInstance().setBool(dir, pref, value);
    }

    /**
     * Recherche une ressource et retourne sa valeur associee.
     * Utilise la categorie specifiee.
     *
     * @param dir Le nom de la categorie.
     * @param pref Le nom de la preference a rechercher.
     *
     * @return La valeur associee a la preference.
     */
    static bool getBool(const QString& dir, const QString& pref)
    {
        return getInstance().getBool(dir, pref);
    }

    /**
     * Mets a jour la valeur associee a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie specifiee.
     *
     * @param dir Le nom de la categorie.
     * @param pref Le nom de la preference.
     * @param value La valeur a associer a la preference.
     */
    static void setFloat(const QString& dir, const QString& pref, const float& value)
    {
        getInstance().setFloat(dir, pref, value);
    }

    /**
     * Recherche une ressource et retourne sa valeur associee.
     * Utilise la categorie specifiee.
     *
     * @param dir Le nom de la categorie.
     * @param pref Le nom de la preference a rechercher.
     *
     * @return La valeur associee a la preference.
     */
    static float getFloat(const QString& dir, const QString& pref)
    {
        return getInstance().getFloat(dir, pref);
    }

    /**
     * Mets a jour la valeur associee a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie specifiee.
     *
     * @param dir Le nom de la categorie.
     * @param pref Le nom de la preference.
     * @param value La valeur a associer a la preference.
     */
    static void setDouble(const QString& dir, const QString& pref, const double& value)
    {
        getInstance().setDouble(dir, pref, value);
    }

    /**
     * Recherche une ressource et retourne sa valeur associee.
     * Utilise la categorie specifiee.
     *
     * @param dir Le nom de la categorie.
     * @param pref Le nom de la preference a rechercher.
     *
     * @return La valeur associee a la preference.
     */
    static double getDouble(const QString& dir, const QString& pref)
    {
        return getInstance().getDouble(dir, pref);
    }

    /**
     * Sauvegarder les coordonnees en pixel d'un point.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie specifiee.
     *
     * @param dir Le nom de la categorie.
     * @param pref Le nom de la preference.
     * @param posX La coordonnee X.
     * @param posX La coordonnee Y.
     */
    static void setPoint(const QString& dir, const QString& pref, const int& x, const int& y)
    {
        getInstance().setPoint(dir, pref, x, y);
    }

    /**
     * Charge les coordonnees en pixel d'un point.
     * Utilise la categorie specifiee.
     *
     * @param dir Le nom de la categorie.
     * @param pref Le nom de la preference a rechercher.
     * @param posX La coordonnee X.
     * @param posX La coordonnee Y.
     */
    static void getPoint(const QString& dir, const QString& pref, int& x, int& y)
    {
        getInstance().getPoint(dir, pref, x, y);
    }

    /**
     * Sauvegarder les composantes RGB d'une couleur, en float.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie specifiee.
     *
     * @param dir Le nom de la categorie.
     * @param pref Le nom de la preference.
     * @param red La composante rouge.
     * @param green La composante verte.
     * @param blue La composante bleue.
     */
    static void setColor(const QString& dir, const QString& pref, const float& r, const float& g, const float& b)
    {
        getInstance().setColor(dir, pref, r, g, b);
    }

    /**
     * Charge les composantes RGB d'une couleur, en float.
     * Utilise la categorie specifiee.
     *
     * @param dir Le nom de la categorie.
     * @param pref Le nom de la preference.
     * @param red La composante rouge.
     * @param green La composante verte.
     * @param blue La composante bleue.
     */
    static void getColor(const QString& dir, const QString& pref, float& r, float& g, float& b)
    {
        getInstance().getColor(dir, pref, r, g, b);
    }

    /**
     * Mets a jour la valeur associee a une preference.
     * Si la preference n'existe pas, celle-ci est ajoutee au tableau
     * de preferences.
     * Utilise la categorie specifiee.
     *
     * @param dir Le nom de la categorie.
     * @param pref Le nom de la preference.
     * @param value La valeur a associer a la preference.
     */
    static void setSpectre(const QString& dir, const QString& pref, const TYSpectre*);

    /**
     * Recherche une ressource et retourne sa valeur associee.
     * Utilise la categorie specifiee.
     *
     * @param dir Le nom de la categorie.
     * @param pref Le nom de la preference a rechercher.
     *
     * @return La valeur associee a la preference.
     */
    static TYSpectre* getSpectre(const QString& dir, const QString& pref);

    /**
     * Renvoie un n° de version permettant de savoir si des valeurs ont ete modifiees
     */
    static int getLastModificationCode() { return getInstance().getLastModificationCode();}

private:
    ///Tableau associatif parametre/valeur.
    static OPreferenceManager* _prefMngr;

    ///Fichier de preference.
    static QString& getFileName();
    static QString* _fileName;
};


#endif // TY_USE_IHM


#endif // __O_PREFERENCE_MANAGER__
