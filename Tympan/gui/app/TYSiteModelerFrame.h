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

/**
 * \file TYSiteModelerFrame.h
 * \brief Classe Modeler specialisee pour l'edition des sites (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 */

#ifndef __TY_SITE_MODELER_FRAME__
#define __TY_SITE_MODELER_FRAME__

#include "TYModelerFrame.h"
#include "Tympan/models/business/TYProjet.h"

class QToolButton;
class QComboBox;

/**
 * \class TYSiteModelerFrame
 * \brief Classe Modeler specialisee pour l'edition des sites
 */
class TYSiteModelerFrame : public TYModelerFrame
{
    Q_OBJECT

public:
    /**
     * Les modes de rendu du maillage.
     */
    enum RenderMaillageMode
    {
        Surface,
        Lines,
        Mixed,
        NbOfRenderMaillageMode
    };

    /**
     * Constructeur par defaut.
     */
    TYSiteModelerFrame(QWidget* parent = 0, const char* name = 0, Qt::WFlags f = 0);
    /**
     * Constructeur a partir d'un projet.
     */
    TYSiteModelerFrame(LPTYProjet pProjet, QWidget* parent = 0, const char* name = 0, Qt::WFlags f = 0);
    /**
     * Constructeur a partir d'un site.
     */
    TYSiteModelerFrame(LPTYSiteNode pSite, QWidget* parent = 0, const char* name = 0, Qt::WFlags f = 0);
    /**
     * Destructeur.
     */
    virtual ~TYSiteModelerFrame();

    /**
     * Set/Get du projet a editer.
     * Dans ce cas le site sera celui qui est associe au projet.
     */
    void setProjet(LPTYProjet pProjet);
    /**
     * Set/Get du projet a editer.
     * Dans ce cas le site sera celui qui est associe au projet.
     */
    LPTYProjet getProjet() { return _pProjet; }

    /**
     * Set/Get du site a editer.
     */
    void setSite(LPTYSiteNode pSite);
    /**
     * Set/Get du site a editer.
     */
    LPTYSiteNode getSite() { return _pSite; }

    /**
     * Indique si on affiche ou pas les courbes de niveau.
     */
    bool getShowCrbNiv() { return _showCrbNiv; }

    /**
     * Mise a jour de la comboBox des maillages disponibles
     */
    void updateSelectMaillageBox();

    ///Les differents modes d'edition d'un site.
    enum SiteModelerMode
    {
        EchelleMode = NbOfModelerMode,
        PointRefMode,
        OrientationMode,
        EmpriseMode,
        CourbeNiveauMode,
        TerrainMode,
        VegetationMode,
        PlanEauMode,
        CoursEauMode,
        SrcPonctMode,
        RecepteurMode,
        RouteMode,
        ReseauTransportMode,
        EtageMode,
        MachineMode,
        BatimentMode,
        EcranMode,
        SiteMode,
        MaillageRectMode,
        MaillageLinearMode,
        MaillageVerticalMode,
        BoundaryNoiseMapMode,
        PointsCtrlMode,
        NbOfSiteModelerMode,
        SilosMode
    };

public slots:
    virtual void setViewType(int view);
    virtual void setRenderMode(int mode, bool bUpdateGL);
    virtual void setEditorMode(int mode);
    virtual void updateView(bool clipping = true, bool axesAndGrid = true);
    virtual void updatePreferences();
    virtual bool close();

    /**
     * Affiche ou masque les courbes de niveau.
     * Fait un updateView().
     */
    void showCrbNiv(bool show);
    /**
     * Affiche ou masque l'altimetrie.
     * Fait un updateView().
     */
    void showAltimetrie(bool show);
    /**
     * Affiche ou masque l'image associee au site.
     * Fait un updateView().
     */
    void showImageSite(bool show);

    /**
     * Calcul l'altimetrie a partir des courbes de niveau.
     */
    void calculAltimetrie();

    /**
     * Mets a jour l'opacite de la transparence en fonction du
     * type de la vue courante.
     */
    void updateAltiOpacity(int view = -1);

    /**
     * Selectionne le mode de rendu du maillage.
     */
    virtual void setRenderMaillageModeSlot(int mode);

    /**
     * Selectionne le maillage.
     */
    virtual void setSelectMaillageBox(const QString& name);

protected:
    /**
     * Initialise le site modeler, appeler par les constructeurs.
     */
    void init();

    virtual bool computeCurPos(int x, int y, float* pos);

    /**
     * Affiche ou masque les elements de site (Courbes de niveau, altimetrie,
     * et image de site) sans faire de mise a jour de la vue.
     */
    void updateVisibilityElementSite();


protected:
    ///Un pointeur sur le projet a editer.
    LPTYProjet _pProjet;
    ///Un pointeur sur le site a editer.
    LPTYSiteNode _pSite;

    // To compute altimetry
    QToolButton* _pAltiBtn;
    ///Bouton pour l'affichage des courbes de niveau.
    QToolButton* _pShowCrbNivBtn;
    ///Indique si on affiche ou masque les courbes de niveau.
    bool _showCrbNiv;

    ///Bouton pour l'affichage de l'altimetrie.
    QToolButton* _pShowAltimetrieBtn;
    ///Indique si on affiche ou masque l'altimetrie.
    bool _showAltimetrie;

    ///Bouton pour l'affichage de l'image de site.
    QToolButton* _pShowImageSiteBtn;
    ///Indique si on affiche ou masque l'image de site.
    bool _showImageSite;

    ///Combo box pour le mode de rendu.
    QComboBox* _pRenderMaillageModeBox;

    ///Combo box pour le choix du maillage.
    QComboBox* _pSelectMaillageBox;

    //Editors.
    TYEchelleEditor* _pEchelleEditor;
    TYPointRefEditor* _pPointRefEditor;
    TYOrientationEditor* _pOrientationEditor;
    TYEmpriseEditor* _pEmpriseEditor;
    TYCourbeNiveauEditor* _pCourbeNiveauEditor;
    TYPlanEauEditor* _pPlanEauEditor;
    TYTerrainEditor* _pTerrainEditor;
#if WITH_NMPB
    TYRouteEditor* _pRouteEditor;
#endif
    TYReseauTransportEditor* _pReseauTransportEditor;
    TYEtageEditor* _pEtageEditor;
    TYSilosEditor* _pSilosEditor;
    TYCoursEauEditor* _pCoursEauEditor;
    TYSourceEditor* _pSourceEditor;
    TYPointCalculEditor* _pPointCalculEditor;
    TYRectangularMaillageEditor* _pRectangularMaillageEditor;
    TYLinearMaillageEditor* _pLinearMaillageEditor;
    TYVerticalMaillageEditor* _pVerticalMaillageEditor;
    TYBoundaryNoiseMapEditor* _pBoundaryNoiseMapEditor;

    ///Nombre d'instance de type TYSiteModelerFrame.
    static int _nbInstance;
};

#endif //__TY_SITE_MODELER_FRAME__
