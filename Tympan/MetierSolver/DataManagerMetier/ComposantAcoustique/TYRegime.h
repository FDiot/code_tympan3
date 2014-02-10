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

#ifndef __TY_REGIME__
#define __TY_REGIME__


#include "Tympan/MetierSolver/DataManagerCore/TYElement.h"
#include "TYSpectre.h"
#include "TYAttenuateur.h"


#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerIHM/TYRegimeWidget.h"
#endif
//*/



/**
 * Classe pour definir les differents regimes d'un element du calcul.
 *
 * @author Projet_Tympan
 *
 */
class TYRegime : public TYElement
{
    OPROTOSUPERDECL(TYRegime, TYElement)
    TY_EXTENSION_DECL(TYRegime)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYRegime();
    /**
     * Constructeur par copie.
     */
    TYRegime(const TYRegime& other);
    /**
     * Destructeur.
     */
    virtual ~TYRegime();

    ///Operateur =.
    TYRegime& operator=(const TYRegime& other);
    ///Operateur ==.
    bool operator==(const TYRegime& other) const;
    ///Operateur !=.
    bool operator!=(const TYRegime& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    /**
     * Get/Set du nom du regime
     */
    void setRegimeName(const QString& nom) { setName(nom); }

    /**
     * Get/Set du nom du regime
     */
    QString getRegimeName() { return getName(); }
    const QString getRegimeName() const { return getName(); }

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
    void setAtt(LPTYAttenuateur pAtt) { _pAtt = pAtt; }

    /**
     * Get/Set du spectre associe au regime
     */
    TYSpectre getSpectre() { return _spectre; }

    /**
     * Get/Set du spectre associe au regime
     */
    const TYSpectre getSpectre() const { return _spectre; }

    /**
     * Get/Set du spectre associe au regime
     */
    void setSpectre(const TYSpectre& spectre) { _spectre = spectre; }

    /**
     * Les differents types de calcul pour la distribution de la puissance.
     */
    enum TypeDistribution
    {
        TY_PUISSANCE_CALCULEE,  /**< La puissance de cette element acoustique est calculee par le gestionnaire de donnees. */
        TY_PUISSANCE_IMPOSEE    /**< La puissance de cette element acoustique est imposee par l'utilisateur. */
    };

    // Membres
public:
    //  string          _nom;               // Nom du regime
    bool            _isRayonnant;       // Etat de l'element
    bool            _useAtt;            // Utilisation d'un attenuateur
    int             _typeDistribution;  // Puissance imposee ou calculee;
    TYSpectre       _spectre;           // Spectre associe au regime
    LPTYAttenuateur _pAtt;              // Attenuateur eventuellement associe au regime

protected:
};

///Tableau de regimes.
typedef std::vector<TYRegime> TYTabRegimes;

#endif // __TY_REGIME__
