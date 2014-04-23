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

#ifndef __TY_USER_SOURCEPONCTUELLE__
#define __TY_USER_SOURCEPONCTUELLE__


#include "TYSourcePonctuelle.h"
#include "TYUserSrcRegime.h"


/**
 * Classe de definition d'une source ponctuelle.
 */
class TYUserSourcePonctuelle: public TYSourcePonctuelle
{
    OPROTOSUPERDECL(TYUserSourcePonctuelle, TYSourcePonctuelle)
    TY_EXTENSION_DECL_ONLY(TYUserSourcePonctuelle)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYUserSourcePonctuelle();
    /**
     * Constructeur par copie.
     */
    TYUserSourcePonctuelle(const TYUserSourcePonctuelle& other);

    /**
     * Construction a partir d'une source ponctuelle
     */
    TYUserSourcePonctuelle(const TYSourcePonctuelle* other);

    /**
     * Destructeur.
     */
    virtual ~TYUserSourcePonctuelle();

    ///Operateur =.
    TYUserSourcePonctuelle& operator=(const TYUserSourcePonctuelle& other);
    ///Operateur ==.
    bool operator==(const TYUserSourcePonctuelle& other) const;
    ///Operateur !=.
    bool operator!=(const TYUserSourcePonctuelle& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    /**
     * Set/Get de la directivite.
     */
    void setDirectivite(const TYDirectivite& dir, int regime = -1) { _pDirectivite = new TYDirectivite(dir); }

    /**
     * Set/Get de l'attenuateur.
     */
    LPTYAttenuateur getAttenuateur() { return _pAttenuateur; }

    /**
     * Set/Get de l'attenuateur.
     */
    const LPTYAttenuateur getAttenuateur() const { return _pAttenuateur; }

    /**
     * Set/Get de l'attenuateur.
     */
    void setAttenuateur(const LPTYAttenuateur pAtt) { _pAttenuateur = pAtt; }

    /**
     * Set/Get du spectre (integrant attenuateur)
     */
    virtual TYSpectre* getSpectre(int regime = -1) const;

    /**
     * Get du spectre de puissance avec son attenuateur eventuel
     */
    LPTYSpectre getRealPowerSpectrum();

    /**
     * Set/Get du spectre.
     */
    virtual TYSpectre* getCurrentSpectre() const;

    /**
     * Ajout d'un regime
     */
    virtual int addRegime(TYUserSrcRegime regime);

    /**
     * Suppression d'un regime
     */
    virtual bool remRegime(int regimeNumber);

    /**
     * Construction d'un regime a partir de l'etat courant
     */
    virtual TYUserSrcRegime buildRegime();

    /**
     * Mise a jour d'un regime
     */
    virtual void updateCurrentRegime();

    /**
     * Chargement d'un regime (-1 = regime courant)
     */
    virtual void loadRegime(int regimeNb = -1);

    /**
     * Get/Set du regime courant
     */
    void setCurrentRegime(const int& regimeNumber);

    /**
     * Get/Set du regime courant
     */
    int getCurrentRegime() { return _curRegime; }
    const int getCurrentRegime() const { return _curRegime; }

    /**
     * Retourne le regime dont le numero est fourni
     */
    TYUserSrcRegime& getRegimeNb(const int& nb);
    const TYUserSrcRegime& getRegimeNb(const int& nb) const;

    /**
     * Retourne le nombre de regimes de la source
     */
    size_t getNbRegimes() { return _tabRegimes.size(); }
    const size_t getNbRegimes() const { return _tabRegimes.size(); }

    /**
     * Get/Set du nom du regime courant
     */
    QString getRegimeName() { return getRegimeNb(_curRegime).getName(); }
    void setRegimeName(const QString& name);

    /**
     * Calcul le spectre de puissance acoustique apparente dans la direction du
     * segment donne pour le regime donne.
     */
    virtual OSpectre lwApparenteSrcDest(const OSegment3D& seg, const TYAtmosphere& Atmo, const int& expGeo = 0, const int& regime = -1) const;

    /**
     * Get/Set de l'etat rayonnant
     */
    void setIsRayonnant(const bool& rayonnant) { _isRayonnant = rayonnant; }

    /**
     * Get/Set de l'etat rayonnant
     */
    bool getIsRayonnant() { return _isRayonnant; }
    const bool getIsRayonnant() const { return _isRayonnant; }

    /**
     * Get/Set de la prise en compte d'un attenuateur
     */
    bool getUseAtt() { return _useAtt; }
    const bool getUseAtt() const { return _useAtt; }

    /**
     * Get/Set de la prise en compte d'un attenuateur
     */
    void setUseAtt(const bool& useAtt) { _useAtt = useAtt; }

    /// Export au format csv sur un flux transmis
    void exportCSV(std::ofstream& ofs);


private:

    // Membres
protected:

    /// Utilisation d'un attenuateur
    bool _useAtt;

    /// Source rayonnante
    bool _isRayonnant;

    /// Directivite associee a la source
    LPTYDirectivite _pDirectivite;

    ///Attenuateur lie a cette source.
    LPTYAttenuateur _pAttenuateur;

    ///Numero du regime de fonctionnement courant.
    int _curRegime;

    /// Numero d'ordre du regime suivant
    int _nextRegime;

    ///Tableau de regimes de fonctionnement
    TYTabUserSrcRegimes _tabRegimes;
};

/// Smart pointeur sur un source ponctuelle utilisateur
typedef SmartPtr<TYUserSourcePonctuelle> LPTYUserSourcePonctuelle;
///Noeud geometrique de type TYSourcePonctuelle.
typedef TYGeometryNode TYUserSourcePonctuelleGeoNode;
///Smart Pointer sur TYSourcePonctuelleGeoNode.
typedef SmartPtr<TYUserSourcePonctuelleGeoNode> LPTYUserSourcePonctuelleGeoNode;
///Collection de noeuds geometriques de type TYSourcePonctuelle.
typedef std::vector<LPTYUserSourcePonctuelleGeoNode> TYTabUserSourcePonctuelleGeoNode;

#endif // __TY_USER_SOURCEPONCTUELLE__
