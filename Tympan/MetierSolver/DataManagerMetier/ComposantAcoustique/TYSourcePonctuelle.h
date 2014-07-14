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

#ifndef __TY_SOURCEPONCTUELLE__
#define __TY_SOURCEPONCTUELLE__

#include "TYSpectre.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYPoint.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYSegment.h"
#include "TYSource.h"
#include "TYAttenuateur.h"
#include "TYDirectivite.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYGeometryNode.h"


///Tableau associatif Regime/Directivite.
typedef std::map<int, TYDirectivite> TYMapRegimeDirectivite;

/**
 * Definition des types de rayonnement.
 * <b>SPHERIC</b> : rayonnement homogene sur une sphere
 * <b>HEMISPHERIC</b> : rayonnement homogene sur une demi sphere
 * <b>SOLID</b> : rayonnement homogene dans un angle solide donne avec phi=theta
 * <b>FORCED</b> : rayonnement a directivite imposee selon l'angle
 * <b>CALCULATED</b> : rayonnment dont la directivite depend du support
 */
typedef enum { SPHERIC, HEMISPHERIC, SOLID, FORCED, CALCULATED } TYTypeRaynt;


class OSegment3D;

/**
 * Classe de definition d'une source ponctuelle.
 */
class TYSourcePonctuelle: public TYSource, public TYColorInterface
{
    OPROTODECL(TYSourcePonctuelle)
    TY_EXTENSION_DECL_ONLY(TYSourcePonctuelle)
    TY_EXT_GRAPHIC_DECL_ONLY(TYSourcePonctuelle)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYSourcePonctuelle();
    /**
     * Constructeur par copie.
     */
    TYSourcePonctuelle(const TYSourcePonctuelle& other);
    /**
     * Destructeur.
     */
    virtual ~TYSourcePonctuelle();

    ///Operateur =.
    TYSourcePonctuelle& operator=(const TYSourcePonctuelle& other);
    ///Operateur ==.
    bool operator==(const TYSourcePonctuelle& other) const;
    ///Operateur !=.
    bool operator!=(const TYSourcePonctuelle& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    /**
     * Set/Get du type de cette source.
     */
    int getType() const { return _type; }
    /**
     * Set/Get du type de cette source.
     */
    void setType(int type);

    /**
     * Set/Get de l'objet a afficher.
     */
    int getObject() const { return _object; }
    /**
     * Set/Get de l'objet a afficher.
     */
    void setObject(int object) { _object = object; }

    /**
     * Set/Get du type de rayonnement.
     */
    int getTypeRaynt() const { return _typeRaynt; }
    /**
     * Set/Get du type de rayonnement.
     */
    void setTypeRaynt(TYTypeRaynt type) { _typeRaynt = type; }

    /**
     * Set/Get de la position.
     */
    LPTYPoint getPos() { return _pPos; }
    /**
     * Set/Get de la position.
     */
    const LPTYPoint getPos() const { return _pPos; }
    /**
     * Set/Get de la position.
     */
    void setPos(const LPTYPoint pPos) { _pPos = pPos; }

    /**
     * Set/Get de l'orientation.
     */
    OVector3D& getOrientation() { return _orientation; }
    /**
     * Set/Get de l'orientation.
     */
    const OVector3D& getOrientation() const { return _orientation; }
    /**
     * Set/Get de l'orientation.
     */
    void setOrientation(const OVector3D& orientation) { _orientation = orientation; }

    /**
     * Set/Get de la hauteur de cette source par rapport au sol (a
     * l'altimetrie en fait).
     */
    void setHauteur(double hauteur) { _hauteur = hauteur; setIsGeometryModified(true); }

    /**
     * Set/Get de la hauteur de cette source par rapport au sol (a
     * l'altimetrie en fait).
     */
    double getHauteur() { return _hauteur; }
    const double getHauteur() const { return _hauteur; }

    /**
     * Les differents types possible d'une source ponctuelle.
     */
    enum
    {
        TypeSurface,    /**< 0 - Sources situees sur une face. */
        TypeBouche,     /**< 1 - Sources bouches de ventilation. */
        TypeCheminee,   /**< 2 - Sources cheminees. */
        TypeUser        /**< 3 - Sources utilisateur. */
    };

    /**
     * Les differents types d'objet graphique que l'on peut afficher.
     */
    enum
    {
        objectCube,
        objectPyramid,
        objectSphere,
        objectStar
    };


    // Membres
protected:
    ///Type de cette source.
    int _type;

    ///Type de rayonnement.
    int _typeRaynt;

    ///Object representant la source
    int _object;

    ///Position.
    LPTYPoint _pPos;

    ///Orientation.
    OVector3D _orientation;

    ///La hauteur de la source par rapport a l'altitude (dans le cas de source utilisateur).
    double _hauteur;
};


///Noeud geometrique de type TYSourcePonctuelle.
typedef TYGeometryNode TYSourcePonctuelleGeoNode;
///Smart Pointer sur TYSourcePonctuelleGeoNode.
typedef SmartPtr<TYSourcePonctuelleGeoNode> LPTYSourcePonctuelleGeoNode;
///Collection de noeuds geometriques de type TYSourcePonctuelle.
typedef std::vector<LPTYSourcePonctuelleGeoNode> TYTabSourcePonctuelleGeoNode;
//Tableau associatif regroupant un pointeur sur TYElement a un tableau de sources
typedef std::map<TYElement*, TYTabSourcePonctuelleGeoNode> TYMapElementTabSources;



#endif // __TY_SOURCEPONCTUELLE__
