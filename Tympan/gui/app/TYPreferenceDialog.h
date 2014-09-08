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
 * \file TYPreferenceDialog.h
 * \brief Tables pour le dialogue de la gestion des preferences (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_PREFERENCE_DIALOG__
#define __TY_PREFERENCE_DIALOG__

//Added by qt3to4:
#include <QHideEvent>
#include <QTabWidget>
#include <QDialog>

class QLineEdit;
class QSlider;
class QButtonGroup;
class QComboBox;
class QCheckBox;
class QRadioButton;
class TYSpectre;
class TYMaillage;
class TYToolButton;

/**
 * \class GeneralTab
 * \brief Table generale dans le dialog de la gestion des preferences.
 */
class GeneralTab : public QWidget
{
    Q_OBJECT

public slots :
    void changeBibDir();
    void changeWorkDir();

public:
    GeneralTab(QWidget* parent = 0);

    QLineEdit* _pWorkDirPathEdit;
    QLineEdit* _pHistoSizeEdit;
    QLineEdit* _pThreadEdit;
    QLineEdit* _pLibraryPathEdit;
};

/**
 * \class GeometrieTab
 * \brief Table Geometrie dans le dialogue de la gestion des preferences
 */
class GeometrieTab : public QWidget
{
    Q_OBJECT

public:
    GeometrieTab(QWidget* parent = 0);

    QLineEdit* _pGridDimXSiteEdit;
    QLineEdit* _pGridDimYSiteEdit;
    QLineEdit* _pGridStepSiteEdit;
    QLineEdit* _pGridMagnStepSiteEdit;

    QLineEdit* _pGridDimXBatEdit;
    QLineEdit* _pGridDimYBatEdit;
    QLineEdit* _pGridStepBatEdit;
    QLineEdit* _pGridMagnStepBatEdit;

    QLineEdit* _pGridDimXMacEdit;
    QLineEdit* _pGridDimYMacEdit;
    QLineEdit* _pGridStepMacEdit;
    QLineEdit* _pGridMagnStepMacEdit;

    QLineEdit* _pGridDimXMailEdit;
    QLineEdit* _pGridDimYMailEdit;
    QLineEdit* _pGridStepMailEdit;
    QLineEdit* _pGridMagnStepMailEdit;

    QLineEdit* _pGridDimXFaceEdit;
    QLineEdit* _pGridDimYFaceEdit;
    QLineEdit* _pGridStepFaceEdit;
    QLineEdit* _pGridMagnStepFaceEdit;

    QButtonGroup* _pAnglesButtonGroup;
    QRadioButton* _p15RadioButton;
    QRadioButton* _p30RadioButton;
    QRadioButton* _p45RadioButton;
    QRadioButton* _p90RadioButton;

    QLineEdit* _pSourceSizeEdit;
    QLineEdit* _pPointControlSizeEdit;
    QLineEdit* _pResoCylEdit;
    QLineEdit* _pSeuilDistNulEdit;
    QLineEdit* _pDistMinPtCrbNivEdit;
    QLineEdit* _pDelaunayToleranceEdit;
    QLineEdit* _pDefaultDimXEdit;
    QLineEdit* _pDefaultDimYEdit;

    QLineEdit* _pDefaultHMurEdit;
    QLineEdit* _pDefaultWEcranEdit;
};

/**
 * \class pref3DTab
 * \brief Table 3D dans le dialogue de la gestion des preferences.
 */
class pref3DTab : public QWidget
{
    Q_OBJECT

public:
    pref3DTab(QWidget* parent = 0);

    QComboBox* _pLeftButton2DComboBox;
    QComboBox* _pMiddleButton2DComboBox;
    QComboBox* _pRightButton2DComboBox;
    QComboBox* _pLeftButton3DComboBox;
    QComboBox* _pMiddleButton3DComboBox;
    QComboBox* _pRightButton3DComboBox;
    QComboBox* _pLeftButton2DShtComboBox;
    QComboBox* _pRightButton2DShtComboBox;
    QComboBox* _pLeftButton3DShtComboBox;
    QComboBox* _pRightButton3DShtComboBox;
    QSlider* _pLightSlider;
    QLineEdit* _pPrecisPickEdit;
    QLineEdit* _pZoomStepEdit;
    QCheckBox* _pViewOnlyHighlightCheckBox;
    QCheckBox* _pCenterOnLocateCheckBox;
    QCheckBox* _pWireframeOnMovingCameraCheckBox;
    QLineEdit* _pCameraStepTranslateEdit;
    QLineEdit* _pCameraStepRotateEdit;
    QLineEdit* _pCameraStepZoomEdit;
};

/**
 * \class ColorsTab
 * \brief Table Colors dans le dialog de la gestion des preferences.
 * \author Projet_Tympan
 */
class ColorsTab : public QWidget
{
    Q_OBJECT

public:
    ColorsTab(QWidget* parent = 0);

    TYToolButton* _pBackgroundColorToolButton;
    TYToolButton* _pGridColorToolButton;
    TYToolButton* _pPoliceColorToolButton;

    TYToolButton* _pTerrainColorToolButton;
    TYToolButton* _pPlanEauColorToolButton;
    TYToolButton* _pCrsEauColorToolButton;
    TYToolButton* _pCrbNivColorToolButton;
    TYToolButton* _pVegColorToolButton;

    TYToolButton* _pCylindreColorToolButton;
    TYToolButton* _pBoxColorToolButton;
    TYToolButton* _pSubRectColorToolButton;

    TYToolButton* _pDalleColorToolButton;
    TYToolButton* _pMurColorToolButton;
    TYToolButton* _pEcranColorToolButton;

    TYToolButton* _pRouteColorToolButton;
    TYToolButton* _pResTranspColorToolButton;

    TYToolButton* _pAltiColorMaxToolButton;
    TYToolButton* _pAltiColorMinToolButton;

    TYToolButton* _pPointControlColorToolButton;
    TYToolButton* _pSourcePonctColorToolButton;

    QSlider* _pAltOpacitySlider;
    QSlider* _pWinOpacitySlider;
    QSlider* _pMailOpacitySlider;
};

/**
 * \class AcoustiqueTab
 * \brief Table Acoustique dans le dialog de la gestion des preferences.
 */
class AcoustiqueTab : public QWidget
{
    Q_OBJECT

public:
    AcoustiqueTab(QWidget* parent = 0);
    virtual ~AcoustiqueTab();

    QCheckBox* _pCheckBoxCondFav;
    QCheckBox* _pCheckBoxUseAtmosphere;
    QCheckBox* _pCheckBoxUseVegetation;
    QCheckBox* _pCheckBoxUseEcran;
    QCheckBox* _pCheckBoxUseReflexion;
    QCheckBox* _pCheckBoxInterference;
    QButtonGroup* _pExpansGeoButtonGroup;
    QRadioButton* _pRadioButtonExpansGeo2Pi;
    QRadioButton* _pRadioButtonExpansGeo4Pi;
    QLineEdit* _pLineEditParamH;
    QLineEdit* _pLineEditDistanceSRMin;
    QLineEdit* _pLineEditResistivite;
    QLineEdit* _pLineEditEpaisseur;
    QLineEdit* _pLineEditPression;
    QLineEdit* _pLineEditTemperature;
    QLineEdit* _pLineEditHygrometrie;
    QLineEdit* _pLineEditVitVent;
    QLineEdit* _pLineEditDirVent;
    QLineEdit* _pLineEditMasseVol;

    /// Spectre d'attenuation par default des materiaux de construction
    TYSpectre* _pSpectreAttMatConst;

    ///Spectre d'absorption par defaut des materiaux de construction
    TYSpectre* _pSpectreAbsoMatConst;

    ///Maillage par defaut.
    TYMaillage* _pMaillage;

public slots:
    /**
     * Edite la fenetre de selection de la resisitvite.
     */
    void editResistivite();
    /**
     * Edite le spectre d'attenuation par default des materiaux de construction.
     */
    void editSpectreAtt();
    /**
     * Edite le spectre d'absorption par default des materiaux de construction.
     */
    void editSpectreAbso();
    /**
     * Edite le maillage par default.
     */
    void editMaillage();
};


/**
 * \class TYPreferenceDialog
 * \brief Boite de dialogue pour la gestion des preferences.
 *
 * Les preferences sont sauvegardees dans le fichier gere
 * par le TYPreferenceManager.
 */
class TYPreferenceDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * Constructeur par defaut.
     */
    TYPreferenceDialog(QWidget* parent = 0, const char* name = 0, Qt::WFlags f = 0);

public slots:
    /**
     * Charge les preferences et mets a jour les widgets correspondants.
     */
    void loadPreferences();

    /**
     * Appelle savePreference si necessaire
     */
    void clickOkApply();

public slots:
    /**
     * Slot appele a l'execution du dialog.
     */
    virtual int exec();

protected:
    virtual void hideEvent(QHideEvent* pEvent);


private:
    void savePreferences();

    void setupTabGeneral();
    void setupTabGeometrie();
    void setupTab3D();
    void setupTabColors();
    void setupTabAcoustique();

    QTabWidget* tabWidget;
    QPushButton* _buttonOK;
    QPushButton* _buttonApply;
    QPushButton* _buttonCancel;
};

#endif //__TY_PREFERENCE_DIALOG__
