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
 * \file TYFaceModelerPickEditor.cpp
 * \brief gestion du picking des faces
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYBatiment.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYPickHandler.h"
#include "Tympan/GraphicIHM/DataManagerIHM/TYWidget.h"

#include "Tympan/Tools/OLocalizator.h"

#include <qdialog.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qmenu.h>
#include <qcursor.h>
#include <qinputdialog.h>
#include <QMessageBox>
//Added by qt3to4:
#include <QHBoxLayout>
#include <QBoxLayout>
#include <QGridLayout>
#include <QPixmap>


#if defined(WIN32)
#include <crtdbg.h>
#endif

#if defined(WIN32)
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif


#define TR(id) OLocalizator::getString("TYFaceModelerPickEditor", (id))
#define IMG(id) OLocalizator::getPicture("TYPickEditor", (id))


TYFaceModelerPickEditor::TYFaceModelerPickEditor(TYModelerFrame* pModeler) :
    TYPickEditor(pModeler)
{
}

TYFaceModelerPickEditor::~TYFaceModelerPickEditor()
{
}

void TYFaceModelerPickEditor::showPopupMenu(LPTYElementCollection pElts)
{
    if (!pElts)
    {
        return;
    }

    QMenu* pPopup = new QMenu(NULL);
    TYAcousticRectangleNode* pParent = NULL;
    std::map<QAction*, int> retCodes;
    std::map<QAction*, TYGeometryNode*> posRetCodes;
    std::map<QAction*, TYAcousticRectangle*> dimAccRectRetCodes;
    std::map<QAction*, TYAcousticRectangle*> remAccRectRetCodes;
    QAction* code = NULL;
    unsigned int i = 0;

    while ((i < pElts->size()) && (i + 1 < pElts->size()))
    {
        if (pElts->at(i)->inherits("TYAcousticRectangle"))
        {
            // Proprietes
            QFont font = pPopup->font();
            code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_editeelt"))), TYWidget::getDisplayName(pElts->at(i)));
            font.setBold(true);
            code->setFont(font);

            retCodes[code] = i;

            // Dimensions
            if (pElts->at(i + 1)->inherits("TYAcousticSurfaceNode"))
            {
                pParent = (LPTYAcousticRectangleNode&) pElts->at(i + 1);
                code = pPopup->addAction(TR("id_popup_dimension"));
                dimAccRectRetCodes[code] = (LPTYAcousticRectangle&) pElts->at(i);
            }

            // Position via le parent (GeoNode)
            if (pElts->at(i + 1)->inherits("TYAcousticSurfaceNode"))
            {
                pParent = (LPTYAcousticRectangleNode&) pElts->at(i + 1);
                code = pPopup->addAction(QIcon(IMG("id_icon_moving")), TR("id_popup_position"));
                posRetCodes[code] = (TYGeometryNode*) pParent->findAcousticSurf((LPTYAcousticSurface&) pElts->at(i)).getRealPointer();
            }

            // Suppression
            code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_del"))), TR("id_popup_remove"));
            remAccRectRetCodes[code] = (LPTYAcousticRectangle&) pElts->at(i);

            pPopup->addSeparator();
        }
        else if (pElts->at(i)->inherits("TYAcousticRectangleNode"))
        {
            // Face parent
            code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_editeelt"))), TYWidget::getDisplayName(pElts->at(i)));
            retCodes[code] = i;

            // On a la sous-face et son parent
            break;
        }

        // Increment
        i++;
    }

    QAction* popupRet = pPopup->exec(QCursor::pos());

    if (retCodes.find(popupRet) != retCodes.end())
    {
        pElts->at(retCodes[popupRet])->edit(_pModeler);
    }
    else if (dimAccRectRetCodes.find(popupRet) != dimAccRectRetCodes.end())
    {
        showDimensionsDialog(dimAccRectRetCodes[popupRet]->getShape(), pParent);
    }
    else if (posRetCodes.find(popupRet) != posRetCodes.end())
    {
        showPositionDialog(posRetCodes[popupRet]);
        if (pParent) { pParent->updateGrid(); }
    }
    else if (remAccRectRetCodes.find(popupRet) != remAccRectRetCodes.end())
    {
        if (_pModeler->askForResetResultat())
        {
            LPTYAcousticSurfaceGeoNode pGeoNode = pParent->findAcousticSurf(remAccRectRetCodes[popupRet]);

            pParent->remAcousticSurf(remAccRectRetCodes[popupRet]);
            pParent->setIsGeometryModified(true);
            pParent->updateGraphicTree();

            TYAction* pAction = new TYRemAccSurfToAccSurfNodeAction(pGeoNode, pParent, _pModeler, TR("id_action_remface"));
            _pModeler->getActionManager()->addAction(pAction);
        }
    }

    // Deselection
    resetPicker();

    // Update
    getTYMainWnd()->updateModelers(false, false);

    delete pPopup;
}

void TYFaceModelerPickEditor::showDimensionsDialog(TYRectangle* pRect, TYAcousticRectangleNode* pParent/*=NULL*/)
{
    if (!pRect) { return; }

    QDialog* pDlg = new QDialog(_pModeler);
    pDlg->setWindowTitle(TR("id_popup_dimension"));

    QGridLayout* pLayout = new QGridLayout();
    pDlg->setLayout(pLayout);

    QBoxLayout* pEditLayout = new QHBoxLayout();
    pEditLayout->setMargin(10);
    pLayout->addLayout(pEditLayout, 0, 1);

    QLineEdit* pXLineEdit = NULL;
    QLineEdit* pYLineEdit = NULL;

    float sizeX, sizeY;
    pRect->getSize(sizeX, sizeY);

    // Size X
    QLabel* pXLabelName = new QLabel(pDlg);
    pXLabelName->setText(TR("id_x_label"));
    pEditLayout->addWidget(pXLabelName);
    pXLineEdit = new QLineEdit(pDlg);
    pXLineEdit->setFixedWidth(60);
    pXLineEdit->setText(QString().setNum(sizeX, 'f', 2));
    pEditLayout->addWidget(pXLineEdit);

    // Size Y
    pEditLayout->addSpacing(10);
    QLabel* pYLabelName = new QLabel(pDlg);
    pYLabelName->setText(TR("id_y_label"));
    pEditLayout->addWidget(pYLabelName);
    pYLineEdit = new QLineEdit(pDlg);
    pYLineEdit->setFixedWidth(60);
    pYLineEdit->setText(QString().setNum(sizeY, 'f', 2));
    pEditLayout->addWidget(pYLineEdit);

    QBoxLayout* pBtnLayout = new QHBoxLayout();
    pLayout->addLayout(pBtnLayout, 1, 1);

    pBtnLayout->addStretch(1);

    QPushButton* pButtonOK = new QPushButton(TR("id_ok_btn"), pDlg);
    pButtonOK->setDefault(true);
    QObject::connect(pButtonOK, SIGNAL(clicked()), pDlg, SLOT(accept()));
    pBtnLayout->addWidget(pButtonOK);

    QPushButton* pButtonCancel = new QPushButton(TR("id_cancel_btn"), pDlg);
    pButtonCancel->setShortcut(Qt::Key_Escape);
    QObject::connect(pButtonCancel, SIGNAL(clicked()), pDlg, SLOT(reject()));
    pBtnLayout->addWidget(pButtonCancel);

    // Affiche la boite de dialogue
    int ret = pDlg->exec();

    // Applique les modificatins si necessaire
    if (ret == QDialog::Accepted)
    {
        if (_pModeler->askForResetResultat())
        {
            float sizeX = pXLineEdit->text().toDouble();
            float sizeY = pYLineEdit->text().toDouble();

			if (sizeX <= 1E-4 || sizeY <= 1.E-4) // Eviter les surfaces nulles
			{
				QMessageBox::warning(_pModeler, "Tympan", TR("id_warning_size_not_ok"), QMessageBox::Yes);//, QMessageBox::No);
				return;
			}

            TYAction* pAction = new TYResizeRectAction(pRect, sizeX, sizeY, _pModeler, TR("id_action_resizeface"));
            _pModeler->getActionManager()->addAction(pAction);

            pRect->setSize(sizeX, sizeY);

            if (pParent) { pParent->updateGrid(); }


            // Refresh
            pRect->updateGraphicTree();
        }
    }

    delete pBtnLayout;
    delete pEditLayout;
}
