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
 * \file TYModelerFrame.cpp
 * \brief Classe generique pour une fenetre de modeleur
 */


#ifdef _MSC_VER
  #pragma warning(disable: 4503)
#endif

#include <qlayout.h>
#include <qstring.h>
#include <qcursor.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qstatusbar.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qmenu.h>
#include <qfiledialog.h>
#include <qslider.h>
#include <qlabel.h>
#include <qprinter.h>
#include <qpainter.h>
#include <qgroupbox.h>
#include <qclipboard.h>
#include <qmessagebox.h>
#include <qtoolbutton.h>
//Added by qt3to4:
#include <QResizeEvent>
#include <QWheelEvent>
#include <QFocusEvent>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QGridLayout>
#include <QShowEvent>
#include <QKeyEvent>
#include <QHBoxLayout>
#include <QEvent>
#include <QImageWriter>
#include <QPrintDialog>

#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/TYProjet.h"
#include "Tympan/models/business/TYCalcul.h"
#include "Tympan/models/business/TYPreferenceManager.h"
#include "Tympan/models/business/infrastructure/TYBatiment.h"
#include "Tympan/models/business/infrastructure/TYMachine.h"
#include "Tympan/models/business/geometry/TYGeometryNode.h"
#include "Tympan/gui/OSizeManager.h"
#include "Tympan/gui/tools/OGLTextElement.h"
#include "Tympan/gui/tools/OGLScalarBarElement.h"
#include "Tympan/gui/tools/OGLLineElement.h"
#include "Tympan/gui/tools/OGLLightElement.h"
#include "Tympan/gui/tools/OGLGridElement.h"
#include "Tympan/gui/tools/OGLCamera.h"
#include "Tympan/gui/gl/TYSourceLineicGraphic.h"
#include "Tympan/gui/gl/TYSourceSurfacicGraphic.h"
#include "Tympan/gui/gl/TYRayGraphic.h"
#include "Tympan/gui/widgets/TYPrintDialog.h"
#include "Tympan/gui/app/TYElementPicker.h"
#include "Tympan/gui/app/TYCameraEditor.h"
#include "Tympan/gui/app/TYCameraZoneEditor.h"
#include "Tympan/gui/app/TYDistanceEditor.h"
#include "Tympan/gui/app/TYPositionEditor.h"
#include "Tympan/gui/app/TYPickEditor.h"
#include "Tympan/gui/app/TYAbstractSceneEditor.h"
#include "Tympan/gui/app/TYApplication.h"
#include "Tympan/gui/app/TYSetCameraCoordinates.h"
#include "TYModelerFrame.h"

#define TR(id) OLocalizator::getString("TYModelerFrame", (id))
#define IMG(id) OLocalizator::getPicture("TYModelerFrame", (id))

using namespace Qt;

// Couleurs
static double gridColor[] = { 0.59, 0.41, 0.42 };
static double fontColor[] = { 0.1, 0.43, 0.37 };
static double rendererColor[] = { 0.91, 0.91, 0.91 };


TYModelerFrame::TYModelerFrame(QWidget* parent, const char* name, Qt::WFlags f)
    : QWidget(parent, f), _actionManager(10)
{
    setObjectName(name);

    setAttribute(Qt::WA_DeleteOnClose);

    // L'element associe a ce modeler
    _pElement = NULL;

    // Flag pour la sauvegarde des settings
    _firstTimeShown = true;

    // Init des membres pour les editors
    _pCurrentEditor = NULL;
    _lastEditorMode = NoMode;
    _editorModeAccepted = false;

    // Init des membres pour la grille
    _showGrid = false;

    _curViewType = PerspView;

    // Le focus se fait avec la touche tab ou la souris
    setFocusPolicy(StrongFocus);

    // Pour pouvoir afficher la position courante du curseur
    setMouseTracking(true);

    // Layout
    _pLayout = new QGridLayout();
    _pLayout->setRowStretch(0, 0);
    _pLayout->setRowStretch(1, 1);
    _pLayout->setRowStretch(2, 0);
    setLayout(_pLayout);

    // Initialisation des parametres de la grille.
    _gridDimX = 200; _gridDimY = 200; _gridStep = 5;

    // Widgets
    _pCtrlLayout = new QHBoxLayout();
    _pLayout->addLayout(_pCtrlLayout, 0, 0);
    _pCtrlLayout->setSpacing(5);

    // Combo box type de Vue
    _pViewTypeBox = new QComboBox(this);
    _pCtrlLayout->addWidget(_pViewTypeBox, 0);
    _pViewTypeBox->insertItem(TopView, TR("id_top_view"));
    _pViewTypeBox->insertItem(LeftView, TR("id_left_view"));
    _pViewTypeBox->insertItem(FrontView, TR("id_front_view"));
    _pViewTypeBox->insertItem(PerspView, TR("id_3d_view"));
    _pViewTypeBox->insertItem(FreeView, TR("id_free_view"));
    connect(_pViewTypeBox, SIGNAL(activated(int)), this, SLOT(setViewType(int)));

    // Combo box mode de Rendu
    _pRenderModeBox = new QComboBox(this);
    _pCtrlLayout->addWidget(_pRenderModeBox, 0);
    _pRenderModeBox->insertItem(Points, TR("id_points_mode"));
    _pRenderModeBox->insertItem(Wireframe, TR("id_wireframe_mode"));
    _pRenderModeBox->insertItem(Surface, TR("id_surface_mode"));
    connect(_pRenderModeBox, SIGNAL(activated(int)), this, SLOT(setRenderModeSlot(int)));

    _pCtrlLayout->addSpacing(7);

    // Btn Fit
    QToolButton* pFitBtn = new QToolButton(this);
    pFitBtn->setIcon(QPixmap(IMG("id_icon_fit_btn")));
    pFitBtn->setText(TR("id_fit_btn"));
    pFitBtn->setToolTip(TR("id_fit_btn"));
    pFitBtn->setFixedSize(24, 24);
    connect(pFitBtn, SIGNAL(clicked()), this, SLOT(fit()));
    _pCtrlLayout->addWidget(pFitBtn, 0);

    // Btn Grille
    _pGridBtn = new QToolButton(this);
    _pGridBtn->setIcon(QPixmap(IMG("id_icon_grid_btn")));
    _pGridBtn->setText(TR("id_grid_btn"));
    _pGridBtn->setToolTip(TR("id_grid_btn"));
    _pGridBtn->setCheckable(true);
    _pGridBtn->setChecked(true);
    _pGridBtn->setFixedSize(24, 24);
    _pCtrlLayout->addWidget(_pGridBtn, 0);
    connect(_pGridBtn, SIGNAL(toggled(bool)), this, SLOT(showGrid(bool)));

    // Btn Show echelle.
    // L'echelle est mise a jour a la fin de chaque deplacement de camera (cf. TYCameraEditor)
    _showScale = true;
    _pShowScale = new QToolButton(this);
    _pShowScale->setIcon(QPixmap(IMG("id_icon_show_scale_btn")));
    _pShowScale->setText(TR("id_show_scale_btn"));
    _pShowScale->setToolTip(TR("id_show_scale_btn"));
    _pShowScale->setCheckable(true);
    _pShowScale->setChecked(_showScale);
    _pShowScale->setFixedSize(24, 24);
    _pCtrlLayout->addWidget(_pShowScale, 0);
    connect(_pShowScale, SIGNAL(toggled(bool)), this, SLOT(showScale(bool)));

    // Btn Grille Magnetique
    _pSnapGridBtn = new QToolButton(this);
    _pSnapGridBtn->setIcon(QPixmap(IMG("id_icon_snapgrid_btn")));
    _pSnapGridBtn->setText(TR("id_snapgrid_btn"));
    _pSnapGridBtn->setToolTip(TR("id_snapgrid_btn"));
    _pSnapGridBtn->setCheckable(true);
    _pSnapGridBtn->setFixedSize(24, 24);
    _pCtrlLayout->addWidget(_pSnapGridBtn, 0);
    connect(_pSnapGridBtn, SIGNAL(toggled(bool)), this, SLOT(setSnapGridActive(bool)));
    setSnapGridActive(true);

    // Btn set camera coordinates
    _pSetCameraCoordinatesBtn = new QToolButton(this);
    _pSetCameraCoordinatesBtn->setIcon(QPixmap(IMG("id_icon_setcameracoordinates_btn")));
    _pSetCameraCoordinatesBtn->setText(TR("id_setcameracoordinates_btn"));
    _pSetCameraCoordinatesBtn->setToolTip(TR("id_setcameracoordinates_btn"));
    _pSetCameraCoordinatesBtn->setFixedSize(24, 24);
    _pCtrlLayout->addWidget(_pSetCameraCoordinatesBtn, 0);
    connect(_pSetCameraCoordinatesBtn, SIGNAL(clicked()), this, SLOT(setCameraCoordinates()));

    _pCtrlLayout->addSpacing(7);

    // Btn d'edition de l'element
    QToolButton*   pEditEltBtn = new QToolButton(this);
    pEditEltBtn->setIcon(QPixmap(IMG("id_icon_editeelt_btn")));
    pEditEltBtn->setText(TR("id_editelt_btn"));
    pEditEltBtn->setToolTip(TR("id_editelt_btn"));
    pEditEltBtn->setFixedSize(24, 24);
    _pCtrlLayout->addWidget(pEditEltBtn, 0);
    connect(pEditEltBtn, SIGNAL(clicked()), this, SLOT(editElement()));

    _pCtrlLayout->addSpacing(7);

    // Btn Screenshot
    _pScreenShotBtn = new QToolButton(this);
    _pScreenShotBtn->setIcon(QPixmap(IMG("id_icon_screenshot_btn")));
    _pScreenShotBtn->setText(TR("id_screenshot_btn"));
    _pScreenShotBtn->setToolTip(TR("id_screenshot_btn"));
    _pScreenShotBtn->setFixedSize(24, 24);
    connect(_pScreenShotBtn, SIGNAL(clicked()), this, SLOT(screenShot()));
    _pCtrlLayout->addWidget(_pScreenShotBtn, 0);

    // Btn Copy
    QToolButton* pCopyBtn = new QToolButton(this);
    pCopyBtn->setIcon(QPixmap(IMG("id_icon_copy_btn")));
    pCopyBtn->setText(TR("id_copy_btn"));
    pCopyBtn->setToolTip(TR("id_copy_btn"));
    pCopyBtn->setFixedSize(24, 24);
    connect(pCopyBtn, SIGNAL(clicked()), this, SLOT(copy()));
    _pCtrlLayout->addWidget(pCopyBtn, 0);

    _pCtrlLayout->addSpacing(7);

    // Btn Show Sources
    _pShowSourcesBtn = new QToolButton(this);
    _pShowSourcesBtn->setIcon(QPixmap(IMG("id_icon_source")));
    _pShowSourcesBtn->setText(TR("id_show_source_btn"));
    _pShowSourcesBtn->setToolTip(TR("id_show_source_btn"));
    _pShowSourcesBtn->setCheckable(true);
    _pShowSourcesBtn->setChecked(false);
    _pShowSourcesBtn->setFixedSize(24, 24);
    _pCtrlLayout->addWidget(_pShowSourcesBtn, 0);
    connect(_pShowSourcesBtn, SIGNAL(toggled(bool)), this, SLOT(showSources(bool)));
    showSources(_pShowSourcesBtn->isChecked());

    // Btn Show Normals
    _pShowNormalsBtn = new QToolButton(this);
    _pShowNormalsBtn->setIcon(QPixmap(IMG("id_icon_normals")));
    _pShowNormalsBtn->setText(TR("id_show_normals_btn"));
    _pShowNormalsBtn->setToolTip(TR("id_show_normals_btn"));
    _pShowNormalsBtn->setCheckable(true);
    _pShowNormalsBtn->setChecked(false);
    _pShowNormalsBtn->setFixedSize(24, 24);
    _pCtrlLayout->addWidget(_pShowNormalsBtn, 0);
    connect(_pShowNormalsBtn, SIGNAL(toggled(bool)), this, SLOT(showNormals(bool)));
    showNormals(_pShowNormalsBtn->isChecked());

    // Btn Show Plafond
    _pShowPlafondBtn = new QToolButton(this);
    _pShowPlafondBtn->setIcon(QPixmap(IMG("id_icon_plafond")));
    _pShowPlafondBtn->setText(TR("id_show_plafond_btn"));
    _pShowPlafondBtn->setToolTip(TR("id_show_plafond_btn"));
    _pShowPlafondBtn->setCheckable(true);
    _pShowPlafondBtn->setChecked(false);
    _pShowPlafondBtn->setFixedSize(24, 24);
    _pCtrlLayout->addWidget(_pShowPlafondBtn, 0);
    connect(_pShowPlafondBtn, SIGNAL(toggled(bool)), this, SLOT(showPlafond(bool)));
    showPlafond(_pShowPlafondBtn->isChecked());

    // Btn Show Rayons
    _pShowRaysBtn = new QToolButton(this);
    _pShowRaysBtn->setIcon(QPixmap(IMG("id_icon_rays")));
    _pShowRaysBtn->setText(TR("id_show_rays_btn"));
    _pShowRaysBtn->setToolTip(TR("id_show_rays_btn"));

    _pShowRaysBtn->setCheckable(true);
    _pShowRaysBtn->setChecked(false);
    _pShowRaysBtn->setFixedSize(24, 24);
    _pCtrlLayout->addWidget(_pShowRaysBtn, 0);
    connect(_pShowRaysBtn, SIGNAL(toggled(bool)), this, SLOT(showRays(bool)));
    showRays(_pShowRaysBtn->isChecked());

    // Vue graphique
    _pView = new TYRenderWindowInteractor(this, "vue graphique");
    _pLayout->addWidget(_pView, 1, 0);

    QObject::connect(_pView, SIGNAL(mouseMoved(int, int, Qt::MouseButtons, Qt::KeyboardModifiers)), this, SLOT(updateCurPosInfo()));

    // Status bar
    _pStatusBar = new QStatusBar(this);
    _pLayout->addWidget(_pStatusBar, 2, 0);

    // Cameras pour chaque type de vue
    NxReal fromTop[3] = {0, 500, 0};
    NxReal toTop[3] = {0, 0, 0};
    NxReal upTop[3] = {0, 0, -1};
    _pOGLCameras[TopView] = new OGLCamera(fromTop, toTop, upTop, 100, 100, PARALLEL);

    NxReal fromLeft[3] = { -500, 0, 0};
    NxReal toLeft[3] = {0, 0, 0};
    NxReal upLeft[3] = {0, 1, 0};
    _pOGLCameras[LeftView] = new OGLCamera(fromLeft, toLeft, upLeft, 100, 100, PARALLEL);

    NxReal fromFront[3] = {0, 0, 500};
    NxReal toFront[3] = {0, 0, 0};
    NxReal upFront[3] = {0, 1, 0};
    _pOGLCameras[FrontView] = new OGLCamera(fromFront, toFront, upFront, 100, 100, PARALLEL);

    NxReal fromPersp[3] = {0, 10000, 10000};
    NxReal toPersp[3] = {0, 0, 0};
    NxReal upPersp[3] = {0, 1, 0};
    _pOGLCameras[PerspView] = new OGLCamera(fromPersp, toPersp, upPersp, 100, 100, PERSPECTIVE);

    NxReal fromFree[3] = {0, 0, 0};
    NxReal toFree[3] = {0, 0, 0.1};
    NxReal upFree[3] = {0, 1, 0};
    _pOGLCameras[FreeView] = new OGLCamera(fromFree, toFree, upFree, 100, 100, FREE);

    // Triedre XYZ
    _pOGLLineElementAxeX = new OGLLineElement();
    _pOGLLineElementAxeX->setColor(OColor(0, 0, 1));
    _pOGLLineElementAxeX->setVisibility(true);
    _pOGLLineElementAxeX->setIs3D(true);
    _pOGLLineElementAxeY = new OGLLineElement();
    _pOGLLineElementAxeY->setColor(OColor(0, 0, 1));
    _pOGLLineElementAxeY->setVisibility(true);
    _pOGLLineElementAxeY->setIs3D(true);
    _pOGLLineElementAxeZ = new OGLLineElement();
    _pOGLLineElementAxeZ->setColor(OColor(0, 0, 1));
    _pOGLLineElementAxeZ->setVisibility(true);
    _pOGLLineElementAxeZ->setIs3D(true);

    _pOGLTextElementLabelX = new OGLTextElement();
    _pOGLTextElementLabelX->setTextToDisplay("X");
    _pOGLTextElementLabelX->setFont(IMG("id_font_bold"));
    OColor oColor;
    oColor.r = fontColor[0];
    oColor.g = fontColor[1];
    oColor.b = fontColor[2];
    _pOGLTextElementLabelX->setColor(OColor(0, 0, 0));
    _pOGLTextElementLabelX->setVisibility(true);

    _pOGLTextElementLabelY = new OGLTextElement();
    _pOGLTextElementLabelY->setTextToDisplay("Y");
    _pOGLTextElementLabelY->setFont(IMG("id_font_bold"));
    _pOGLTextElementLabelY->setColor(OColor(0, 0, 0));
    _pOGLTextElementLabelY->setVisibility(true);

    _pOGLTextElementLabelZ = new OGLTextElement();
    _pOGLTextElementLabelZ->setTextToDisplay("Z");
    _pOGLTextElementLabelZ->setFont(IMG("id_font_bold"));
    _pOGLTextElementLabelZ->setColor(OColor(0, 0, 0));
    _pOGLTextElementLabelZ->setVisibility(true);

    _pView->getRenderer()->addOGLElement(_pOGLLineElementAxeX);
    _pView->getRenderer()->addOGLElement(_pOGLLineElementAxeY);
    _pView->getRenderer()->addOGLElement(_pOGLLineElementAxeZ);
    _pView->getRenderer()->addOGLElement(_pOGLTextElementLabelX);
    _pView->getRenderer()->addOGLElement(_pOGLTextElementLabelY);
    _pView->getRenderer()->addOGLElement(_pOGLTextElementLabelZ);

    // Label
    _pOGLTextElement = new OGLTextElement();
    _pOGLTextElement->setTextToDisplay("");
    _pOGLTextElement->setFont(IMG("id_font_bold"));
    _pOGLTextElement->setColor(oColor);
    _pOGLTextElement->setVisibility(true);
    _pView->getRenderer()->addOGLElement(_pOGLTextElement);

#ifndef NO_GRID
    // Construction de la grille
    _pOGLGridElement = new OGLGridElement();
    _pOGLGridElement->setColor(OColor(gridColor[0], gridColor[1], gridColor[2]));
    _pOGLGridElement->setVisibility(true);
    _pOGLGridElement->setIs3D(true);
    _pView->getRenderer()->addOGLElement(_pOGLGridElement);
#endif//NO_GRID

    // Axes de la grille
    _pOGLLineElementX = new OGLLineElement();
    _pOGLLineElementX->setPoint1(OPoint3D(-100, 0, 0));
    _pOGLLineElementX->setPoint2(OPoint3D(100, 0, 0));
    _pOGLLineElementX->setColor(OColor(0, 0, 0));
    _pOGLLineElementX->setVisibility(true);
    _pOGLLineElementX->setIs3D(true);
    _pOGLLineElementX->setLineWidth(2.0);

    _pOGLLineElementY = new OGLLineElement();
    _pOGLLineElementY->setPoint1(OPoint3D(0, -100, 0));
    _pOGLLineElementY->setPoint2(OPoint3D(0, 100, 0));
    _pOGLLineElementY->setColor(OColor(0, 0, 0));
    _pOGLLineElementY->setVisibility(true);
    _pOGLLineElementY->setIs3D(true);
    _pOGLLineElementY->setLineWidth(2.0);

    _pView->getRenderer()->addOGLElement(_pOGLLineElementX);
    _pView->getRenderer()->addOGLElement(_pOGLLineElementY);

    // Echelle
    _pOGLScalarBarElement = new OGLScalarBarElement();
    _pOGLScalarBarElement->setPosition(OPoint3D(100, 50, 0));
    _pOGLScalarBarElement->setSizes(100, 8);
    _pOGLScalarBarElement->initPositions();
    _pOGLScalarBarElement->setFont(IMG("id_font"));
    _pOGLScalarBarElement->setFontColor(OColor(0, 0, 0));
    _pOGLScalarBarElement->setTexts("0", "0", "0");
    _pOGLScalarBarElement->setVisibility(true);
    _pView->getRenderer()->addOGLElement(_pOGLScalarBarElement);

    // Lumieres
    _pLightElement = new OGLLightElement();
    _pLightElement->init(0, OPoint3D(0.0,  400.0,    0.0), 1.0f);
    _pView->getRenderer()->addLight(_pLightElement);


    // Couleur de fond
    getRenderer()->setBackground(rendererColor);

    // Type de rendu
    setRenderMode(Surface, true);

    _wireframeOnMovingCamera = false;
    _lastRenderMode = _curRenderMode;

    // Gestion du picking
    _pPicker = new TYElementPicker(this);

    // Init chaque view
    for (int i = 0; i < NbOfViews; i++)
    {
        setViewType(i);
    }

    // Editors
    _pCameraEditor = new TYCameraEditor(this);
    _pCameraZoneEditor = new TYCameraZoneEditor(this);
    _pDistanceEditor = new TYDistanceEditor(this);
    _pPositionEditor = new TYPositionEditor(this);

    // Le pick editor
    _pPickEditor = new TYPickEditor(this);
    _pPickEditor->connect();

    // Mode d'edition
    setEditorModeToCamera();

    // Init chaque view
    //  for (int i = 0; i < NbOfViews; i++)
    //  {
    //      setViewType(i);
    //  }

    resizeGrid();
    showGrid(_pGridBtn->isChecked());
    showScale(_pShowScale->isChecked());

    setViewType(TopView);

    // Chargement des preferences
    updatePreferences();

    showMaximized();
    _pOGLTextElement->setDisplayPosition(5, rect().bottom() - rect().top());
}


TYModelerFrame::~TYModelerFrame()
{
    disconnect(_pViewTypeBox, SIGNAL(activated(int)), this, SLOT(setViewType(int)));
    disconnect(_pRenderModeBox, SIGNAL(activated(int)), this, SLOT(setRenderModeSlot(int)));
    disconnect(_pGridBtn, SIGNAL(toggled(bool)), this, SLOT(showGrid(bool)));
    disconnect(_pShowScale, SIGNAL(toggled(bool)), this, SLOT(showScale(bool)));
    disconnect(_pSnapGridBtn, SIGNAL(toggled(bool)), this, SLOT(setSnapGridActive(bool)));
    disconnect(_pSetCameraCoordinatesBtn, SIGNAL(clicked()), this, SLOT(setCameraCoordinates()));
    disconnect(_pScreenShotBtn, SIGNAL(clicked()), this, SLOT(screenShot()));
    disconnect(_pShowSourcesBtn, SIGNAL(toggled(bool)), this, SLOT(showSources(bool)));
    disconnect(_pShowNormalsBtn, SIGNAL(toggled(bool)), this, SLOT(showNormals(bool)));
    disconnect(_pShowPlafondBtn, SIGNAL(toggled(bool)), this, SLOT(showPlafond(bool)));
    disconnect(_pShowRaysBtn, SIGNAL(toggled(bool)), this, SLOT(showRays(bool)));
    disconnect(_pView, SIGNAL(mouseMoved(int, int, Qt::MouseButtons, Qt::KeyboardModifiers)), this, SLOT(updateCurPosInfo()));


    delete _pCameraEditor;
    delete _pCameraZoneEditor;
    delete _pDistanceEditor;
    delete _pPositionEditor;
    delete _pPickEditor;
    delete _pPicker;

    for (int i = 0; i < NbOfViews; i++)
    {
        delete _pOGLCameras[i];
        _pOGLCameras[i] = NULL;
    }

    _pView->getRenderer()->removeOGLElement(_pOGLLineElementAxeX);
    delete _pOGLLineElementAxeX;
    _pView->getRenderer()->removeOGLElement(_pOGLLineElementAxeY);
    delete _pOGLLineElementAxeY;
    _pView->getRenderer()->removeOGLElement(_pOGLLineElementAxeZ);
    delete _pOGLLineElementAxeZ;
    _pView->getRenderer()->removeOGLElement(_pOGLTextElementLabelX);
    delete _pOGLTextElementLabelX;
    _pView->getRenderer()->removeOGLElement(_pOGLTextElementLabelY);
    delete _pOGLTextElementLabelY;
    _pView->getRenderer()->removeOGLElement(_pOGLTextElementLabelZ);
    delete _pOGLTextElementLabelZ;

    _pView->getRenderer()->removeOGLElement(_pOGLTextElement);
    delete _pOGLTextElement;
    _pView->getRenderer()->removeOGLElement(_pOGLScalarBarElement);
    delete _pOGLScalarBarElement;
    _pView->getRenderer()->removeOGLElement(_pOGLLineElementX);
    delete _pOGLLineElementX;
    _pView->getRenderer()->removeOGLElement(_pOGLLineElementY);
    delete _pOGLLineElementY;



#ifndef NO_GRID
    _pView->getRenderer()->removeOGLElement(_pOGLGridElement);
    delete _pOGLGridElement;
#endif// NO_GRID
    _pView->getRenderer()->removeLights();
    delete _pLightElement;
}

bool TYModelerFrame::close()
{
    return QWidget::close();
}

bool TYModelerFrame::isElementInCurrentCalcul()
{
    bool ret = false;

    if (getTYApp()->getCurProjet() && getTYApp()->getCurProjet()->getCurrentCalcul() && _pElement)
    {
        // L'element fait-il parti de la selection du calcul courant ?
        ret = getTYApp()->getCurProjet()->getCurrentCalcul()->isInSelection(_pElement);
    }

    return ret;
}

bool TYModelerFrame::isElementInCurrentProjet()
{
    bool ret = false;

    if (getTYApp()->getCurProjet() && _pElement)
    {
        // On commence directement a partir de l'element lui-meme
        TYElement* pParent = _pElement;

        while (pParent)
        {
            // Le parent est le projet courant ?
            if (pParent == getTYApp()->getCurProjet())
            {
                ret = true;
                break;
            }
            // Parent du parent
            pParent = pParent->getParent();
        }
    }

    return ret;
}

bool TYModelerFrame::askForResetResultat()
{
    bool ret = true;

    if (isElementInCurrentCalcul())
    {
        ret = getTYApp()->getCalculManager()->askForResetResultat();
    }

    return ret;
}

void TYModelerFrame::setViewType(int view)
{
    if (view >= NbOfViews) { return; }

    // View type
    _curViewType = view;

    bool showGridXY = false;
    bool showGridXZ = false;
    bool showGridZY = false;
    QString labelStr;

    // Pre traitement selon la vue activee
    switch (view)
    {
        case TopView:
            showGridXZ = _showGrid;
            labelStr = TR("id_top_view");
            _pSetCameraCoordinatesBtn->setEnabled(false);
            break;
        case LeftView:
            showGridZY = _showGrid;
            labelStr = TR("id_left_view");
            _pSetCameraCoordinatesBtn->setEnabled(false);
            break;
        case FrontView:
            showGridXY = _showGrid;
            labelStr = TR("id_front_view");
            _pSetCameraCoordinatesBtn->setEnabled(false);
            break;
        case PerspView:
            showGridXZ = _showGrid;
            labelStr = TR("id_3d_view");
            _pSetCameraCoordinatesBtn->setEnabled(false);
            break;
        case FreeView:
            showGridXZ = _showGrid;
            labelStr = TR("id_free_view");
            _pSetCameraCoordinatesBtn->setEnabled(true);
            break;
        default: break;
    }

    //_pView->setFocus();

    // Affichage de la grille correspondant au type de vue
    setGridLinesActorsVisibility(showGridXY, showGridXZ, showGridZY);

    // Label
    _pOGLTextElement->setTextToDisplay(labelStr);

    // Camera
    getRenderer()->setActiveCamera(_pOGLCameras[view]);
    getRenderer()->init(_pView->width(), _pView->height());

    // Update l'element associe a ce modeler
    if (_pElement)
    {
        _pElement->updateGraphicTree();
        _pElement->getGraphicObject()->update(true);
    }

    _pView->getRenderer()->updateDisplayList();
    // Updates
    updateView();

    _pViewTypeBox->setCurrentIndex(_curViewType);
    emit viewTypeChanged(_curViewType);
}

void TYModelerFrame::setRenderModeSlot(int mode)
{
    setRenderMode(mode, true);
}


void TYModelerFrame::setRenderMode(int mode, bool bUpdateGL)
{
    if (mode >= NbOfRenderMode) { return; }

    // Mode
    _curRenderMode = mode;

    if (mode == Points)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }
    else if (mode == Wireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else if (mode == Surface)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    else if (mode == Flat)
    {
        glShadeModel(GL_FLAT);
    }
    else if (mode == Gouraud)
    {
        glShadeModel(GL_SMOOTH);
    }

    _pRenderModeBox->setCurrentIndex(_curRenderMode);

    if (bUpdateGL == true)
    {
        _pView->updateGL();
    }
}

void TYModelerFrame::setEditorMode(int mode)
{
    if (!_editorModeAccepted)
    {
        getPickEditor()->usePopup(true);
        getPickEditor()->useHighlight(false);

        if (_pCurrentEditor)
        {
            _pCurrentEditor->disconnect();
            _pCurrentEditor->close();
        }

        _editorModeAccepted = true;

        switch (mode)
        {
            case CameraMode:
                //fit();
                getPickEditor()->useHighlight(true);
                _pCurrentEditor = _pCameraEditor;
                break;
            case CameraZoneMode:
                getPickEditor()->useHighlight(true);
                _pCurrentEditor = _pCameraZoneEditor;
                break;
            case DistanceMode:
                _pCurrentEditor = _pDistanceEditor;
                break;
            case MovingMode:
                _pCurrentEditor = _pPositionEditor;
                _pPositionEditor->setMode(TYPositionEditor::Moving);
                break;
            case RotationMode:
                _pCurrentEditor = _pPositionEditor;
                _pPositionEditor->setMode(TYPositionEditor::Rotation);
                break;
            case EditionMode:
                _pCurrentEditor = _pPositionEditor;
                _pPositionEditor->setMode(TYPositionEditor::Edition);
                break;
            case NoMode:
            default:
                getPickEditor()->useHighlight(true);
                _pCurrentEditor = NULL;
                mode = NoMode;
                _editorModeAccepted = false;
                break;
        }
    }

    // On sauve le mode courant
    _lastEditorMode = mode;

    // Init
    if (_pCurrentEditor)
    {
        _pCurrentEditor->connect();
        _pCurrentEditor->init();
    }

    // Pour la prochaine requete
    _editorModeAccepted = false;

    // The bug goes by here...
    emit(editorModeChanged(mode));

}

void TYModelerFrame::showGrid(bool show)
{
    _showGrid = show;

    _pOGLLineElementX->setVisibility(_showGrid);
    _pOGLLineElementY->setVisibility(_showGrid);

    _pGridBtn->setChecked(_showGrid);

    _pView->getRenderer()->updateDisplayList();
    // Update
    setViewType(_curViewType);
}

void TYModelerFrame::showSources(bool show)
{
    _showSources = show;

    if ((_showSources != TYSourceLineicGraphic::_gVisible) ||
        (_showSources != TYSourceSurfacicGraphic::_gVisible))
    {

        TYApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

        TYSourceLineicGraphic::_gVisible = _showSources;
        TYSourceSurfacicGraphic::_gVisible = _showSources;

        if (_pElement && _pElement->getGraphicObject())
        {
            // Update l'element associe a ce modeler
            _pElement->getGraphicObject()->update(true);

            _pView->getRenderer()->updateDisplayList();

            // Update view
            updateView();
        }

        TYApplication::restoreOverrideCursor();
    }

    _pShowSourcesBtn->setChecked(_showSources);
}

void TYModelerFrame::showRays(bool show)
{
    _showRays = show;
    if (_showRays != TYRayGraphic::_gVisible)
    {

        TYApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

        TYRayGraphic::_gVisible = _showRays;

        if (_pElement && _pElement->getGraphicObject())
        {
            // Update l'element associe a ce modeler
            _pElement->getGraphicObject()->update(true);

            _pView->getRenderer()->updateDisplayList();

            // Update view
            updateView();
        }

        TYApplication::restoreOverrideCursor();
    }

    _pShowRaysBtn->setChecked(_showRays);
}

void TYModelerFrame::showNormals(bool show)
{
    _showNormals = show;

    if (_showNormals != TYElementGraphic::_gDrawNormals)
    {

        TYApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

        TYElementGraphic::_gDrawNormals = _showNormals;

        if (_pElement && _pElement->getGraphicObject())
        {
            // Update l'element associe a ce modeler
            _pElement->getGraphicObject()->update(true);

            _pView->getRenderer()->updateDisplayList();

            // Update view
            updateView();
        }

        TYApplication::restoreOverrideCursor();
    }

    _pShowNormalsBtn->setChecked(_showNormals);

}

void TYModelerFrame::showPlafond(bool show)
{
    _showPlafond = !show; // inversion entre l'etat variable et du bouton

    if (_showPlafond != TYElementGraphic::_gDrawPlafond)
    {

        TYApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

        TYElementGraphic::_gDrawPlafond = _showPlafond;

        if (_pElement && _pElement->getGraphicObject())
        {
            // Update l'element associe a ce modeler
            _pElement->getGraphicObject()->update(true);

            _pView->getRenderer()->updateDisplayList();

            // Update view
            updateView();
        }

        TYApplication::restoreOverrideCursor();
    }

    _pShowPlafondBtn->setChecked(!_showPlafond);

}


void TYModelerFrame::setSnapGridActive(bool state)
{
    _snapGridActive = state;
    _pSnapGridBtn->setChecked(_snapGridActive);
}


void TYModelerFrame::setCameraCoordinates()
{
    if ((getRenderer()->getActiveCamera()) && (getRenderer()->getActiveCamera()->m_eCameraType == FREE))
    {
        TYSetCameraCoordinates* pSetCameraCoordinates = new TYSetCameraCoordinates(this);
        double x, y, z;
        getRenderer()->getActiveCamera()->getTranslation(x, y, z);
        pSetCameraCoordinates->setXCoord(x);
        pSetCameraCoordinates->setYCoord(-z);
        pSetCameraCoordinates->setZCoord(y);
        pSetCameraCoordinates->exec();

        if (pSetCameraCoordinates->result() == QDialog::Accepted)
        {
            if (getRenderer()->getActiveCamera())
            {
                getRenderer()->getActiveCamera()->setTranslation(pSetCameraCoordinates->getXCoord(), pSetCameraCoordinates->getZCoord(), -pSetCameraCoordinates->getYCoord());

                // Update view
                updateView();
            }
        }
    }
}

void TYModelerFrame::screenShot()
{
    QMenu* pMenu = new QMenu(NULL);

    QHash<QAction*, QString> formats;

    for (unsigned int i = 0; i < QImageWriter::supportedImageFormats().count(); i++)
    {
        QString str = QString(QImageWriter::supportedImageFormats().at(i));
        formats.insert(pMenu->addAction(QString("%1...").arg(str)), QString(str));
    }

    pMenu->setMouseTracking(true);
    QAction* id = pMenu->exec(_pScreenShotBtn->mapToGlobal(QPoint(0, _pScreenShotBtn->height() + 1)));

    if (id)
    {
        QString format = formats[id];

        QString filename = QFileDialog::getSaveFileName(this, "", QString::null, QString("*.%1").arg(format.toLower()));
        if (!filename.isEmpty())
        {
            // Prefixe
            if (!filename.endsWith(format.toLower()))
            {
                filename += "." + format.toLower();
            }

            // Restauration des deux buffers
            _pView->repaint(); _pView->repaint();
            if (qApp->hasPendingEvents())
            {
                qApp->processEvents();
            }

            // Snap
            QImage img = _pView->grabFrameBuffer();

            // Save
            QPixmap pix = QPixmap::fromImage(img);
            pix.save(filename, format.toAscii().data(), 80); // qualite 80 (de 0 a 100)
        }
    }

    delete pMenu;
}

void TYModelerFrame::copy()
{
    // Snap
    QImage img = _pView->grabFrameBuffer();

    // Copie dans le presse papier
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setImage(img);
}

void TYModelerFrame::print()
{
    LPTYProjet pProjet = getTYApp()->getCurProjet();
    LPTYCalcul pCalcul = NULL;
    if (pProjet) { pCalcul = pProjet->getCurrentCalcul(); }

    // Print dialog
    TYPrintDialog* pDialog = new TYPrintDialog(this);

    if (pProjet == NULL)
    {
        pDialog->_groupBoxProjet->setEnabled(false);
    }
    if (pCalcul == NULL)
    {
        pDialog->_groupBoxCalcul->setEnabled(false);
    }

    if (_pElement)
    {
        if (dynamic_cast<TYBatiment*>(_pElement._pObj) != nullptr)
        {
            pDialog->_groupBoxBatiment->show();
        }
        else if (dynamic_cast<TYMachine*>(_pElement._pObj) != nullptr)
        {
            pDialog->_groupBoxMachine->show();
        }
        else if (dynamic_cast<TYSiteNode*>(_pElement._pObj) != nullptr)
        {
            pDialog->_groupBoxSite->show();
        }
    }

    if (pDialog->exec() == QDialog::Accepted)
    {
        QPrinter* printer = new QPrinter();

        QPrintDialog dialog(printer, this);
        if (dialog.exec())
        {
            QPainter paint(printer);

            double w = double(printer->width());
            double h = double(printer->height());
            double scalew = w / 700;
            double scaleh = h / 700;

            // Snap
            QImage img = _pView->grabFrameBuffer();
            img = img.scaled(int(w * 9 / 10), int(h * 2 / 3), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            int x = int(double(printer->width() - img.width()) / 2);
            int y = int(double(printer->height() - img.height()) * 1 / 3);
            QPixmap pm = QPixmap::fromImage(img);
            paint.drawPixmap(x, y, pm);

            paint.setFont(QFont("Times", (int)(10 * scaleh)));
            int mid = int(w / 2) - int(paint.fontMetrics().width(pDialog->_lineEditTete->text()) / 2);
            paint.drawText(mid, 20, pDialog->_lineEditTete->text());

            mid = int(w / 2) - int(paint.fontMetrics().width(pDialog->_lineEditPied->text()) / 2);
            paint.drawText(mid, printer->height() - 20, pDialog->_lineEditPied->text());

            paint.setFont(QFont("Times", (int)(15 * scaleh), QFont::Bold));
            mid = int(w / 2) - int(paint.fontMetrics().width(pDialog->_lineEditTitre->text()) / 2);
            paint.drawText(mid, y - 20, pDialog->_lineEditTitre->text());

            x = int(50 * scalew);
            int x1 = int(w / 2);
            y = int(double(printer->height()) * 2 / 3 + 20 * scaleh);
            int stepy = int (10 * scaleh);
            paint.setFont(QFont("Times", (int)(10 * scaleh)));

            if (pProjet)
            {
                if (pDialog->_checkBoxNomProjet->isChecked())
                {
                    paint.drawText(x, y, TR("id_print_nom_projet"));
                    paint.drawText(x1, y, pProjet->getName());
                    y += stepy;
                }
                if (pDialog->_checkBoxAuteurProjet->isChecked())
                {
                    paint.drawText(x, y, TR("id_print_auteur_projet"));
                    paint.drawText(x1, y, pProjet->getAuteur());
                    y += stepy;
                }
                if (pDialog->_checkBoxDateProjet->isChecked())
                {
                    paint.drawText(x, y, TR("id_print_date_creation"));
                    paint.drawText(x1, y, pProjet->getDateCreation());
                    y += stepy;
                    paint.drawText(x, y, TR("id_print_date_modif"));
                    paint.drawText(x1, y, pProjet->getDateModif());
                    y += stepy;
                }
                if (pDialog->_checkBoxCommentProjet->isChecked())
                {
                    paint.drawText(x, y, TR("id_print_comment"));
                    paint.drawText(x1, y, pProjet->getComment());
                    y += stepy;
                }
            }

            if (pCalcul)
            {
                y += int(double(stepy) * 1.5);
                if (pDialog->_checkBoxNomCalcul->isChecked())
                {
                    paint.drawText(x, y, TR("id_print_nom_calcul"));
                    paint.drawText(x1, y, pCalcul->getName());
                    y += stepy;
                }
                if (pDialog->_checkBoxDateCalcul->isChecked())
                {
                    paint.drawText(x, y, TR("id_print_date_creation"));
                    paint.drawText(x1, y, pCalcul->getDateCreation());
                    y += stepy;
                    paint.drawText(x, y, TR("id_print_date_modif"));
                    paint.drawText(x1, y, pCalcul->getDateModif());
                    y += stepy;
                }
                if (pDialog->_checkBoxCommentCalcul->isChecked())
                {
                    paint.drawText(x, y, TR("id_print_comment"));
                    paint.drawText(x1, y, pCalcul->getComment());
                    y += stepy;
                }
            }

            if (_pElement)
            {
                y += int(double(stepy) * 1.5);
                TYElement* pElement = (TYElement*) _pElement;
                if (strcmp(_pElement->getClassName(), "TYBatiment") == 0)
                {
                    TYBatiment* pBatiment = (TYBatiment*) pElement;
                    if (pDialog->_checkBoxNomBatiment->isChecked())
                    {
                        paint.drawText(x, y, TR("id_print_nom_batiment"));
                        paint.drawText(x1, y, pBatiment->getName());
                    }
                }
                else if (strcmp(_pElement->getClassName(), "TYMachine") == 0)
                {
                    TYMachine* pMachine = (TYMachine*) pElement;
                    if (pDialog->_checkBoxNomMachine->isChecked())
                    {
                        paint.drawText(x, y, TR("id_print_nom_machine"));
                        paint.drawText(x1, y, pMachine->getName());
                        y += stepy;
                    }
                    if (pDialog->_checkBoxConstrMachine->isChecked())
                    {
                        paint.drawText(x, y, TR("id_print_constr"));
                        paint.drawText(x1, y, pMachine->getConstructeur());
                        y += stepy;
                    }
                    if (pDialog->_checkBoxModelMachine->isChecked())
                    {
                        paint.drawText(x, y, TR("id_print_model"));
                        paint.drawText(x1, y, pMachine->getModele());
                        y += stepy;
                    }
                    if (pDialog->_checkBoxCatMachine->isChecked())
                    {
                        paint.drawText(x, y, TR("id_print_cat"));
                        paint.drawText(x1, y, QString().setNum(pMachine->getCategorie()));
                        y += stepy;
                    }
                    if (pDialog->_checkBoxCommentProjet->isChecked())
                    {
                        paint.drawText(x, y, TR("id_print_comment"));
                        paint.drawText(x1, y, pMachine->getCommentaire());
                    }
                }
                else
                {
                    TYSiteNode* pSite = dynamic_cast<TYSiteNode*>(pElement);
                    if (pSite != nullptr)
                    {
                        if (pDialog->_checkBoxNomSite->isChecked())
                        {
                            paint.drawText(x, y, TR("id_print_nom_site"));
                            paint.drawText(x1, y, pSite->getName());
                        }
                    }
                }
            }
        }

        delete printer;
    }
}

void TYModelerFrame::editElement()
{
    if (_pElement)
    {
        if (_pElement->edit(this) == QDialog::Accepted)
        {
            emit eltModified(_pElement);
        }

        // On update ds ts les cas si un child a ete modifie...
        _pElement->getGraphicObject()->update(true);
        _pView->getRenderer()->updateDisplayList();
        updateView(false, false);
    }
}

OBox getBoundingBox(TYElement* pElement)
{
    // XBH: version simplifiee et +logique...

    OBox boundingBox;
    TYElementGraphic* pTYElementGraphic = pElement->getGraphicObject();

    if (pTYElementGraphic != NULL)
    {
        pTYElementGraphic->computeBoundingBox();
        boundingBox = pTYElementGraphic->GetBox();
    }

    return boundingBox;
}

OBox TYModelerFrame::getGlobalBoundingBox()
{
    OBox globalBoundingBox;
    if (_pElement)
    {
        TYElement* pTYElement = (TYElement*)_pElement;
        TYProjet* pTYProjet = dynamic_cast<TYProjet*>(pTYElement);
        if (pTYProjet != nullptr)
        {
            TYSiteNode* pTYSiteNode = (TYSiteNode*)pTYProjet->getSite();
            pTYElement = pTYSiteNode;
        }

        globalBoundingBox = getBoundingBox(pTYElement);
    }
    return globalBoundingBox;
}

void TYModelerFrame::fit()
{
    int visibilityStates[9];

    // Sauvegarde des etats de visibilite des objets graphiques
    // de "decoration", ils ne doivent pas participer au recadrage
#ifndef NO_GRID
    visibilityStates[0] = _pOGLGridElement->getShowGridXY();
    visibilityStates[1] = _pOGLGridElement->getShowGridXZ();
    visibilityStates[2] = _pOGLGridElement->getShowGridZY();
#endif//NO_GRID

    setGridLinesActorsVisibility(false, false, false);

    visibilityStates[3] = _pOGLLineElementAxeX->getVisibility();
    _pOGLLineElementAxeX->setVisibility(false);
    _pOGLLineElementAxeY->setVisibility(false);
    _pOGLLineElementAxeZ->setVisibility(false);
    visibilityStates[4] = _pOGLTextElementLabelX->getVisibility();
    _pOGLTextElementLabelX->setVisibility(false);
    visibilityStates[5] = _pOGLTextElementLabelY->getVisibility();
    _pOGLTextElementLabelY->setVisibility(false);
    visibilityStates[6] = _pOGLTextElementLabelZ->getVisibility();
    _pOGLTextElementLabelZ->setVisibility(false);

    visibilityStates[7] = _pOGLLineElementX->getVisibility();
    _pOGLLineElementX->setVisibility(false);
    visibilityStates[8] = _pOGLLineElementY->getVisibility();
    _pOGLLineElementY->setVisibility(false);

    //az-- : pour tests reperes machines:
    OBox globalBoundingBox = getGlobalBoundingBox();

    float xMin = globalBoundingBox._min._x;
    float xMax = globalBoundingBox._max._x;
    float yMin = globalBoundingBox._min._y;
    float yMax = globalBoundingBox._max._y;
    float zMin = globalBoundingBox._min._z;
    float zMax = globalBoundingBox._max._z;
    float xDist = max(abs(xMin), abs(xMax)) * 2;
    float yDist = max(abs(yMin), abs(yMax)) * 2;
    float zDist = max(abs(zMin), abs(zMax)) * 2;

    // Cameras pour chaque type de vue
    NxReal fromTop[3] = {0, 500, 0};
    NxReal toTop[3] = {0, 0, 0};
    NxReal upTop[3] = {0, 0, -1};
    _pOGLCameras[TopView]->setFromToUp(fromTop, toTop, upTop);
    _pOGLCameras[TopView]->resetRotations();
    _pOGLCameras[TopView]->resetZoom(xDist, yDist);

    NxReal fromLeft[3] = { -500, 0, 0};
    NxReal toLeft[3] = {0, 0, 0};
    NxReal upLeft[3] = {0, 1, 0};
    _pOGLCameras[LeftView]->setFromToUp(fromLeft, toLeft, upLeft);
    _pOGLCameras[LeftView]->resetZoom(yDist, zDist);

    NxReal fromFront[3] = {0, 0, 500};
    NxReal toFront[3] = {0, 0, 0};
    NxReal upFront[3] = {0, 1, 0};
    _pOGLCameras[FrontView]->setFromToUp(fromFront, toFront, upFront);
    _pOGLCameras[FrontView]->resetZoom(xDist, zDist);

    NxReal fromPersp[3] = {0, 1000, 1000};
    NxReal toPersp[3] = {0, 0, 0};
    NxReal upPersp[3] = {0, 1, 0};
    _pOGLCameras[PerspView]->setFromToUp(fromPersp, toPersp, upPersp);
    _pOGLCameras[PerspView]->resetZoom(xDist, yDist);
    _pOGLCameras[PerspView]->resetRotations();
    _pOGLCameras[PerspView]->setDistanceStep(30, 70, 30);

    NxReal fromFree[3] = {0, 0, 0};
    NxReal toFree[3] = {0, 0, 0.1};
    NxReal upFree[3] = {0, 1, 0};
    _pOGLCameras[FreeView]->setFromToUp(fromFree, toFree, upFree);
    _pOGLCameras[FreeView]->resetZoom();
    _pOGLCameras[FreeView]->resetRotations();
    _pOGLCameras[FreeView]->setTranslation(0, 2, 0);
    _pOGLCameras[FreeView]->setDistanceStep(5, 5, 5);


    // Restauration des etats de visibilite des objets graphiques
#ifndef NO_GRID
    setGridLinesActorsVisibility(visibilityStates[0], visibilityStates[1], visibilityStates[2]);
#endif//NO_GRID
    _pOGLLineElementAxeX->setVisibility(visibilityStates[3]);
    _pOGLLineElementAxeY->setVisibility(visibilityStates[3]);
    _pOGLLineElementAxeZ->setVisibility(visibilityStates[3]);
    _pOGLTextElementLabelX->setVisibility(visibilityStates[4]);
    _pOGLTextElementLabelY->setVisibility(visibilityStates[5]);
    _pOGLTextElementLabelZ->setVisibility(visibilityStates[6]);
    _pOGLLineElementX->setVisibility(visibilityStates[7]);
    _pOGLLineElementY->setVisibility(visibilityStates[8]);

    _pView->getRenderer()->updateDisplayList();
    updateView();
}

void TYModelerFrame::updateView(bool clipping /*=true*/, bool axesAndGrid /*=true*/)
{
    //first look at to update camera positions
    if (getRenderer()->getActiveCamera())
    {
        getRenderer()->getActiveCamera()->lookAt();
        glMatrixMode(GL_MODELVIEW);

        if (axesAndGrid)
        {
            updateAxes();
            updateGrid();
            updateCurPosInfo();
            updateScale();
        }

        if (clipping)
        {
            // Update le render mode
            setRenderMode(getRenderMode(), false);
        }

        _pOGLTextElement->setDisplayPosition(7, rect().bottom() - rect().top() - 88);
        _pOGLScalarBarElement->setPosition(OPoint3D(rect().right() - rect().left() - 100, 50, 0));
        _pOGLScalarBarElement->initPositions();

        //RNU
        //      _pView->getRenderer()->updateDisplayList();

        // Update la vue 3D
        _pView->updateGL();
    }
}

void TYModelerFrame::updateAxes()
{
    _pOGLLineElementAxeX->setVisibility(true);
    _pOGLLineElementAxeY->setVisibility(true);
    _pOGLLineElementAxeZ->setVisibility(true);
    _pOGLTextElementLabelX->setVisibility(true);
    _pOGLTextElementLabelY->setVisibility(true);
    _pOGLTextElementLabelZ->setVisibility(true);

    NxVec3 org = OGLCamera::displayToWorld(NxVec3(50, 50, 0.1));
    OCoord3D origine(org.x, org.y, org.z);

    _pOGLLineElementAxeX->setPoint1(OPoint3D(org.x, org.y, org.z));
    _pOGLLineElementAxeY->setPoint1(OPoint3D(org.x, org.y, org.z));
    _pOGLLineElementAxeZ->setPoint1(OPoint3D(org.x, org.y, org.z));

    NxVec3 extX, extY, extZ;

    switch (getCurrentView())
    {
        case TopView:
        {
            extX = OGLCamera::displayToWorld(NxVec3(90, 50, 0.1));
            extY = OGLCamera::displayToWorld(NxVec3(50, 90, 0.1));
            extZ = OGLCamera::displayToWorld(NxVec3(50, 50, 40.1));
            _pOGLTextElementLabelX->setDisplayPosition(90, 50);
            _pOGLTextElementLabelY->setDisplayPosition(50, 90);
            _pOGLTextElementLabelZ->setDisplayPosition(52, 50);
        }
        break;
        case LeftView:
        {
            extX = OGLCamera::displayToWorld(NxVec3(50, 50, 40.1));
            extY = OGLCamera::displayToWorld(NxVec3(10, 50, 0.1));
            extZ = OGLCamera::displayToWorld(NxVec3(50, 90, 0.1));
            _pOGLTextElementLabelX->setDisplayPosition(52, 50);
            _pOGLTextElementLabelY->setDisplayPosition(10, 50);
            _pOGLTextElementLabelZ->setDisplayPosition(50, 90);
        }
        break;
        case FrontView:
        {
            extX = OGLCamera::displayToWorld(NxVec3(90, 50, 0.1));
            extY = OGLCamera::displayToWorld(NxVec3(50, 50, 40.1));
            extZ = OGLCamera::displayToWorld(NxVec3(50, 90, 0.1));
            _pOGLTextElementLabelX->setDisplayPosition(90, 50);
            _pOGLTextElementLabelY->setDisplayPosition(52, 50);
            _pOGLTextElementLabelZ->setDisplayPosition(50, 90);
        }
        break;
        case PerspView:
        case FreeView:
        {
            NxVec3 org2 = OGLCamera::displayToWorld(NxVec3(50, 90, 0.1));
            double dist = org.distance(org2);
            extX.x = org.x + dist;
            extX.y = org.y;
            extX.z = org.z;
            extY.x = org.x;
            extY.y = org.y;
            extY.z = org.z - dist;
            extZ.x = org.x;
            extZ.y = org.y + dist;
            extZ.z = org.z;

            NxVec3 dispX = OGLCamera::worldToDisplay(extX);
            NxVec3 dispY = OGLCamera::worldToDisplay(extY);
            NxVec3 dispZ = OGLCamera::worldToDisplay(extZ);

            _pOGLTextElementLabelX->setDisplayPosition(dispX.x, dispX.y);
            _pOGLTextElementLabelY->setDisplayPosition(dispY.x, dispY.y);
            _pOGLTextElementLabelZ->setDisplayPosition(dispZ.x, dispZ.y);
        }
        break;
    }

    OPoint3D extremiteX(extX.x, extX.y, extX.z);
    OPoint3D extremiteY(extY.x, extY.y, extY.z);
    OPoint3D extremiteZ(extZ.x, extZ.y, extZ.z);

    _pOGLLineElementAxeX->setPoint2(extremiteX);
    _pOGLLineElementAxeY->setPoint2(extremiteY);
    _pOGLLineElementAxeZ->setPoint2(extremiteZ);
}

void TYModelerFrame::keyPressEvent(QKeyEvent* pEvent)
{
    switch (pEvent->key())
    {
        case Qt::Key_1: setViewType(TopView); break;
        case Qt::Key_2: setViewType(LeftView); break;
        case Qt::Key_3: setViewType(FrontView); break;
        case Qt::Key_4: setViewType(PerspView); break;
        case Qt::Key_5: setViewType(FreeView); break;
        case Qt::Key_V: setViewType((getCurrentView() + 1) % NbOfViews); break;
        case Qt::Key_I: _pView->showInfos(!_pView->getShowInfos()); break;
        case Qt::Key_G: showGrid(!_showGrid); break;
        case Qt::Key_R: setRenderMode((getRenderMode() + 1) % NbOfRenderMode, true); break;
        case Qt::Key_W: setRenderMode(Wireframe, true); break;
        case Qt::Key_S: setRenderMode(Surface, true); break;
        case Qt::Key_Control:   setSnapGridActive(false); break;
        case Qt::Key_C: if (pEvent->modifiers() == Qt::ControlModifier) { copy(); }
            else { setEditorModeToCamera(); }
            break;
        case Qt::Key_F:
        case Qt::Key_F5: fit(); break;
        default: TYApplication::sendEvent(_pView, pEvent);
    }
}

void TYModelerFrame::keyReleaseEvent(QKeyEvent* pEvent)
{
    switch (pEvent->key())
    {
        case Qt::Key_Control:   setSnapGridActive(true); break;
        default: TYApplication::sendEvent(_pView, pEvent);
    }
}

void TYModelerFrame::mouseMoveEvent(QMouseEvent* pEvent)
{
}

void TYModelerFrame::updateCurPosInfo()
{
    // On recupere la pos du curseur
    QPoint curPos = _pView->mapFromGlobal(QCursor::pos());

    // Calcul des coords
    float* pos = new float[3];

    if (computeCurPos(curPos.x(), curPos.y(), pos))
    {
        // Si la grille magnetique est activee
        if (getSnapGridActive())
        {
            TYAbstractSceneEditor::snapToGrid(pos[0], pos[1], pos[2]);
        }

        // Formatage du msg
        QString msg("pos : (%1, %2, %3)");
        // On inverse y et z, et -y... (VTK to TY)
        msg = msg.arg(pos[0], 0, 'f', 2).arg(-pos[2], 0, 'f', 2).arg(pos[1], 0, 'f', 2);
        // Affichage
        statusBar()->showMessage(msg);
    }
    else
    {
        if (getCurrentView() == FreeView)
        {
            double x, y, z;
            getRenderer()->getActiveCamera()->getTranslation(x, y, z);
            QString msg = QString("camera pos : (%1, %2, %3)").arg(x).arg(-z).arg(y);
            statusBar()->showMessage(msg);
        }
        else
        {
            statusBar()->showMessage("");
        }
    }

    delete[] pos;
}

//float* TYModelerFrame::computeCurPos(int x, int y)
bool TYModelerFrame::computeCurPos(int x, int y, float* ret)
{
    int view = getCurrentView();
    if ((view != PerspView) && (view != FreeView))
    {
        // Position du curseur
        NxVec3 pos = OGLCamera::displayToWorld(NxVec3(x, _pView->height() - y, 0));
        ret[0] = pos.x;
        ret[1] = pos.y;
        ret[2] = pos.z;

        switch (getCurrentView())
        {
            case TopView:   ret[1] = 0.0; break;
            case LeftView:  ret[0] = 0.0; break;
            case FrontView: ret[2] = 0.0; break;
        }

        return true;
    }

    return false;
}

void TYModelerFrame::setGridLinesActorsVisibility(bool showGridXY, bool showGridXZ, bool showGridZY)
{
#ifndef NO_GRID
    _pOGLGridElement->setShowGridXY(showGridXY);
    _pOGLGridElement->setShowGridXZ(showGridXZ);
    _pOGLGridElement->setShowGridZY(showGridZY);
#endif//NO_GRID
}

void TYModelerFrame::wheelEvent(QWheelEvent* pEvent)
{
    // Reroute l'event sur le TYRenderWindowInteractor
    TYApplication::sendEvent(_pView, pEvent);
}

void TYModelerFrame::resizeEvent(QResizeEvent* pEvent)
{
    /*  _pView->getRenderer()->updateDisplayList();
        updateView();
        _pView->getRenderer()->updateDisplayList();*/
    updateView();

    // CLM-NT33 : Gestion du maximized et titre fenetre principal
    emit frameResized();
}

void TYModelerFrame::focusInEvent(QFocusEvent* pEvent)
{
    /*  _pView->getRenderer()->updateDisplayList();
        updateView();
        _pView->getRenderer()->updateDisplayList();*/
    updateView();
}

void TYModelerFrame::showEvent(QShowEvent* pEvent)
{
    if (_firstTimeShown)
    {
        TYPreferenceManager::loadGeometryFromPreferences(metaObject()->className(), this);
        _firstTimeShown = false;
    }
    /*  _pView->getRenderer()->updateDisplayList();
        updateView();
        _pView->getRenderer()->updateDisplayList();*/
    updateView();
}

void TYModelerFrame::closeEvent(QCloseEvent* pEvent)
{
    TYPreferenceManager::saveGeometryToPreferences(metaObject()->className(), this);
    pEvent->accept();

    // CLM-NT35 : Gestion du maximized et titre fenetre principal
    emit frameResized();
}

void TYModelerFrame::enterEvent(QEvent* pEvent)
{
    /*  _pView->getRenderer()->updateDisplayList();
        updateView();
        _pView->getRenderer()->updateDisplayList();
        updateView();*/
    emit mouseEnter();
}

void TYModelerFrame::leaveEvent(QEvent* pEvent)
{
    emit mouseLeave();
}

void TYModelerFrame::updateGrid()
{
    if ((getCurrentView() == PerspView) || (getCurrentView() == FreeView))
    {
        _pOGLLineElementX->setPoint1(OPoint3D(0, 0, _gridDimY / 2));
        _pOGLLineElementX->setPoint2(OPoint3D(0, 0, -_gridDimY / 2));

        _pOGLLineElementY->setPoint1(OPoint3D(_gridDimX / 2, 0, 0));
        _pOGLLineElementY->setPoint2(OPoint3D(-_gridDimX / 2, 0, 0));
    }
    else
    {
        int gridXMax = _pView->width();
        int gridYMax = _pView->height();
        if (gridXMax == 0) { gridXMax = 100; }
        if (gridYMax == 0) { gridYMax = 100; }

        //getCenter
        NxVec3 center = OGLCamera::worldToDisplay(NxVec3(0, 0, 0));

        //X
        NxVec3 posX = OGLCamera::displayToWorld(NxVec3(center.x, 0, 0));
        NxVec3 posX2 = OGLCamera::displayToWorld(NxVec3(center.x, gridYMax, 0));
        OPoint3D pt1(posX.x, posX.y, posX.z);
        _pOGLLineElementX->setPoint1(pt1);
        OPoint3D pt2(posX2.x, posX2.y, posX2.z);
        if ((pt1._x != pt2._x) || (pt1._y != pt2._y) || (pt1._z != pt2._z))
        {
            _pOGLLineElementX->setPoint2(OPoint3D(posX2.x, posX2.y, posX2.z));
            OVector3D vecX(pt1, pt2);
        }

        //Y
        NxVec3 posY = OGLCamera::displayToWorld(NxVec3(0, center.y, 0));
        NxVec3 posY2 = OGLCamera::displayToWorld(NxVec3(gridXMax, center.y, 0));
        pt1.setCoords(posY.x, posY.y, posY.z);
        _pOGLLineElementY->setPoint1(pt1);
        pt2.setCoords(posY2.x, posY2.y, posY2.z);
        if ((pt1._x != pt2._x) || (pt1._y != pt2._y) || (pt1._z != pt2._z))
        {
            _pOGLLineElementY->setPoint2(pt2);
            OVector3D vecY(pt1, pt2);
        }
    }
}

void TYModelerFrame::updateElementGraphic(bool force /* = false */)
{
    if (_pElement && _pElement->getGraphicObject())
    {
        _pElement->getGraphicObject()->update(force);
    }
}

void TYModelerFrame::resizeGrid()
{
    _pOGLGridElement->setGridDimX(_gridDimX);
    _pOGLGridElement->setGridDimY(_gridDimY);
    _pOGLGridElement->setGridStep(_gridStep);

    _pOGLLineElementX->setColor(OColor(gridColor[0], gridColor[1], gridColor[2]));
    _pOGLLineElementY->setColor(OColor(gridColor[0], gridColor[1], gridColor[2]));
}

void TYModelerFrame::updatePreferences()
{
    // Mise a jour de la taille de l'historique de l'Action Manager
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "HistoSize"))
    {
        _actionManager.setHistorySize(TYPreferenceManager::getInt(TYDIRPREFERENCEMANAGER, "HistoSize"));
    }
    else
    {
        TYPreferenceManager::setInt(TYDIRPREFERENCEMANAGER, "HistoSize", _actionManager.getHistorySize());
    }

    // Mise a jour de l'etat d'activation du delplacement de camera en mode Wireframe
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "WireframeOnMovingCamera"))
    {
        setWireframeOnMovingCamera(TYPreferenceManager::getBool(TYDIRPREFERENCEMANAGER, "WireframeOnMovingCamera"));
    }
    else
    {
        TYPreferenceManager::setBool(TYDIRPREFERENCEMANAGER, "WireframeOnMovingCamera", false);
        setWireframeOnMovingCamera(false);
    }

    // Mise a jour des couleurs a partir des preferences.
    // Couleur de fond
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "BackgroundColorR"))
    {
        float r, g, b;
        TYPreferenceManager::getColor(TYDIRPREFERENCEMANAGER, "BackgroundColor", r, g, b);
        rendererColor[0] = r / 255;
        rendererColor[1] = g / 255;
        rendererColor[2] = b / 255;
    }
    else
    {
        float r = rendererColor[0] * 255;
        float g = rendererColor[1] * 255;
        float b = rendererColor[2] * 255;
        TYPreferenceManager::setColor(TYDIRPREFERENCEMANAGER, "BackgroundColor", r, g, b);
    }

    getRenderer()->setBackground(rendererColor);

    // Couleur de la grille
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "GridColorR"))
    {
        float r, g, b;
        TYPreferenceManager::getColor(TYDIRPREFERENCEMANAGER, "GridColor", r, g, b);
        gridColor[0] = r / 255;
        gridColor[1] = g / 255;
        gridColor[2] = b / 255;
    }
    else
    {
        float r = gridColor[0] * 255;
        float g = gridColor[1] * 255;
        float b = gridColor[2] * 255;
        TYPreferenceManager::setColor(TYDIRPREFERENCEMANAGER, "GridColor", r, g, b);
    }

    // Couleur de la police
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "FontColorR"))
    {
        float r, g, b;
        TYPreferenceManager::getColor(TYDIRPREFERENCEMANAGER, "FontColor", r, g, b);
        fontColor[0] = r / 255;
        fontColor[1] = g / 255;
        fontColor[2] = b / 255;
    }
    else
    {
        float r = fontColor[0] * 255;
        float g = fontColor[1] * 255;
        float b = fontColor[2] * 255;
        TYPreferenceManager::setColor(TYDIRPREFERENCEMANAGER, "FontColor", r, g, b);
    }

    _pOGLTextElementLabelX->setColor(OColor(0, 0, 0));
    _pOGLTextElementLabelY->setColor(OColor(0, 0, 0));
    _pOGLTextElementLabelZ->setColor(OColor(0, 0, 0));
    _pOGLScalarBarElement->setFontColor(OColor(fontColor[0], fontColor[1], fontColor[2]));

    // Mise a jour de l'eclairage de la scene.
    float intensity = 0.83f;
    if ((TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "Luminosite")))
    {
        intensity = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "Luminosite");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "Luminosite", intensity);
    }

    if (intensity < 0.1f) { intensity = 0.1f; }
    _pLightElement->setIntensity(intensity);

    // Mise a jour de la tolerance du picking.
    float precisPick = 3.0;
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "PrecisPick"))
    {
        precisPick = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, "PrecisPick");
    }
    else
    {
        TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, "PrecisPick", precisPick);
    }
    _pPickEditor->setPickTolerance(precisPick);
    _pPositionEditor->setPickPointPrecision(precisPick);

    // Mise a jour du pas de zoom de la mollette de la souris.
    float zoomStep = 0.2f;
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "ZoomStep"))
    {
        zoomStep = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, "ZoomStep");
    }
    else
    {
        TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, "ZoomStep", zoomStep);
    }
    _pCameraEditor->setWheelStep(zoomStep);

    // Mise a jour du pas de l'angle du positionEditor
    float angle = 15.0;
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "Angle"))
    {
        angle = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, "Angle");
    }
    else
    {
        TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, "Angle", angle);
    }

    _pPositionEditor->setAngleStep(angle);

    // Mise a jour des fonctions attachees a la souris.
    // 2D
    // bouton gauche
    int mouseLeftButtonFunct2D = 1;
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "MouseLeftButtonFunct2D"))
    {
        mouseLeftButtonFunct2D = TYPreferenceManager::getInt(TYDIRPREFERENCEMANAGER, "MouseLeftButtonFunct2D");
    }

    else
    {
        TYPreferenceManager::setInt(TYDIRPREFERENCEMANAGER, "MouseLeftButtonFunct2D", mouseLeftButtonFunct2D);
    }

    switch (mouseLeftButtonFunct2D)
    {
        case 0 : _pCameraEditor->setLeftButtonFunction2D(SLOT(cameraZoom())); break;
        case 1 : _pCameraEditor->setLeftButtonFunction2D(SLOT(cameraTranslate())); break;
    }

    // bouton droit
    int mouseRightButtonFunct2D = 0;
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "MouseRightButtonFunct2D"))
    {
        mouseRightButtonFunct2D = TYPreferenceManager::getInt(TYDIRPREFERENCEMANAGER, "MouseRightButtonFunct2D");
    }
    else
    {
        TYPreferenceManager::setInt(TYDIRPREFERENCEMANAGER, "MouseRightButtonFunct2D", mouseRightButtonFunct2D);
    }

    switch (mouseRightButtonFunct2D)
    {
        case 0 : _pCameraEditor->setRightButtonFunction2D(SLOT(cameraZoom())); break;
        case 1 : _pCameraEditor->setRightButtonFunction2D(SLOT(cameraTranslate())); break;
    }


    // bouton central
    int mouseMiddleButtonFunct2D = 0;
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "MouseMiddleButtonFunct2D"))
    {
        mouseMiddleButtonFunct2D = TYPreferenceManager::getInt(TYDIRPREFERENCEMANAGER, "MouseMiddleButtonFunct2D");
    }
    else
    {
        TYPreferenceManager::setInt(TYDIRPREFERENCEMANAGER, "MouseMiddleButtonFunct2D", mouseMiddleButtonFunct2D);
    }

    switch (mouseMiddleButtonFunct2D)
    {
        case 0 : _pCameraEditor->setMiddleButtonFunction2D(SLOT(cameraZoom())); break;
        case 1 : _pCameraEditor->setMiddleButtonFunction2D(SLOT(cameraTranslate())); break;
    }


    // en 3D
    // bouton gauche
    int mouseLeftButtonFunct3D = 3;
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "MouseLeftButtonFunct3D"))
    {
        mouseLeftButtonFunct3D = TYPreferenceManager::getInt(TYDIRPREFERENCEMANAGER, "MouseLeftButtonFunct3D");
    }
    else
    {
        TYPreferenceManager::setInt(TYDIRPREFERENCEMANAGER, "MouseLeftButtonFunct3D", mouseLeftButtonFunct3D);
    }

    switch (mouseLeftButtonFunct3D)
    {
        case 0 : _pCameraEditor->setLeftButtonFunction3D(SLOT(cameraZoom())); break;
        case 1 : _pCameraEditor->setLeftButtonFunction3D(SLOT(cameraTranslate())); break;
        case 2 : _pCameraEditor->setLeftButtonFunction3D(SLOT(cameraRoll())); break;
        case 3 : _pCameraEditor->setLeftButtonFunction3D(SLOT(cameraRotate())); break;
    }


    // bouton droit
    int mouseRightButtonFunct3D = 0;
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "MouseRightButtonFunct3D"))
    {
        mouseRightButtonFunct3D = TYPreferenceManager::getInt(TYDIRPREFERENCEMANAGER, "MouseRightButtonFunct3D");
    }
    else
    {
        TYPreferenceManager::setInt(TYDIRPREFERENCEMANAGER, "MouseRightButtonFunct3D", mouseRightButtonFunct3D);
    }

    switch (mouseRightButtonFunct3D)
    {
        case 0 : _pCameraEditor->setRightButtonFunction3D(SLOT(cameraZoom())); break;
        case 1 : _pCameraEditor->setRightButtonFunction3D(SLOT(cameraTranslate())); break;
        case 2 : _pCameraEditor->setRightButtonFunction3D(SLOT(cameraRoll())); break;
        case 3 : _pCameraEditor->setRightButtonFunction3D(SLOT(cameraRotate())); break;
    }


    // bouton central
    int mouseMiddleButtonFunct3D = 2;
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "MouseMiddleButtonFunct3D"))
    {
        mouseMiddleButtonFunct3D = TYPreferenceManager::getInt(TYDIRPREFERENCEMANAGER, "MouseMiddleButtonFunct3D");
    }
    else
    {
        TYPreferenceManager::setInt(TYDIRPREFERENCEMANAGER, "MouseMiddleButtonFunct3D", mouseMiddleButtonFunct3D);
    }

    switch (mouseMiddleButtonFunct3D)
    {
        case 0 : _pCameraEditor->setMiddleButtonFunction3D(SLOT(cameraZoom())); break;
        case 1 : _pCameraEditor->setMiddleButtonFunction3D(SLOT(cameraTranslate())); break;
        case 2 : _pCameraEditor->setMiddleButtonFunction3D(SLOT(cameraRoll())); break;
        case 3 : _pCameraEditor->setMiddleButtonFunction3D(SLOT(cameraRotate())); break;
    }

    // Mode shift.
    // 2D
    // bouton gauche
    int mouseShiftLeftButtonFunct2D = 1;
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "MouseShiftLeftButtonFunct2D"))
    {
        mouseShiftLeftButtonFunct2D = TYPreferenceManager::getInt(TYDIRPREFERENCEMANAGER, "MouseShiftLeftButtonFunct2D");
    }
    else
    {
        TYPreferenceManager::setInt(TYDIRPREFERENCEMANAGER, "MouseShiftLeftButtonFunct2D", mouseShiftLeftButtonFunct2D);
    }

    switch (mouseShiftLeftButtonFunct2D)
    {
        case 0 : _pCameraEditor->setShiftLeftButtonFunction2D(SLOT(cameraZoom())); break;
        case 1 : _pCameraEditor->setShiftLeftButtonFunction2D(SLOT(cameraTranslate())); break;
    }


    // bouton droit
    int mouseShiftRightButtonFunct2D = 0;
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "MouseShiftRightButtonFunct2D"))
    {
        mouseShiftRightButtonFunct2D = TYPreferenceManager::getInt(TYDIRPREFERENCEMANAGER, "MouseShiftRightButtonFunct2D");
    }
    else
    {
        TYPreferenceManager::setInt(TYDIRPREFERENCEMANAGER, "MouseShiftRightButtonFunct2D", mouseShiftRightButtonFunct2D);
    }

    switch (mouseShiftRightButtonFunct2D)
    {
        case 0 : _pCameraEditor->setShiftRightButtonFunction2D(SLOT(cameraZoom())); break;
        case 1 : _pCameraEditor->setShiftRightButtonFunction2D(SLOT(cameraTranslate())); break;
    }


    // en 3D
    // bouton gauche
    int mouseShiftLeftButtonFunct3D = 1;
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "MouseShiftLeftButtonFunct3D"))
    {
        mouseShiftLeftButtonFunct3D = TYPreferenceManager::getInt(TYDIRPREFERENCEMANAGER, "MouseShiftLeftButtonFunct3D");
    }
    else
    {
        TYPreferenceManager::setInt(TYDIRPREFERENCEMANAGER, "MouseShiftLeftButtonFunct3D", mouseShiftLeftButtonFunct3D);
    }

    switch (mouseShiftLeftButtonFunct3D)
    {
        case 0 : _pCameraEditor->setShiftLeftButtonFunction3D(SLOT(cameraZoom())); break;
        case 1 : _pCameraEditor->setShiftLeftButtonFunction3D(SLOT(cameraTranslate())); break;
        case 2 : _pCameraEditor->setShiftLeftButtonFunction3D(SLOT(cameraRoll())); break;
        case 3 : _pCameraEditor->setShiftLeftButtonFunction3D(SLOT(cameraRotate())); break;
    }


    // bouton droit
    int mouseShiftRightButtonFunct3D = 2;
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "MouseShiftRightButtonFunct3D"))
    {
        mouseShiftRightButtonFunct3D = TYPreferenceManager::getInt(TYDIRPREFERENCEMANAGER, "MouseShiftRightButtonFunct3D");
    }
    else
    {
        TYPreferenceManager::setInt(TYDIRPREFERENCEMANAGER, "MouseShiftRightButtonFunct3D", mouseShiftRightButtonFunct3D);
    }

    switch (mouseShiftRightButtonFunct3D)
    {
        case 0 : _pCameraEditor->setShiftRightButtonFunction3D(SLOT(cameraZoom())); break;
        case 1 : _pCameraEditor->setShiftRightButtonFunction3D(SLOT(cameraTranslate())); break;
        case 2 : _pCameraEditor->setShiftRightButtonFunction3D(SLOT(cameraRoll())); break;
        case 3 : _pCameraEditor->setShiftRightButtonFunction3D(SLOT(cameraRotate())); break;
    }

    // Force un update de la camera
    _pCameraEditor->setNavigationOnViewType(getCurrentView());

    // Update de l'editor courant
    //  setEditorMode(_lastEditorMode); // DTn : Correction du bug #0009661

    // Mets a jour la structure graphique de l'element
    updateElementGraphic(true);

    // Refresh
    updateView(false);
}

void TYModelerFrame::startMovingRenderMode()
{
    // On passe en mode Wireframe si l'option est activee et qu'on est
    // pas deja dans ce mode
    if (_wireframeOnMovingCamera && (_curRenderMode != Wireframe))
    {
        // On conserve le mode courant
        _lastRenderMode = _curRenderMode;
        // On passe en Wireframe
        setRenderMode(Wireframe, false);
    }
}

void TYModelerFrame::stopMovingRenderMode()
{
    if (_wireframeOnMovingCamera)
    {
        // On revient dans le mode courant
        setRenderMode(_lastRenderMode, true);
    }
}

void TYModelerFrame::updateScale()
{
    if (_showScale)
    {
        NxVec3 val3D = OGLCamera::displayToWorld(NxVec3(0.0, 0.0, 0.1));
        TYPoint pt0(val3D.x, val3D.y, val3D.z);
        val3D = OGLCamera::displayToWorld(NxVec3(10, 0.0, 0.1));
        TYPoint pt1(val3D.x, val3D.y, val3D.z);

        OVector3D vect(pt0, pt1);
        double value = vect.norme();

        QString max = QString().setNum(value * 10, 'f', 1);
        QString mid = QString().setNum(value * 10 / 2, 'f', 1);

        _pOGLScalarBarElement->setTexts("0", mid.toStdString(), max.toStdString());
    }
}

void TYModelerFrame::showScale(bool show)
{
    _showScale = show;

    _pOGLScalarBarElement->setVisibility(_showScale);

    _pView->getRenderer()->updateDisplayList();
    updateView(false, true);
}


