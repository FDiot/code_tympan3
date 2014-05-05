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

#ifndef __TY_ACOUSTICLINE__
#define __TY_ACOUSTICLINE__


#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYRegime.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSourceLineic.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYGeometryNode.h"
#include "TYAcousticInterface.h"

/**
* Permet de creer de elements acoustiques lineaires.
*/
class TYAcousticLine: public TYSource, public TYColorInterface
{
    OPROTOSUPERDECL(TYAcousticLine, TYSource)
    TY_EXTENSION_DECL_ONLY(TYAcousticLine)
    TY_EXT_GRAPHIC_DECL_ONLY(TYAcousticLine)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYAcousticLine();
    /**
     * Constructeur par copie.
     */
    TYAcousticLine(const TYAcousticLine& other);
    /**
     * Destructeur.
     */
    virtual ~TYAcousticLine();

    ///Operateur =.
    TYAcousticLine& operator=(const TYAcousticLine& other);
    ///Operateur ==.
    bool operator==(const TYAcousticLine& other) const;
    ///Operateur !=.
    bool operator!=(const TYAcousticLine& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    /**
     * Set/Get de la largeur.
     */
    double getLargeur() const { return _largeur; }
    /**
     * Set/Get de la largeur.
     */
    void setLargeur(double larg) { _largeur = larg; setIsGeometryModified(true); }

    /**
     * Set/Get de l'element source lineique.
     */
    LPTYSourceLineic getSrcLineic() { return _pSrcLineic; }
    /**
     * Set/Get de l'element source lineique.
     */
    const LPTYSourceLineic getSrcLineic() const { return _pSrcLineic; }
    /**
     * Set/Get de l'element source lineique.
     */
    void setSrcLineic(const LPTYSourceLineic pSrcLin) { _pSrcLineic = pSrcLin; _pSrcLineic->setParent(this); }

    /**
     * Set/Get du tableau de points.
     */
    TYTabPoint& getTabPoint() { return _tabPoint; }
    /**
     * Set/Get du tableau de points.
     */
    const TYTabPoint& getTabPoint() const { return _tabPoint; }
    /**
     * Set/Get du tableau de points.
     */
    void setTabPoint(const TYTabPoint& tab) { _tabPoint = tab; setIsGeometryModified(true); }

    /**
     * Creation des sources de la source lineique
     */
    virtual void distriSrcs(); // Rajoute pour avoir une interface commune avec les autres objets

    /**
     * Suppression des sources de la source lineique
     */
    virtual void remAllSrcs(); // Rajoute pour avoir une interface commune avec les autres objets

    /**
     * Mise a jour de l'acoustique de l'element
     */
    virtual bool updateAcoustic(const bool& force = false);

    /**
     * Calcul le spectre de puissance global.
     */
    virtual void computeSpectre();

    /**
     * Retourne l'ensemble des sources ponctuelles dont est compose cet objet.
     *
     * @return Une collection de pointeurs de source ponctuelle.
     */
    virtual TYTabSourcePonctuelleGeoNode getSrcs() const;

    /**
     * Affectation de la puissance aux sources ponctuelles
     */
    virtual bool setSrcsLw();

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
    virtual int addRegime(TYRegime regime);

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
     * Retourne le regime dont le numero est fourni
     */
    TYRegime& getRegimeNb(const int& nb) { return _tabRegimes[nb]; }
    const TYRegime& getRegimeNb(const int& nb) const { return _tabRegimes[nb]; }

    /**
     * Set/Get du nombre de regime de fonctionnement de cette source.
     */
    size_t getNbRegimes() const { return _tabRegimes.size(); }

    /**
     * Set/Get du numero de regime de fonctionnement courant.
     */
    int getCurRegime() const { return _curRegime; }

    /**
     * Set/Get du numero de regime de fonctionnement courant.
     */
    virtual void setCurRegime(int regimeNumber);

    /**
     * Construction d'un regime a partir de l'etat courant
     */
    virtual TYRegime buildRegime();

    /**
     * Chargement d'un regime (-1 = regime courant)
     */
    virtual void loadRegime(int regimeNb = -1);

    /**
     * Mise a jour du regime a partir de l'etat courant
     */
    virtual void updateCurrentRegime();

    /**
     * Retourne le nom du regime courant
     */
    QString getRegimeName() { return getRegimeNb(_curRegime).getName(); }
    void setRegimeName(const QString& name);

    /**
     * Get/Set de l'etat rayonnant
     */
    bool getIsRayonnant() { return _isRayonnant; }
    const bool getIsRayonnant() const { return _isRayonnant; }

    /**
     * Set/Get du type de calcul pour la distribution de la puissance.
     */
    int getTypeDistribution() const { return _typeDistribution; }

    /**
     * Set/Get du type de calcul pour la distribution de la puissance.
     */
    void setTypeDistribution(int typeDistri);

    /**
     * Get/Set de l'etat rayonnant
     */
    void setIsRayonnant(const bool& rayonnant) { _isRayonnant = rayonnant; }

    /**
     * Get/Set de la prise en compte d'un attenuateur
     */
    bool getUseAtt() { return _useAtt; }
    const bool getUseAtt() const { return _useAtt; }
    /**
     * Get/Set de la prise en compte d'un attenuateur
     */
    void setUseAtt(const bool& useAtt) { _useAtt = useAtt; }

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

    /**
     * Get/Set du spectre de la source lineique
     */
    TYSpectre* getSpectre(int regime = -1) const { return _pSpectre; }

    /**
     * Indique si l'element peut changer de regime de lui meme
     */
    bool getIsRegimeChangeAble() { return _regimeChangeAble; }

    /**
     * Get du spectre de puissance avec son attenuateur eventuel
     */
    virtual LPTYSpectre getRealPowerSpectrum();

    /// Export au format csv sur un flux transmis
    void exportCSV(std::ofstream& ofs);

    /**
     * Les differents types de calcul pour la distribution de la puissance.
     */
    enum TypeDistribution
    {
        TY_PUISSANCE_CALCULEE,  /**< La puissance de cette element acoustique est calculee par le gestionnaire de donnees. */
        TY_PUISSANCE_IMPOSEE    /**< La puissance de cette element acoustique est imposee par l'utilisateur. */
    };


    // Membres
protected:
    /// Regime de fonctionnement courant
    int _curRegime;

    /// Mise en fonctionnement
    bool _isRayonnant;

    /// Utilisation d'un attenuateur
    bool _useAtt;

    /// Attenuateur associe a l'element;
    LPTYAttenuateur _pAtt;

    ///Indique le type de distribution a utiliser. pour l'etat courant
    int _typeDistribution;

    ///Indique si on peut ajouter un regime a un element (cas des routes)
    bool _regimeChangeAble;

    /// Numero d'ordre du regime suivant
    int _nextRegime;

    ///Largeur.
    double _largeur;

    ///Source Lineique.
    LPTYSourceLineic _pSrcLineic;

    ///Tableau des points.
    TYTabPoint _tabPoint;

    /// Tableau des regimes
    TYTabRegimes _tabRegimes;
};


///Noeud geometrique de type TYAcousticLine.
typedef TYGeometryNode TYAcousticLineGeoNode;
///Smart Pointer sur TYAcousticLineGeoNode.
typedef SmartPtr<TYAcousticLineGeoNode> LPTYAcousticLineGeoNode;
///Collection de noeuds geometriques de type TYAcousticLine.
typedef std::vector<LPTYAcousticLineGeoNode> TYTabAcousticLineGeoNode;


#endif // __TY_ACOUSTICLINE__
