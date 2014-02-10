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

#ifndef __TY_SOURCELINEIC__
#define __TY_SOURCELINEIC__

#include "TYSource.h"
#include "TYSourcePonctuelle.h"

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerIHM/TYSourceLineicWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYSourceLineicGraphic.h"
#endif


/**
 * Classe de definition d'une source lineique.
 */
class TYSourceLineic: public TYSource
{
    OPROTOSUPERDECL(TYSourceLineic, TYSource)
    TY_EXTENSION_DECL(TYSourceLineic)
    TY_EXT_GRAPHIC_DECL(TYSourceLineic)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYSourceLineic();
    /**
     * Constructeur par copie.
     */
    TYSourceLineic(const TYSourceLineic& other);
    /**
     * Destructeur.
     */
    virtual ~TYSourceLineic();

    ///Operateur =.
    TYSourceLineic& operator=(const TYSourceLineic& other);
    ///Operateur ==.
    bool operator==(const TYSourceLineic& other) const;
    ///Operateur !=.
    bool operator!=(const TYSourceLineic& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    /**
     * Vide le tableau de sources ponctuelles.
     */
    void purge() { remAllSrc(); }

    /**
     * Set/Get de la densite de sources ponctuelles.
     */
    double  getDensiteSrcs() const { return _densiteSrcs; }
    /**
     * Set/Get de la densite de sources ponctuelles.
     */
    void setDensiteSrcs(const double& densite) { _densiteSrcs = densite; }

    /**
     * Set/Get de l'indicateur de correlation.
     */
    bool getSrcsCorr() const { return _srcsCorr; }
    /**
     * Set/Get de l'indicateur de correlation.
     */
    void setSrcsCorr(bool corr) { _srcsCorr = corr; }

    /**
     * Get du nombre de sources ponctuelles.
     */
    size_t getNbSrcs() const { return _listSrcPonct.size(); }

    /**
     * Ajoute une source ponctuelle au tableau.
     *
     * @param pSrcPonct Une source ponctuelle a ajouter au tableau.
     *
     * @return <code>true</code> si la source a pu etre ajoutee au tableau;
     *         <code>false</code> sinon.
     */
    bool addSrc(LPTYSourcePonctuelle pSrcPonct);

    /**
     * Suppression d'une source ponctuelle au tableau.
     *
     * @param pSrcPonct La source a supprimer.
     *
     * @return <code>true</code> si la source a ete trouvee et supprimee;
     *         <code>false</code> sinon.
     */
    bool remSrc(const LPTYSourcePonctuelle pSrcPonct);

    /**
     * Suppression d'une source ponctuelle au tableau
     * a partir de son identifiant.
     *
     * @param idSrc Identifiant de la source a supprimer.
     *
     * @return <code>true</code> si la source a ete trouvee et supprimee;
     *         <code>false</code> sinon.
     */
    bool remSrc(QString idSrc);

    /**
     * Vide le tableau de sources ponctuelles.
     */
    void remAllSrc();

    /**
     * Retourne le pointeur d'une source ponctuelle par son index.
     */
    LPTYSourcePonctuelle getSrc(int index) { return _listSrcPonct.at(index); }
    /**
     * Retourne le pointeur d'une source ponctuelle par son index.
     */
    const LPTYSourcePonctuelle getSrc(int index) const { return _listSrcPonct.at(index); }

    /**
     * Set de l'ensemble des sources ponctuelles dont est compose cet objet.
     *
     * @param listSrcPonct Une collection de pointeurs de source ponctuelle.
     */
    void setSrcs(const TYTabLPSourcePonctuelle& listSrcPonct) { _listSrcPonct = listSrcPonct; }
    /**
     * Retourne l'ensemble des sources ponctuelles dont est compose cet objet.
     *
     * @return Une collection de pointeurs de source ponctuelle.
     */
    TYTabLPSourcePonctuelle& getSrcs() { return _listSrcPonct; }
    /**
     * Retourne l'ensemble des sources ponctuelles dont est compose cet objet.
     *
     * @return Une collection de pointeurs de source ponctuelle.
     */
    const TYTabLPSourcePonctuelle& getSrcs() const { return _listSrcPonct; }

    // Membres
protected:
    ///Densite des sources ponctuelles (exprimee en sources/metre).
    double _densiteSrcs;

    ///Indiquateur de correlation des sources ponctuelles.
    bool _srcsCorr;

    ///Liste des sources ponctuelles.
    TYTabLPSourcePonctuelle _listSrcPonct;
};


#endif // __TY_SOURCELINEIC__
