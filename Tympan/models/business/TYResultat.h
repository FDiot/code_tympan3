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

#ifndef __TY_RESULTAT__
#define __TY_RESULTAT__

#include <cstddef>

#include "Tympan/models/common/spectrum_matrix.h"
#include "Tympan/models/business/acoustic/TYSourcePonctuelle.h"
#include "TYPointCalcul.h"


class TYTrajet;

/// Map ptrElement-index
typedef std::map<TYElement*, int> TYMapElementIndex;

/**
* \file TYResultat.h
* \class TYResultat
* \brief Classe qui Permet de centraliser les resultats d'un calcul acoustique.
* \author Projet_Tympan
* \version v 1.1
* \date 2008/01/25
*
*/

class TYResultat: public TYElement
{
    OPROTODECL(TYResultat)
    TY_EXTENSION_DECL_ONLY(TYResultat)
    TY_EXT_GRAPHIC_DECL_ONLY(TYResultat)

    // Methodes
public:
    /**
     * \fn TYResultat()
     * \brief Constructeur.
     * Le constructeur de la classe TYResultat
     */
    TYResultat();

    /**
     * \fn TYResultat(const TYResultat& other)
     * \brief Constructeur par copie.
     * \Le constructeur par copie de la classe TYResultat .
     */
    TYResultat(const TYResultat& other);

    /**
     * \fn virtual ~TYResultat()
     * \brief Destructeur.
     * Le destructeur de la classe TYResultat .
     */
    virtual ~TYResultat();

    ///Operateur =.
    TYResultat& operator=(const TYResultat& other);

    ///Operateur ==.
    bool operator==(const TYResultat& other) const;

    ///Operateur !=.
    bool operator!=(const TYResultat& other) const;

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);

    virtual int fromXML(DOM_Element domElement);

    /**
     * \fn void purge()
     * \brief Reinitialise la matrice resultat.
     */
    void purge();

    /**
     * \fn size_t getNbOfSources() const
     * \brief Retourne le nombre de sources.
     * \return _sources.size(): Le nombre de source.
     */
    size_t getNbOfSources() const { return _sources.size(); }

    /**
     * \fn size_t getInitialNbOfSources() const
     * \brief Retourne le nombre de sources initialement utilisees par le calcul
     * \return _backupSources.size():le nombre de sources initialement utilisees par le calcul
     */
    size_t getInitialNbOfSources() const { return _backupSources.size(); }


    /**
     * \fn size_t getNbOfRecepteurs()
     * \brief Retourne le nombre de recepteurs.
     * \return _recepteurs.size():Le nombre de recepteurs.
     */
    size_t getNbOfRecepteurs() const { return _recepteurs.size(); }

    /**
     * \fn void addSource(TYSourcePonctuelle* pSource)
     * \brief Ajoute une source.
     */
    void addSource(TYElement* pSource);

    /**
     * \fn void addRecepteur(TYPointCalcul* pRecepteur)
     * \brief Ajoute un recepteur et indique s'il faut reconstruire la matrice
     */
    bool addRecepteur(TYPointCalcul* pRecepteur);

    /*!
     * \brief Supprime un recepteur du tableau et indique s'il faut reconstruire la matrice
     * \param TYPointCalcul * pRecepteur : recepteur a supprimer
     */
    bool remRecepteur(TYPointCalcul* pRecepteur);

    /**
     * \fn void buildSources(const TYTabSourcePonctuelleGeoNode& sources)
     * \brief Construit la liste des sources
     */
    void buildSources(const TYTabSourcePonctuelleGeoNode& sources);

    /**
     * \fn void buildRecepteurs(const TYTabPointCalculGeoNode& sources)
     * \brief Construit la liste des sources
     */
    void buildRecepteurs(const TYTabPointCalculGeoNode& sources);

    /**
     * \fn void buildMatrix()
     * \brief Construit la matrice resultat a partir des sources et recepteurs entres.
     */
    void buildMatrix();

    /**
     * \fn  int getIndexRecepteur(TYPointCalcul* pRecepteur)
     * \brief Retourne l'index d'un recepteur
     * \return _recepteurs[pRecepteur]:l'index d'un recepteur
     */
    int getIndexRecepteur(TYPointCalcul* pRecepteur) { return _recepteurs[pRecepteur]; }

    /**
     * \fn bool setSpectre(TYPointCalcul* pRecepteur, TYSourcePonctuelle* pSource, OSpectre & Spectre)
     * \brief Assigne un spectre a un couple S-R.
     */
    bool setSpectre(TYPointCalcul* pRecepteur, TYSourcePonctuelle* pSource, OSpectre& Spectre);

    /**
     * \fn bool setSpectre(const int& indexRecepteur, const int& indexSource, OSpectre & Spectre)
     * \brief Assigne un spectre a un couple S-R.
     */
    bool setSpectre(int indexRecepteur, int indexSource, OSpectre& Spectre);

    /**
     * \fn bool setSpectre(const int& indexRecepteur, const int& indexSource, OSpectre & Spectre, OSpectreMatrix& matrix)
     * \brief Ajoute un spectre dans une matrice de resultat
     */
    static bool setSpectre(int indexRecepteur, int indexSource, OSpectre& Spectre, tympan::SpectrumMatrix& matrix);

    /**
     * \fn OSpectre getSpectre(TYPointCalcul* pRecepteur,TYElement* pSource)
     * \brief Retourne un spectre pour un couple S-R.
     */
    OSpectre getSpectre(TYPointCalcul* pRecepteur, TYElement* pSource);

    /**
     * \fn OSpectre getSpectre(const int& indexRecepteur, const int& indexSource)
     * \brief Retourne un spectre pour un couple S-R.
     */
    const OSpectre& getSpectre(int indexRecepteur, int indexSource) const;

    /**
     * \fn OSpectre getElementSpectre(const int& indexRecepteur, const int& indexSource)
     * \brief Retourne le spectre de la matrice brute (avant condensation)pour un couple S-R (S = Source elementaire)
     */
    const OSpectre& getElementSpectre(int indexRecepteur, int indexSource) const;

    /**
     * \fn OTabSpectre getSpectres(TYPointCalcul* pRecepteur)
     *     void getSpectres(TYPointCalcul* pRecepteur, OTabSpectre& tab)
     * \brief Retourne les spectres pour un recepteur donne.
     */
    OTabSpectre getSpectres(TYPointCalcul* pRecepteur);

    /**
     * \fn OTabSpectre getSpectres(const int& indexRecepteur)
     *     void getSpectres(const int& indexRecepteur, OTabSpectre& tab)
     * \brief Retourne les spectres pour un recepteur donne.
     */
    OTabSpectre getSpectres(const int& indexRecepteur) const;

    /**
     * \fn void remSpectres(TYPointCalcul* pRecepteur)
     * \brief Efface les spectres d'un recepteur donne
     */
    void remSpectres(TYPointCalcul* pRecepteur);

    /**
     * \fn LPTYElement getSource(const int& indexSource)
     * \brief Retourne la source correspondant a l'indice passe
     */
    LPTYElement getSource(const int& indexSource);

    /**
     * \fn LPTYElement getElementSource(const int& indexSource)
     * \brief Retourne la source elementaire correspondant a l'indice passe
     */
    LPTYElement getElementSource(const int& indexSource);

    /**
     * \fn LPTYPointCalcul getRecepteur (const int& indexRecepteur)
     * \brief Retourne le recepteur corresponadnt a l'indice passe
     */
    LPTYPointCalcul getRecepteur(const int& indexRecepteur);

    /**
     * \fn TYMapElementTabSources& getMapEmetteurSrcs()
     * \brief Retourne le tableau associatif "emetteur/Liste de sources"
     * \return _mapEmetteurSources le tableau associatif "emetteur/Liste de sources"
     */
    TYMapElementTabSources& getMapEmetteurSrcs() { return _mapEmetteurSources; }

    /*!
     * \fn std::vector<TYElement*> getSources();
     * \brief return the list of all sources (business sources)
     */
    std::vector<LPTYElement> getSources();


    /*!
     * \fn std::vector<TYElement*> getReceptors();
     * \brief return the list of all sources (business sources)
     */
    std::vector<LPTYElement> getReceptors();


    /**
     * \fn void saveSpectre(const std::string& filename, TYCalcul* pSubstCalcul=NULL)
     * \brief Sauvegarde des spectres dans un fichier
     */
    void saveSpectre(const std::string& filename, TYCalcul* pSubstCalcul = NULL);

    /**
     * \fn void saveValue(const std::string& filename, bool affichage, TYCalcul* pSubstCalcul=NULL)
     * \brief Sauvegarde des valeurs dans un fichier affichage : false -> dBA et true -> dBLin
     */
    void saveValue(const std::string& filename, const int& affichage, double freq = 100);

    /**
     * \fn void setPartialState(const bool& bPartial)
     *     bool getPartialState()
     * \brief Get/Set du parametre de conservation de la matrice brut _bPartial
     * \return _bPartial
     */
    void setPartialState(const bool& bPartial);
    bool getPartialState() const { return _bPartial; }

    /**
     * \fn OSpectre getEmergence(OSpectre& spectre, OSpectre& substSpectre)
     * \brief Renvoit la difference entre 2 resultats de calcul (spectre)
     * \return OSpectre
     */
    OSpectre getEmergence(OSpectre& spectre, OSpectre& substSpectre);

    /**
     * \fn double getEmergence(const double& val1, const double& val2)
     * \brief Renvois la difference entre deux valeurs de calcul (valeurs)
     * \return double
     */
    double getEmergence(const double& val1, const double& val2);

    /**
     * \fn OSpectre getAmbiant(OSpectre& spectre, OSpectre& substSpectre)
     * \brief Renvoi du bruit ambiant
     * \return OSpectre
     */
    OSpectre getAmbiant(OSpectre& spectre, OSpectre& substSpectre);

    /**
     * \fn double getAmbiant(const double& val1, const double& val2)
     * \brief Calcul du bruit ambiant pour deux valeurs
     * \return double
     */
    double getAmbiant(const double& val1, const double& val2);

    /**
     * \fn void buildMapSourceSpectre()
     * \brief Build and store powerSpectrum of all sources in calcul
     */
    void buildMapSourceSpectre();

    /**
     *\fn
     *\brief access the map of sources with power spectrum
     *\return std::map<TYElement*, LPTYSpectre>&
     */
    std::map<TYElement*, LPTYSpectre>& getMapElementSpectre() { return _mapElementSpectre; }

    void setResultMatrix(tympan::SpectrumMatrix matrix){ _matrix = matrix; }
    tympan::SpectrumMatrix& getResultMatrix(){ return _matrix; }
    void setSources(TYMapElementIndex sources){ _sources = sources;}

private:
    /**
     * Ecrit sur 2 lignes les parametres du calcul.
     */
    void saveParamValue(std::ofstream& ofs, TYCalcul* pCalcul);


    // Membres
protected:
    ///La matrice de resultat.
    tympan::SpectrumMatrix _matrix;

    /// Sauvegarde de la matrice brute
    bool _bPartial;
    tympan::SpectrumMatrix _backupMatrix;
    TYMapElementIndex _backupSources;

    ///Les sources contenues dans la matrice resultat.
    TYMapElementIndex _sources;

    ///Les recepteurs contenus dans la matrice resultat.
    TYMapElementIndex _recepteurs;

    ///Les spectres de puissance associe a chaque source
    std::map<TYElement*, LPTYSpectre> _mapElementSpectre;

    /// Tableau associatif "emetteur"/liste des sources de l'emetteur
    TYMapElementTabSources _mapEmetteurSources;
};


#endif // __TY_RESULTAT__
