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

#ifndef __TY_SPECTRE__
#define __TY_SPECTRE__

#include "Tympan/MetierSolver/DataManagerCore/TYElement.h"
#include "Tympan/MetierSolver/ToolsMetier/OSpectreComplex.h"

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerIHM/TYSpectreWidget.h"
#endif

///Collection des frequences
typedef std::vector<float> TYTabFreq;

/**
 * Permet de stocker des valeurs de puissance accoustique pour differentes frequences.
 *
 * @author Projet_Tympan
 *
 */
class TYSpectre: public TYElement, public OSpectre
{

    OPROTOSUPERDECL(TYSpectre, TYElement)
    TY_EXTENSION_DECL(TYSpectre)


    // Methodes
public:
    /**
     * Constructeur par defaut.
     */
    TYSpectre();

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

    bool toXML(const std::string& sFilePath);
    bool fromXML(const std::string& sFilePath);

    std::string toXMLString();
    bool fromXMLString(const std::string& sXMLString);

    // Compare le contenu de deux spectre
    bool equivTo(const TYSpectre& other) const;

    /// Set/Get de la forme du spectre.
    TYSpectreForm getForm() {return _form;}
    void setForm(const TYSpectreForm& form) { _form = form; }

    /// Set/Get des remarques.
    QString getRemarque() const { return _remarque; }

    /// Set/Get des remarques.
    void setRemarque(QString rq) { _remarque = rq; }

    /// Set/Get du flag de conservation en BDD.
    //  bool getKeepInBDD() { return _keepInBDD; }

    /// Set/Get du flag de conservation en BDD.
    //  void setKeepInBDD(bool flag) { _keepInBDD = flag; }

    /// Set/Get du flag _isReadOnly.
    bool getIsReadOnly() { return _isReadOnly; }

    /// Set/Get du flag _isReadOnly.
    void setIsReadOnly(bool flag) { _isReadOnly = flag; }

    /// Conversion en tiers d'octave.
    TYSpectre toTOct() const;

    /// Conversion en octave.
    TYSpectre toOct() const;

    /// Export du spectre au format csv en creant le fichier
    void exportCSV(const std::string& filename);

    /// Export du spectre au format csv sur un flux transmis
    void exportCSV(std::ofstream& ofs);


    // ======== METHODES EQUIVALENTES A OSPECTRE POUR COMPATIBILITE PYTHON
    /// Conversion en dB.
//    TYSpectre toDB() const;

    /// Conversion en grandeur physique.
//    TYSpectre toGPhy() const;

    ///// Sommation arithmetique de deux spectres en 1/3 d'octave.
    //TYSpectre sum(const TYSpectre& spectre) const;

    ///// Ajoute une valeur constante a l'ensemble du spectre
    //TYSpectre sum(const double& valeur) const;

    ///// Sommation energetique de deux spectres en 1/3 d'octave.
    //TYSpectre sumdB(const TYSpectre& spectre) const;

    ///// Soustraction arithmetique de deux spectres en 1/3 d'octave.
    //TYSpectre subst(const TYSpectre& spectre) const;

    ///// soustrait une valeur constante a l'ensemble du spectre
    //TYSpectre subst(const double& valeur) const;

    ///// Soustraction energetique de deux spectres en 1/3 d'octave.
    //TYSpectre substdB(const TYSpectre& spectre) const;

    ///// multiplication de deux spectres terme a terme.
    //TYSpectre mult(const TYSpectre& spectre) const;

    ///// Multiplication d'un spectre par un scalaire.
    //TYSpectre mult(const double& coefficient) const;

    ///// division de deux spectres terme a terme.
    //TYSpectre div(const TYSpectre& spectre) const;

    ///// Division d'un spectre par une constante.
    //TYSpectre div(const double& coefficient) const;

    ///// Division d'une constante par un spectre.
    //TYSpectre invMult(const double& coefficient = 1.0) const;

    ///// Eleve un spectre a une puissance.
    //TYSpectre power(const double& puissance) const;

    ///// Calcule le log base n d'un spectre (n=10 par defaut).
    //TYSpectre log(const double& base = 10.0) const;

    ///// calcule 10^spectre pour chaque terme du spectre.
    //TYSpectre tenPow() const;

    ///// calcule la racine carree d'un spectre.
    //TYSpectre racine() const;

    ///// calcul le sin de la partie reelle du spectre
    //TYSpectre sin() const;

    ///// calcul le cos de la partie reelle du spectre
    //TYSpectre cos() const;

    ///// calcul le tan de la partie reelle du spectre
    //TYSpectre tan() const;

    ///// Retourne le spectre en valeur absolues
    //TYSpectre abs() const;

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

    /// Construit un spectre en d'octave.
    static TYSpectre makeOctSpect();

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
