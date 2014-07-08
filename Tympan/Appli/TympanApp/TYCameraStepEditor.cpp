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
 * \file TYCameraStepEditor.cpp
 * \brief gestion de la camera pas a pas (fichier obsolete)
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/models/business/OLocalizator.h"

#include <qdialog.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qcursor.h>
#include <qgroupbox.h>
#include <qpixmap.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

#if defined(WIN32)
#include <crtdbg.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif


#define TR(id) OLocalizator::getString("TYCameraStepEditor", (id))
#define IMG(id) OLocalizator::getPicture("TYCameraStepEditor", (id))


TYCameraStepEditor::TYCameraStepEditor(TYModelerFrame* pModeler) :
    TYAbstractSceneEditor(pModeler)
{
    _pCameraEditor = _pModeler->getCameraEditor();

    _pDlg = new QDialog(_pModeler);
    _pDlg->setWindowTitle(TR("id_caption"));
    _pDlg->setFixedSize(290, 185);

    QGridLayout* pLayout = new QGridLayout();
    _pDlg->setLayout(pLayout);

    QGridLayout* pEditLayout = new QGridLayout();
    pEditLayout->setMargin(5);
    pLayout->addLayout(pEditLayout, 0, 0);

    QGroupBox* pTranslationGroupBox = new QGroupBox(_pDlg);
    pTranslationGroupBox->setTitle(TR("id_translation"));
    QGridLayout* pTranslationLayout = new QGridLayout();
    pTranslationGroupBox->setLayout(pTranslationLayout);
    pTranslationLayout->setMargin(5);
    //pTranslationLayout->setHorizontalSpacing(0);
    //pTranslationLayout->setVerticalSpacing(0);

    QPushButton* pUpPushButton = new QPushButton(pTranslationGroupBox);
    pUpPushButton->setFixedSize(25, 25);
    pUpPushButton->setIcon(QPixmap(IMG("id_icon_arrow_up")));
    QObject::connect(pUpPushButton, SIGNAL(clicked()), _pCameraEditor, SLOT(cameraStepTranslateUp()));
    pTranslationLayout->addWidget(pUpPushButton, 0, 1);
    QPushButton* pDownPushButton = new QPushButton(pTranslationGroupBox);
    pDownPushButton->setFixedSize(25, 25);
    pDownPushButton->setIcon(QPixmap(IMG("id_icon_arrow_down")));
    QObject::connect(pDownPushButton, SIGNAL(clicked()), _pCameraEditor, SLOT(cameraStepTranslateDown()));
    pTranslationLayout->addWidget(pDownPushButton, 2, 1);
    QPushButton* pLeftPushButton = new QPushButton(pTranslationGroupBox);
    pLeftPushButton->setFixedSize(25, 25);
    pLeftPushButton->setIcon(QPixmap(IMG("id_icon_arrow_left")));
    QObject::connect(pLeftPushButton, SIGNAL(clicked()), _pCameraEditor, SLOT(cameraStepTranslateLeft()));
    pTranslationLayout->addWidget(pLeftPushButton, 1, 0);
    QPushButton* pRightPushButton = new QPushButton(pTranslationGroupBox);
    pRightPushButton->setFixedSize(25, 25);
    pRightPushButton->setIcon(QPixmap(IMG("id_icon_arrow_right")));
    QObject::connect(pRightPushButton, SIGNAL(clicked()), _pCameraEditor, SLOT(cameraStepTranslateRight()));
    pTranslationLayout->addWidget(pRightPushButton, 1, 2);
    _pTranslateStepLineEdit = new QLineEdit(pTranslationGroupBox);
    _pTranslateStepLineEdit->setText(QString().arg(_pCameraEditor->getTranslateStep()));
    _pTranslateStepLineEdit->setFixedSize(25, 20);
    pTranslationLayout->addWidget(_pTranslateStepLineEdit, 3, 2);
    QLabel* pLabelTranslateStep = new QLabel(pTranslationGroupBox);
    pLabelTranslateStep->setText(TR("id_step"));
    pTranslationLayout->addWidget(pLabelTranslateStep, 3, 0, 1, 2);
    QObject::connect(_pTranslateStepLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(slotTranslateChanged()));

    pEditLayout->addWidget(pTranslationGroupBox, 0, 0);

    QGroupBox* pRotationGroupBox = new QGroupBox(_pDlg);
    pRotationGroupBox->setTitle(TR("id_rotation"));
    QGridLayout* pRotationLayout = new QGridLayout();
    pRotationGroupBox->setLayout(pRotationLayout);
    pRotationLayout->setMargin(5);
    //pRotationLayout->setHorizontalSpacing(0);
    //pRotationLayout->setVerticalSpacing(0);

    QPushButton* pRotUpPushButton = new QPushButton(pRotationGroupBox);
    pRotUpPushButton->setFixedSize(25, 25);
    pRotUpPushButton->setIcon(QPixmap(IMG("id_icon_arrow_up")));
    QObject::connect(pRotUpPushButton, SIGNAL(clicked()), _pCameraEditor, SLOT(cameraStepRotateUp()));
    pRotationLayout->addWidget(pRotUpPushButton, 0, 1);
    QPushButton* pRotDownPushButton = new QPushButton(pRotationGroupBox);
    pRotDownPushButton->setFixedSize(25, 25);
    pRotDownPushButton->setIcon(QPixmap(IMG("id_icon_arrow_down")));
    QObject::connect(pRotDownPushButton, SIGNAL(clicked()), _pCameraEditor, SLOT(cameraStepRotateDown()));
    pRotationLayout->addWidget(pRotDownPushButton, 2, 1);
    QPushButton* pRotLeftPushButton = new QPushButton(pRotationGroupBox);
    pRotLeftPushButton->setFixedSize(25, 25);
    pRotLeftPushButton->setIcon(QPixmap(IMG("id_icon_arrow_left")));
    QObject::connect(pRotLeftPushButton, SIGNAL(clicked()), _pCameraEditor, SLOT(cameraStepRotateLeft()));
    pRotationLayout->addWidget(pRotLeftPushButton, 1, 0);
    QPushButton* pRotRightPushButton = new QPushButton(pRotationGroupBox);
    pRotRightPushButton->setFixedSize(25, 25);
    pRotRightPushButton->setIcon(QPixmap(IMG("id_icon_arrow_right")));
    QObject::connect(pRotRightPushButton, SIGNAL(clicked()), _pCameraEditor, SLOT(cameraStepRotateRight()));
    pRotationLayout->addWidget(pRotRightPushButton, 1, 2);
    _pRotateStepLineEdit = new QLineEdit(pRotationGroupBox);
    _pRotateStepLineEdit->setText(QString().arg(_pCameraEditor->getRotateStep()));
    _pRotateStepLineEdit->setFixedSize(25, 20);
    pRotationLayout->addWidget(_pRotateStepLineEdit, 3, 2);
    QLabel* pLabelRotateStep = new QLabel(pRotationGroupBox);
    pLabelRotateStep->setText(TR("id_step"));
    pRotationLayout->addWidget(pLabelRotateStep, 3, 0, 1, 2);
    QObject::connect(_pRotateStepLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(slotRotateChanged()));

    pEditLayout->addWidget(pRotationGroupBox, 0, 1);

    QGroupBox* pZoomGroupBox = new QGroupBox(_pDlg);
    pZoomGroupBox->setTitle(TR("id_zoom"));
    QGridLayout* pZoomLayout = new QGridLayout();
    pZoomGroupBox->setLayout(pZoomLayout);
    pZoomLayout->setMargin(5);
    //pZoomLayout->setHorizontalSpacing(0);
    //pZoomLayout->setVerticalSpacing(0);

    QPushButton* pInPushButton = new QPushButton(pZoomGroupBox);
    pInPushButton->setFixedSize(25, 25);
    pInPushButton->setIcon(QPixmap(IMG("id_icon_more")));
    QObject::connect(pInPushButton, SIGNAL(clicked()), _pCameraEditor, SLOT(cameraStepZoomIn()));
    pZoomLayout->addWidget(pInPushButton, 1, 0);
    QPushButton* pOutPushButton = new QPushButton(pZoomGroupBox);
    pOutPushButton->setFixedSize(25, 25);
    pOutPushButton->setIcon(QPixmap(IMG("id_icon_less")));
    QObject::connect(pOutPushButton, SIGNAL(clicked()), _pCameraEditor, SLOT(cameraStepZoomOut()));
    pZoomLayout->addWidget(pOutPushButton, 1, 2);
    _pZoomStepLineEdit = new QLineEdit(pZoomGroupBox);
    _pZoomStepLineEdit->setText(QString().arg(_pCameraEditor->getZoomStep()));
    _pZoomStepLineEdit->setFixedSize(25, 20);
    pZoomLayout->addWidget(_pZoomStepLineEdit, 3, 2);
    QLabel* pLabelZoomStep = new QLabel(pZoomGroupBox);
    pLabelZoomStep->setText(TR("id_step"));
    pZoomLayout->addWidget(pLabelZoomStep, 3, 0, 1, 2);
    QObject::connect(_pZoomStepLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(slotZoomStepChanged()));

    pEditLayout->addWidget(pZoomGroupBox, 0, 2);

    QBoxLayout* pBtnLayout = new QHBoxLayout();
    pLayout->addLayout(pBtnLayout, 1, 0);

    pBtnLayout->addStretch(1);

    QPushButton* pButtonOK = new QPushButton(TR("id_ok_btn"), _pDlg);
    pButtonOK->setDefault(true);
    QObject::connect(pButtonOK, SIGNAL(clicked()), this, SLOT(close()));
    pBtnLayout->addWidget(pButtonOK);

    _active = false;
    slotViewTypeChanged(_pModeler->getCurrentView());
}

TYCameraStepEditor::~TYCameraStepEditor()
{

}

void TYCameraStepEditor::init()
{
    // Affiche la boite de dialogue
    slotViewTypeChanged(_pModeler->getCurrentView());

    if (_active) { _pDlg->show(); }
}

void TYCameraStepEditor::cancel()
{
    _active = false;

    showText(false);

    _pDlg->close();

}

void TYCameraStepEditor::close()
{
    cancel();
}

void TYCameraStepEditor::slotViewTypeChanged(int view)
{
    if (view == TYModelerFrame::PerspView)
    {
        _active = true;
    }
    else
    {
        close();
    }
}


void TYCameraStepEditor::slotZoomStepChanged()
{
    _pCameraEditor->setZoomStep(_pZoomStepLineEdit->text().toFloat());
}

void TYCameraStepEditor::slotTranslateChanged()
{
    _pCameraEditor->setTranslateStep(_pTranslateStepLineEdit->text().toFloat());
}

void TYCameraStepEditor::slotRotateChanged()
{
    _pCameraEditor->setRotateStep(_pRotateStepLineEdit->text().toFloat());
}


