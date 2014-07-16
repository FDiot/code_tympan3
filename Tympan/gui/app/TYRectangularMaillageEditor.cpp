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
 * \file TYRectangularMaillageEditor.cpp
 * \brief gestion de l'edition d'un maillage rectangulaire
 *
 *
 */





#include "Tympan/gui/tools/OGLRectangleElement.h"

#include "Tympan/models/business/TYRectangularMaillage.h"

#include "Tympan/gui/gl/TYPointGraphic.h"

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


#define TR(id) OLocalizator::getString("TYRectangularMaillageEditor", (id))


TYRectangularMaillageEditor::TYRectangularMaillageEditor(TYSiteModelerFrame* pModeler) :
    TYAbstractSceneEditor(pModeler)
{
    _pOGLRectangleElement = new OGLRectangleElement();
    _pOGLRectangleElement->setVisibility(false);
    OColor oLineColor;
    oLineColor.r = 1.0;
    oLineColor.g = 0.0;
    oLineColor.b = 0.0;
    _pOGLRectangleElement->setColor(oLineColor);
    _pOGLRectangleElement->setPoint0(OPoint3D(0.0, 0.0, 0.0));
    _pOGLRectangleElement->setPoint1(OPoint3D(0.0, 0.0, 0.0));
    _pOGLRectangleElement->setPoint2(OPoint3D(0.0, 0.0, 0.0));
    _pOGLRectangleElement->setPoint3(OPoint3D(0.0, 0.0, 0.0));
    _pInteractor->getRenderer()->addOGLElement(_pOGLRectangleElement);

    _moving = false;
    _active = false;
    slotViewTypeChanged(_pModeler->getCurrentView());
}

TYRectangularMaillageEditor::~TYRectangularMaillageEditor()
{
    _pInteractor->getRenderer()->removeOGLElement(_pOGLRectangleElement);
    delete _pOGLRectangleElement;
}

void TYRectangularMaillageEditor::init()
{
}

void TYRectangularMaillageEditor::cancel()
{
    _moving = false;
    _active = false;

    showText(false);

    _pOGLRectangleElement->setVisibility(false);
    _pInteractor->updateGL();
}

void TYRectangularMaillageEditor::close()
{
    cancel();
}

void TYRectangularMaillageEditor::slotViewTypeChanged(int view)
{
    if (view == TYModelerFrame::TopView)
    {
        _active = true;
    }
    else
    {
        _active = false;
    }
}

void TYRectangularMaillageEditor::slotMousePressed(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
{
    if ((button == Qt::LeftButton) && _active)
    {
        if (getTYApp()->getCurProjet() && getTYApp()->getCurProjet()->getCurrentCalcul())
        {
            // Init points
            _pOGLRectangleElement->setPoint0(OPoint3D(x, _pInteractor->height() - y, 0.0));
            _pOGLRectangleElement->setPoint1(OPoint3D(x, _pInteractor->height() - y, 0.0));
            _pOGLRectangleElement->setPoint2(OPoint3D(x, _pInteractor->height() - y, 0.0));
            _pOGLRectangleElement->setPoint3(OPoint3D(x, _pInteractor->height() - y, 0.0));
            _moving = true;
        }
        else
        {
            _moving = false;

            // Msg "Pas de calcul courant..."
            QString msg(TR("id_warning_no_curcalcul"));
            writeOutputMsg(msg);
            QMessageBox::warning(_pModeler, "Tympan", msg, QMessageBox::Ok, QMessageBox::NoButton);
        }
    }
}

void TYRectangularMaillageEditor::slotMouseReleased(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
{
    if ((button == Qt::LeftButton) && _active && _moving)
    {
        _moving = false;

        TYPoint pt0, pt1, pt2;
        NxVec3 point0 = OGLCamera::displayToWorld(NxVec3(_pOGLRectangleElement->getPoint0()[0], _pOGLRectangleElement->getPoint0()[1], _pOGLRectangleElement->getPoint0()[2]));
        NxVec3 point1 = OGLCamera::displayToWorld(NxVec3(_pOGLRectangleElement->getPoint1()[0], _pOGLRectangleElement->getPoint1()[1], _pOGLRectangleElement->getPoint1()[2]));
        NxVec3 point2 = OGLCamera::displayToWorld(NxVec3(_pOGLRectangleElement->getPoint2()[0], _pOGLRectangleElement->getPoint2()[1], _pOGLRectangleElement->getPoint2()[2]));
        pt0._x = point0.x;
        pt0._y = -point0.z;
        pt0._z = point0.y;
        pt1._x = point1.x;
        pt1._y = -point1.z;
        pt1._z = point1.y;
        pt2._x = point2.x;
        pt2._y = -point2.z;
        pt2._z = point2.y;

        double sizeX = OVector3D(pt0, pt1).norme();
        double sizeY = OVector3D(pt1, pt2).norme();
        double sizeZ = 0;

        // Si la grille magnetique est activee
        if (_pModeler->getSnapGridActive())
        {
            snapToGrid(sizeX, sizeY, sizeZ);
        }

        // Si la taille est valide
        if (sizeX && sizeY)
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

                // Densite X
                QLabel* pDensiteXLabelName = new QLabel(TR("id_densitex_label"));
                QLineEdit* pDensiteXLineEdit = new QLineEdit();
                pDensiteXLineEdit->setFixedWidth(60);
                pDensiteXLineEdit->setText(QString().setNum(TY_MAILLAGE_DEFAULT_DENSITE, 'f', 4));
                pEditLayout->addWidget(pDensiteXLabelName, 1, 0);
                pEditLayout->addWidget(pDensiteXLineEdit, 1, 1);

                // Densite Y
                QLabel* pDensiteYLabelName = new QLabel(TR("id_densitey_label"));
                QLineEdit* pDensiteYLineEdit = new QLineEdit();
                pDensiteYLineEdit->setFixedWidth(60);
                pDensiteYLineEdit->setText(QString().setNum(TY_MAILLAGE_DEFAULT_DENSITE, 'f', 4));
                pEditLayout->addWidget(pDensiteYLabelName, 2, 0);
                pEditLayout->addWidget(pDensiteYLineEdit, 2, 1);

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
                    TYPoint org = OVector3D(OVector3D(pt0) + (OVector3D(pt2) - OVector3D(pt0)) * 0.5);
                    org._z = 0.0;

                    // Si la grille magnetique est activee
                    if (_pModeler->getSnapGridActive())
                    {
                        snapToGrid(org._x, org._y, org._z);
                    }

                    TYRepere rep;
                    rep._origin  = org;

                    // Nouveau rectangle
                    LPTYRectangle pRectangle = new TYRectangle();
                    pRectangle->setSize(sizeX, sizeY);

                    // Nouveau maillage
                    LPTYRectangularMaillage pMaillage = new TYRectangularMaillage();
                    LPTYRectangularMaillageGeoNode pMaillageGeoNode = new TYRectangularMaillageGeoNode(rep, (LPTYElement)pMaillage);

                    TYProjet* pProjet = pSiteModeler->getProjet();
                    if (pProjet != NULL)
                    {
                        TYCalcul* pCalcul = pProjet->getCurrentCalcul();
                        // Ajout au Calcul courant
                        if (pCalcul != NULL)
                        {
                            // Init
                            // Ajoute le maillage dans le calcul courant
                            pCalcul->addMaillage((LPTYMaillageGeoNode&) pMaillageGeoNode);

                            pMaillage->setHauteur(pHauteurLineEdit->text().toDouble());
                            pMaillage->make(pRectangle, pDensiteXLineEdit->text().toDouble(), pDensiteYLineEdit->text().toDouble());

                            TYAction* pAction = new TYAddMaillageToCalculAction((LPTYMaillageGeoNode&) pMaillageGeoNode, pCalcul, _pModeler, TR("id_action_addrectmaillage"));

                            // Ajout de l'action creation de maillage pour mettre le undo
                            _pModeler->getActionManager()->addAction(pAction);


                            // Mise a jour de l'altimetrie du maillage
                            pCalcul->updateAltiMaillage(pMaillageGeoNode);

                            // Genere les objets graphiques representant le maillage
                            pMaillage->updateGraphicTree();
                        }
                    }
                }

                delete pBtnLayout;
                delete pEditLayout;
            }

            TYElement::setIsSavedOk(true);
        }

        showText(false);

        _pOGLRectangleElement->setVisibility(false);
        refreshProjectFrame();
        _pModeler->getView()->getRenderer()->updateDisplayList();
        _pModeler->updateView();

        TYApplication::restoreOverrideCursor();
    }
}

void TYRectangularMaillageEditor::slotMouseMoved(int x, int y, Qt::MouseButtons button, Qt::KeyboardModifiers state)
{
    if (_active && _moving)
    {
        _pOGLRectangleElement->setPoint1(OPoint3D(x, _pOGLRectangleElement->getPoint0()[1], 0.0));
        _pOGLRectangleElement->setPoint2(OPoint3D(x, _pInteractor->height() - y, 0.0));
        _pOGLRectangleElement->setPoint3(OPoint3D(_pOGLRectangleElement->getPoint0()[0], _pInteractor->height() - y, 0.0));


        float pt0[3], pt1[3], pt2[3];
        NxVec3 point0 = OGLCamera::displayToWorld(NxVec3(_pOGLRectangleElement->getPoint0()[0], _pOGLRectangleElement->getPoint0()[1], _pOGLRectangleElement->getPoint0()[2]));
        NxVec3 point1 = OGLCamera::displayToWorld(NxVec3(_pOGLRectangleElement->getPoint1()[0], _pOGLRectangleElement->getPoint1()[1], _pOGLRectangleElement->getPoint1()[2]));
        NxVec3 point2 = OGLCamera::displayToWorld(NxVec3(_pOGLRectangleElement->getPoint2()[0], _pOGLRectangleElement->getPoint2()[1], _pOGLRectangleElement->getPoint2()[2]));
        pt0[0] = point0.x;
        pt0[1] = point0.z;
        pt1[0] = point1.x;
        pt1[1] = point1.z;
        pt2[0] = point2.x;
        pt2[1] = point2.z;

        double sizeX = ABS(pt1[0] - pt0[0]);
        double sizeY = ABS(pt2[1] - pt1[1]);
        double sizeZ = (sizeX + sizeY) / 2.0;

        // Si la grille magnetique est activee
        if (_pModeler->getSnapGridActive())
        {
            snapToGrid(sizeX, sizeY, sizeZ);
        }

        updateText(QString(TR("id_size_info")).arg(sizeX, 0, 'f', 2).arg(sizeY, 0, 'f', 2),
                   (int)(_pOGLRectangleElement->getPoint0()[0] + (_pOGLRectangleElement->getPoint1()[0] - _pOGLRectangleElement->getPoint0()[0]) / 2.0),
                   (int)(_pOGLRectangleElement->getPoint1()[1] + (_pOGLRectangleElement->getPoint2()[1] - _pOGLRectangleElement->getPoint1()[1]) / 2.0));

        _pOGLRectangleElement->setVisibility(true);
        _pInteractor->updateGL();
    }
}
