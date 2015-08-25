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
* \file TYPreferenceDialog.cpp
* \brief Table generale dans le dialog de la gestion des preferences
*/


#include <qslider.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qgridlayout.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qvalidator.h>
#include <qfiledialog.h>
#include <QHideEvent>
#include <QMdiSubWindow>
#include <QMdiArea>

#include "Tympan/models/business/TYPreferenceManager.h"
#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/TYMaillage.h"
#include "Tympan/models/business/acoustic/TYSpectre.h"
#include "Tympan/gui/widgets/TYSolResistanceDialog.h"
#include "Tympan/gui/widgets/TYToolButton.h"
#include "Tympan/gui/app/TYModelerFrame.h"
#include "Tympan/gui/app/TYApplication.h"
#include "Tympan/gui/app/TYMainWindow.h"
#include "TYPreferenceDialog.h"

#define TR(id) OLocalizator::getString("TYPreferenceDialog", (id))


TYPreferenceDialog::TYPreferenceDialog(QWidget* parent, const char* name, Qt::WFlags f):
QDialog(parent, f)
{
	setWindowTitle(TR("id_caption"));

	tabWidget = new QTabWidget;
	tabWidget->addTab(new GeometrieTab(), TR("id_tab_geometry"));   //0
	tabWidget->addTab(new pref3DTab(), TR("id_tab_3d"));            //1
	tabWidget->addTab(new ColorsTab(), TR("id_tab_colors"));        //2
	tabWidget->addTab(new AcoustiqueTab(), TR("id_tab_acoustic"));  //3
	tabWidget->addTab(new GeneralTab(), TR("id_tab_general"));      //4

	_buttonOK = new QPushButton(TR("id_ok_btn"), this);
	_buttonOK->setDefault(true);
	connect(_buttonOK, SIGNAL(clicked()), this, SLOT(accept()));
	connect(_buttonOK, SIGNAL(clicked()), this, SLOT(clickOkApply()));

	_buttonCancel = new QPushButton(TR("id_cancel_btn"), this);
	_buttonCancel->setShortcut(Qt::Key_Escape);
	connect(_buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));

	_buttonApply = new QPushButton(TR("id_apply_btn"), this);
	connect(_buttonApply, SIGNAL(clicked()), this, SLOT(clickOkApply()));

	QGridLayout* buttonLayout = new QGridLayout();
	buttonLayout->addWidget(_buttonOK, 0, 1);
	buttonLayout->addWidget(_buttonCancel, 0, 2);
	buttonLayout->addWidget(_buttonApply, 0, 3);

	QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->addWidget(tabWidget);
	mainLayout->addLayout(buttonLayout);
	//mainLayout->addStretch(1);
	setLayout(mainLayout);
}

int TYPreferenceDialog::exec()
{
	TYPreferenceManager::loadGeometryFromPreferences(metaObject()->className(), this);

	loadPreferences();

	return QDialog::exec();
}

GeneralTab::GeneralTab(QWidget* parent)
	: QWidget(parent)
{
	//historique
	QLabel* pHistoSizeLabel =  new QLabel(TR("id_label_histo_size"));
	_pHistoSizeEdit = new QLineEdit();

	QGridLayout* groupBoxLayout = new QGridLayout();
	groupBoxLayout->addWidget(pHistoSizeLabel, 0, 0);
	groupBoxLayout->addWidget(_pHistoSizeEdit, 0, 1);

	QGroupBox* groupBox = new QGroupBox();
	groupBox->setTitle(TR("id_histo"));
	groupBox->setLayout(groupBoxLayout);

	//threads
	QLabel* pThreadLabel =  new QLabel(TR("id_label_thread"));
	_pThreadEdit = new QLineEdit();

	QGridLayout* groupBoxThreadLayout = new QGridLayout();
	groupBoxThreadLayout->addWidget(pThreadLabel, 0, 0);
	groupBoxThreadLayout->addWidget(_pThreadEdit, 0, 1);

	QGroupBox* groupBoxThread = new QGroupBox();
	groupBoxThread->setTitle(TR("id_thread"));
	groupBoxThread->setLayout(groupBoxThreadLayout);

	//bibliotheque
	QLabel* pLibraryPathLabel =  new QLabel(TR("id_label_biblio_path"));
	_pLibraryPathEdit = new QLineEdit();
	QPushButton* buttonBiblioDirChoice = new QPushButton(TR("id_choose_btn"), this);
	connect(buttonBiblioDirChoice, SIGNAL(clicked()), this, SLOT(changeBibDir()));

	QGridLayout* groupBoxLibraryLayout = new QGridLayout();
	groupBoxLibraryLayout->addWidget(pLibraryPathLabel, 0, 0);
	groupBoxLibraryLayout->addWidget(_pLibraryPathEdit, 0, 1);
	groupBoxLibraryLayout->addWidget(buttonBiblioDirChoice, 0, 2);

	QGroupBox* groupBoxLibrary = new QGroupBox();
	groupBoxLibrary->setTitle(TR("id_biblio"));
	groupBoxLibrary->setLayout(groupBoxLibraryLayout);

	//repertoire de travail
	QLabel* pWorkDirPathLabel =  new QLabel(TR("id_label_workdir_path"));
	_pWorkDirPathEdit = new QLineEdit();
	QPushButton* buttonWorkDirChoice = new QPushButton(TR("id_choose_btn"), this);
	connect(buttonWorkDirChoice, SIGNAL(clicked()), this, SLOT(changeWorkDir()));

	QGridLayout* groupBoxWorkDirLayout = new QGridLayout();
	groupBoxWorkDirLayout->addWidget(pWorkDirPathLabel, 0, 0);
	groupBoxWorkDirLayout->addWidget(_pWorkDirPathEdit, 0, 1);
	groupBoxWorkDirLayout->addWidget(buttonWorkDirChoice, 0, 2);

	QGroupBox* groupBoxWorkDir = new QGroupBox();
	groupBoxWorkDir->setTitle(TR("id_workdir"));
	groupBoxWorkDir->setLayout(groupBoxWorkDirLayout);

	QVBoxLayout* pTabLayout = new QVBoxLayout();
	pTabLayout->addWidget(groupBoxWorkDir);
	pTabLayout->addWidget(groupBoxLibrary);
	pTabLayout->addWidget(groupBox);
	pTabLayout->addWidget(groupBoxThread);
	pTabLayout->addStretch(1);

	setLayout(pTabLayout);
}

void GeneralTab::changeBibDir()
{
	QString strDir = _pLibraryPathEdit->text();
	QFileDialog* pDialog = new QFileDialog(this, "Choose a directory" , strDir);
	pDialog->setFileMode(QFileDialog::DirectoryOnly);

	pDialog->exec();

	if (pDialog->result() == QDialog::Accepted)
	{
		QDir dir = pDialog->directory();
		_pLibraryPathEdit->setText(dir.absolutePath());
	}

	pDialog = NULL;
}

void GeneralTab::changeWorkDir()
{
	QString strDir = _pWorkDirPathEdit->text();
	QFileDialog* pDialog = new QFileDialog(this, "Choose a directory" , strDir);
	pDialog->setFileMode(QFileDialog::DirectoryOnly);

	pDialog->exec();

	if (pDialog->result() == QDialog::Accepted)
	{
		QDir dir = pDialog->directory();
		_pWorkDirPathEdit->setText(dir.absolutePath());
	}

	pDialog = NULL;
}



// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>



GeometrieTab::GeometrieTab(QWidget* parent)
	: QWidget(parent)
{
	//propriete de grilles
	QLabel* pSiteLabel =  new QLabel(TR("id_site"));
	QLabel* pBatLabel =  new QLabel(TR("id_batiment"));
	QLabel* pMacLabel =  new QLabel(TR("id_machine"));
	QLabel* pFaceLabel =  new QLabel(TR("id_face"));
	QLabel* pMailLabel =  new QLabel(TR("id_maillage"));

	QLabel* pGridDimXLabel =  new QLabel(TR("id_label_dimx_grid"));
	_pGridDimXSiteEdit = new QLineEdit();
	_pGridDimXBatEdit = new QLineEdit();
	_pGridDimXMacEdit = new QLineEdit();
	_pGridDimXFaceEdit = new QLineEdit();
	_pGridDimXMailEdit = new QLineEdit();

	QLabel* pGridDimYLabel =  new QLabel(TR("id_label_dimy_grid"));
	_pGridDimYSiteEdit = new QLineEdit();
	_pGridDimYBatEdit = new QLineEdit();
	_pGridDimYMacEdit = new QLineEdit();
	_pGridDimYFaceEdit = new QLineEdit();
	_pGridDimYMailEdit = new QLineEdit();

	QLabel* pGridStepLabel =  new QLabel(TR("id_label_step_grid"));
	_pGridStepSiteEdit = new QLineEdit();
	_pGridStepBatEdit = new QLineEdit();
	_pGridStepMacEdit = new QLineEdit();
	_pGridStepFaceEdit = new QLineEdit();
	_pGridStepMailEdit = new QLineEdit();

	QLabel* pGridMagnStepLabel =  new QLabel(TR("id_label_step_grid_magn"));
	_pGridMagnStepSiteEdit = new QLineEdit();
	_pGridMagnStepBatEdit = new QLineEdit();
	_pGridMagnStepMacEdit = new QLineEdit();
	_pGridMagnStepFaceEdit = new QLineEdit();
	_pGridMagnStepMailEdit = new QLineEdit();

	QGridLayout* groupBoxLayout = new QGridLayout();
	groupBoxLayout->addWidget(pSiteLabel, 0, 1);
	groupBoxLayout->addWidget(pBatLabel, 0, 2);
	groupBoxLayout->addWidget(pMacLabel, 0, 3);
	groupBoxLayout->addWidget(pFaceLabel, 0, 4);
	groupBoxLayout->addWidget(pMailLabel, 0, 5);

	groupBoxLayout->addWidget(pGridDimXLabel, 1, 0);
	groupBoxLayout->addWidget(_pGridDimXSiteEdit, 1, 1);
	groupBoxLayout->addWidget(_pGridDimXBatEdit, 1, 2);
	groupBoxLayout->addWidget(_pGridDimXMacEdit, 1, 3);
	groupBoxLayout->addWidget(_pGridDimXFaceEdit, 1, 4);
	groupBoxLayout->addWidget(_pGridDimXMailEdit, 1, 5);

	groupBoxLayout->addWidget(pGridDimYLabel, 2, 0);
	groupBoxLayout->addWidget(_pGridDimYSiteEdit, 2, 1);
	groupBoxLayout->addWidget(_pGridDimYBatEdit, 2, 2);
	groupBoxLayout->addWidget(_pGridDimYMacEdit, 2, 3);
	groupBoxLayout->addWidget(_pGridDimYFaceEdit, 2, 4);
	groupBoxLayout->addWidget(_pGridDimYMailEdit, 2, 5);

	groupBoxLayout->addWidget(pGridStepLabel, 3, 0);
	groupBoxLayout->addWidget(_pGridStepSiteEdit, 3, 1);
	groupBoxLayout->addWidget(_pGridStepBatEdit, 3, 2);
	groupBoxLayout->addWidget(_pGridStepMacEdit, 3, 3);
	groupBoxLayout->addWidget(_pGridStepFaceEdit, 3, 4);
	groupBoxLayout->addWidget(_pGridStepMailEdit, 3, 5);

	groupBoxLayout->addWidget(pGridMagnStepLabel, 4, 0);
	groupBoxLayout->addWidget(_pGridMagnStepSiteEdit, 4, 1);
	groupBoxLayout->addWidget(_pGridMagnStepBatEdit, 4, 2);
	groupBoxLayout->addWidget(_pGridMagnStepMacEdit, 4, 3);
	groupBoxLayout->addWidget(_pGridMagnStepFaceEdit, 4, 4);
	groupBoxLayout->addWidget(_pGridMagnStepMailEdit, 4, 5);

	QGroupBox* groupBox = new QGroupBox();
	groupBox->setTitle(TR("id_grid"));
	groupBox->setLayout(groupBoxLayout);

	//Angles
	_p15RadioButton = new QRadioButton("15");
	_p30RadioButton = new QRadioButton("30");
	_p45RadioButton = new QRadioButton("45");
	_p90RadioButton = new QRadioButton("90");

	_pAnglesButtonGroup = new QButtonGroup();
	_pAnglesButtonGroup->setExclusive(true);
	_pAnglesButtonGroup->addButton(_p15RadioButton, 0);
	_pAnglesButtonGroup->addButton(_p30RadioButton, 1);
	_pAnglesButtonGroup->addButton(_p45RadioButton, 2);
	_pAnglesButtonGroup->addButton(_p90RadioButton, 3);

	QGridLayout* groupBoxAnglesLayout = new QGridLayout();
	groupBoxAnglesLayout->addWidget(_p15RadioButton, 0, 0);
	groupBoxAnglesLayout->addWidget(_p30RadioButton, 0, 1);
	groupBoxAnglesLayout->addWidget(_p45RadioButton, 0, 2);
	groupBoxAnglesLayout->addWidget(_p90RadioButton, 0, 3);

	QGroupBox* groupBoxAngles = new QGroupBox();
	groupBoxAngles->setTitle("Angle");
	groupBoxAngles->setLayout(groupBoxAnglesLayout);

	//Geometrie
	QLabel* pSourceSize = new QLabel(TR("id_label_source_size"));
	_pSourceSizeEdit = new QLineEdit();
	QLabel* pPointControlSize = new QLabel(TR("id_label_pointcontrol_size"));
	_pPointControlSizeEdit = new QLineEdit();
	QLabel* pResoCylLabel =  new QLabel(TR("id_label_nb_face_cylinder"));
	_pResoCylEdit = new QLineEdit();
	QLabel* pSeuilDistNulLabel =  new QLabel(TR("id_label_seuil_dist_nulle"));
	_pSeuilDistNulEdit = new QLineEdit();

	QGridLayout* groupBox1Layout = new QGridLayout();
	groupBox1Layout->addWidget(pSourceSize, 0, 0);
	groupBox1Layout->addWidget(_pSourceSizeEdit, 0, 1);
	groupBox1Layout->addWidget(pPointControlSize, 1, 0);
	groupBox1Layout->addWidget(_pPointControlSizeEdit, 1, 1);
	groupBox1Layout->addWidget(pResoCylLabel, 2, 0);
	groupBox1Layout->addWidget(_pResoCylEdit, 2, 1);
	groupBox1Layout->addWidget(pSeuilDistNulLabel, 3, 0);
	groupBox1Layout->addWidget(_pSeuilDistNulEdit, 3, 1);

	QGroupBox* groupBox1 = new QGroupBox();
	groupBox1->setTitle(TR("id_geometry"));
	groupBox1->setLayout(groupBox1Layout);

	//Topographie
	QLabel* pDelaunayToleranceLabel =  new QLabel(TR("id_label_delaunay_tolerance"));
	_pDelaunayToleranceEdit = new QLineEdit();
	QLabel* pDistMinPtCrbNivLabel =  new QLabel(TR("id_label_dist_min_pts_crbniv"));
	_pDistMinPtCrbNivEdit = new QLineEdit();
	QLabel* pDefaultDimXLabel =  new QLabel(TR("id_label_def_dimx"));
	_pDefaultDimXEdit = new QLineEdit();
	QLabel* pDefaultDimYLabel =  new QLabel(TR("id_label_def_dimy"));
	_pDefaultDimYEdit = new QLineEdit();

	QGridLayout* groupBox2Layout = new QGridLayout();
	groupBox2Layout->addWidget(pDelaunayToleranceLabel, 0, 0);
	groupBox2Layout->addWidget(_pDelaunayToleranceEdit, 0, 1);
	groupBox2Layout->addWidget(pDistMinPtCrbNivLabel, 1, 0);
	groupBox2Layout->addWidget(_pDistMinPtCrbNivEdit, 1, 1);
	groupBox2Layout->addWidget(pDefaultDimXLabel, 2, 0);
	groupBox2Layout->addWidget(_pDefaultDimXEdit, 2, 1);
	groupBox2Layout->addWidget(pDefaultDimYLabel, 3, 0);
	groupBox2Layout->addWidget(_pDefaultDimYEdit, 3, 1);

	QGroupBox* groupBox2 = new QGroupBox();
	groupBox2->setTitle(TR("id_topography"));
	groupBox2->setLayout(groupBox2Layout);

	//Constructions
	QLabel* pDefaultHMurLabel =  new QLabel(TR("id_label_def_h_mur"));
	_pDefaultHMurEdit = new QLineEdit();
	QLabel* pDefaultWEcranLabel =  new QLabel(TR("id_label_def_w_ecran"));
	_pDefaultWEcranEdit = new QLineEdit();

	QGridLayout* groupBox3Layout = new QGridLayout();
	groupBox3Layout->addWidget(pDefaultHMurLabel, 0, 0);
	groupBox3Layout->addWidget(_pDefaultHMurEdit, 0, 1);
	groupBox3Layout->addWidget(pDefaultWEcranLabel, 1, 0);
	groupBox3Layout->addWidget(_pDefaultWEcranEdit, 1, 1);

	QGroupBox* groupBox3 = new QGroupBox();
	groupBox3->setTitle(TR("id_Ecran"));
	groupBox3->setLayout(groupBox3Layout);

	QVBoxLayout* pTabLayout = new QVBoxLayout;
	pTabLayout->addWidget(groupBox);
	pTabLayout->addWidget(groupBoxAngles);
	pTabLayout->addWidget(groupBox1);
	pTabLayout->addWidget(groupBox2);
	pTabLayout->addWidget(groupBox3);
	pTabLayout->addStretch(1);

	setLayout(pTabLayout);
}


pref3DTab::pref3DTab(QWidget* parent)
	: QWidget(parent)
{
	//luminosite
	QLabel* pClairLabel = new QLabel(TR("id_clair"));
	_pLightSlider = new QSlider(Qt::Horizontal);
	_pLightSlider->setRange(10, 100);
	QLabel* pSombreLabel = new QLabel(TR("id_sombre"));

	QGridLayout* groupBox1Layout = new QGridLayout();
	groupBox1Layout->addWidget(pSombreLabel, 0, 0);
	groupBox1Layout->addWidget(_pLightSlider, 0, 1);
	groupBox1Layout->addWidget(pClairLabel, 0, 2);

	QGroupBox* groupBox1 = new QGroupBox();
	groupBox1->setTitle(TR("id_luminosite"));
	groupBox1->setLayout(groupBox1Layout);

	//Selection des objets
	QLabel* pPrecisLabel = new QLabel(TR("id_precision"));
	_pPrecisPickEdit = new QLineEdit();
	QGridLayout* groupPrecisLayout = new QGridLayout();
	groupPrecisLayout->addWidget(pPrecisLabel, 0, 0);
	groupPrecisLayout->addWidget(_pPrecisPickEdit, 0, 1);

	_pViewOnlyHighlightCheckBox = new QCheckBox();
	_pViewOnlyHighlightCheckBox->setText(TR("id_label_view_only_highlight"));

	_pCenterOnLocateCheckBox = new QCheckBox();
	_pCenterOnLocateCheckBox->setText(TR("id_label_center_on_locate"));

	QGridLayout* groupBox2Layout = new QGridLayout();
	groupBox2Layout->addLayout(groupPrecisLayout, 0, 0);
	groupBox2Layout->addWidget(_pViewOnlyHighlightCheckBox, 1, 0);
	groupBox2Layout->addWidget(_pCenterOnLocateCheckBox, 2, 0);

	QGroupBox* groupBox2 = new QGroupBox();
	groupBox2->setTitle(TR("id_picking"));
	groupBox2->setLayout(groupBox2Layout);

	//Deplacement de la camera
	_pWireframeOnMovingCameraCheckBox = new QCheckBox();
	_pWireframeOnMovingCameraCheckBox->setText(TR("id_label_wireframe_on_moving_camera"));

	QGridLayout* groupBox2bLayout = new QGridLayout();
	groupBox2bLayout->addWidget(_pWireframeOnMovingCameraCheckBox, 1, 0);

	QGroupBox* groupBox2b = new QGroupBox();
	groupBox2b->setTitle(TR("id_wireframe"));
	groupBox2b->setLayout(groupBox2bLayout);

	//Zoom
	QLabel* pZoomStepLabel = new QLabel(TR("id_step"));
	_pZoomStepEdit = new QLineEdit();

	QGridLayout* groupBox3Layout = new QGridLayout();
	groupBox3Layout->addWidget(pZoomStepLabel, 0, 0);
	groupBox3Layout->addWidget(_pZoomStepEdit, 0, 1);

	QGroupBox* groupBox3 = new QGroupBox();
	groupBox3->setTitle(TR("id_zoom"));
	groupBox3->setLayout(groupBox3Layout);

	//Fonctions attachees a la souris
	QLabel* p2DLabel = new QLabel(TR("id_2D"));
	QLabel* p3DLabel = new QLabel(TR("id_3D"));
	QLabel* pLeftLabel2D = new QLabel(TR("id_left_button"));
	_pLeftButton2DComboBox = new QComboBox();
	_pLeftButton2DComboBox->insertItem(0, TR("id_zoom"));
	_pLeftButton2DComboBox->insertItem(1, TR("id_translate"));
	QLabel* pMiddleLabel2D = new QLabel(TR("id_middle_button"));
	_pMiddleButton2DComboBox = new QComboBox();
	_pMiddleButton2DComboBox->insertItem(0, TR("id_zoom"));
	_pMiddleButton2DComboBox->insertItem(1, TR("id_translate"));
	QLabel* pRightLabel2D = new QLabel(TR("id_right_button"));
	_pRightButton2DComboBox = new QComboBox();
	_pRightButton2DComboBox->insertItem(0, TR("id_zoom"));
	_pRightButton2DComboBox->insertItem(1, TR("id_translate"));
	_pLeftButton3DComboBox = new QComboBox();
	_pLeftButton3DComboBox->insertItem(0, TR("id_zoom"));
	_pLeftButton3DComboBox->insertItem(1, TR("id_translate"));
	_pLeftButton3DComboBox->insertItem(2, TR("id_roll"));
	_pLeftButton3DComboBox->insertItem(3, TR("id_rotate"));
	QLabel* pMiddleLabel3D = new QLabel(TR("id_middle_button"));
	_pMiddleButton3DComboBox = new QComboBox();
	_pMiddleButton3DComboBox->insertItem(0, TR("id_zoom"));
	_pMiddleButton3DComboBox->insertItem(1, TR("id_translate"));
	_pMiddleButton3DComboBox->insertItem(2, TR("id_roll"));
	_pMiddleButton3DComboBox->insertItem(3, TR("id_rotate"));
	_pRightButton3DComboBox = new QComboBox();
	_pRightButton3DComboBox->insertItem(0, TR("id_zoom"));
	_pRightButton3DComboBox->insertItem(1, TR("id_translate"));
	_pRightButton3DComboBox->insertItem(2, TR("id_roll"));
	_pRightButton3DComboBox->insertItem(3, TR("id_rotate"));

	QGridLayout* groupBoxLayout = new QGridLayout();
	groupBoxLayout->addWidget(p2DLabel, 0, 1);
	groupBoxLayout->addWidget(p3DLabel, 0, 2);
	groupBoxLayout->addWidget(pLeftLabel2D, 1, 0);
	groupBoxLayout->addWidget(_pLeftButton2DComboBox, 1, 1);
	groupBoxLayout->addWidget(pMiddleLabel2D, 2, 0);
	groupBoxLayout->addWidget(_pMiddleButton2DComboBox, 2, 1);
	groupBoxLayout->addWidget(pRightLabel2D, 3, 0);
	groupBoxLayout->addWidget(_pRightButton2DComboBox, 3, 1);
	groupBoxLayout->addWidget(_pLeftButton3DComboBox, 1, 2);
	groupBoxLayout->addWidget(pMiddleLabel3D, 2, 2);
	groupBoxLayout->addWidget(_pMiddleButton3DComboBox, 2, 2);
	groupBoxLayout->addWidget(_pRightButton3DComboBox, 3, 2);

	QGroupBox* groupBox = new QGroupBox();
	groupBox->setTitle(TR("id_mouse_button_functions"));
	groupBox->setLayout(groupBoxLayout);

	//Fonctions souris "shift"
	QLabel* p2DLabelSht = new QLabel(TR("id_2D"));
	QLabel* p3DLabelSht = new QLabel(TR("id_3D"));
	QLabel* pLeftLabel2DSht = new QLabel(TR("id_left_button"));
	_pLeftButton2DShtComboBox = new QComboBox();
	_pLeftButton2DShtComboBox->insertItem(0, TR("id_zoom"));
	_pLeftButton2DShtComboBox->insertItem(1, TR("id_translate"));
	QLabel* pRightLabel2DSht = new QLabel(TR("id_right_button"));
	_pRightButton2DShtComboBox = new QComboBox();
	_pRightButton2DShtComboBox->insertItem(0, TR("id_zoom"));
	_pRightButton2DShtComboBox->insertItem(1, TR("id_translate"));
	_pLeftButton3DShtComboBox = new QComboBox();
	_pLeftButton3DShtComboBox->insertItem(0, TR("id_zoom"));
	_pLeftButton3DShtComboBox->insertItem(1, TR("id_translate"));
	_pLeftButton3DShtComboBox->insertItem(2, TR("id_roll"));
	_pLeftButton3DShtComboBox->insertItem(3, TR("id_rotate"));
	_pRightButton3DShtComboBox = new QComboBox();
	_pRightButton3DShtComboBox->insertItem(0, TR("id_zoom"));
	_pRightButton3DShtComboBox->insertItem(1, TR("id_translate"));
	_pRightButton3DShtComboBox->insertItem(2, TR("id_roll"));
	_pRightButton3DShtComboBox->insertItem(3, TR("id_rotate"));

	QGridLayout* groupBox4Layout = new QGridLayout();
	groupBox4Layout->addWidget(p2DLabelSht, 0, 1);
	groupBox4Layout->addWidget(p3DLabelSht, 0, 2);
	groupBox4Layout->addWidget(pLeftLabel2DSht, 1, 0);
	groupBox4Layout->addWidget(_pLeftButton2DShtComboBox, 1, 1);
	groupBox4Layout->addWidget(pRightLabel2DSht, 2, 0);
	groupBox4Layout->addWidget(_pRightButton2DShtComboBox, 2, 1);
	groupBox4Layout->addWidget(_pLeftButton3DShtComboBox, 1, 2);
	groupBox4Layout->addWidget(_pRightButton3DShtComboBox, 2, 2);

	QGroupBox* groupBox4 = new QGroupBox();
	groupBox4->setTitle(TR("id_mouse_button_functions_sht"));
	groupBox4->setLayout(groupBox4Layout);

	//Camera pas a pas
	QLabel* pStepLabelSht = new QLabel(TR("id_camera_step"));
	QLabel* pTranslateLabelSht = new QLabel(TR("id_translate"));
	QLabel* pRotateLabelSht = new QLabel(TR("id_rotate"));
	QLabel* pZoomLabelSht = new QLabel(TR("id_zoom"));
	_pCameraStepTranslateEdit = new QLineEdit();
	_pCameraStepRotateEdit = new QLineEdit();
	_pCameraStepZoomEdit = new QLineEdit();

	QGridLayout* groupBox5Layout = new QGridLayout();
	groupBox5Layout->addWidget(pStepLabelSht, 1, 0);
	groupBox5Layout->addWidget(pTranslateLabelSht, 0, 1);
	groupBox5Layout->addWidget(pRotateLabelSht, 0, 2);
	groupBox5Layout->addWidget(pZoomLabelSht, 0, 3);
	groupBox5Layout->addWidget(_pCameraStepTranslateEdit, 1, 1);
	groupBox5Layout->addWidget(_pCameraStepRotateEdit, 1, 2);
	groupBox5Layout->addWidget(_pCameraStepZoomEdit, 1, 3);

	QGroupBox* groupBox5 = new QGroupBox();
	groupBox5->setTitle(TR("id_camera_step_title"));
	groupBox5->setLayout(groupBox5Layout);

	QVBoxLayout* pTabLayout = new QVBoxLayout();
	pTabLayout->addWidget(groupBox1);
	pTabLayout->addWidget(groupBox2);
	pTabLayout->addWidget(groupBox2b);
	pTabLayout->addWidget(groupBox3);
	pTabLayout->addWidget(groupBox);
	pTabLayout->addWidget(groupBox4);
	pTabLayout->addWidget(groupBox5);
	pTabLayout->addStretch(1);

	setLayout(pTabLayout);
}

ColorsTab::ColorsTab(QWidget* parent)
	: QWidget(parent)
{
	// RENDERER
	QLabel* pBackgroundLabel = new QLabel(TR("id_background_color"));
	_pBackgroundColorToolButton = new TYToolButton();
	_pBackgroundColorToolButton->setFixedSize(60, 30);
	QLabel* pGridLabel = new QLabel(TR("id_grid_color"));
	_pGridColorToolButton = new TYToolButton();
	_pGridColorToolButton->setFixedSize(60, 30);
	QLabel* pPoliceColorLabel = new QLabel(TR("id_police_color"));
	_pPoliceColorToolButton = new TYToolButton();
	_pPoliceColorToolButton->setFixedSize(60, 30);

	QGridLayout* groupBoxLayout = new QGridLayout();
	groupBoxLayout->addWidget(pBackgroundLabel, 0, 0);
	groupBoxLayout->addWidget(_pBackgroundColorToolButton, 0, 1);
	groupBoxLayout->addWidget(pGridLabel, 1, 0);
	groupBoxLayout->addWidget(_pGridColorToolButton, 1, 1);
	groupBoxLayout->addWidget(pPoliceColorLabel, 2, 0);
	groupBoxLayout->addWidget(_pPoliceColorToolButton, 2, 1);

	QGroupBox* groupBox = new QGroupBox();
	groupBox->setTitle(TR("id_renderer"));
	groupBox->setLayout(groupBoxLayout);

	// TOPOGRAPHIE
	QLabel* pTerrainColorLabel = new QLabel(TR("id_terrain_color"));
	_pTerrainColorToolButton = new TYToolButton();
	_pTerrainColorToolButton->setFixedSize(60, 30);
	QLabel* pPlanEauColorLabel = new QLabel(TR("id_planeau_color"));
	_pPlanEauColorToolButton = new TYToolButton();
	_pPlanEauColorToolButton->setFixedSize(60, 30);
	QLabel* pCrsEauColorLabel = new QLabel(TR("id_crseau_color"));
	_pCrsEauColorToolButton = new TYToolButton();
	_pCrsEauColorToolButton->setFixedSize(60, 30);
	QLabel* pCrbNivColorLabel = new QLabel(TR("id_crbniv_color"));
	_pCrbNivColorToolButton = new TYToolButton();
	_pCrbNivColorToolButton->setFixedSize(60, 30);
	QLabel* pVegColorLabel = new QLabel(TR("id_veg_color"));
	_pVegColorToolButton = new TYToolButton();
	_pVegColorToolButton->setFixedSize(60, 30);

	QGridLayout* groupBox1Layout = new QGridLayout();
	groupBox1Layout->addWidget(pTerrainColorLabel, 0, 0);
	groupBox1Layout->addWidget(_pTerrainColorToolButton, 0, 1);
	groupBox1Layout->addWidget(pPlanEauColorLabel, 1, 0);
	groupBox1Layout->addWidget(_pPlanEauColorToolButton, 1, 1);
	groupBox1Layout->addWidget(pCrsEauColorLabel, 2, 0);
	groupBox1Layout->addWidget(_pCrsEauColorToolButton, 2, 1);
	groupBox1Layout->addWidget(pCrbNivColorLabel, 3, 0);
	groupBox1Layout->addWidget(_pCrbNivColorToolButton, 3, 1);
	groupBox1Layout->addWidget(pVegColorLabel, 4, 0);
	groupBox1Layout->addWidget(_pVegColorToolButton, 4, 1);

	QGroupBox* groupBox1 = new QGroupBox();
	groupBox1->setTitle(TR("id_topography"));
	groupBox1->setLayout(groupBox1Layout);

	// MACHINES
	QLabel* pCylindreColorLabel = new QLabel(TR("id_cylindre_color"));
	_pCylindreColorToolButton = new TYToolButton();
	_pCylindreColorToolButton->setFixedSize(60, 30);
	QLabel* pBoxColorLabel = new QLabel(TR("id_box_color"));
	_pBoxColorToolButton = new TYToolButton();
	_pBoxColorToolButton->setFixedSize(60, 30);
	QLabel* pSubRectColorLabel = new QLabel(TR("id_bouche_color"));
	_pSubRectColorToolButton = new TYToolButton();
	_pSubRectColorToolButton->setFixedSize(60, 30);

	QGridLayout* groupBox2Layout = new QGridLayout();
	groupBox2Layout->addWidget(pCylindreColorLabel, 0, 0);
	groupBox2Layout->addWidget(_pCylindreColorToolButton, 0, 1);
	groupBox2Layout->addWidget(pBoxColorLabel, 1, 0);
	groupBox2Layout->addWidget(_pBoxColorToolButton, 1, 1);
	groupBox2Layout->addWidget(pSubRectColorLabel, 2, 0);
	groupBox2Layout->addWidget(_pSubRectColorToolButton, 2, 1);

	QGroupBox* groupBox2 = new QGroupBox();
	groupBox2->setTitle(TR("id_machine"));
	groupBox2->setLayout(groupBox2Layout);

	// BATIMENTS
	QLabel* pMurColorLabel = new QLabel(TR("id_mur_color"));
	_pMurColorToolButton = new TYToolButton();
	_pMurColorToolButton->setFixedSize(60, 30);
	QLabel* pDalleColorLabel = new QLabel(TR("id_dalle_color"));
	_pDalleColorToolButton = new TYToolButton();
	_pDalleColorToolButton->setFixedSize(60, 30);
	QLabel* pEcranColorLabel = new QLabel(TR("id_ecran_color"));
	_pEcranColorToolButton = new TYToolButton();
	_pEcranColorToolButton->setFixedSize(60, 30);

	QGridLayout* groupBox3Layout = new QGridLayout();
	groupBox3Layout->addWidget(pMurColorLabel, 0, 0);
	groupBox3Layout->addWidget(_pMurColorToolButton, 0, 1);
	groupBox3Layout->addWidget(pDalleColorLabel, 1, 0);
	groupBox3Layout->addWidget(_pDalleColorToolButton, 1, 1);
	groupBox3Layout->addWidget(pEcranColorLabel, 2, 0);
	groupBox3Layout->addWidget(_pEcranColorToolButton, 2, 1);

	QGroupBox* groupBox3 = new QGroupBox();
	groupBox3->setTitle(TR("id_batiment"));
	groupBox3->setLayout(groupBox3Layout);

	// INFRASTRUCTURE
	QLabel* pRouteColorLabel = new QLabel(TR("id_route_color"));
	_pRouteColorToolButton = new TYToolButton();
	_pRouteColorToolButton->setFixedSize(60, 30);
	QLabel* pResTranspColorLabel = new QLabel(TR("id_restransp_color"));
	_pResTranspColorToolButton = new TYToolButton();
	_pResTranspColorToolButton->setFixedSize(60, 30);

	QGridLayout* groupBox5Layout = new QGridLayout();
	groupBox5Layout->addWidget(pRouteColorLabel, 0, 0);
	groupBox5Layout->addWidget(_pRouteColorToolButton, 0, 1);
	groupBox5Layout->addWidget(pResTranspColorLabel, 1, 0);
	groupBox5Layout->addWidget(_pResTranspColorToolButton, 1, 1);

	QGroupBox* groupBox5 = new QGroupBox();
	groupBox5->setTitle(TR("id_infrastructure"));
	groupBox5->setLayout(groupBox5Layout);

	// CALCUL
	QLabel* pPointControlColorLabel = new QLabel(TR("id_pointcontrol_color"));
	_pPointControlColorToolButton = new TYToolButton();
	_pPointControlColorToolButton->setFixedSize(60, 30);

	QGridLayout* groupBox7Layout = new QGridLayout();
	groupBox7Layout->addWidget(pPointControlColorLabel, 0, 0);
	groupBox7Layout->addWidget(_pPointControlColorToolButton, 0, 1);

	QGroupBox* groupBox7 = new QGroupBox();
	groupBox7->setTitle(TR("id_group_calcul_name"));
	groupBox7->setLayout(groupBox7Layout);

	// SOURCE
	QLabel* pSourcePonctColorLabel = new QLabel(TR("id_sourceponct_color"));
	_pSourcePonctColorToolButton = new TYToolButton();
	_pSourcePonctColorToolButton->setFixedSize(60, 30);

	QGridLayout* groupBox8Layout = new QGridLayout();
	groupBox8Layout->addWidget(pSourcePonctColorLabel, 0, 0);
	groupBox8Layout->addWidget(_pSourcePonctColorToolButton, 0, 1);

	QGroupBox* groupBox8 = new QGroupBox();
	groupBox8->setTitle(TR("id_group_source_name"));
	groupBox8->setLayout(groupBox8Layout);

	// ALTIMETRIE
	_pAltiColorMinToolButton = new TYToolButton();
	_pAltiColorMinToolButton->setFixedSize(60, 30);
	QFrame* pLine = new QFrame();
	pLine->setProperty("frameShape", (int)QFrame::HLine);
	pLine->setFrameShadow(QFrame::Sunken);
	pLine->setFrameShape(QFrame::HLine);
	_pAltiColorMaxToolButton = new TYToolButton();
	_pAltiColorMaxToolButton->setFixedSize(60, 30);

	QGridLayout* groupBox6Layout = new QGridLayout();
	groupBox6Layout->addWidget(_pAltiColorMinToolButton, 0, 0);
	groupBox6Layout->addWidget(pLine, 0, 1, 1, 2);
	groupBox6Layout->addWidget(_pAltiColorMaxToolButton, 0, 3);

	QGroupBox* groupBox6 = new QGroupBox();
	groupBox6->setTitle(TR("id_alti_color_map"));
	groupBox6->setLayout(groupBox6Layout);

	// OPACITE
	QLabel* pWinOpacityLabel = new QLabel(TR("id_win_opacity"));
	_pWinOpacitySlider = new QSlider(Qt::Horizontal);
	_pWinOpacitySlider->setMinimum(0);
	_pWinOpacitySlider->setMaximum(100);
	_pWinOpacitySlider->setPageStep(1);
	_pWinOpacitySlider->setValue(50);
	QLabel* pAltOpacityLabel = new QLabel(TR("id_atm_opacity"));
	_pAltOpacitySlider = new QSlider(Qt::Horizontal);
	_pAltOpacitySlider->setMinimum(0);
	_pAltOpacitySlider->setMaximum(100);
	_pAltOpacitySlider->setPageStep(1);
	_pAltOpacitySlider->setValue(50);
	QLabel* pMailOpacityLabel = new QLabel(TR("id_maillage_opacity"));
	_pMailOpacitySlider = new QSlider(Qt::Horizontal);
	_pMailOpacitySlider->setMinimum(0);
	_pMailOpacitySlider->setMaximum(100);
	_pMailOpacitySlider->setPageStep(1);
	_pMailOpacitySlider->setValue(50);

	QGridLayout* groupBox4Layout = new QGridLayout();
	groupBox4Layout->addWidget(pWinOpacityLabel, 0, 0);
	groupBox4Layout->addWidget(_pWinOpacitySlider, 1, 0);
	groupBox4Layout->addWidget(pAltOpacityLabel, 2, 0);
	groupBox4Layout->addWidget(_pAltOpacitySlider, 3, 0);
	groupBox4Layout->addWidget(pMailOpacityLabel, 4, 0);
	groupBox4Layout->addWidget(_pMailOpacitySlider, 5, 0);

	QGroupBox* groupBox4 = new QGroupBox();
	groupBox4->setTitle(TR("id_opacity"));
	groupBox4->setLayout(groupBox4Layout);

	QGridLayout* pTabLayout = new QGridLayout();
	pTabLayout->addWidget(groupBox, 0, 0);
	pTabLayout->addWidget(groupBox1, 1, 0, 2, 1);
	pTabLayout->addWidget(groupBox2, 0, 1);
	pTabLayout->addWidget(groupBox3, 1, 1);
	pTabLayout->addWidget(groupBox5, 2, 1);
	pTabLayout->addWidget(groupBox7, 3, 0);
	pTabLayout->addWidget(groupBox8, 3, 1);
	pTabLayout->addWidget(groupBox6, 4, 0, 1, 2);
	pTabLayout->addWidget(groupBox4, 5, 0, 1, 2);

	setLayout(pTabLayout);
}

AcoustiqueTab::AcoustiqueTab(QWidget* parent)
	: QWidget(parent)
{
	_pSpectreAttMatConst = new TYSpectre();
	_pSpectreAttMatConst->setDefaultValue(15);
	_pSpectreAbsoMatConst = new TYSpectre();
	_pSpectreAbsoMatConst->setDefaultValue(0.2);

	_pMaillage = new TYMaillage();

	//Sol
	QLabel* pLabelResistivite = new QLabel(TR("id_resistivite_label"));
	_pLineEditResistivite = new QLineEdit();
	QLabel* pUnitResis = new QLabel(TR("id_unite_resistivite"));
	QLabel* pLabelEpaisseur = new QLabel(TR("id_epaisseur_label"));
	_pLineEditEpaisseur = new QLineEdit();
	QLabel* pUnitEpais = new QLabel(TR("id_unite_epaisseur"));
	QLabel* pLabelEcartType = new QLabel(TR("id_ecarttype_label"));
	_pLineEditEcartType = new QLineEdit();
	QLabel* pUnitET = new QLabel(TR("id_unite_ecarttype"));
	QLabel* pLabelLongueur = new QLabel(TR("id_longueur_label"));
	_pLineEditLongueur = new QLineEdit();
	QLabel* pUnitLong = new QLabel(TR("id_unite_longueur"));
	QGridLayout* groupBoxResistEpaiLayout = new QGridLayout();
	groupBoxResistEpaiLayout->addWidget(pLabelResistivite, 0, 0);
	groupBoxResistEpaiLayout->addWidget(_pLineEditResistivite, 0, 1);
	groupBoxResistEpaiLayout->addWidget(pUnitResis, 0, 2);
	groupBoxResistEpaiLayout->addWidget(pLabelEpaisseur, 1, 0);
	groupBoxResistEpaiLayout->addWidget(_pLineEditEpaisseur, 1, 1);
	groupBoxResistEpaiLayout->addWidget(pUnitEpais, 1, 2);
	groupBoxResistEpaiLayout->addWidget(pLabelEcartType, 2, 0);
	groupBoxResistEpaiLayout->addWidget(_pLineEditEcartType, 2, 1);
	groupBoxResistEpaiLayout->addWidget(pUnitET, 2, 2);
	groupBoxResistEpaiLayout->addWidget(pLabelLongueur, 3, 0);
	groupBoxResistEpaiLayout->addWidget(_pLineEditLongueur, 3, 1);
	groupBoxResistEpaiLayout->addWidget(pUnitLong, 3, 2);

	QPushButton* pPushButtonResistivite = new QPushButton(TR("id_edit_resistivite"));
	connect(pPushButtonResistivite, SIGNAL(clicked()), this, SLOT(editResistivite()));

	QGridLayout* groupBox1Layout = new QGridLayout();
	groupBox1Layout->addLayout(groupBoxResistEpaiLayout, 0, 0);
	groupBox1Layout->addWidget(pPushButtonResistivite, 1, 0);

	QGroupBox* groupBox1 = new QGroupBox();
	groupBox1->setTitle(TR("id_default_sol"));
	groupBox1->setLayout(groupBox1Layout);

	//Materiau de construction
	QLabel* pLabelMasseVol = new QLabel(TR("id_masse_vol_label"));
	_pLineEditMasseVol = new QLineEdit();
	QLabel* pUnitMasseVol = new QLabel(TR("id_unite_masse_vol"));
	QGridLayout* groupBoxMasseLayout = new QGridLayout();
	groupBoxMasseLayout->addWidget(pLabelMasseVol, 0, 0);
	groupBoxMasseLayout->addWidget(_pLineEditMasseVol, 0, 1);
	groupBoxMasseLayout->addWidget(pUnitMasseVol, 0, 2);

	QPushButton* pPushButtonSpectreAbso = new QPushButton(TR("id_edit_spectre_abso"));
	QPushButton* pPushButtonSpectreAtt = new QPushButton(TR("id_edit_spectre_att"));
	connect(pPushButtonSpectreAbso, SIGNAL(clicked()), this, SLOT(editSpectreAbso()));
	connect(pPushButtonSpectreAtt, SIGNAL(clicked()), this, SLOT(editSpectreAtt()));
	QGridLayout* groupBoxSpectreLayout = new QGridLayout();
	groupBoxSpectreLayout->addWidget(pPushButtonSpectreAbso, 0, 0);
	groupBoxSpectreLayout->addWidget(pPushButtonSpectreAtt, 0, 1);

	QGridLayout* groupBox3Layout = new QGridLayout();
	groupBox3Layout->addLayout(groupBoxMasseLayout, 0, 0);
	groupBox3Layout->addLayout(groupBoxSpectreLayout, 1, 0);

	QGroupBox* groupBox3 = new QGroupBox();
	groupBox3->setTitle(TR("id_default_materiau"));
	groupBox3->setLayout(groupBox3Layout);

	//Maillage
	QPushButton* pPushButtonMaillage = new QPushButton(TR("id_edit_maillage"));
	QObject::connect(pPushButtonMaillage, SIGNAL(clicked()), this, SLOT(editMaillage()));

	QGridLayout* groupBox4Layout = new QGridLayout();
	groupBox4Layout->addWidget(pPushButtonMaillage, 0, 0);

	QGroupBox* groupBox4 = new QGroupBox();
	groupBox4->setTitle(TR("id_default_maillage"));
	groupBox4->setLayout(groupBox4Layout);

	QVBoxLayout* pTabLayout = new QVBoxLayout();
	pTabLayout->addWidget(groupBox1);
	pTabLayout->addWidget(groupBox3);
	pTabLayout->addWidget(groupBox4);
	pTabLayout->addStretch(1);

	setLayout(pTabLayout);
}

AcoustiqueTab::~AcoustiqueTab()
{
	delete _pSpectreAttMatConst;
	delete _pSpectreAbsoMatConst;
	delete _pMaillage;
}

void AcoustiqueTab::editResistivite()
{
	TYSolResistanceDialog* pDialog = new TYSolResistanceDialog(this, _pLineEditResistivite->text().toDouble());

	int ret = pDialog->exec();

	if (ret == QDialog::Accepted)
	{
		_pLineEditResistivite->setText(QString().setNum(pDialog->getResistivite()));
	}
}

void AcoustiqueTab::editSpectreAtt()
{
	_pSpectreAttMatConst->edit(this);
}

void AcoustiqueTab::editSpectreAbso()
{
	_pSpectreAbsoMatConst->edit(this);
}

void AcoustiqueTab::editMaillage()
{
	_pMaillage->edit(this);
}

void TYPreferenceDialog::loadPreferences()
{
	// Directory specifique aux preferences traitees ici
	QString oldDir = TYPreferenceManager::getCurrentDirectory();
	TYPreferenceManager::setCurrentDirectory(TYDIRPREFERENCEMANAGER);

	// General
	if (TYPreferenceManager::exists("HistoSize"))
	{
		((GeneralTab*)tabWidget->widget(4))->_pHistoSizeEdit->setText(QString().setNum(TYPreferenceManager::getInt("HistoSize")));
	}
	else
	{
		((GeneralTab*)tabWidget->widget(4))->_pHistoSizeEdit->setText(QString().setNum(10));
	}

	if (TYPreferenceManager::exists("BiblioPath"))
	{
		((GeneralTab*)tabWidget->widget(4))->_pLibraryPathEdit->setText(TYPreferenceManager::getString("BiblioPath"));
	}
	else
	{
		// Recuperation du dossier par defaut contenant la bibliotheque
		QString dirPath = QDir::convertSeparators(getTYApp()->tympanUserDir() + "/library/");
		((GeneralTab*)tabWidget->widget(4))->_pLibraryPathEdit->setText(dirPath);
	}




	if (TYPreferenceManager::exists("WorkDirPath"))
	{
		((GeneralTab*)tabWidget->widget(4))->_pWorkDirPathEdit->setText(TYPreferenceManager::getString("WorkDirPath"));
	}
	else
	{
		// Recuperation du dossier par defaut contenant la bibliotheque
		QString dirPath = QDir::convertSeparators(getTYApp()->getSettingsDir() + "/echange/");
		((GeneralTab*)tabWidget->widget(4))->_pWorkDirPathEdit->setText(dirPath);
	}






	// Geometrie
	if (TYPreferenceManager::exists("GridDimXSite"))
	{
		((GeometrieTab*)tabWidget->widget(0))->_pGridDimXSiteEdit->setText(QString().setNum(TYPreferenceManager::getFloat("GridDimXSite")));
	}
	if (TYPreferenceManager::exists("GridDimYSite"))
	{
		((GeometrieTab*)tabWidget->widget(0))->_pGridDimYSiteEdit->setText(QString().setNum(TYPreferenceManager::getFloat("GridDimYSite")));
	}
	if (TYPreferenceManager::exists("GridStepSite"))
	{
		((GeometrieTab*)tabWidget->widget(0))->_pGridStepSiteEdit->setText(QString().setNum(TYPreferenceManager::getFloat("GridStepSite")));
	}
	if (TYPreferenceManager::exists("GridMagnStepSite"))
	{
		((GeometrieTab*)tabWidget->widget(0))->_pGridMagnStepSiteEdit->setText(QString().setNum(TYPreferenceManager::getFloat("GridMagnStepSite")));
	}

	if (TYPreferenceManager::exists("GridDimXBatiment"))
	{
		((GeometrieTab*)tabWidget->widget(0))->_pGridDimXBatEdit->setText(QString().setNum(TYPreferenceManager::getFloat("GridDimXBatiment")));
	}
	if (TYPreferenceManager::exists("GridDimYBatiment"))
	{
		((GeometrieTab*)tabWidget->widget(0))->_pGridDimYBatEdit->setText(QString().setNum(TYPreferenceManager::getFloat("GridDimYBatiment")));
	}
	if (TYPreferenceManager::exists("GridStepBatiment"))
	{
		((GeometrieTab*)tabWidget->widget(0))->_pGridStepBatEdit->setText(QString().setNum(TYPreferenceManager::getFloat("GridStepBatiment")));
	}
	if (TYPreferenceManager::exists("GridMagnStepBatiment"))
	{
		((GeometrieTab*)tabWidget->widget(0))->_pGridMagnStepBatEdit->setText(QString().setNum(TYPreferenceManager::getFloat("GridMagnStepBatiment")));
	}

	if (TYPreferenceManager::exists("GridDimXMachine"))
	{
		((GeometrieTab*)tabWidget->widget(0))->_pGridDimXMacEdit->setText(QString().setNum(TYPreferenceManager::getFloat("GridDimXMachine")));
	}
	if (TYPreferenceManager::exists("GridDimYMachine"))
	{
		((GeometrieTab*)tabWidget->widget(0))->_pGridDimYMacEdit->setText(QString().setNum(TYPreferenceManager::getFloat("GridDimYMachine")));
	}
	if (TYPreferenceManager::exists("GridStepMachine"))
	{
		((GeometrieTab*)tabWidget->widget(0))->_pGridStepMacEdit->setText(QString().setNum(TYPreferenceManager::getFloat("GridStepMachine")));
	}
	if (TYPreferenceManager::exists("GridMagnStepMachine"))
	{
		((GeometrieTab*)tabWidget->widget(0))->_pGridMagnStepMacEdit->setText(QString().setNum(TYPreferenceManager::getFloat("GridMagnStepMachine")));
	}

	if (TYPreferenceManager::exists("GridDimXFace"))
	{
		((GeometrieTab*)tabWidget->widget(0))->_pGridDimXFaceEdit->setText(QString().setNum(TYPreferenceManager::getFloat("GridDimXFace")));
	}
	if (TYPreferenceManager::exists("GridDimYFace"))
	{
		((GeometrieTab*)tabWidget->widget(0))->_pGridDimYFaceEdit->setText(QString().setNum(TYPreferenceManager::getFloat("GridDimYFace")));
	}
	if (TYPreferenceManager::exists("GridStepFace"))
	{
		((GeometrieTab*)tabWidget->widget(0))->_pGridStepFaceEdit->setText(QString().setNum(TYPreferenceManager::getFloat("GridStepFace")));
	}
	if (TYPreferenceManager::exists("GridMagnStepFace"))
	{
		((GeometrieTab*)tabWidget->widget(0))->_pGridMagnStepFaceEdit->setText(QString().setNum(TYPreferenceManager::getFloat("GridMagnStepFace")));
	}

	if (TYPreferenceManager::exists("GridDimXMaillage"))
	{
		((GeometrieTab*)tabWidget->widget(0))->_pGridDimXMailEdit->setText(QString().setNum(TYPreferenceManager::getFloat("GridDimXMaillage")));
	}
	if (TYPreferenceManager::exists("GridDimYMaillage"))
	{
		((GeometrieTab*)tabWidget->widget(0))->_pGridDimYMailEdit->setText(QString().setNum(TYPreferenceManager::getFloat("GridDimYMaillage")));
	}
	if (TYPreferenceManager::exists("GridStepMaillage"))
	{
		((GeometrieTab*)tabWidget->widget(0))->_pGridStepMailEdit->setText(QString().setNum(TYPreferenceManager::getFloat("GridStepMaillage")));
	}
	if (TYPreferenceManager::exists("GridMagnStepMaillage"))
	{
		((GeometrieTab*)tabWidget->widget(0))->_pGridMagnStepMailEdit->setText(QString().setNum(TYPreferenceManager::getFloat("GridMagnStepMaillage")));
	}

	if (TYPreferenceManager::exists("Angle"))
	{
		switch (ROUND(TYPreferenceManager::getFloat("Angle")))
		{
		default:
		case 15:
			{
				((GeometrieTab*)tabWidget->widget(0))->_p15RadioButton->setChecked(true);
			}
			break;
		case 30:
			{
				((GeometrieTab*)tabWidget->widget(0))->_p30RadioButton->setChecked(true);
			}
			break;
		case 45:
			{
				((GeometrieTab*)tabWidget->widget(0))->_p45RadioButton->setChecked(true);
			}
			break;
		case 90:
			{
				((GeometrieTab*)tabWidget->widget(0))->_p90RadioButton->setChecked(true);
			}
			break;
		}
	}

	// Taille des sources
	if (TYPreferenceManager::exists("UserSrcPonctGraphicSize"))
	{
		((GeometrieTab*)tabWidget->widget(0))->_pSourceSizeEdit->setText(QString().setNum(TYPreferenceManager::getFloat("UserSrcPonctGraphicSize")));
	}
	// Taille des points de controles
	if (TYPreferenceManager::exists("PointControlGraphicSize"))
	{
		((GeometrieTab*)tabWidget->widget(0))->_pPointControlSizeEdit->setText(QString().setNum(TYPreferenceManager::getFloat("PointControlGraphicSize")));
	}

	if (TYPreferenceManager::exists("ResolutionCircle"))
	{
		((GeometrieTab*)tabWidget->widget(0))->_pResoCylEdit->setText(QString().setNum(TYPreferenceManager::getFloat("ResolutionCircle")));
	}
	if (TYPreferenceManager::exists("DistMinPtCrbNiv"))
	{
		((GeometrieTab*)tabWidget->widget(0))->_pDistMinPtCrbNivEdit->setText(QString().setNum(TYPreferenceManager::getFloat("DistMinPtCrbNiv")));
	}
	if (TYPreferenceManager::exists("DelaunayTolerance"))
	{
		((GeometrieTab*)tabWidget->widget(0))->_pDelaunayToleranceEdit->setText(QString().setNum(TYPreferenceManager::getFloat("DelaunayTolerance")));
	}
	if (TYPreferenceManager::exists("DefaultDimX"))
	{
		((GeometrieTab*)tabWidget->widget(0))->_pDefaultDimXEdit->setText(QString().setNum(TYPreferenceManager::getFloat("DefaultDimX")));
	}
	if (TYPreferenceManager::exists("DefaultDimY"))
	{
		((GeometrieTab*)tabWidget->widget(0))->_pDefaultDimYEdit->setText(QString().setNum(TYPreferenceManager::getFloat("DefaultDimY")));
	}
	if (TYPreferenceManager::exists("DefaultHMur"))
	{
		((GeometrieTab*)tabWidget->widget(0))->_pDefaultHMurEdit->setText(QString().setNum(TYPreferenceManager::getFloat("DefaultHMur")));
	}
	if (TYPreferenceManager::exists("DefaultWidthEcran"))
	{
		((GeometrieTab*)tabWidget->widget(0))->_pDefaultWEcranEdit->setText(QString().setNum(TYPreferenceManager::getFloat("DefaultWidthEcran")));
	}

	// 3D
	if (TYPreferenceManager::exists("MouseLeftButtonFunct2D"))
	{
		((pref3DTab*)tabWidget->widget(1))->_pLeftButton2DComboBox->setCurrentIndex(TYPreferenceManager::getInt("MouseLeftButtonFunct2D"));
	}
	if (TYPreferenceManager::exists("MouseRightButtonFunct2D"))
	{
		((pref3DTab*)tabWidget->widget(1))->_pRightButton2DComboBox->setCurrentIndex(TYPreferenceManager::getInt("MouseRightButtonFunct2D"));
	}
	if (TYPreferenceManager::exists("MouseMiddleButtonFunct2D"))
	{
		((pref3DTab*)tabWidget->widget(1))->_pMiddleButton2DComboBox->setCurrentIndex(TYPreferenceManager::getInt("MouseMiddleButtonFunct2D"));
	}
	if (TYPreferenceManager::exists("MouseLeftButtonFunct3D"))
	{
		((pref3DTab*)tabWidget->widget(1))->_pLeftButton3DComboBox->setCurrentIndex(TYPreferenceManager::getInt("MouseLeftButtonFunct3D"));
	}
	if (TYPreferenceManager::exists("MouseRightButtonFunct3D"))
	{
		((pref3DTab*)tabWidget->widget(1))->_pRightButton3DComboBox->setCurrentIndex(TYPreferenceManager::getInt("MouseRightButtonFunct3D"));
	}
	if (TYPreferenceManager::exists("MouseMiddleButtonFunct3D"))
	{
		((pref3DTab*)tabWidget->widget(1))->_pMiddleButton3DComboBox->setCurrentIndex(TYPreferenceManager::getInt("MouseMiddleButtonFunct3D"));
	}
	if (TYPreferenceManager::exists("MouseShiftLeftButtonFunct2D"))
	{
		((pref3DTab*)tabWidget->widget(1))->_pLeftButton2DShtComboBox->setCurrentIndex(TYPreferenceManager::getInt("MouseShiftLeftButtonFunct2D"));
	}
	if (TYPreferenceManager::exists("MouseShiftRightButtonFunct2D"))
	{
		((pref3DTab*)tabWidget->widget(1))->_pRightButton2DShtComboBox->setCurrentIndex(TYPreferenceManager::getInt("MouseShiftRightButtonFunct2D"));
	}
	if (TYPreferenceManager::exists("MouseShiftLeftButtonFunct3D"))
	{
		((pref3DTab*)tabWidget->widget(1))->_pLeftButton3DShtComboBox->setCurrentIndex(TYPreferenceManager::getInt("MouseShiftLeftButtonFunct3D"));
	}
	if (TYPreferenceManager::exists("MouseShiftRightButtonFunct3D"))
	{
		((pref3DTab*)tabWidget->widget(1))->_pRightButton3DShtComboBox->setCurrentIndex(TYPreferenceManager::getInt("MouseShiftRightButtonFunct3D"));
	}
	if (TYPreferenceManager::exists("ZoomStep"))
	{
		((pref3DTab*)tabWidget->widget(1))->_pZoomStepEdit->setText(QString().setNum(TYPreferenceManager::getFloat("ZoomStep")));
	}
	if (TYPreferenceManager::exists("PrecisPick"))
	{
		((pref3DTab*)tabWidget->widget(1))->_pPrecisPickEdit->setText(QString().setNum(TYPreferenceManager::getFloat("PrecisPick")));
	}
	if (TYPreferenceManager::exists("Luminosite"))
	{
		((pref3DTab*)tabWidget->widget(1))->_pLightSlider->setValue(ROUND(TYPreferenceManager::getFloat("Luminosite") * 100.0f));
	}
	if (TYPreferenceManager::exists("ViewOnlyHighlight"))
	{
		((pref3DTab*)tabWidget->widget(1))->_pViewOnlyHighlightCheckBox->setChecked(TYPreferenceManager::getBool("ViewOnlyHighlight"));
	}
	if (TYPreferenceManager::exists("CenterOnLocate"))
	{
		((pref3DTab*)tabWidget->widget(1))->_pCenterOnLocateCheckBox->setChecked(TYPreferenceManager::getBool("CenterOnLocate"));
	}
	if (TYPreferenceManager::exists("WireframeOnMovingCamera"))
	{
		((pref3DTab*)tabWidget->widget(1))->_pWireframeOnMovingCameraCheckBox->setChecked(TYPreferenceManager::getBool("WireframeOnMovingCamera"));
	}
	if (TYPreferenceManager::exists("CameraTranslateStep"))
	{
		((pref3DTab*)tabWidget->widget(1))->_pCameraStepTranslateEdit->setText(QString().setNum(TYPreferenceManager::getFloat("CameraTranslateStep")));
	}
	if (TYPreferenceManager::exists("CameraRotateStep"))
	{
		((pref3DTab*)tabWidget->widget(1))->_pCameraStepRotateEdit->setText(QString().setNum(TYPreferenceManager::getFloat("CameraRotateStep")));
	}
	if (TYPreferenceManager::exists("CameraZoomStep"))
	{
		((pref3DTab*)tabWidget->widget(1))->_pCameraStepZoomEdit->setText(QString().setNum(TYPreferenceManager::getFloat("CameraZoomStep")));
	}

	// Colors
	float r, g, b;
	if (TYPreferenceManager::exists("BackgroundColorR"))
	{
		TYPreferenceManager::getColor("BackgroundColor", r, g, b);
		((ColorsTab*)tabWidget->widget(2))->_pBackgroundColorToolButton->setColor(QColor(int(r), int(g), int(b)));//TYPreferenceManager::getInt("BackgroundColor_r"), TYPreferenceManager::getInt("BackgroundColor_g"), TYPreferenceManager::getInt("BackgroundColor_b")));
	}
	if (TYPreferenceManager::exists("AltiGraphicColorMinR"))
	{
		TYPreferenceManager::getColor("AltiGraphicColorMin", r, g, b);
		QColor color;
		color.setHsv(int(r), int(g), int(b));
		((ColorsTab*)tabWidget->widget(2))->_pAltiColorMinToolButton->setColor(color);
	}
	if (TYPreferenceManager::exists("AltiGraphicColorMaxR"))
	{
		TYPreferenceManager::getColor("AltiGraphicColorMax", r, g, b);
		QColor color;
		color.setHsv(int(r), int(g), int(b));
		((ColorsTab*)tabWidget->widget(2))->_pAltiColorMaxToolButton->setColor(color);
	}
	if (TYPreferenceManager::exists("TYReseauTransportGraphicColorR"))
	{
		TYPreferenceManager::getColor("TYReseauTransportGraphicColor", r, g, b);
		((ColorsTab*)tabWidget->widget(2))->_pResTranspColorToolButton->setColor(QColor(int(r), int(g), int(b)));
	}
	if (TYPreferenceManager::exists("TYRouteGraphicColorR"))
	{
		TYPreferenceManager::getColor("TYRouteGraphicColor", r, g, b);
		((ColorsTab*)tabWidget->widget(2))->_pRouteColorToolButton->setColor(QColor(int(r), int(g), int(b)));
	}
	if (TYPreferenceManager::exists("TYMurGraphicColorR"))
	{
		TYPreferenceManager::getColor("TYMurGraphicColor", r, g, b);
		((ColorsTab*)tabWidget->widget(2))->_pMurColorToolButton->setColor(QColor(int(r), int(g), int(b)));
	}
	if (TYPreferenceManager::exists("TYEcranGraphicColorR"))
	{
		TYPreferenceManager::getColor("TYEcranGraphicColor", r, g, b);
		((ColorsTab*)tabWidget->widget(2))->_pEcranColorToolButton->setColor(QColor(int(r), int(g), int(b)));
	}
	if (TYPreferenceManager::exists("TYDalleGraphicColorR"))
	{
		TYPreferenceManager::getColor("TYDalleGraphicColor", r, g, b);
		((ColorsTab*)tabWidget->widget(2))->_pDalleColorToolButton->setColor(QColor(int(r), int(g), int(b)));
	}
	if (TYPreferenceManager::exists("SubRectColorR"))
	{
		TYPreferenceManager::getColor("SubRectColor", r, g, b);
		((ColorsTab*)tabWidget->widget(2))->_pSubRectColorToolButton->setColor(QColor(int(r), int(g), int(b)));
	}
	if (TYPreferenceManager::exists("TYAcousticCylinderGraphicColorR"))
	{
		TYPreferenceManager::getColor("TYAcousticCylinderGraphicColor", r, g, b);
		((ColorsTab*)tabWidget->widget(2))->_pCylindreColorToolButton->setColor(QColor(int(r), int(g), int(b)));
	}
	if (TYPreferenceManager::exists("TYAcousticBoxGraphicColorR"))
	{
		TYPreferenceManager::getColor("TYAcousticBoxGraphicColor", r, g, b);
		((ColorsTab*)tabWidget->widget(2))->_pBoxColorToolButton->setColor(QColor(int(r), int(g), int(b)));
	}
	if (TYPreferenceManager::exists("TYVegetationGraphicColorR"))
	{
		TYPreferenceManager::getColor("TYVegetationGraphicColor", r, g, b);
		((ColorsTab*)tabWidget->widget(2))->_pVegColorToolButton->setColor(QColor(int(r), int(g), int(b)));
	}
	if (TYPreferenceManager::exists("TYCourbeNiveauGraphicColorR"))
	{
		TYPreferenceManager::getColor("TYCourbeNiveauGraphicColor", r, g, b);
		((ColorsTab*)tabWidget->widget(2))->_pCrbNivColorToolButton->setColor(QColor(int(r), int(g), int(b)));
	}
	if (TYPreferenceManager::exists("TYCoursEauGraphicColorR"))
	{
		TYPreferenceManager::getColor("TYCoursEauGraphicColor", r, g, b);
		((ColorsTab*)tabWidget->widget(2))->_pCrsEauColorToolButton->setColor(QColor(int(r), int(g), int(b)));
	}
	if (TYPreferenceManager::exists("TYPlanEauGraphicColorR"))
	{
		TYPreferenceManager::getColor("TYPlanEauGraphicColor", r, g, b);
		((ColorsTab*)tabWidget->widget(2))->_pPlanEauColorToolButton->setColor(QColor(int(r), int(g), int(b)));
	}
	if (TYPreferenceManager::exists("TYTerrainGraphicColorR"))
	{
		TYPreferenceManager::getColor("TYTerrainGraphicColor", r, g, b);
		((ColorsTab*)tabWidget->widget(2))->_pTerrainColorToolButton->setColor(QColor(int(r), int(g), int(b)));
	}
	if (TYPreferenceManager::exists("FontColorR"))
	{
		TYPreferenceManager::getColor("FontColor", r, g, b);
		((ColorsTab*)tabWidget->widget(2))->_pPoliceColorToolButton->setColor(QColor(int(r), int(g), int(b)));
	}
	if (TYPreferenceManager::exists("GridColorR"))
	{
		TYPreferenceManager::getColor("GridColor", r, g, b);
		((ColorsTab*)tabWidget->widget(2))->_pGridColorToolButton->setColor(QColor(int(r), int(g), int(b)));
	}
	// Couleur des points de controle
	if (TYPreferenceManager::exists("PointControlGraphicColorR"))
	{
		TYPreferenceManager::getColor("PointControlGraphicColor", r, g, b);
		((ColorsTab*)tabWidget->widget(2))->_pPointControlColorToolButton->setColor(QColor(int(r), int(g), int(b)));
	}
	// Couleur des Sources
	if (TYPreferenceManager::exists("SrcPonctGraphicColorR"))
	{
		TYPreferenceManager::getColor("SrcPonctGraphicColor", r, g, b);
		((ColorsTab*)tabWidget->widget(2))->_pSourcePonctColorToolButton->setColor(QColor(int(r), int(g), int(b)));
	}


	if (TYPreferenceManager::exists("WinOpacity"))
	{
		((ColorsTab*)tabWidget->widget(2))->_pWinOpacitySlider->setValue(ROUND(TYPreferenceManager::getFloat("WinOpacity") * 100.0f));
	}
	if (TYPreferenceManager::exists("AltOpacity"))
	{
		((ColorsTab*)tabWidget->widget(2))->_pAltOpacitySlider->setValue(ROUND(TYPreferenceManager::getFloat("AltOpacity") * 100.0f));
	}
	if (TYPreferenceManager::exists("MaillageOpacity"))
	{
		((ColorsTab*)tabWidget->widget(2))->_pMailOpacitySlider->setValue(ROUND(TYPreferenceManager::getFloat("MaillageOpacity") * 100.0f));
	}



	// Acoustique
	if (TYPreferenceManager::exists("ResisSolDefault"))
	{
		((AcoustiqueTab*)tabWidget->widget(3))->_pLineEditResistivite->setText(QString().setNum(TYPreferenceManager::getDouble("ResisSolDefault")));
	}
	if (TYPreferenceManager::exists("EpaisSolDefault"))
	{
		((AcoustiqueTab*)tabWidget->widget(3))->_pLineEditEpaisseur->setText(QString().setNum(TYPreferenceManager::getDouble("EpaisSolDefault")));
	}
	if (TYPreferenceManager::exists("EcartTypeDefault"))
	{
		((AcoustiqueTab*)tabWidget->widget(3))->_pLineEditEcartType->setText(QString().setNum(TYPreferenceManager::getDouble("EcartTypeDefault")));
	}
	if (TYPreferenceManager::exists("LongueurDefault"))
	{
		((AcoustiqueTab*)tabWidget->widget(3))->_pLineEditLongueur->setText(QString().setNum(TYPreferenceManager::getDouble("LongueurDefault")));
	}
	if (TYPreferenceManager::exists("MasseVolMatDefault"))
	{
		((AcoustiqueTab*)tabWidget->widget(3))->_pLineEditMasseVol->setText(QString().setNum(TYPreferenceManager::getDouble("MasseVolMatDefault")));
	}
	if (TYPreferenceManager::exists("SpectreTransmDefault0"))
	{
		TYSpectre* pTmpSpectre = TYPreferenceManager::getSpectre("SpectreTransmDefault");

		if (pTmpSpectre)
		{
			if (((AcoustiqueTab*)tabWidget->widget(3))->_pSpectreAttMatConst) { delete((AcoustiqueTab*)tabWidget->widget(3))->_pSpectreAttMatConst; }
			((AcoustiqueTab*)tabWidget->widget(3))->_pSpectreAttMatConst = pTmpSpectre;
		}
	}
	if (TYPreferenceManager::exists("SpectreAbsoDefault0"))
	{
		TYSpectre* pTmpSpectre = TYPreferenceManager::getSpectre("SpectreAbsoDefault");

		if (pTmpSpectre)
		{
			if (((AcoustiqueTab*)tabWidget->widget(3))->_pSpectreAbsoMatConst) { delete((AcoustiqueTab*)tabWidget->widget(3))->_pSpectreAbsoMatConst; }
			((AcoustiqueTab*)tabWidget->widget(3))->_pSpectreAbsoMatConst = pTmpSpectre;
		}
	}


	// Maillage par defaut
	QString pref = "MaillageDefault";
	if (TYPreferenceManager::exists(pref + "Hauteur"))
	{
		((AcoustiqueTab*)tabWidget->widget(3))->_pMaillage->setHauteur(TYPreferenceManager::getFloat(pref + "Hauteur"));
		((AcoustiqueTab*)tabWidget->widget(3))->_pMaillage->setDataType(TYPreferenceManager::getInt(pref + "DataType"));
		((AcoustiqueTab*)tabWidget->widget(3))->_pMaillage->setDataFreq(TYPreferenceManager::getFloat(pref + "DataFreq"));

		pref = "PaletteDefault";
		if (TYPreferenceManager::exists(pref + "Min"))
		{
			OLookupTable legacyTable;
			OColor color;

			const size_t nb_colors = TYPreferenceManager::getFloat(pref + "NbColors");
			const float valueMin = TYPreferenceManager::getFloat(pref + "Min");
			const float valueMax = TYPreferenceManager::getFloat(pref + "Max");

			TYPalette* pPalette = ((AcoustiqueTab*)tabWidget->widget(3))->_pMaillage->getPalette();
			legacyTable.resize(nb_colors);
			for (size_t i = 0; i < nb_colors; ++i)
			{
				TYPreferenceManager::getColor(pref + "Color" + QString(uintToStr(i).c_str()), color.r, color.g, color.b);
			}
			pPalette->resetcolorMapFromColors(valueMin, valueMax, legacyTable);

		}
	}

	// On restaure le directory courant precedent
	TYPreferenceManager::setCurrentDirectory(oldDir);
}

void TYPreferenceDialog::clickOkApply()
{
	savePreferences();
}

void TYPreferenceDialog::savePreferences()
{
	// Directory specifique aux preferences traitees ici
	QString oldDir = TYPreferenceManager::getCurrentDirectory();
	TYPreferenceManager::setCurrentDirectory(TYDIRPREFERENCEMANAGER);
	QDoubleValidator qValidator(NULL);
	int res;

	// General
	QString aQString;
	qValidator.setBottom(1);
	qValidator.setTop(256);
	if (qValidator.validate(aQString = ((GeneralTab*)tabWidget->widget(4))->_pHistoSizeEdit->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setInt("HistoSize", ((GeneralTab*)tabWidget->widget(4))->_pHistoSizeEdit->text().toInt());
	}

	getTYApp()->getActionManager()->setHistorySize(((GeneralTab*)tabWidget->widget(4))->_pHistoSizeEdit->text().toInt());

	//repertoire de la bibliotheque
	QDir libraryPath(((GeneralTab*)tabWidget->widget(4))->_pLibraryPathEdit->text());
	if (libraryPath.exists())
	{
		TYPreferenceManager::setString("BiblioPath", ((GeneralTab*)tabWidget->widget(4))->_pLibraryPathEdit->text());
	}






	//repertoire de travail
	QDir workDirPath(((GeneralTab*)tabWidget->widget(4))->_pWorkDirPathEdit->text());
	if (workDirPath.exists())
	{
		TYPreferenceManager::setString("WorkDirPath", ((GeneralTab*)tabWidget->widget(4))->_pWorkDirPathEdit->text());
	}



	// Geometrie
	qValidator.setTop(256000);
	if (qValidator.validate(aQString = ((GeometrieTab*)tabWidget->widget(0))->_pGridDimXSiteEdit->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setFloat("GridDimXSite", ((GeometrieTab*)tabWidget->widget(0))->_pGridDimXSiteEdit->text().toFloat());
	}
	if (qValidator.validate(aQString = ((GeometrieTab*)tabWidget->widget(0))->_pGridDimYSiteEdit->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setFloat("GridDimYSite", ((GeometrieTab*)tabWidget->widget(0))->_pGridDimYSiteEdit->text().toFloat());
	}
	qValidator.setBottom(0.1);
	if (qValidator.validate(aQString = ((GeometrieTab*)tabWidget->widget(0))->_pGridStepSiteEdit->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setFloat("GridStepSite", ((GeometrieTab*)tabWidget->widget(0))->_pGridStepSiteEdit->text().toFloat());
	}
	if (qValidator.validate(aQString = ((GeometrieTab*)tabWidget->widget(0))->_pGridMagnStepSiteEdit->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setFloat("GridMagnStepSite", ((GeometrieTab*)tabWidget->widget(0))->_pGridMagnStepSiteEdit->text().toFloat());
	}

	qValidator.setBottom(1);
	if (qValidator.validate(aQString = ((GeometrieTab*)tabWidget->widget(0))->_pGridDimXBatEdit->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setFloat("GridDimXBatiment", ((GeometrieTab*)tabWidget->widget(0))->_pGridDimXBatEdit->text().toFloat());
	}
	if (qValidator.validate(aQString = ((GeometrieTab*)tabWidget->widget(0))->_pGridDimYBatEdit->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setFloat("GridDimYBatiment", ((GeometrieTab*)tabWidget->widget(0))->_pGridDimYBatEdit->text().toFloat());
	}
	qValidator.setBottom(0.1);
	if (qValidator.validate(aQString = ((GeometrieTab*)tabWidget->widget(0))->_pGridMagnStepBatEdit->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setFloat("GridMagnStepBatiment", ((GeometrieTab*)tabWidget->widget(0))->_pGridMagnStepBatEdit->text().toFloat());
	}
	if (qValidator.validate(aQString = ((GeometrieTab*)tabWidget->widget(0))->_pGridStepBatEdit->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setFloat("GridStepBatiment", ((GeometrieTab*)tabWidget->widget(0))->_pGridStepBatEdit->text().toFloat());
	}

	qValidator.setBottom(1);
	if (qValidator.validate(aQString = ((GeometrieTab*)tabWidget->widget(0))->_pGridDimXMacEdit->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setFloat("GridDimXMachine", ((GeometrieTab*)tabWidget->widget(0))->_pGridDimXMacEdit->text().toFloat());
	}
	if (qValidator.validate(aQString = ((GeometrieTab*)tabWidget->widget(0))->_pGridDimYMacEdit->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setFloat("GridDimYMachine", ((GeometrieTab*)tabWidget->widget(0))->_pGridDimYMacEdit->text().toFloat());
	}
	qValidator.setBottom(0.1);
	if (qValidator.validate(aQString = ((GeometrieTab*)tabWidget->widget(0))->_pGridStepMacEdit->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setFloat("GridStepMachine", ((GeometrieTab*)tabWidget->widget(0))->_pGridStepMacEdit->text().toFloat());
	}
	if (qValidator.validate(aQString = ((GeometrieTab*)tabWidget->widget(0))->_pGridMagnStepMacEdit->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setFloat("GridMagnStepMachine", ((GeometrieTab*)tabWidget->widget(0))->_pGridMagnStepMacEdit->text().toFloat());
	}

	qValidator.setBottom(1);
	if (qValidator.validate(aQString = ((GeometrieTab*)tabWidget->widget(0))->_pGridDimXMailEdit->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setFloat("GridDimXMaillage", ((GeometrieTab*)tabWidget->widget(0))->_pGridDimXMailEdit->text().toFloat());
	}
	if (qValidator.validate(aQString = ((GeometrieTab*)tabWidget->widget(0))->_pGridDimYMailEdit->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setFloat("GridDimYMaillage", ((GeometrieTab*)tabWidget->widget(0))->_pGridDimYMailEdit->text().toFloat());
	}
	qValidator.setBottom(0.1);
	if (qValidator.validate(aQString = ((GeometrieTab*)tabWidget->widget(0))->_pGridMagnStepMailEdit->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setFloat("GridMagnStepMaillage", ((GeometrieTab*)tabWidget->widget(0))->_pGridMagnStepMailEdit->text().toFloat());
	}
	if (qValidator.validate(aQString = ((GeometrieTab*)tabWidget->widget(0))->_pGridStepMailEdit->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setFloat("GridStepMaillage", ((GeometrieTab*)tabWidget->widget(0))->_pGridStepMailEdit->text().toFloat());
	}

	qValidator.setBottom(1);
	if (qValidator.validate(aQString = ((GeometrieTab*)tabWidget->widget(0))->_pGridDimXFaceEdit->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setFloat("GridDimXFace", ((GeometrieTab*)tabWidget->widget(0))->_pGridDimXFaceEdit->text().toFloat());
	}
	if (qValidator.validate(aQString = ((GeometrieTab*)tabWidget->widget(0))->_pGridDimYFaceEdit->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setFloat("GridDimYFace", ((GeometrieTab*)tabWidget->widget(0))->_pGridDimYFaceEdit->text().toFloat());
	}
	qValidator.setBottom(0.1);
	if (qValidator.validate(aQString = ((GeometrieTab*)tabWidget->widget(0))->_pGridStepFaceEdit->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setFloat("GridStepFace", ((GeometrieTab*)tabWidget->widget(0))->_pGridStepFaceEdit->text().toFloat());
	}
	if (qValidator.validate(aQString = ((GeometrieTab*)tabWidget->widget(0))->_pGridMagnStepFaceEdit->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setFloat("GridMagnStepFace", ((GeometrieTab*)tabWidget->widget(0))->_pGridMagnStepFaceEdit->text().toFloat());
	}

	if (((GeometrieTab*)tabWidget->widget(0))->_p15RadioButton->isChecked())
	{
		TYPreferenceManager::setFloat("Angle", ((GeometrieTab*)tabWidget->widget(0))->_p15RadioButton->text().toFloat());
	}
	else if (((GeometrieTab*)tabWidget->widget(0))->_p30RadioButton->isChecked())
	{
		TYPreferenceManager::setFloat("Angle", ((GeometrieTab*)tabWidget->widget(0))->_p30RadioButton->text().toFloat());
	}
	else if (((GeometrieTab*)tabWidget->widget(0))->_p45RadioButton->isChecked())
	{
		TYPreferenceManager::setFloat("Angle", ((GeometrieTab*)tabWidget->widget(0))->_p45RadioButton->text().toFloat());
	}
	else if (((GeometrieTab*)tabWidget->widget(0))->_p90RadioButton->isChecked())
	{
		TYPreferenceManager::setFloat("Angle", ((GeometrieTab*)tabWidget->widget(0))->_p90RadioButton->text().toFloat());
	}

	// Taille des sources ponctuelles utilisateur
	qValidator.setBottom(0.5);
	if (qValidator.validate(aQString = ((GeometrieTab*)tabWidget->widget(0))->_pSourceSizeEdit->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setFloat("UserSrcPonctGraphicSize", ((GeometrieTab*)tabWidget->widget(0))->_pSourceSizeEdit->text().toFloat());
	}
	// Taille des points de controle
	if (qValidator.validate(aQString = ((GeometrieTab*)tabWidget->widget(0))->_pPointControlSizeEdit->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setFloat("PointControlGraphicSize", ((GeometrieTab*)tabWidget->widget(0))->_pPointControlSizeEdit->text().toFloat());
	}

	qValidator.setBottom(3);
	if (qValidator.validate(aQString = ((GeometrieTab*)tabWidget->widget(0))->_pResoCylEdit->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setFloat("ResolutionCircle", ((GeometrieTab*)tabWidget->widget(0))-> _pResoCylEdit->text().toFloat());
	}
	qValidator.setBottom(0.001);
	if (qValidator.validate(aQString = ((GeometrieTab*)tabWidget->widget(0))->_pDistMinPtCrbNivEdit->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setFloat("DistMinPtCrbNiv", ((GeometrieTab*)tabWidget->widget(0))->_pDistMinPtCrbNivEdit->text().toFloat());
	}
	qValidator.setBottom(0);
	if (qValidator.validate(aQString = ((GeometrieTab*)tabWidget->widget(0))->_pDelaunayToleranceEdit->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setFloat("DelaunayTolerance", ((GeometrieTab*)tabWidget->widget(0))->_pDelaunayToleranceEdit->text().toFloat());
	}
	qValidator.setBottom(1);
	if (qValidator.validate(aQString = ((GeometrieTab*)tabWidget->widget(0))->_pDefaultDimXEdit->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setFloat("DefaultDimX", ((GeometrieTab*)tabWidget->widget(0))->_pDefaultDimXEdit->text().toFloat());
	}
	if (qValidator.validate(aQString = ((GeometrieTab*)tabWidget->widget(0))->_pDefaultDimYEdit->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setFloat("DefaultDimY", ((GeometrieTab*)tabWidget->widget(0))->_pDefaultDimYEdit->text().toFloat());
	}
	qValidator.setBottom(0.1);
	if (qValidator.validate(aQString = ((GeometrieTab*)tabWidget->widget(0))->_pDefaultHMurEdit->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setFloat("DefaultHMur", ((GeometrieTab*)tabWidget->widget(0))->_pDefaultHMurEdit->text().toFloat());
	}
	if (qValidator.validate(aQString = ((GeometrieTab*)tabWidget->widget(0))->_pDefaultWEcranEdit->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setFloat("DefaultWidthEcran", ((GeometrieTab*)tabWidget->widget(0))->_pDefaultWEcranEdit->text().toFloat());
	}

	// 3D
	TYPreferenceManager::setFloat("Luminosite", float(((pref3DTab*)tabWidget->widget(1))->_pLightSlider->value()) / 100);
	qValidator.setBottom(0.001);
	if (qValidator.validate(aQString = ((pref3DTab*)tabWidget->widget(1))->_pZoomStepEdit->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setFloat("ZoomStep", ((pref3DTab*)tabWidget->widget(1))->_pZoomStepEdit->text().toFloat());
	}
	if (qValidator.validate(aQString = ((pref3DTab*)tabWidget->widget(1))->_pPrecisPickEdit->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setFloat("PrecisPick", ((pref3DTab*)tabWidget->widget(1))->_pPrecisPickEdit->text().toFloat());
	}
	TYPreferenceManager::setInt("MouseLeftButtonFunct2D", ((pref3DTab*)tabWidget->widget(1))->_pLeftButton2DComboBox->currentIndex());
	TYPreferenceManager::setInt("MouseRightButtonFunct2D", ((pref3DTab*)tabWidget->widget(1))->_pRightButton2DComboBox->currentIndex());
	TYPreferenceManager::setInt("MouseMiddleButtonFunct2D", ((pref3DTab*)tabWidget->widget(1))->_pMiddleButton2DComboBox->currentIndex());
	TYPreferenceManager::setInt("MouseLeftButtonFunct3D", ((pref3DTab*)tabWidget->widget(1))->_pLeftButton3DComboBox->currentIndex());
	TYPreferenceManager::setInt("MouseRightButtonFunct3D", ((pref3DTab*)tabWidget->widget(1))->_pRightButton3DComboBox->currentIndex());
	TYPreferenceManager::setInt("MouseMiddleButtonFunct3D", ((pref3DTab*)tabWidget->widget(1))->_pMiddleButton3DComboBox->currentIndex());
	TYPreferenceManager::setInt("MouseShiftLeftButtonFunct2D", ((pref3DTab*)tabWidget->widget(1))->_pLeftButton2DShtComboBox->currentIndex());
	TYPreferenceManager::setInt("MouseShiftRightButtonFunct2D", ((pref3DTab*)tabWidget->widget(1))->_pRightButton2DShtComboBox->currentIndex());
	TYPreferenceManager::setInt("MouseShiftLeftButtonFunct3D", ((pref3DTab*)tabWidget->widget(1))->_pLeftButton3DShtComboBox->currentIndex());
	TYPreferenceManager::setInt("MouseShiftRightButtonFunct3D", ((pref3DTab*)tabWidget->widget(1))->_pRightButton3DShtComboBox->currentIndex());
	TYPreferenceManager::setBool("ViewOnlyHighlight", ((pref3DTab*)tabWidget->widget(1))->_pViewOnlyHighlightCheckBox->isChecked());
	TYPreferenceManager::setBool("CenterOnLocate", ((pref3DTab*)tabWidget->widget(1))->_pCenterOnLocateCheckBox->isChecked());
	TYPreferenceManager::setBool("WireframeOnMovingCamera", ((pref3DTab*)tabWidget->widget(1))->_pWireframeOnMovingCameraCheckBox->isChecked());
	TYPreferenceManager::setFloat("CameraTranslateStep", ((pref3DTab*)tabWidget->widget(1))->_pCameraStepTranslateEdit->text().toFloat());
	TYPreferenceManager::setFloat("CameraRotateStep", ((pref3DTab*)tabWidget->widget(1))->_pCameraStepRotateEdit->text().toFloat());
	TYPreferenceManager::setFloat("CameraZoomStep", ((pref3DTab*)tabWidget->widget(1))->_pCameraStepZoomEdit->text().toFloat());

	// Colors
	TYPreferenceManager::setColor("BackgroundColor",
		((ColorsTab*)tabWidget->widget(2))->_pBackgroundColorToolButton->getColor().red(),
		((ColorsTab*)tabWidget->widget(2))->_pBackgroundColorToolButton->getColor().green(),
		((ColorsTab*)tabWidget->widget(2))->_pBackgroundColorToolButton->getColor().blue());
	TYPreferenceManager::setColor("GridColor",
		((ColorsTab*)tabWidget->widget(2))->_pGridColorToolButton->getColor().red(),
		((ColorsTab*)tabWidget->widget(2))->_pGridColorToolButton->getColor().green(),
		((ColorsTab*)tabWidget->widget(2))->_pGridColorToolButton->getColor().blue());
	TYPreferenceManager::setColor("FontColor",
		((ColorsTab*)tabWidget->widget(2))->_pPoliceColorToolButton->getColor().red(),
		((ColorsTab*)tabWidget->widget(2))->_pPoliceColorToolButton->getColor().green(),
		((ColorsTab*)tabWidget->widget(2))->_pPoliceColorToolButton->getColor().blue());
	TYPreferenceManager::setColor("TYTerrainGraphicColor",
		((ColorsTab*)tabWidget->widget(2))->_pTerrainColorToolButton->getColor().red(),
		((ColorsTab*)tabWidget->widget(2))->_pTerrainColorToolButton->getColor().green(),
		((ColorsTab*)tabWidget->widget(2))->_pTerrainColorToolButton->getColor().blue());
	TYPreferenceManager::setColor("TYPlanEauGraphicColor",
		((ColorsTab*)tabWidget->widget(2))->_pPlanEauColorToolButton->getColor().red(),
		((ColorsTab*)tabWidget->widget(2))->_pPlanEauColorToolButton->getColor().green(),
		((ColorsTab*)tabWidget->widget(2))->_pPlanEauColorToolButton->getColor().blue());
	TYPreferenceManager::setColor("TYCoursEauGraphicColor",
		((ColorsTab*)tabWidget->widget(2))->_pCrsEauColorToolButton->getColor().red(),
		((ColorsTab*)tabWidget->widget(2))->_pCrsEauColorToolButton->getColor().green(),
		((ColorsTab*)tabWidget->widget(2))->_pCrsEauColorToolButton->getColor().blue());
	TYPreferenceManager::setColor("TYCourbeNiveauGraphicColor",
		((ColorsTab*)tabWidget->widget(2))->_pCrbNivColorToolButton->getColor().red(),
		((ColorsTab*)tabWidget->widget(2))->_pCrbNivColorToolButton->getColor().green(),
		((ColorsTab*)tabWidget->widget(2))->_pCrbNivColorToolButton->getColor().blue());
	TYPreferenceManager::setColor("TYVegetationGraphicColor",
		((ColorsTab*)tabWidget->widget(2))->_pVegColorToolButton->getColor().red(),
		((ColorsTab*)tabWidget->widget(2))->_pVegColorToolButton->getColor().green(),
		((ColorsTab*)tabWidget->widget(2))->_pVegColorToolButton->getColor().blue());
	TYPreferenceManager::setColor("TYAcousticBoxGraphicColor",
		((ColorsTab*)tabWidget->widget(2))->_pBoxColorToolButton->getColor().red(),
		((ColorsTab*)tabWidget->widget(2))->_pBoxColorToolButton->getColor().green(),
		((ColorsTab*)tabWidget->widget(2))->_pBoxColorToolButton->getColor().blue());
	TYPreferenceManager::setColor("TYAcousticCylinderGraphicColor",
		((ColorsTab*)tabWidget->widget(2))->_pCylindreColorToolButton->getColor().red(),
		((ColorsTab*)tabWidget->widget(2))->_pCylindreColorToolButton->getColor().green(),
		((ColorsTab*)tabWidget->widget(2))->_pCylindreColorToolButton->getColor().blue());
	TYPreferenceManager::setColor("TYAcousticSemiCylinderGraphicColor",
		((ColorsTab*)tabWidget->widget(2))->_pCylindreColorToolButton->getColor().red(),
		((ColorsTab*)tabWidget->widget(2))->_pCylindreColorToolButton->getColor().green(),
		((ColorsTab*)tabWidget->widget(2))->_pCylindreColorToolButton->getColor().blue());
	TYPreferenceManager::setColor("TYAcousticCircleGraphicColor",
		((ColorsTab*)tabWidget->widget(2))->_pCylindreColorToolButton->getColor().red(),
		((ColorsTab*)tabWidget->widget(2))->_pCylindreColorToolButton->getColor().green(),
		((ColorsTab*)tabWidget->widget(2))->_pCylindreColorToolButton->getColor().blue());
	TYPreferenceManager::setColor("TYAcousticSemiCircleGraphicColor",
		((ColorsTab*)tabWidget->widget(2))->_pCylindreColorToolButton->getColor().red(),
		((ColorsTab*)tabWidget->widget(2))->_pCylindreColorToolButton->getColor().green(),
		((ColorsTab*)tabWidget->widget(2))->_pCylindreColorToolButton->getColor().blue());
	TYPreferenceManager::setColor("SubRectColor",
		((ColorsTab*)tabWidget->widget(2))->_pSubRectColorToolButton->getColor().red(),
		((ColorsTab*)tabWidget->widget(2))->_pSubRectColorToolButton->getColor().green(),
		((ColorsTab*)tabWidget->widget(2))->_pSubRectColorToolButton->getColor().blue());
	TYPreferenceManager::setColor("TYMurGraphicColor",
		((ColorsTab*)tabWidget->widget(2))->_pMurColorToolButton->getColor().red(),
		((ColorsTab*)tabWidget->widget(2))->_pMurColorToolButton->getColor().green(),
		((ColorsTab*)tabWidget->widget(2))->_pMurColorToolButton->getColor().blue());
	TYPreferenceManager::setColor("TYDalleGraphicColor",
		((ColorsTab*)tabWidget->widget(2))->_pDalleColorToolButton->getColor().red(),
		((ColorsTab*)tabWidget->widget(2))->_pDalleColorToolButton->getColor().green(),
		((ColorsTab*)tabWidget->widget(2))->_pDalleColorToolButton->getColor().blue());
	TYPreferenceManager::setColor("TYEcranGraphicColor",
		((ColorsTab*)tabWidget->widget(2))->_pEcranColorToolButton->getColor().red(),
		((ColorsTab*)tabWidget->widget(2))->_pEcranColorToolButton->getColor().green(),
		((ColorsTab*)tabWidget->widget(2))->_pEcranColorToolButton->getColor().blue());
	TYPreferenceManager::setColor("TYRouteGraphicColor",
		((ColorsTab*)tabWidget->widget(2))->_pRouteColorToolButton->getColor().red(),
		((ColorsTab*)tabWidget->widget(2))->_pRouteColorToolButton->getColor().green(),
		((ColorsTab*)tabWidget->widget(2))->_pRouteColorToolButton->getColor().blue());
	TYPreferenceManager::setColor("TYReseauTransportGraphicColor",
		((ColorsTab*)tabWidget->widget(2))->_pResTranspColorToolButton->getColor().red(),
		((ColorsTab*)tabWidget->widget(2))->_pResTranspColorToolButton->getColor().green(),
		((ColorsTab*)tabWidget->widget(2))->_pResTranspColorToolButton->getColor().blue());
	// Couleur des points de controle
	TYPreferenceManager::setColor("PointControlGraphicColor",
		((ColorsTab*)tabWidget->widget(2))->_pPointControlColorToolButton->getColor().red(),
		((ColorsTab*)tabWidget->widget(2))->_pPointControlColorToolButton->getColor().green(),
		((ColorsTab*)tabWidget->widget(2))->_pPointControlColorToolButton->getColor().blue());
	// Couleur des sources ponctuelles
	TYPreferenceManager::setColor("SrcPonctGraphicColor",
		((ColorsTab*)tabWidget->widget(2))->_pSourcePonctColorToolButton->getColor().red(),
		((ColorsTab*)tabWidget->widget(2))->_pSourcePonctColorToolButton->getColor().green(),
		((ColorsTab*)tabWidget->widget(2))->_pSourcePonctColorToolButton->getColor().blue());

	int hsvColor[3];
	((ColorsTab*)tabWidget->widget(2))->_pAltiColorMinToolButton->getColor().getHsv(&hsvColor[0], &hsvColor[1], &hsvColor[2]);
	TYPreferenceManager::setColor("AltiGraphicColorMin", hsvColor[0], hsvColor[1], hsvColor[2]);
	((ColorsTab*)tabWidget->widget(2))->_pAltiColorMaxToolButton->getColor().getHsv(&hsvColor[0], &hsvColor[1], &hsvColor[2]);
	TYPreferenceManager::setColor("AltiGraphicColorMax", hsvColor[0], hsvColor[1], hsvColor[2]);


	TYPreferenceManager::setFloat("WinOpacity", float(((ColorsTab*)tabWidget->widget(2))->_pWinOpacitySlider->value()) / 100.0f);
	TYPreferenceManager::setFloat("AltOpacity", float(((ColorsTab*)tabWidget->widget(2))->_pAltOpacitySlider->value()) / 100.0f);
	TYPreferenceManager::setFloat("MaillageOpacity", float(((ColorsTab*)tabWidget->widget(2))->_pMailOpacitySlider->value()) / 100.0f);

	// Acoustique
	qValidator.setBottom(0.001);
	TYPreferenceManager::setDouble("ResisSolDefault", ((AcoustiqueTab*)tabWidget->widget(3))->_pLineEditResistivite->text().toDouble());
	if (qValidator.validate(aQString = ((AcoustiqueTab*)tabWidget->widget(3))->_pLineEditEpaisseur->text(), res) == QValidator::Acceptable)
	{
		TYPreferenceManager::setDouble("EpaisSolDefault", ((AcoustiqueTab*)tabWidget->widget(3))->_pLineEditEpaisseur->text().toDouble());
	}
	TYPreferenceManager::setDouble("EcartTypeDefault", ((AcoustiqueTab*)tabWidget->widget(3))->_pLineEditEcartType->text().toDouble());
	TYPreferenceManager::setDouble("LongueurDefault", ((AcoustiqueTab*)tabWidget->widget(3))->_pLineEditLongueur->text().toDouble());
	TYPreferenceManager::setDouble("MasseVolMatDefault", ((AcoustiqueTab*)tabWidget->widget(3))->_pLineEditMasseVol->text().toDouble());
	TYPreferenceManager::setSpectre("SpectreTransmDefault", ((AcoustiqueTab*)tabWidget->widget(3))->_pSpectreAttMatConst);
	TYPreferenceManager::setSpectre("SpectreAbsoDefault", ((AcoustiqueTab*)tabWidget->widget(3))->_pSpectreAbsoMatConst);

	QString pref = "MaillageDefault";
	TYPreferenceManager::setFloat(pref + "Hauteur", ((AcoustiqueTab*)tabWidget->widget(3))->_pMaillage->getHauteur());
	TYPreferenceManager::setInt(pref + "DataType", ((AcoustiqueTab*)tabWidget->widget(3))->_pMaillage->getDataType());
	TYPreferenceManager::setFloat(pref + "DataFreq", ((AcoustiqueTab*)tabWidget->widget(3))->_pMaillage->getDataFreq());
	pref = "PaletteDefault";

	/*
	+               OLookupTable legacyTable;
	+
	+
	+               const size_t nb_colors = TYPreferenceManager::getFloat(pref + "NbColors");
	+               const float valueMin = TYPreferenceManager::getFloat(pref + "Min");
	+               const float valueMax = TYPreferenceManager::getFloat(pref + "Max");
	+
	+
	+               legacyTable.resize(nb_colors);
	+            for (size_t i = 0; i < nb_colors; ++i)
	*/
	TYPalette* pPalette = ((AcoustiqueTab*)tabWidget->widget(3))->_pMaillage->getPalette();
	TYPreferenceManager::setFloat(pref + "Min", pPalette->getValueMin());
	TYPreferenceManager::setFloat(pref + "Max", pPalette->getValueMax());
	TYPreferenceManager::setUInt(pref + "NbColors", pPalette->getNbColors());
	// XXX Saving those Colors has no meaning without the values
	TYPalette::color_map_const_iter it = pPalette->getColorMap().begin();
	for (size_t i = 0; i < pPalette->getNbColors(); ++i)
	{
		assert(it != pPalette->getColorMap().end() && "getNbColors() should be the number of colors in the color map.");
		const OColor& color = it->second;
		TYPreferenceManager::setColor(pref + "Color" + QString(uintToStr(i).c_str()), color.r, color.g, color.b);
	}

	// On restaure le directory courant precedent
	TYPreferenceManager::setCurrentDirectory(oldDir);

	QList<QMdiSubWindow*> windows = getTYMainWnd()->getWorkspace()->subWindowList();

	for (int i = 0; i < int(windows.count()); ++i)
	{
		QWidget* internal_window = windows.at(i)->widget();

		TYModelerFrame* pM = dynamic_cast<TYModelerFrame*>(internal_window);
		if (pM != nullptr)
		{
			pM->getActionManager()->setHistorySize(((GeneralTab*)tabWidget->widget(4))->_pHistoSizeEdit->text().toInt());
			pM->updatePreferences();
			pM->getView()->getRenderer()->updateDisplayList();
			pM->updateView(true, true);//az--
		}
	}
}

void TYPreferenceDialog::hideEvent(QHideEvent* pEvent)
{
	TYPreferenceManager::saveGeometryToPreferences(metaObject()->className(), this);
}

