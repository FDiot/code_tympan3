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

#ifndef __TY_ACOUSTICINTERFACE__
#define __TY_ACOUSTICINTERFACE__


#include "Tympan/MetierSolver/DataManagerCore/TYElement.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSpectre.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYAttenuateur.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSourcePonctuelle.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYRegime.h"

/**
 * Classe abstraite permettant de constituer des elements acoustiques.
 */
class TYAcousticInterface
{
    // Methodes
public:
    /**
     * @name Construction et Destruction
     */
    //@{

    /**
     * Constructeur.
     */
    TYAcousticInterface();
    /**
     * Constructeur par copie.
     */
    TYAcousticInterface(const TYAcousticInterface& other);
    /**
     * Destructeur.
     */
    virtual ~TYAcousticInterface();

    //@}

    /**
     * @name Operateurs
     */
    //@{

    /**
     * Operateur de copie.
     */
    TYAcousticInterface& operator=(const TYAcousticInterface& other);
    /**
     * Operateur de comparaison.
     */
    bool operator==(const TYAcousticInterface& other) const;
    /**
     * Operateur de comparaison.
     */
    bool operator!=(const TYAcousticInterface& other) const;

    /**
     * Operateur de copie.
     */
    void deepCopy(const TYAcousticInterface* pOther, bool copyId = true);

    //@}

    /**
     * @name Interface XML
     */
    //@{

    /**
     * Construit une arborescence DOM a partir du noeud passe
     * afin de sauvegarder les parametres au format XML.
     *
     * @param domElement Le noeud DOM parent a partir duquel l'arborescence doit etre creee.
     *
     * @return  Le noeud DOM nouvellement cree.
     */
    DOM_Element toXML(DOM_Element& domElement);

    /**
     * Extrait les informations a partir du noeud DOM passe afin de
     * recuperer les parametres de la classe.
     *
     * @param domElement Le noeud DOM a partir duquel les parametres doivent etre recuperes.
     *
     * @return 1 si la recuperation des parametres a reussie.
     */
    int fromXML(DOM_Element domElement);

    //@}

    /**
     * @name Accesseurs
     */
    //@{

    /**
     * Set/Get de la densite horizontale de sources.
     */
    double getDensiteSrcsH() const { return _densiteSrcsH; }
    /**
     * Set/Get de la densite horizontale de sources.
     */
    virtual void setDensiteSrcsH(double densite, bool recursif = true);

    /**
     * Set/Get de la densite verticale de sources.
     */
    double getDensiteSrcsV() const { return _densiteSrcsV; }
    /**
     * Set/Get de la densite verticale de sources.
     */
    virtual void setDensiteSrcsV(double densite, bool recursif = true) ;

    /**
     * Set/Get de la densite surfacique de sources.
     */
    double getDensiteSrcs() const;
    /**
     * Set/Get de la densite surfacique de sources.
     */
    virtual void setDensiteSrcs(double densite, bool recursif = true);

    /**
     * Set/Get de l'etat rayonnant ou pas
     */
    bool getIsRayonnant() { return _isRayonnant; }

    /**
     * Set/Get de l'etat rayonnant ou pas
     */
    virtual void setIsRayonnant(bool rayonnant = true, bool recursif = true);

    /**
     * Set/Get du type de calcul pour la distribution de la puissance.
     */
    int getTypeDistribution() const { return _typeDistribution; }
    /**
     * Set/Get du type de calcul pour la distribution de la puissance.
     */
    void setTypeDistribution(int typeDistri);

    /**
     * Set/Get du test d'utilisation d'un attenuateur.
     */
    bool getUseAtt() const { return _useAtt; }
    /**
     * Set/Get du test d'utilisation d'un attenuateur.
     */
    virtual void setUseAtt(bool state);

    /**
     * Set/Get de l'attenuateur.
     */
    LPTYAttenuateur getAtt() { return _pAtt; }
    /**
     * Set/Get de l'attenuateur.
     */
    const LPTYAttenuateur getAtt() const { return _pAtt; }
    /**
     * Set/Get de l'attenuateur.
     */
    void setAtt(LPTYAttenuateur pAtt);

    //@}

    /**
     * @name Gestion des regimes
     */
    //@{

    /**
     * Recuperation du spectre courant
     */
    TYSpectre& getCurrentSpectre() { return _pCurrentSpectre; }

    /**
     * Associe un spectre a un regime donne.
     *
     * @param regime Le regime auquel on souhaite associe un spectre.
     * @param spectre Le spectre a associer.
     * @param recursif Pour que l'appel se fasse de maniere recursive.
     */
    virtual void setRegime(TYSpectre& Spectre, int regime = -1, bool recursif = false);

    /**
     * Ajout d'un regime dans le tableau des regimes
     */
    virtual int addRegime(const TYRegime& regime);

    /**
     * Ajout d'un regime dans le tableau des regimes
     */
    virtual int addRegime();

    /**
     * Supprime le regime specifie.
     *
     * @param regime L'indice du regime a supprimer.
     *
     * @return <code>true</code> si le regime a correctement ete supprime;
     *         <code>false</code> sinon.
     */
    virtual bool remRegime(int regime);

    /**
     * Chargement d'un regime (-1 = regime courant)
     */
    virtual void loadRegime(int regimeNb = -1);

    /**
     * Set/Get du nombre de regime de fonctionnement de cette source.
     */
    size_t getNbRegimes() const { return _tabRegimes.size(); }

    /**
     * Force le nombre de regimes
     */
    void setNbRegimes(unsigned int nb) { _tabRegimes.resize(nb); }

    /**
     * Retourne le regime dont le numero est fourni
     */
    TYRegime& getRegimeNb(const int& nb, bool& status);
    const TYRegime& getRegimeNb(const int& nb, bool& status) const;

    /**
     * Set/Get du numero de regime de fonctionnement courant.
     */
    int getCurRegime() const { return _curRegime; }

    /**
     * Set/Get du numero de regime de fonctionnement courant.
     */
    virtual void setCurRegime(int regimeNumber);

    /**
     * Propagation du regime
     * Cette methode peut etre surchargee pour propager un regime
     * a des sous-elements (notament des Volume vers les Surfaces).
     */
    virtual void propagateRegime();

    //@}

    /**
     * @name Interface Acoustique
     */
    //@{

    /**
     * Propagation de l'attenuateur, contrairement a setAtt(), c'est une
     * copie (deepCopy()) qui est faite ici.
     * Cette methode peut etre surchargee pour propager un attenuateur
     * a des sous-elements (notament des Volume vers les Surfaces).
     */
    virtual void propagateAtt(LPTYAttenuateur pAtt);

    /**
     * Retourne l'ensemble des sources ponctuelles dont est compose cet objet.
     * Chaque source se trouve dans un GeoNode, ainsi la matrice de celui-ci
     * permet d'effectuer le changement de repere de la position de la source
     * dans le repere local a cet objet.
     *
     * @return Une collection de pointeurs sur des GeoNodes de sources ponctuelles.
     */
    virtual TYTabSourcePonctuelleGeoNode getSrcs() const = 0;

    /**
     * Retourne une source ponctuelle equivalente a l'ensemble des sources.
     * <B>Cette methode est experimentale.</B>
     *
     * @return La source ponctuelle equivalente.
     */
    virtual TYSourcePonctuelle srcPonctEquiv() const = 0;

    /**
     * Calcul la repartition des sources ponctuelles pour cet element
     * en fonction des densites etablies.
     */
    virtual void distriSrcs() = 0;

    /**
     * Supprime toutes les sources d'un element
     */
    virtual void remAllSrcs() = 0;

    /**
     * Assigne une puissance a chaque source ponctuelle en fonction de la
     * puissance acoutique globale de cet objet.
     *
     * @return <code>true</code> si le calcul a ete possible;
     *         <code>false</code> sinon.
     */
    virtual bool setSrcsLw() = 0;

    /**
     * Execute les calculs acoustiques afin que l'element metier soit
     * a jour.
     */
    virtual bool updateAcoustic(const bool& force = false) = 0;

    /**
     * Indique au volume le spectre global de la machine (volumeNode) pour lui
     * permettre de calculer sa puissance (connaissant sa surface et la surface globale)
     * @param le spectre global de la machine
     * @return le spectre du volume
     */
    virtual TYSpectre setGlobalLW(const TYSpectre& spectre, const double& surfGlobale, const int& regime = -1);

    /**
     * Construction d'un regime a partir de l'etat courant
     */
    virtual TYRegime buildRegime();

    /**
     * Mise a jour du regime a partir de l'etat courant
     */
    virtual void updateCurrentRegime();

    /**
     * Indique si l'element peut changer de regime de lui meme
     */
    bool getIsRegimeChangeAble() { return _regimeChangeAble; }

    //@}

    /**
     * Get/Set du numero de regime suivant
     */
    virtual int getNextRegimeNb() { return _nextRegime; }
    virtual const int getNextRegimeNb() const { return _nextRegime;}
    virtual void setNextRegimeNb(const int& next) { _nextRegime = next; }

    /**
     * Get/Set du nom du regime
     */
    virtual QString getRegimeName();
    virtual const QString getRegimeName() const;
    virtual void setRegimeName(const QString& name);

    /// Export au format csv sur un flux transmis
    virtual void exportCSV(std::ofstream& ofs);

    /**
     * Les differents types de calcul pour la distribution de la puissance.
     */
    enum TypeDistribution
    {
        TY_PUISSANCE_CALCULEE,  /**< La puissance de cette element acoustique est calculee par le gestionnaire de donnees. */
        TY_PUISSANCE_IMPOSEE    /**< La puissance de cette element acoustique est imposee par l'utilisateur. */
    };


protected:
    /**
     * Verifie si le regime de fonctionnement existe.
     * Si regime = -1, c'est le regime courant qui est utilise.
     *
     * @param regime Le regime de fonctionnement concerne.
     *
     * @return Le regime de fonctionnement.
     */
    int checkRegimeNb(int regime) const;


    // Membres
protected:
    ///Densite horizontale de sources.
    double _densiteSrcsH;
    ///Densite verticale de sources.
    double _densiteSrcsV;

    /// Offset de decalage des sources ponctuelles sur les surfaces
    double _offsetSources;

    ///Etat courant de l'element (rayonnant ou non rayonnant)
    bool _isRayonnant;

    ///Indique si un element peux changer de regime de lui meme
    bool _regimeChangeAble;

    ///Indique le type de distribution a utiliser. pour l'etat courant
    int _typeDistribution;

    ///Numero du regime de fonctionnement courant.
    int _curRegime;

    /// Numero d'ordre du regime suivant
    int _nextRegime;

    // Spectre de l'etat courant
    TYSpectre _pCurrentSpectre;

    ///Indique l'utilisation d'un attenuateur pour l'etat courant.
    bool _useAtt;

    ///Attenuateur de l'etat courant.
    LPTYAttenuateur _pAtt;

    /// Tableau des regimes de fonctionnement
    TYTabRegimes _tabRegimes;

};


#endif // __TY_ACOUSTICINTERFACE__

