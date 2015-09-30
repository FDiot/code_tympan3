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
 * \file TYLinearMaillageEditor.cpp
 * \brief Indique si cet editor est actif
 */


#include <qdialog.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qcursor.h>
//Added by qt3to4:
#include <QHBoxLayout>
#include <QBoxLayout>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QBoxLayout>
#include <QGroupBox>

#include "Tympan/models/business/TYLinearMaillage.h"
#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/TYProjet.h"
#include "Tympan/models/business/TYCalcul.h"

#include "Tympan/gui/tools/OGLLineElement.h"
#include "Tympan/gui/app/TYRenderWindowInteractor.h"
#include "Tympan/gui/app/TYModelerFrame.h"
#include "Tympan/gui/app/TYSiteModelerFrame.h"
#include "Tympan/gui/app/TYActions.h"
#include "Tympan/gui/app/TYApplication.h"
#include "TYLinearMaillageEditor.h"


#define TR(id) OLocalizator::getString("TYLinearMaillageEditor", (id))


TYLinearMaillageEditor::TYLinearMaillageEditor(TYSiteModelerFrame* pModeler) :
    TYAbstractSceneEditor(pModeler)
{
    _active = false;
    _noCalcul = true;

    _pOGLLineElement = new OGLLineElement();
    _pOGLLineElement->setVisibility(false);
    OColor oLineColor;
    oLineColor.r = 1.0;
    oLineColor.g = 0.0;
    oLineColor.b = 0.0;
    _pOGLLineElement->setColor(oLineColor);
    _pInteractor->getRenderer()->addOGLElement(_pOGLLineElement);

    slotViewTypeChanged(_pModeler->getCurrentView());
}

TYLinearMaillageEditor::~TYLinearMaillageEditor()
{
    _pInteractor->getRenderer()->removeOGLElement(_pOGLLineElement);
    delete _pOGLLineElement;
}

void TYLinearMaillageEditor::init()
{
}

void TYLinearMaillageEditor::cancel()
{
    close();
}

void TYLinearMaillageEditor::close()
{
    _active = false;
    showText(false);

    _pOGLLineElement->setVisibility(false);
    _pInteractor->updateGL();
}

void TYLinearMaillageEditor::slotKeyPressed(int key)
{
    switch (key)
    {
        case Qt::Key_Escape:
            if (_active)
            {
                cancel();
            }
            break;
    }
}

void TYLinearMaillageEditor::slotViewTypeChanged(int view)
{
    if (view == TYModelerFrame::PerspView)
    {
        _active = false;
        cancel();
    }
    else
    {
        _active = true;
    }
}

void TYLinearMaillageEditor::slotMousePressed(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
{
    if (_active && (button == Qt::LeftButton))
    {
        if (getTYApp()->getCurProjet() && getTYApp()->getCurProjet()->getCurrentCalcul())
        {
            _noCalcul = false;

            // 1er point
            _pOGLLineElement->setPoint1(OPoint3D(x, _pInteractor->height() - y , 0.0));

            // On peut dessiner la ligne
            _pOGLLineElement->setVisibility(true);
        }
        else
        {
            _noCalcul = true;

            // Msg "Pas de calcul courant..."
            QString msg(TR("id_warning_no_curcalcul"));
            writeOutputMsg(msg);
            QMessageBox::warning(_pModeler, "Tympan", msg, QMessageBox::Ok, QMessageBox::NoButton);
        }
    }
}

void TYLinearMaillageEditor::slotMouseMoved(int x, int y, Qt::MouseButtons button, Qt::KeyboardModifiers state)
{
    if (_active && (button == Qt::LeftButton) && !_noCalcul)
    {
        // On recupere la position du curseur
        QPoint pos = _pInteractor->mapFromGlobal(QCursor::pos());

        // 2eme point
        _pOGLLineElement->setPoint2(OPoint3D(pos.x(), _pInteractor->height() - y , 0.0));

        NxVec3 point1 = OGLCamera::displayToWorld(NxVec3(_pOGLLineElement->getPoint1()[0], _pOGLLineElement->getPoint1()[1], _pOGLLineElement->getPoint1()[2]));
        NxVec3 point2 = OGLCamera::displayToWorld(NxVec3(_pOGLLineElement->getPoint2()[0], _pOGLLineElement->getPoint2()[1], _pOGLLineElement->getPoint2()[2]));

        // Calcul de la distance
        double dist = point1.distance(point2);

        // Msg Dist
        QString msg = QString(TR("id_output_dist")).arg(dist, 0, 'f', 2);

        updateText(msg, (int)(_pOGLLineElement->getPoint1()[0] + (_pOGLLineElement->getPoint2()[0] - _pOGLLineElement->getPoint1()[0]) / 2.0),
                   (int)(_pOGLLineElement->getPoint2()[1] + (_pOGLLineElement->getPoint1()[1] - _pOGLLineElement->getPoint2()[1]) / 2.0));

        _pInteractor->updateGL();
    }
}

void TYLinearMaillageEditor::slotMouseReleased(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
{
    if (_active && (button == Qt::LeftButton) && !_noCalcul)
    {
        // Calcul de la ligne
        LPTYSegment pSeg = new TYSegment();

        float pt1[3], pt2[3];
        NxVec3 point1 = OGLCamera::displayToWorld(NxVec3(   _pOGLLineElement->getPoint1()[0], 
                                                            _pOGLLineElement->getPoint1()[1], 
                                                            _pOGLLineElement->getPoint1()[2]));
        NxVec3 point2 = OGLCamera::displayToWorld(NxVec3(   _pOGLLineElement->getPoint2()[0], 
                                                            _pOGLLineElement->getPoint2()[1], 
                                                            _pOGLLineElement->getPoint2()[2]));
        pt1[0] = point1.x;
        pt1[1] = point1.y;
        pt1[2] = point1.z;
        pt2[0] = point2.x;
        pt2[1] = point2.y;
        pt2[2] = point2.z;
        pSeg->_ptA.setFromOGL(pt1);
        pSeg->_ptA._z = 0;
        pSeg->_ptB.setFromOGL(pt2);
        pSeg->_ptB._z = 0;

        // Si la grille magnetique est activee
        if (_pModeler->getSnapGridActive())
        {
            snapToGrid(pSeg->_ptA._x, pSeg->_ptA._y, pSeg->_ptA._z);
            snapToGrid(pSeg->_ptB._x, pSeg->_ptB._y, pSeg->_ptB._z);
        }

        // Si la taille est valide
        if (pSeg->longueur() > 0.0)
        {
            if (_pModeler->askForResetResultat())
            {
                QDialog* pDlg = new QDialog(_pModeler);
                pDlg->setWindowTitle(TR("id_caption"));

                QGridLayout* pLayout = new QGridLayout();
                pDlg->setLayout(pLayout);

                QGridLayout* pEditLayout = new QGridLayout();
                pEditLayout->setMargin(10);
                pLayout->addLayout(pEditLayout, 0, 0);

                // Hauteur
                QLabel* pHauteurLabelName = new QLabel(TR("id_hauteur_label"));
                QLineEdit* pHauteurLineEdit = new QLineEdit();
                pHauteurLineEdit->setFixedWidth(60);
                pHauteurLineEdit->setText(QString().setNum(2.0, 'f', 2));
                pEditLayout->addWidget(pHauteurLabelName, 0, 0);
                pEditLayout->addWidget(pHauteurLineEdit, 0, 1);

                // Densite
                QLabel* pDensiteLabelName = new QLabel(TR("id_densite_label"));
                QLineEdit* pDensiteLineEdit = new QLineEdit();
                pDensiteLineEdit->setFixedWidth(60);
                pDensiteLineEdit->setText(QString().setNum(TY_MAILLAGE_DEFAULT_DENSITE, 'f', 4));
                pEditLayout->addWidget(pDensiteLabelName, 1, 0);
                pEditLayout->addWidget(pDensiteLineEdit, 1, 1);

                QBoxLayout* pBtnLayout = new QHBoxLayout();
                pLayout->addLayout(pBtnLayout, 1, 0);

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

                TYApplication::setOverrideCursor(Qt::WaitCursor);

                // Applique les modificatins si necessaire
                if (ret == QDialog::Accepted)
                {
                    TYSiteModelerFrame* pSiteModeler = (TYSiteModelerFrame*) _pModeler;

                    // Nouveau maillage
                    LPTYLinearMaillage pMaillage = new TYLinearMaillage();
                    LPTYLinearMaillageGeoNode pMaillageGeoNode = new TYLinearMaillageGeoNode((LPTYElement)pMaillage);

                    TYProjet* pProjet = pSiteModeler->getProjet();
                    if (pProjet)
                    {
                        // Init
                        pMaillage->setHauteur(pHauteurLineEdit->text().toDouble());
                        pMaillage->make(pSeg, pDensiteLineEdit->text().toDouble());

                        // Ajout au Calcul courant
                        TYAction* pAction = new TYAddMaillageToProjetAction( (LPTYMaillageGeoNode&) pMaillageGeoNode, 
                                                                                pProjet, 
                                                                                _pModeler,
                                                                                TR("id_action_addlinearmaillage") );
                        _pModeler->getActionManager()->addAction(pAction);

                        pProjet->addMaillage((LPTYMaillageGeoNode&) pMaillageGeoNode);
                        if(pProjet->getSite()->getAltimetry()->containsData())
                        {
                            // Altimetrisation du maillage
                            pProjet->updateAltiMaillage(pMaillageGeoNode);
                        }

                        pMaillage->updateGraphicTree();
                    }
                }

                delete pEditLayout;
                delete pBtnLayout;
            }

            TYElement::setIsSavedOk(true);
        }

        showText(false);

        _pOGLLineElement->setVisibility(false);

        refreshProjectFrame();
        _pModeler->getView()->getRenderer()->updateDisplayList();

        _pModeler->updateView();
        dynamic_cast<TYSiteModelerFrame *>(_pModeler)->updateSelectMaillageBox();

        TYApplication::restoreOverrideCursor();
    }
}
