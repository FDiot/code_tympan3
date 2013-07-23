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

#ifndef __TY_INFRASTRUCTURE__
#define __TY_INFRASTRUCTURE__


#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYBatiment.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYReseauTransport.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYRoute.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYRouteDeblai.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYUserSourcePonctuelle.h"

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerIHM/TYInfrastructureWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYInfrastructureGraphic.h"
#endif


/**
 * Centralise les elements d'infrastructure.
 *
 * @author Projet_Tympan
 *
 */
class TYInfrastructure: public TYElement
{
    OPROTOSUPERDECL(TYInfrastructure, TYElement)
    TY_EXTENSION_DECL(TYInfrastructure)
    TY_EXT_GRAPHIC_DECL(TYInfrastructure)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYInfrastructure();
    /**
     * Constructeur par copie.
     */
    TYInfrastructure(const TYInfrastructure& other);
    /**
     * Destructeur.
     */
    virtual ~TYInfrastructure();

    ///Operateur =.
    TYInfrastructure& operator=(const TYInfrastructure& other);
    ///Operateur ==.
    bool operator==(const TYInfrastructure& other) const;
    ///Operateur !=.
    bool operator!=(const TYInfrastructure& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    virtual void getChilds(TYElementCollection& childs, bool recursif = true);

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
     * Retourne le projet utilisant cette infrastructure
     * (utilise pour gerer la suppression de machines dans un calcul);
     */
    TYProjet* getProjet();

    /**
     * Reattribut le bon parent aux composants de cette infrastructure.
     */
    virtual void reparent();

    /**
     * Vide toutes les listes d'elements composants cet objet.
     */
    void purge();

    /**
     * Set/Get de la liste des routes.
     */
    TYTabRouteGeoNode& getListRoute() { return _listRoute; }

    /**
     * Set/Get de la liste des routes.
     */
    const TYTabRouteGeoNode& getListRoute() const { return _listRoute; }

    /**
     * Set/Get de la liste des routes.
     */
    void setListRoute(const TYTabRouteGeoNode& list) { _listRoute = list; }

    /**
     * Concatene 2 infrastructures
     */
    void concatInfra(const TYInfrastructure* infra);

    /**
     * Ajoute une entree a la liste des routes.
     */
    bool addRoute(LPTYRouteGeoNode pRouteGeoNode);

    /**
     * Ajoute une entree a la liste des routes.
     */
    bool addRoute(LPTYRoute pRoute);

    /**
     * Ajoute une entree a la liste des routes.
     */
    bool addRoute(LPTYRouteDeblai pRoute);

    /**
     * Supprime une entree a la liste des routes.
     */
    bool remRoute(const LPTYRouteGeoNode pRouteGeoNode);

    /**
     * Supprime une entree a la liste des routes.
     */
    bool remRoute(const LPTYRoute pRoute);

    /**
     * Supprime une entree a la liste des routes
     * a partir de son identifiant.
     */
    bool remRoute(QString idRoute);

    /**
     * Suppression de tous les elements.
     */
    void remAllRoute();

    /**
     * Retourne une route a partir de son index.
     */
    LPTYRoute getRoute(int index) { return TYRoute::safeDownCast(_listRoute.at(index)->getElement()); }

    /**
     * Retrouve le GeoNode associe a une route.
     *
     * @param pRoute L'element a chercher.
     *
     * @return Le GeoNode associe a l'element a chercher si trouve sinon NULL.
     */
    LPTYRouteGeoNode findRoute(const LPTYRoute pRoute);

    /**
     * Set/Get de la liste des reseaux de transport.
     */
    TYTabReseauTransportGeoNode& getListResTrans() { return _listResTrans; }

    /**
     * Set/Get de la liste des reseaux de transport.
     */
    const TYTabReseauTransportGeoNode& getListResTrans() const { return _listResTrans; }

    /**
     * Set/Get de la liste des reseaux de transport.
     */
    void setListResTrans(const TYTabReseauTransportGeoNode& list) { _listResTrans = list; }

    /**
     * Ajoute une entree a la liste des reseaux de transport.
     */
    bool addResTrans(LPTYReseauTransportGeoNode pResTransGeoNode);

    /**
     * Ajoute une entree a la liste des reseaux de transport.
     */
    bool addResTrans(LPTYReseauTransport pResTrans);

    /**
     * Supprime une entree a la liste des reseaux de transport.
     */
    bool remResTrans(const LPTYReseauTransportGeoNode pResTransGeoNode);

    /**
     * Supprime une entree a la liste des reseaux de transport.
     */
    bool remResTrans(const LPTYReseauTransport pResTrans);

    /**
     * Supprime une entree a la liste des reseaux de transport
     * a partir de son identifiant.
     */
    bool remResTrans(QString idResTrans);

    /**
     * Suppression de tous les elements.
     */
    void remAllResTrans();

    /**
     * Retourne un reseau de transport a partir de son index.
     */
    LPTYReseauTransport getResTrans(int index) { return TYReseauTransport::safeDownCast(_listResTrans.at(index)->getElement()); }

    /**
     * Retrouve le GeoNode associe a un reseau de transport.
     *
     * @param pResTrans L'element a chercher.
     *
     * @return Le GeoNode associe a l'element a chercher si trouve sinon NULL.
     */
    LPTYReseauTransportGeoNode findResTrans(const LPTYReseauTransport pResTrans);

    /**
     * Set/Get de la liste des batiments.
     */
    TYTabBatimentGeoNode& getListBatiment() { return _listBatiment; }

    /**
     * Set/Get de la liste des batiments.
     */
    const TYTabBatimentGeoNode& getListBatiment() const { return _listBatiment; }

    /**
     * Set/Get de la liste des batiments.
     */
    void setListBatiment(const TYTabBatimentGeoNode& list) { _listBatiment = list; }

    /**
     * Ajoute une entree a la liste des batiments et la position associee.
     */
    bool addBatiment(LPTYBatimentGeoNode pBatimentGeoNode);

    /**
     * Ajoute une entree a la liste des batiments et la position associee.
     */
    bool addBatiment(LPTYBatiment pBatiment, const TYRepere& pos);

    /**
     * Supprime une entree a la liste des batiments.
     */
    bool remBatiment(const LPTYBatimentGeoNode pBatimentGeoNode);

    /**
     * Supprime une entree a la liste des batiments.
     */
    bool remBatiment(const LPTYBatiment pBatiment);

    /**
     * Supprime une entree a la liste des batiments
     * a partir de son identifiant.
     */
    bool remBatiment(QString idBatiment);

    /**
     * Suppression de tous les elements.
     */
    void remAllBatiment();

    /**
     * Retourne un bâtiment a partir de son index.
     */
    LPTYBatimentGeoNode getBatiment(int index) { return _listBatiment.at(index); }

    /**
     * Retrouve le GeoNode associe a un bâtiment.
     *
     * @param pBatiment L'element a chercher.
     *
     * @return Le GeoNode associe a l'element a chercher si trouve sinon NULL.
     */
    LPTYBatimentGeoNode findBatiment(const LPTYBatiment pBatiment);

    /**
     * Set/Get de la liste des machines.
     */
    TYTabMachineGeoNode& getListMachine() { return _listMachine; }

    /**
     * Set/Get de la liste des machines.
     */
    const TYTabMachineGeoNode& getListMachine() const { return _listMachine; }

    /**
     * Set/Get de la liste des machines.
     */
    void setListMachine(const TYTabMachineGeoNode& list) { _listMachine = list; }

    /**
     * Ajoute une entree a la liste des machines et la position associee.
     */
    bool addMachine(LPTYMachineGeoNode pMachineGeoNode);

    /**
     * Ajoute une entree a la liste des machines et la position associee.
     */
    bool addMachine(LPTYMachine pMachine, const TYRepere& pos);

    /**
     * Supprime une entree a la liste des machines.
     */
    bool remMachine(const LPTYMachine pMachine);

    /**
     * Supprime une entree a la liste des machines.
     */
    bool remMachine(const LPTYMachineGeoNode pMachineGeoNode);

    /**
     * Supprime une entree a la liste des machines
     * a partir de son identifiant.
     */
    bool remMachine(QString idMachine);

    /**
     * Suppression de tous les elements.
     */
    void remAllMachine();

    /**
     * Retourne une machine a partir de son index.
     */
    LPTYMachineGeoNode getMachine(int index) { return _listMachine.at(index); }

    /**
     * Retrouve le GeoNode associe a une machine.
     *
     * @param pMachine L'element a chercher.
     *
     * @return Le GeoNode associe a l'element a chercher si trouve sinon NULL.
     */
    LPTYMachineGeoNode findMachine(const LPTYMachine pMachine);

    /**
     * Set/Get de la liste des sources ponctuelles.
     */
    TYTabUserSourcePonctuelleGeoNode& getSrcs() { return _listSrc; }

    /**
     * Set/Get de la liste des sources ponctuelles.
     */
    const TYTabUserSourcePonctuelleGeoNode& getSrcs() const { return _listSrc; }

    /**
     * Set/Get de la liste des sources ponctuelles.
     */
    void setSrcs(const TYTabUserSourcePonctuelleGeoNode& list) { _listSrc = list; }

    /**
     * Ajoute une entree a la liste des sources ponctuelles.
     */
    bool addSrc(LPTYUserSourcePonctuelle pSrc);

    /**
     * Ajoute une entree a la liste des sources ponctuelles.
     */
    bool addSrc(LPTYUserSourcePonctuelleGeoNode pSrcGeoNode);

    /**
     * Supprime une entree a la liste des sources ponctuelles.
     */
    bool remSrc(const LPTYUserSourcePonctuelle pSrc);

    /**
     * Supprime une entree a la liste des sources ponctuelles.
     */
    bool remSrc(const LPTYUserSourcePonctuelleGeoNode pSrcGeoNode);

    /**
     * Supprime une entree a la liste des sources ponctuelles
     * a partir de son identifiant.
     */
    bool remSrc(QString idSrc);

    /**
     * Suppression de tous les elements.
     */
    void remAllSrc();

    /**
     * Retourne une source a partir de son index.
     */
    LPTYUserSourcePonctuelleGeoNode getSrc(int index) { return _listSrc.at(index); }

    /**
     * Retrouve le GeoNode associe a une source ponctuelle.
     *
     * @param pSrc L'element a chercher.
     *
     * @return Le GeoNode associe a l'element a chercher si trouve sinon NULL.
     */
    LPTYUserSourcePonctuelleGeoNode findSrc(const LPTYUserSourcePonctuelle pSrc);

    /**
     * Retourne la liste de toutes les sources de l'infrastructure.
     *
     * @return Un tableau de pointeurs de GeoNodes de sources ponctuelles.
     */
    void getAllSrcs(TYTabSourcePonctuelleGeoNode& tab);

    /**
     * Retourne la liste de toutes les sources de l'infrastructure actives dansle calcul.
     *
     * @return Un tableau de pointeurs de GeoNodes de sources ponctuelles.
     */
    void getAllSrcs(const TYCalcul* pCalcul, TYTabSourcePonctuelleGeoNode& tab);

    /**
     * Retourne un tableau associatif des sources et de leur machine support
     */
    void getAllSrcs(const TYCalcul* pCalcul, TYMapElementTabSources& mapElementSrcs);

    /**
     * Met a jour l'acoustique des elements d'infrastructure
     */
    bool updateAcoustic(const TYCalcul* pCalcul, const bool& force = false);

    /**
     * Vide les tableaux sans mettre a jour les calculs
     */
    void clean();

    /**
     * Get du tableau des elements qui n'ont pu etre mis a jour
     */
    std::vector<LPTYElement> getTabElemNOk() { return _tabUpdateNOk; }


    /**
     * Ajoute le nouvel element au calcul
     */
    bool addToCalcul(TYGeometryNode* pNode);

    /**
     * Ajoute les elements de l'infrastructure au calcul
     */
    bool addToCalcul();

    /**
     * Supprime les element de l'infrastructure du calcul
     */
    bool remFromCalcul();

    // Membres
protected:
    ///Liste des routes.
    TYTabRouteGeoNode _listRoute;

    ///Liste des reseaux de transport.
    TYTabReseauTransportGeoNode _listResTrans;

    ///Liste des batiments.
    TYTabBatimentGeoNode _listBatiment;

    ///Liste des machines.
    TYTabMachineGeoNode _listMachine;

    ///Liste des sources ponctuelles.
    TYTabUserSourcePonctuelleGeoNode _listSrc;

    /// Liste des elements dont l'update a echoue
    std::vector<LPTYElement> _tabUpdateNOk;
};


///Noeud geometrique de type TYInfrastructure.
typedef TYGeometryNode TYInfrastructureGeoNode;
///Smart Pointer sur TYInfrastructureGeoNode.
typedef SmartPtr<TYInfrastructureGeoNode> LPTYInfrastructureGeoNode;
///Collection de noeuds geometriques de type TYInfrastructure.
typedef std::vector<LPTYInfrastructureGeoNode> TYTabInfrastructureGeoNode;


#endif // __TY_INFRASTRUCTURE__
