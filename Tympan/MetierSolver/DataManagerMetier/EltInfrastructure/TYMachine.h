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

#ifndef __TY_MACHINE__
#define __TY_MACHINE__


#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticVolumeNode.h"
#include "TYMurElement.h"

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerIHM/TYMachineWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYMachineGraphic.h"
#endif


/**
 * Comprend les proprietes acoustiques et geometriques d'une machine.
 *
 * @author Projet_Tympan
 *
 */
class TYMachine: public TYAcousticVolumeNode
{
    OPROTOSUPERDECL(TYMachine, TYAcousticVolumeNode)
    TY_EXTENSION_DECL(TYMachine)
    TY_EXT_GRAPHIC_DECL(TYMachine)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYMachine();
    /**
     * Constructeur par copie.
     */
    TYMachine(const TYMachine& other);
    /**
     * Destructeur.
     */
    virtual ~TYMachine();

    ///Operateur =.
    TYMachine& operator=(const TYMachine& other);
    ///Operateur ==.
    bool operator==(const TYMachine& other) const;
    ///Operateur !=.
    bool operator!=(const TYMachine& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    virtual TYTabSourcePonctuelleGeoNode getSrcs() const;
    virtual TYSourcePonctuelle srcPonctEquiv() const;
    virtual bool updateAcoustic(const bool& force = false);
    virtual void distriSrcs();
    virtual bool setSrcsLw();

    virtual double volume() const;
    virtual double surface() const;
    virtual TYTabVector normals() const;
    virtual TYTabPoint sommets() const;
    virtual TYTabLPPolygon faces() const;
    virtual TYBox volEnglob() const;
    virtual TYPoint centreGravite() const;
    virtual int intersects(const OSegment3D& seg, TYTabPoint& ptList) const;
    virtual int isInside(const TYPoint& pt) const;

    /**
     * Set/Get de la categorie.
     */
    int getCategorie() const { return _categorie; }
    /**
     * Set/Get de la categorie.
     */
    void setCategorie(int cate) { _categorie = cate; }

    /**
     * Set/Get du nom du constructeur.
     */
    QString getConstructeur() const { return _constructeur; }
    /**
     * Set/Get du nom du constructeur.
     */
    void setConstructeur(QString name) { _constructeur = name; }

    /**
     * Set/Get du nom du modele.
     */
    QString getModele() const { return _modele; }
    /**
     * Set/Get du nom du modele.
     */
    void setModele(QString name) { _modele = name; }

    /**
     * Set/Get des remarques.
     */
    QString getCommentaire() const { return _commentaire; }
    /**
     * Set/Get des remarques.
     */
    void setCommentaire(QString name) { _commentaire = name; }


    // Membres
protected:
    ///Categorie.
    int _categorie;
    ///Nom du constructeur.
    QString _constructeur;
    ///Nom du modele.
    QString _modele;
    ///Commentaires.
    QString _commentaire;
};


///Noeud geometrique de type TYMachine.
typedef TYGeometryNode TYMachineGeoNode;
///Smart Pointer sur TYMachineGeoNode.
typedef SmartPtr<TYMachineGeoNode> LPTYMachineGeoNode;
///Collection de noeuds geometriques de type TYMachine.
typedef std::vector<LPTYMachineGeoNode> TYTabMachineGeoNode;


#endif // __TY_MACHINE__
