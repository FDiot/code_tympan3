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
 * \file TYVerticalMaillageEditor.cpp
 * \brief gestion de l'edition d'un maillage verticale
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/gui/tools/OGLLineElement.h"

#include "Tympan/models/business/TYRectangularMaillage.h"

#include "Tympan/GraphicIHM/DataManagerGraphic/TYPointGraphic.h"

#include "Tympan/models/business/geometry/TYRepere.h"
#include "Tympan/models/business/OLocalizator.h"

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
#include <QGroupBox>


#if defined(WIN32)
#include <crtdbg.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif


#define TR(id) OLocalizator::getString("TYVerticalMaillageEditor", (id))


TYVerticalMaillageEditor::TYVerticalMaillageEditor(TYSiteModelerFrame* pModeler) :
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

TYVerticalMaillageEditor::~TYVerticalMaillageEditor()
{
    _pInteractor->getRenderer()->removeOGLElement(_pOGLLineElement);
    delete _pOGLLineElement;
}

void TYVerticalMaillageEditor::init()
{
}

void TYVerticalMaillageEditor::cancel()
{
    _active = false;

    showText(false);

    _pOGLLineElement->setVisibility(false);
    _pInteractor->updateGL();
}

void TYVerticalMaillageEditor::close()
{
    cancel();
}

void TYVerticalMaillageEditor::slotViewTypeChanged(int view)
{
    if (view == TYModelerFrame::TopView)
    {
        _active = true;
    }
    else
    {
        _active = false;
        cancel();
    }
}

void TYVerticalMaillageEditor::slotMousePressed(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
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

void TYVerticalMaillageEditor::slotMouseMoved(int x, int y, Qt::MouseButtons button, Qt::KeyboardModifiers state)
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
        QString msg = QString(TR("id_size_info")).arg(dist, 0, 'f', 2);

        updateText(msg, (int)(_pOGLLineElement->getPoint1()[0] + (_pOGLLineElement->getPoint2()[0] - _pOGLLineElement->getPoint1()[0]) / 2.0),
                   (int)(_pOGLLineElement->getPoint2()[1] + (_pOGLLineElement->getPoint1()[1] - _pOGLLineElement->getPoint2()[1]) / 2.0));

        _pInteractor->updateGL();
    }
}

void TYVerticalMaillageEditor::slotMouseReleased(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
{
    if ((button == Qt::LeftButton) && _active && !_noCalcul)
    {
        // Calcul de la ligne
        LPTYSegment pSeg = new TYSegment();

        float pt1[3], pt2[3];
        NxVec3 point1 = OGLCamera::displayToWorld(NxVec3(_pOGLLineElement->getPoint1()[0], _pOGLLineElement->getPoint1()[1], _pOGLLineElement->getPoint1()[2]));
        NxVec3 point2 = OGLCamera::displayToWorld(NxVec3(_pOGLLineElement->getPoint2()[0], _pOGLLineElement->getPoint2()[1], _pOGLLineElement->getPoint2()[2]));
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

                // Hauteur au sol
                QLabel* pHauteurSolLabelName = new QLabel(TR("id_hauteursol_label"));
                QLineEdit* pHauteurSolLineEdit = new QLineEdit();
                pHauteurSolLineEdit->setFixedWidth(60);
                pHauteurSolLineEdit->setText(QString().setNum(2.0, 'f', 2));
                pEditLayout->addWidget(pHauteurSolLabelName, 0, 0);
                pEditLayout->addWidget(pHauteurSolLineEdit, 0, 1);

                // Hauteur maillage
                QLabel* pHauteurLabelName = new QLabel(TR("id_hauteur_label"));
                QLineEdit* pHauteurLineEdit = new QLineEdit();
                pHauteurLineEdit->setFixedWidth(60);
                pHauteurLineEdit->setText(QString().setNum(2.0, 'f', 2));
                pEditLayout->addWidget(pHauteurLabelName, 1, 0);
                pEditLayout->addWidget(pHauteurLineEdit, 1, 1);

                // Densite X
                QLabel* pDensiteXLabelName = new QLabel(TR("id_densitex_label"));
                QLineEdit* pDensiteXLineEdit = new QLineEdit();
                pDensiteXLineEdit->setFixedWidth(60);
                pDensiteXLineEdit->setText(QString().setNum(TY_MAILLAGE_DEFAULT_DENSITE, 'f', 4));
                pEditLayout->addWidget(pDensiteXLabelName, 2, 0);
                pEditLayout->addWidget(pDensiteXLineEdit, 2, 1);

                // Densite Y
                QLabel* pDensiteYLabelName = new QLabel(TR("id_densitey_label"));
                QLineEdit* pDensiteYLineEdit = new QLineEdit();
                pDensiteYLineEdit->setFixedWidth(60);
                pDensiteYLineEdit->setText(QString().setNum(TY_MAILLAGE_DEFAULT_DENSITE, 'f', 4));
                pEditLayout->addWidget(pDensiteYLabelName, 3, 0);
                pEditLayout->addWidget(pDensiteYLineEdit, 3, 1);

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

                    // Position
                    TYPoint org = pSeg->centreOf();
                    org._z = 0.0;

                    // Si la grille magnetique est activee
                    if (_pModeler->getSnapGridActive())
                    {
                        snapToGrid(org._x, org._y, org._z);
                    }

                    TYRepere rep;
                    rep._origin  = org;
                    rep._vecI = pSeg->toVector3D();
                    rep._vecI.normalize();
                    rep._vecJ.setCoords(0.0, 0.0, 1.0);
                    rep._vecK = rep._vecI.cross(rep._vecJ);

                    // Construction du rectangle
                    LPTYRectangle pRectangle = new TYRectangle();
                    pRectangle->setSize(pSeg->longueur(), pHauteurLineEdit->text().toDouble());

                    // Nouveau maillage
                    LPTYRectangularMaillage pMaillage = new TYRectangularMaillage();
                    LPTYRectangularMaillageGeoNode pMaillageGeoNode = new TYRectangularMaillageGeoNode(rep, (LPTYElement)pMaillage);

                    TYProjet* pProjet = pSiteModeler->getProjet();
                    if (pProjet)
                    {
                        TYCalcul* pCalcul = pProjet->getCurrentCalcul();
                        // Ajout au Calcul courant
                        if (pCalcul)
                        {
                            pCalcul->addMaillage((LPTYMaillageGeoNode&) pMaillageGeoNode);

                            // Init
                            pMaillage->setHauteur(pHauteurSolLineEdit->text().toDouble());
                            pMaillage->make(pRectangle, pDensiteXLineEdit->text().toDouble(), pDensiteYLineEdit->text().toDouble());

                            // On ne calcule pas l'alti des points pour ce type de maillage
                            pMaillage->setComputeAlti(false);

                            TYAction* pAction = new TYAddMaillageToCalculAction((LPTYMaillageGeoNode&) pMaillageGeoNode, pSiteModeler->getProjet()->getCurrentCalcul(), _pModeler, TR("id_action_addrectmaillage"));
                            _pModeler->getActionManager()->addAction(pAction);

                            // Altimetrisation du maillage
                            pSiteModeler->getProjet()->getCurrentCalcul()->updateAltiMaillage(pMaillageGeoNode);

                            pMaillage->updateGraphicTree();
                        }
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

        TYApplication::restoreOverrideCursor();
    }
}
