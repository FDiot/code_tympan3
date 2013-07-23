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

#ifndef __TY_TOPOGRAPHIE__
#define __TY_TOPOGRAPHIE__


#include "Tympan/MetierSolver/ToolsMetier/OSegment3D.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltTopographique/TYPlanEau.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltTopographique/TYCoursEau.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltTopographique/TYAltimetrie.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltMateriaux/TYSol.h"

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerIHM/TYTopographieWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYTopographieGraphic.h"
#endif


///Structure contenant un segment et un terrain.
typedef struct
{
    OSegment3D _segment;
    TYTerrain* _terrain;
} TYStructSegPtrTer;

///Tableau de structures contenant un segment et un terrain.
typedef std::vector<TYStructSegPtrTer> TYTabStructSegPtrTer;

typedef enum {TYTERRAIN, TYPLANEAU} typeTerrain;
typedef struct
{
    LPTYElement pElement;   // Pointeur sur l'element
    OPoint3D* pTabPoints;   // Pointeur sur un tableau de points
    OBox box ;              // Boite englobante de l'ensemble des points
    OVector3D normale;      // Normale au plan du terrain;
    typeTerrain ter;        // Type de terrain
    short nbPts;            // Nombre de points dans le tableau
} TYStructElemPts;



/**
 * Centralise les elements de topographie.
 *
 * @author Projet_Tympan
 *
 */
class TYTopographie: public TYElement
{
    OPROTOSUPERDECL(TYTopographie, TYElement)
    TY_EXTENSION_DECL(TYTopographie)
    TY_EXT_GRAPHIC_DECL(TYTopographie)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYTopographie();
    /**
     * Constructeur par copie.
     */
    TYTopographie(const TYTopographie& other);
    /**
     * Destructeur.
     */
    virtual ~TYTopographie();

    ///Operateur =.
    TYTopographie& operator=(const TYTopographie& other);
    ///Operateur ==.
    bool operator==(const TYTopographie& other) const;
    ///Operateur !=.
    bool operator!=(const TYTopographie& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    virtual void setIsGeometryModified(bool isModified);

    //@}

    /**
     * @name Gestion du Calcul courant
     */
    //@{

    /**
     * Verifie si cet element est present dans la liste d'ID passee.
     * S'il est present cela signifie qu'il est actif dans le
     * Calcul courant, c'est cet etat qui est retourne.
     *
     * @param listID La liste d'ID du Calcul courant.
     * @param recursif Indique si la mise a jour du Calcul courant doit etre recursive.
     *
     * @return <code>true</code> si cet objet est actif dans le Calcul concerne;
     *         <code>false</code> sinon.
     */
    virtual void updateCurrentCalcul(TYListID& listID, bool recursif = true);

    /**
     * Reattribut le bon parent aux composants de cette topographie.
     */
    virtual void reparent();

    /**
     * Vide tous les tableaux.
     */
    void purge();

    /**
     * Concatene 2 topographie
     */
    void concatTopo(const TYTopographie* pTopo, bool emprise = true);

    /**
     * Set/Get de la liste des plans d'eau.
     */
    TYTabPlanEauGeoNode& getListPlanEau() { return _listPlanEau; }
    /**
     * Set/Get de la liste des plans d'eau.
     */
    const TYTabPlanEauGeoNode& getListPlanEau() const { return _listPlanEau; }
    /**
     * Set/Get de la liste des plans d'eau.
     */
    void setListPlanEau(const TYTabPlanEauGeoNode& list) { _listPlanEau = list; setIsGeometryModified(true); }
    /**
     * Ajoute un element a la liste des plans d'eau.
     */
    bool addPlanEau(LPTYPlanEauGeoNode pPlanEauGeoNode);
    /**
     * Ajoute un element a la liste des plans d'eau.
     */
    bool addPlanEau(LPTYPlanEau pPlanEau);
    /**
     * Supprime une entree a la liste des plans d'eau.
     */
    bool remPlanEau(const LPTYPlanEauGeoNode pPlanEauGeoNode);
    /**
     * Supprime une entree a la liste des plans d'eau.
     */
    bool remPlanEau(const LPTYPlanEau pPlanEau);
    /**
     * Supprime une entree a la liste des plans d'eau
     * a partir de son identifiant.
     */
    bool remPlanEau(QString idPlanEau);
    /**
     * Suppression de tous les elements.
     */
    void remAllPlanEau();
    /**
     * Retourne un plan d'eau a partir de son index.
     */
    LPTYPlanEau getPlanEau(int index) { return TYPlanEau::safeDownCast(_listPlanEau.at(index)->getElement()); }
    /**
     * Retrouve le GeoNode associe a un plan d'eau.
     *
     * @param pPlanEau L'element a chercher.
     *
     * @return Le GeoNode associe a l'element a chercher si trouve sinon NULL.
     */
    LPTYPlanEauGeoNode findPlanEau(const LPTYPlanEau pPlanEau);

    /**
     * Set/Get de la liste des cours d'eau.
     */
    TYTabCoursEauGeoNode& getListCrsEau() { return _listCrsEau; }
    /**
     * Set/Get de la liste des cours d'eau.
     */
    const TYTabCoursEauGeoNode& getListCrsEau() const { return _listCrsEau; }
    /**
     * Set/Get de la liste des cours d'eau.
     */
    void setListCrsEau(const TYTabCoursEauGeoNode& list) { _listCrsEau = list; setIsGeometryModified(true); }
    /**
     * Ajoute un element a la liste des cours d'eau.
     */
    bool addCrsEau(LPTYCoursEauGeoNode pCoursEauGeoNode);
    /**
     * Ajoute un element a la liste des cours d'eau.
     */
    bool addCrsEau(LPTYCoursEau pCoursEau);
    /**
     * Supprime une entree a la liste des cours d'eau.
     */
    bool remCrsEau(const LPTYCoursEauGeoNode pCoursEauGeoNode);
    /**
     * Supprime une entree a la liste des cours d'eau.
     */
    bool remCrsEau(const LPTYCoursEau pCoursEau);
    /**
     * Supprime une entree a la liste des cours d'eau
     * a partir de son identifiant.
     */
    bool remCrsEau(QString idCrsEau);
    /**
     * Suppression de tous les elements.
     */
    void remAllCrsEau();
    /**
     * Retourne un cours d'eau a partir de son index.
     */
    LPTYCoursEau getCrsEau(int index) { return TYCoursEau::safeDownCast(_listCrsEau.at(index)->getElement()); }
    /**
     * Retrouve le GeoNode associe a un cours d'eau.
     *
     * @param pCrsEau L'element a chercher.
     *
     * @return Le GeoNode associe a l'element a chercher si trouve sinon NULL.
     */
    LPTYCoursEauGeoNode findCrsEau(const LPTYCoursEau pCrsEau);

    /**
     * Set/Get de la liste des terrains.
     */
    TYTabTerrainGeoNode& getListTerrain() { return _listTerrain; }
    /**
     * Set/Get de la liste des terrains.
     */
    const TYTabTerrainGeoNode& getListTerrain() const { return _listTerrain; }
    /**
     * Set/Get de la liste des terrains.
     */
    void setTerrain(const TYTabTerrainGeoNode& list) { _listTerrain = list; setIsGeometryModified(true); }
    /**
     * Ajoute un element a la liste des terrains.
     */
    bool addTerrain(LPTYTerrainGeoNode pTerGeoNode);
    /**
     * Ajoute un element a la liste des terrains.
     */
    bool addTerrain(LPTYTerrain pTer);
    /**
     * Supprime une entree a la liste des terrains.
     */
    bool remTerrain(const LPTYTerrainGeoNode pTerGeoNode);
    /**
     * Supprime une entree a la liste des terrains.
     */
    bool remTerrain(const LPTYTerrain pTer);
    /**
     * Supprime une entree a la liste des terrains
     * a partir de son identifiant.
     */
    bool remTerrain(QString idTerrain);
    /**
     * Suppression de tous les elements.
     */
    void remAllTerrain();
    /**
     * Retourne un terrain a partir de son index.
     */
    LPTYTerrain getTerrain(int index) { return TYTerrain::safeDownCast(_listTerrain.at(index)->getElement()); }
    /**
     * Retrouve le GeoNode associe a un terrain.
     *
     * @param pTerrain L'element a chercher.
     *
     * @return Le GeoNode associe a l'element a chercher si trouve sinon NULL.
     */
    LPTYTerrainGeoNode findTerrain(const LPTYTerrain pTerrain);

    /**
     * Set/Get de la liste des courbes de niveau.
     */
    TYTabCourbeNiveauGeoNode& getListCrbNiv() { return _listCrbNiv; }
    /**
     * Set/Get de la liste des courbes de niveau.
     */
    const TYTabCourbeNiveauGeoNode& getListCrbNiv() const { return _listCrbNiv; }
    /**
     * Set/Get de la liste des courbes de niveau.
     */
    void setListCrbNiv(const TYTabCourbeNiveauGeoNode& list) { _listCrbNiv = list; setIsGeometryModified(true); }
    /**
     * Ajoute un element a la liste des courbes de niveau.
     */
    bool addCrbNiv(LPTYCourbeNiveauGeoNode pCrbNivGeoNode);
    /**
     * Ajoute un element a la liste des courbes de niveau.
     */
    bool addCrbNiv(LPTYCourbeNiveau pCrbNiv);
    /**
     * Supprime une entree a la liste des courbes de niveau.
     */
    bool remCrbNiv(const LPTYCourbeNiveauGeoNode pCrbNivGeoNode);
    /**
     * Supprime une entree a la liste des courbes de niveau.
     */
    bool remCrbNiv(const LPTYCourbeNiveau pCrbNiv);
    /**
     * Supprime une entree a la liste des courbes de niveau
     * a partir de son identifiant.
     */
    bool remCrbNiv(QString idCrbNiv);
    /**
     * Suppression de tous les elements.
     */
    void remAllCrbNiv();
    /**
     * Retourne une courbe de niveau a partir de son index.
     */
    LPTYCourbeNiveau getCrbNiv(int index) { return TYCourbeNiveau::safeDownCast(_listCrbNiv.at(index)->getElement()); }
    /**
     * Retrouve le GeoNode associe a une courbe de niveau.
     *
     * @param pCrbNiv L'element a chercher.
     *
     * @return Le GeoNode associe a l'element a chercher si trouve sinon NULL.
     */
    LPTYCourbeNiveauGeoNode findCrbNiv(const LPTYCourbeNiveau pCrbNiv);

    /**
     * Set/Get du terrain par default.
     */
    int getDefTerrainIdx() { return _DefTerrainIdx; }
    void setDefTerrainIdx(const int& defTerrainIdx) { _DefTerrainIdx = defTerrainIdx; }
    /**
     * Set/Get du terrain par default.
     */
    TYTerrain* getDefTerrain();
    /**
     * Set/Get du terrain par default.
     */
    void setDefTerrain(int defTerrainIdx);

    /**
     * Set/Get de l'emprise de la topographie.
     */
    TYTabPoint& getEmprise() { return _emprise; }
    /**
     * Set/Get de l'emprise de la topographie.
     */
    const TYTabPoint& getEmprise() const { return _emprise; }
    /**
     * Set/Get de l'emprise de la topographie.
     */
    void setEmprise(const TYTabPoint& pts, const bool& defTerrain = true);
    /**
     * Ajoute un point pour definir l'emprise.
     */
    void addPointEmprise(TYPoint pt) { _emprise.push_back(pt); setIsGeometryModified(true); }
    /**
     * Retourne un point de l'emprise a partir de son index.
     */
    TYPoint& getPointEmprise(int index) { return _emprise[index]; }
    /**
     * Retourne un point de l'emprise a partir de son index.
     */
    const TYPoint& getPointEmprise(int index) const { return _emprise[index]; }
    /**
     * Reinitialise l'emprise.
     */
    void resetEmprise() { _emprise.clear(); setIsGeometryModified(true); }

    /**
     * Set/Get de l'altimetrie.
     */
    LPTYAltimetrie getAltimetrie() { return _pAltimetrie; }
    /**
     * Set/Get de l'altimetrie.
     */
    const LPTYAltimetrie getAltimetrie() const { return _pAltimetrie; }
    /**
     * Set/Get de l'altimetrie.
     */
    void setAltimetrie(const LPTYAltimetrie pAltimetrie)
    {
        _pAltimetrie = pAltimetrie;
        _pAltimetrie->setParent(this);
        setIsGeometryModified(true);
    }

    /**
     * Calcule l'altimetrie a partir des elements de topographie.
     */
    //  void computeAltimetrie();

    /**
     * Collecte l'ensemble des points necessaires a la generation
     * de l'altimetrie. Version 1D
     */
    //  TYTabPoint collectPointsForAltimetrie(const double& distanceMax, bool bEmpriseAsCrbNiv = false ) const;
    TYTabPoint collectPointsForAltimetrie(bool bEmpriseAsCrbNiv = false) const;

    /**
     * Calcule la pente moyenne pour le segment donne.
     *
     * @param seg Segment pour lequel on calcul la pente moyenne.
     * @param penteMoy Segment resultant representant la pente moyenne.
     *
     * @return <code>false</code> si le segment passe est valide
     *         (inclu dans la zone de travail); <code>false</code> sinon.
     */
    bool penteMoy(const OSegment3D& seg, OSegment3D& penteMoy) const;

    /**
     * Calcul du sol moyen pour un segment donne.
     *
     * @param seg Segment pour lequel on calcul le sol moyen.
     *
     * @return Le sol moyen calcule pour le segment donne.
     */
    LPTYTerrain solMoy(const OSegment3D& seg, const TYAtmosphere& atmo);

    /**
     * Recherche le terrain pour un point donne de la topographie.
     *
     * @param pt Point de la topographie.
     *
     * @return La reference du terrain pour le point passe.
     */
    TYTerrain* terrainAt(const OPoint3D& pt);

    /**
     * Retourne la liste des terrains traverses par un segment donne
     * et les sous_segments associes.
     *
     * @param seg Segment traversant la topographie.
     *
     * @return Un tableau de structure contenant les terrains et segments associes.
     */
    TYTabStructSegPtrTer sols(const OSegment3D& seg);

    /**
     * Hauteur du point passe par rapport a la pente moyenne.
     *
     * @param pt Point de la topographie.
     * @param seg Segment pour lequel on calcul la pente moyenne.
     * @param hauteur La hauteur resultante du point par rapport a la pente moyenne.
     *
     * @return <code>false</code> si le segment passe est valide
     *         (inclu dans la zone de travail); <code>false</code> sinon.
     */
    bool hauteurFromPenteMoy(const OPoint3D& pt, const OSegment3D& seg, double& hauteur) const;

    /**
     * Met a jour les caracteristique des sols definis dans la topo
     * Calcul du nombre d'onde dans le sol en fonction de l'atmosphere
     */
    void updateSol(const TYAtmosphere& atmo);

    /**
     * Retourne la dimension caracteristique de la topographie (diagonale de la boite englobante)
     */
    double getTopoSize(OSegment3D& segDiagonale);

    /// Tri des terrains par surface decroissante
    void sortTerrains();

    // Accessor a la couleur de l'emprise
    void setEmpriseColor(const OColor& color) { _empriseColor = color; };
    OColor getEmpriseColor() const { return _empriseColor; };

private :

    // Membres
protected:
    ///Liste des courbes de niveau.
    TYTabCourbeNiveauGeoNode _listCrbNiv;

    ///Altimetrie.
    LPTYAltimetrie _pAltimetrie;

    ///Terrain par default.
    int _DefTerrainIdx;

    ///Liste des terrains.
    TYTabTerrainGeoNode _listTerrain;

    ///Liste des cours d'eau.
    TYTabCoursEauGeoNode _listCrsEau;

    ///Liste des plans d'eau.
    TYTabPlanEauGeoNode _listPlanEau;

    ///Emprise.
    TYTabPoint _emprise;
    OColor _empriseColor;

    /// Liste des terrains tries (on fait expres un tableau a part pour ne pas perturber le terrain par defaut)
    LPTYTerrainGeoNode* _pSortedTerrains;

    /// Seuils confondus
    double _seuilConfondus;

private :
    std::vector<TYStructElemPts> _tabElemPts; // Tableau des terrains
    short _lastIndice;

};


///Noeud geometrique de type TYTopographie.
typedef TYGeometryNode TYTopographieGeoNode;
///Smart Pointer sur TYTopographieGeoNode.
typedef SmartPtr<TYTopographieGeoNode> LPTYTopographieGeoNode;
///Collection de noeuds geometriques de type TYTopographie.
typedef std::vector<LPTYTopographieGeoNode> TYTabTopographieGeoNode;


#endif // __TY_TOPOGRAPHIE__
