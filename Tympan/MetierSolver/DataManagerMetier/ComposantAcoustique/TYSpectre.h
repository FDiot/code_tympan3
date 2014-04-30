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

#ifndef __TY_SPECTRE__
#define __TY_SPECTRE__

#include "Tympan/MetierSolver/DataManagerCore/TYElement.h"
#include "Tympan/MetierSolver/ToolsMetier/OSpectreComplex.h"

///Collection des frequences
typedef std::vector<float> TYTabFreq;

/**
 * Permet de stocker des valeurs de puissance accoustique pour differentes frequences.
 */
class TYSpectre: public TYElement, public OSpectre
{

    OPROTOSUPERDECL(TYSpectre, TYElement)
    TY_EXTENSION_DECL_ONLY(TYSpectre)


    // Methodes
public:
    /**
     * Constructeur par defaut.
     */
    TYSpectre(const double& defaultValue = TY_SPECTRE_DEFAULT_VALUE);

    /**
     * Constructeur a partir d'un OSpectre
     */
    TYSpectre(const OSpectre& spectre);

    /// Constructeur a partir d'un OSpectreComplex
    TYSpectre(const OSpectreComplex& spectre);

    /**
     * Constructeur par copie.
     */
    TYSpectre(const TYSpectre& other);

    /**
     * Destructeur.
     */
    virtual ~TYSpectre();

    /// Operateur d'affectation
    virtual TYSpectre& operator= (const TYSpectre& other);

    /// Operateur d'affectation depuis un OSpectre
    virtual TYSpectre& operator= (const OSpectre& other);

    /// Operateur d'egalite
    virtual bool operator== (const TYSpectre& other) const;

    /// Operateur d'inegalite
    virtual bool operator != (const TYSpectre& otehr) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    //  void copyFromStruct(spectre a, MysqlQuery *query);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    virtual bool toXML(const std::string& sFilePath);
    virtual bool fromXML(const std::string& sFilePath);

    virtual std::string toXMLString();
    virtual bool fromXMLString(const std::string& sXMLString);

    /// Set/Get de la forme du spectre.
    virtual TYSpectreForm getForm() {return _form;}
    virtual void setForm(const TYSpectreForm& form) { _form = form; }

    /// Set/Get des remarques.
    virtual QString getRemarque() const { return _remarque; }

    /// Set/Get des remarques.
    virtual void setRemarque(QString rq) { _remarque = rq; }

    /// Set/Get du flag de conservation en BDD.
    //  bool getKeepInBDD() { return _keepInBDD; }

    /// Set/Get du flag de conservation en BDD.
    //  void setKeepInBDD(bool flag) { _keepInBDD = flag; }

    /// Set/Get du flag _isReadOnly.
    virtual bool getIsReadOnly() { return _isReadOnly; }

    /// Set/Get du flag _isReadOnly.
    virtual void setIsReadOnly(bool flag) { _isReadOnly = flag; }

    /// Export du spectre au format csv en creant le fichier
    virtual void exportCSV(const std::string& filename);

    /// Export du spectre au format csv sur un flux transmis
    virtual void exportCSV(std::ofstream& ofs);

    /// Cree un spectre en lin
    static TYSpectre getEmptyLinSpectre(const double& valInit = 1.0E-20);

    /// Construit un spectre de ponderation A.
    static TYSpectre pondA();

    /// Construit un spectre de ponderation B.
    static TYSpectre pondB();

    /// Construit un spectre de ponderation C
    static TYSpectre pondC();

    /**
     * Retourne un spectre representant la longueur d'onde associee a chaque frequence
     * @param double Vitesse de propagation de l'onde
     * @return un TYSpectre
     */
    static TYSpectre getLambda(const double& c);


    // ======= METHODES STATIQUES

    /// Get/Set de la precision de stockage des resultats (XML)
    static void setXMLPrecision(const int precision = 1) { _XMLprecision = precision; }

    /// Get/Set de la precision de stockage des resultats (XML)
    static int getXMLPrecision() { return _XMLprecision; }

    /**
     * Retourne le tableau des frequences normalisees.
     * L'appelant ne doit pas liberer la memoire allouee pour
     * creer ce tableau.
     *
     * @return Le tableau des frequences normalisees.
     */
    static const TYTabFreq getTabFreqNorm(TYSpectreForm form = SPECTRE_FORM_TIERS);

    /**
     * Retourne la frequence correspondant a l'index freqIndex.
     **/
    static const float getFreqNorm(int freqIndex);


    // Membres
public:

protected:

    // ==== MEMBRES STATIQUES

    /// Precision de stockage des resultats
    static int _XMLprecision;

    ///Indique si la widget du spectre est read-only
    bool _isReadOnly;

    ///Remarques.
    QString _remarque;
};

#endif // __TY_SPECTRE__
