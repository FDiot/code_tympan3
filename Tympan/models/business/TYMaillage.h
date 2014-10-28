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

#ifndef __TY_Maillage__
#define __TY_Maillage__


#include "Tympan/core/defines.h"
#include "Tympan/models/business/geometry/TYGeometryNode.h"
#include "Tympan/models/business/TYPointCalcul.h"
#include "Tympan/models/business/TYPalette.h"
#include "Tympan/models/business/TYPanel.h"

class TYCalcul;

///Densite (nb de points par metre) par defaut pour la construction des maillages.
static const double TY_MAILLAGE_DEFAULT_DENSITE = 0.1; // 1 point tous les 10m

struct MPoint
{
    OVector3D  pt;
    TYPalette::values_type scalar;
};

struct MTriangle
{
    MPoint pts[3];
};

/**
 * \file TYMaillage.h
 * \class TYMaillage
 * \brief Classe de definition d'un maillage.
 * \version v 1.1
 * \author Projet_Tympan
 * \date 2008/01/21
 *
 */
class TYMaillage: public TYElement
{
    OPROTODECL(TYMaillage)
    TY_EXTENSION_DECL_ONLY(TYMaillage)

    // Methodes
public:
    /**
     * \fn TYMaillage()
     * \brief Constructeur.
     *  Constructeur de la classe TYMaillage
     */
    TYMaillage();

    /**
     * \fn TYMaillage(const TYMaillage& other)
     * \brief Constructeur par copie.
     *  Constructeur par copie de la classe TYMaillage
     */
    TYMaillage(const TYMaillage& other);

    /**
     * \fn virtual ~TYMaillage()
     * \brief Destructeur
     *  Destructeur de la classe TYMaillage
     */
    virtual ~TYMaillage();

    ///Operateur =.
    TYMaillage& operator=(const TYMaillage& other);

    ///Operateur ==.
    bool operator==(const TYMaillage& other) const;

    ///Operateur !=.
    bool operator!=(const TYMaillage& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    /*!
     * \brief copie l'etat des points de controle pour un calcul pour un autre calcul
     * \param pCalculRef : Calcul dont l'etat pour les points doit etre copie
     * \param pCalculNew : Calcul pour lequel l'etat doit etre duplique
     */
    void duplicatePtCalcState(const TYCalcul* pCalculRef, TYCalcul* pCalculNew);


    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    /**
     * Suppression du resultat de calcul
     */
    virtual void clearResult();


    /**
     * \fn TYTabLPPointCalcul& getPtsCalcul()
     * \brief Set/Get de la liste des points de calcul.
     * \return _ptsCalcul:Liste des points de calcul
     */
    TYTabLPPointCalcul& getPtsCalcul() { return _ptsCalcul; }

    /**
     * Set/Get de la liste des points de calcul.
     */
    const TYTabLPPointCalcul& getPtsCalcul() const { return _ptsCalcul; }

    /**
     * Set/Get de la liste des points de calcul.
     */
    void setPtsCalcul(const TYTabLPPointCalcul& list) { _ptsCalcul = list; setIsGeometryModified(true); }

    /**
     * \fn bool addPointCalcul(LPTYPointCalcul pPtCalcul)
     * \brief Ajoute un nouveau point de calcul.
     * \param pPtCalcul: Le point a ajouter.
     */
    bool addPointCalcul(LPTYPointCalcul pPtCalcul);

    /**
     * \fn bool remPointCalcul(const LPTYPointCalcul pPtCalcul)
     * \brief Supprime une entree a la liste des points de calculs.
     * \param pPtCalcul
     */
    bool remPointCalcul(const LPTYPointCalcul pPtCalcul);

    /**
     * \fn bool remPointCalcul(QString idPtCalcul)
     * \brief Supprime une entree a la liste des points de calculs a partir de son identifiant.
     * \param idPtCalcul:identifiant
     */
    bool remPointCalcul(QString idPtCalcul);

    /**
     * \fn void remAllPointCalcul()
     * \brief Suppression de tous les elements.
     */
    void remAllPointCalcul();

    /**
     * \fn void setHauteur(double hauteur)
     *     double getHauteur()
     * \brief Set/Get de la hauteur de ce maillage par rapport au sol (a l'altimetrie en fait).
     * \return _hauteur: la hauteur de ce maillage par rapport au sol
     */
    void setHauteur(double hauteur) { _hauteur = hauteur; setIsGeometryModified(true); }

    /**
     * Set/Get de la hauteur de ce maillage par rapport au sol (a
     * l'altimetrie en fait).
     */
    double getHauteur() const { return _hauteur; }

    /**
     * \fn void setComputeAlti(bool state)
     *     bool getComputeAlti()
     * \brief Set/Get de l'indicateur de calcul de l'altitude des points de ce maillage
     * \return _computeAlti
     */
    void setComputeAlti(bool state) { _computeAlti = state; setIsGeometryModified(true); }

    /**
     * Set/Get de l'indicateur de calcul de l'altitude des points
     * de ce maillage
     */
    bool getComputeAlti() const { return _computeAlti; }

    /**
     * \fn void setDataType(int type)
     *     int getDataType()
     * \brief Set/Get de l'indicateur de type des donnees a representer.
     * \return _dataType
     */
    void setDataType(int type) { _dataType = type; setIsGeometryModified(true); }
    /**
     * Set/Get de l'indicateur de type des donnees a representer.
     */
    int getDataType() { return _dataType; }

    /**
     * \fn void setDataFreq(float freq)
     *     float getDataFreq()
     * \brief Set/Get de la frequence des donnees a representer lorsque _dataType vaut DataFreq.
     * \return _dataFreq
     */
    void setDataFreq(float freq) { _dataFreq = freq; setIsGeometryModified(true); }
    /**
     * Set/Get de la frequence des donnees a representer lorsque _dataType vaut DataFreq.
     */
    float getDataFreq() { return _dataFreq; }

    /**
     * \fn void setPalette(LPTYPalette pPalette)
     *     LPTYPalette getPalette()
     * \brief Set/Get de la palette
     * \return _pPalette
     */
    void setPalette(LPTYPalette pPalette) { _pPalette = pPalette; }
    LPTYPalette getPalette() { return _pPalette; }

    /**
     * \fn void setPanel(LPTYPanel pPanel)
     * \brief Set du panel
     */
    void setPanel(LPTYPanel pPanel) { _pPanel = pPanel; }

    /**
     * \fn LPTYPanel getPanel()
     * \brief Get du panel
     * \return _pPanel
     */
    LPTYPanel getPanel() { return _pPanel; }

    /**
     * \fn void make(const TYTabPoint& points)
     * \brief Rempli la structure de points de calcul a partir d'un tableau de points classiques.
     */
    void make(const TYTabPoint& points);

    /**
     * \fn  void setState(const int& etat)
     *      int getState()
     *      const int getState()
     * \brief Get/Set de l'etat du maillage
     * \return _state
     */
    void setState(const int& etat) { _state = etat; }
    int getState() { return _state; }
    const int getState() const { return _state; }

    /**
     * \fn bool isLocked()
     *     const bool isLocked()
     *     void setLocked(const bool& locked)
     * \brief Set/Get de l'etat de blocage du maillage.
     * \return _locked
     */
    bool isLocked() { return _locked; }
    const bool isLocked() const { return _locked; }
    void setLocked(const bool& locked) { _locked = locked; }

    /**
     * \fn enum MaillageDataType
     * \brief Les differents type pour les donnees a representer
     * \param ValGlobalDBA        La valeur globale du spectre en dBA.
     *         ValGlobalDBLin     La valeur globale du spectre en dBLin.
     *         DataFreq           Les donnees d'une frequence particuliere du spectre, cf. _valueFreq.
     */
    enum MaillageDataType
    {
        ValGlobalDBA,
        ValGlobalDBLin,
        DataFreq
    };

    /**
     *\fn enum MaillageState
     *\brief Etat du Maillage : Actif / Bloque
     *\param Actif           Parametres modifiables
     *       Inactif         Parametres non modifiables
     */
    enum MaillageState
    {
        Actif,
        Inactif
    };

    static bool checkSimilar(const TYMaillage* const pMaillage1, const TYMaillage* const pMaillage2);

    /**
     * \brief Compute the mesh
     *
     * It simply loops over the calcul points and trianglulate them. The points are ordered regularly
     * (at least from a topology point of view). The array of calcul points might be sparse, in that case
     * we only create a triangle if it's possible (i.e. tha 3 points are defined)
     *
     * \param mesh The mesh
     */
    void computeMesh(std::vector<MTriangle> &mesh) const;

    /**
     * \brief Compute the triangle
     * \param pt1 The point 1
     * \param pt2 The point 2
     * \param pt3 The point 3
     * \return The newly created triangle
     */
    MTriangle computeTriangle(TYPointCalcul& pt1, TYPointCalcul& pt2, TYPointCalcul& pt3) const;

    /**
     * \brief Get the spectrum value of a calcul point
     * \param pt The point
     * \return The spectrum value
     */
    double getSpectrumValue(TYPointCalcul& pt) const;

    /**
     * \brief Compute the isocurve
     *
     * The function does several plane-triangle intersection in order to compute the new
     * position of the isopoints. Before the test, the third coordinate of the points of the
     * mesh are replaced by their spectrum value. For each new plane (on each step we gradually
     * increase its height), every triangles of the mesh are tested.
     *
     * \param mesh The mesh (the spectrum value of the mesh will be clamped between [0,1])
     * \param isoCurve The isocurve
     */
    void computeIsoCurve(std::vector<MTriangle> &mesh, std::vector<MPoint> &isoCurve) const;

    /**
     * \brief Compute an isopoint by interpolating the value if the segment intersects the plane
     *
     * An intersection test is performed between the plane and the segment. If there's an
     * intersection, the position of the isopoint is computed.
     *
     * \param plane The plane
     * \param pt1 The point 1 of the segment
     * \param pt2 The point 2 of the segment
     * \param mp The isoPoint to update
     * \return True if the plane intersects the segment and the isoPoint has been calculated
     */
    bool computeIsoPoint(const OHPlane3D& plane, const MPoint& pt1, const MPoint& pt2, MPoint& mp) const;

    /**
     * \brief Return the index (should be pure virtual)
     * \param x Row value
     * \param y Column value
     * \return The index of point calcul
     */
    virtual int getIndexPtCalcul(int x, int y) const {return 0;}

    /**
     * \brief Return the dimensions in x and y (should be pure virtual)
     * \param x Dimension x
     * \param y Dimension y
     */
    virtual void getDimensions(int& x, int& y) const {}

    // Membres
protected:
    ///Liste des points de calcul.
    TYTabLPPointCalcul _ptsCalcul;

    ///La hauteur par rapport au sol (a l'altimetrie en fait) a laquelle se trouve ce maillage.
    double _hauteur;

    ///Indique si l'altitude des points constituant ce maillage doit etre calculee.
    bool _computeAlti;

    ///Indique le type des donnees a representer.
    int _dataType;

    ///La frequence des donnees a representer lorsque _dataType vaut DataFreq.
    float _dataFreq;

    /// Etat du maillage (Actif/Inactif)
    int _state;

    /// Permet de bloquer le maillage si le calcul est verrouille
    bool _locked;

    ///Palette
    LPTYPalette _pPalette;

    ///Panel
    LPTYPanel _pPanel;
};


///Noeud geometrique de type TYMaillage.
typedef TYGeometryNode TYMaillageGeoNode;
///Smart Pointer sur TYMaillageGeoNode.
typedef SmartPtr<TYMaillageGeoNode> LPTYMaillageGeoNode;
///Collection de noeuds geometriques de type TYMaillage.
typedef std::vector<LPTYMaillageGeoNode> TYTabMaillageGeoNode;
///Collection de maillages.
typedef std::vector<LPTYMaillage> TYTabLPMaillage;


#endif // __TY_Maillage__
