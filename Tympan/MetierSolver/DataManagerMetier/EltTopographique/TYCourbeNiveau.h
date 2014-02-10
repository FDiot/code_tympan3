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

#ifndef __TY_COURBENIVEAU__
#define __TY_COURBENIVEAU__


#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYGeometryNode.h"
#include "Tympan/MetierSolver/DataManagerCore/TYColorInterface.h"

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerIHM/TYCourbeNiveauWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYCourbeNiveauGraphic.h"
#endif


/**
 * Permet de creer une altimetrie.
 */
class TYCourbeNiveau: public TYElement, public TYColorInterface
{
    OPROTOSUPERDECL(TYCourbeNiveau, TYElement)
    TY_EXTENSION_DECL(TYCourbeNiveau)
    TY_EXT_GRAPHIC_DECL(TYCourbeNiveau)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYCourbeNiveau();
    /**
     * Constructeur par copie.
     */
    TYCourbeNiveau(const TYCourbeNiveau& other);
    /**
     * Constructeur.
     */
    TYCourbeNiveau(const TYTabPoint& pts, double alt);
    /**
     * Destructeur.
     */
    virtual ~TYCourbeNiveau();

    ///Operateur =.
    TYCourbeNiveau& operator=(const TYCourbeNiveau& other);
    ///Operateur ==.
    bool operator==(const TYCourbeNiveau& other) const;
    ///Operateur !=.
    bool operator!=(const TYCourbeNiveau& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    virtual void setIsGeometryModified(bool isModified);

    /**
     * Vide la collection de points.
     */
    void purge() { _listPoints.clear(); setIsGeometryModified(true); }

    /**
     * Ajoute un point.
     */
    void addPoint(const TYPoint& pt) { _listPoints.push_back(pt); }

    /**
     * Ajoute un point.
     */
    void addPoint(double x, double y, double z = 0.0) { _listPoints.push_back(TYPoint(x, y, z)); setIsGeometryModified(true); }

    /**
     * Set/Get de la liste de points.
     */
    TYTabPoint& getListPoints() { return _listPoints; }
    /**
     * Set/Get de la liste de points.
     */
    const TYTabPoint& getListPoints() const { return _listPoints; }
    /**
     * Set/Get de la liste de points.
     */
    void setListPoints(const TYTabPoint& pts);

    /**
     * Fermeture d'une courbe de niveau
     */
    void close();

    /**
     * Set/Get de l'_altitude.
     */
    double getAltitude() const { return _altitude; }
    /**
     * Set/Get de l'_altitude.
     */
    void setAltitude(double alt);

    /**
     * Set/Get de la distance minimale entre les points pour la triangulation de Delaunay
     */
    void setDistMax(const double dist) { _distMax = dist;}

    /**
     * Get/Set de la distance minimale entre deux points
     */
    double getDistMax();

    /**
     * Get/Set de l'etat de la valeur distance min
     */
    bool getIsDMaxDefault() { return _isDMaxDefault; }
    const bool getIsDMaxDefault() const { return _isDMaxDefault; }

    /**
     * Get/Set de l'etat de la valeur distance min par defaut
     */
    void setIsDMaxDefault(const bool& etat);

    /**
     * Attribut l'altitude a la coordonnee Z de chaque point
     * composant cette courbe de niveau.
     */
    void applyAlitudeToPoints();

    /**
     * Fonction d'initialisation de la valeur de distance max par defaut pour les courbes
     */
    static double setDefaultDistMax();

    /**
     * Fonction d'initialisation de la valeur de distance max par defaut
     * pour les courbes (fonction appelee par le projet
     */
    static void setDefaultDistMax(const double& val) { _defaultDistMax = val; }

    /**
     * Get de la distance max par defaut
     */
    static double getDefaultDistMax() { return _defaultDistMax; }

    /**
     * Mise a jour de la valeur par defaut de la distance max entre les points
     */
    static void updateDefaultDistMax() { _defaultDistMax = setDefaultDistMax(); }

    /**
     * Mise a jour de la distance max
     */
    void updateDistMax();

    /**
     * Mise a jour des couleurs a partir des preferences
     */
    virtual void updateColor();



    // Membres
protected:
    // Valeur de distance max entre les points qui defini dans les preferences utilisateurs
    static double _defaultDistMax;
    // Flag pour indiquer que distance max n'est pas egal a la valeur par defaut (false)
    bool _isDMaxDefault;
    // Distance max specifique a la courbe
    double _distMax;
    ///Altitude.
    double _altitude;
    ///Liste de points.
    TYTabPoint _listPoints;
};


///Noeud geometrique de type TYCourbeNiveau.
typedef TYGeometryNode TYCourbeNiveauGeoNode;
///Smart Pointer sur TYCourbeNiveauGeoNode.
typedef SmartPtr<TYCourbeNiveauGeoNode> LPTYCourbeNiveauGeoNode;
///Collection de noeuds geometriques de type TYCourbeNiveau.
typedef std::vector<LPTYCourbeNiveauGeoNode> TYTabCourbeNiveauGeoNode;


#endif // __TY_COURBENIVEAU__
