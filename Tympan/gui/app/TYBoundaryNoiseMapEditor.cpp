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
 * \file TYBoundaryNoiseMapEditor.cpp
 * \brief Creation of a boundary noise map
 *
 *
 */

#include <qmessagebox.h>
#include <qdialog.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
//Added by qt3to4:
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>


#include <limits>

#include "Tympan/models/business/TYPreferenceManager.h"
#include "Tympan/models/business/OLocalizator.h"

#include "Tympan/models/business/TYBoundaryNoiseMap.h"
#include "TYActions.h"

#if defined(WIN32)
#include <crtdbg.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif


#define TR(id) OLocalizator::getString("TYBoundaryNoiseMapEditor", (id))

#undef min
#undef max

TYBoundaryNoiseMapEditor::TYBoundaryNoiseMapEditor(TYSiteModelerFrame* pModeler) :
    TYPolyLineEditor(pModeler)
{
    _useAngleStep = true;
    _dispDist = true;
    QObject::connect(this, SIGNAL(endedSavingPoints()), this, SLOT(endBoundaryNoiseMap()));
}

TYBoundaryNoiseMapEditor::~TYBoundaryNoiseMapEditor()
{
}

void TYBoundaryNoiseMapEditor::endBoundaryNoiseMap()
{
    bool forceOpened = false;
    if (!checkValidity(forceOpened))
    {
        return;
    }
    writeDebugMsg("Boundary Noise Map valid.");
    createPropertiesDlg(forceOpened);
}

bool TYBoundaryNoiseMapEditor::checkValidity(bool& forceOpened)
{
    if (!_pModeler->askForResetResultat())
    {
        return false;
    }

    const TYTabPoint& tabPts = this->getSavedPoints();
    size_t nbPts = tabPts.size();
    if (nbPts < 3)
    {
        // Not enough point to be a closed polyline
        forceOpened = true;
        return true;
    }

    // We test if the polyline doesn't intersect itself
    TYPoint ptInter;
    for (size_t i = 0; i < nbPts; i++)
    {
        TYSegment seg1(tabPts[i], tabPts[(i + 1) % nbPts]);
        for (size_t j = 0; j < nbPts; j++)
        {
            TYSegment seg2(tabPts[j], tabPts[(j + 1) % nbPts]);
            // Test if at least one vertex is in common
            if ((seg1._ptA != seg2._ptA) && (seg1._ptB != seg2._ptB) && (seg1._ptA != seg2._ptB) && (seg1._ptB != seg2._ptA))
            {
                // Test if there's an intersection
                if (seg1.intersects(seg2, ptInter) != INTERS_NULLE)
                {
                    // If the intersection occurs between the extremal point of the array
                    if ((i == nbPts - 1) || (j == nbPts - 1))
                    {
                        forceOpened = true;
                    }
                    else
                    {
                        writeDebugMsg("BoundaryNoiseMap invalid.");
                        QMessageBox::warning(_pModeler, TR("id_caption"), TR("id_msg_boundarynoisemap_invalid"));
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

void TYBoundaryNoiseMapEditor::createPropertiesDlg(bool forceOpened)
{
    QDialog* pDlg = new QDialog(_pModeler);
    pDlg->setWindowTitle(TR("id_caption"));

    QGridLayout* pLayout = new QGridLayout();
    pDlg->setLayout(pLayout);

    QGridLayout* pEditLayout = new QGridLayout();
    pEditLayout->setMargin(10);
    pLayout->addLayout(pEditLayout, 0, 0);

    // Height
    QDoubleSpinBox* pHeightSpinBox = new QDoubleSpinBox();
    pHeightSpinBox->setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
    pHeightSpinBox->setCorrectionMode(QAbstractSpinBox::CorrectToNearestValue);
    pHeightSpinBox->setFixedWidth(60);
    pHeightSpinBox->setValue(2.0);
    pEditLayout->addWidget(new QLabel(TR("id_height_label")), 0, 0);
    pEditLayout->addWidget(pHeightSpinBox, 0, 1);

    // Thickness
    _pThicknessSpinBox = new QDoubleSpinBox();
    _pThicknessSpinBox->setRange(0.1, std::numeric_limits<double>::max());
    _pThicknessSpinBox->setCorrectionMode(QAbstractSpinBox::CorrectToNearestValue);
    _pThicknessSpinBox->setFixedWidth(60);
    pEditLayout->addWidget(new QLabel(TR("id_thickness_label")), 1, 0);
    pEditLayout->addWidget(_pThicknessSpinBox, 1, 1);

    // Closed
    QCheckBox* pClosedCheckBox = new QCheckBox();
    pClosedCheckBox->setChecked(false);
    if (forceOpened)
    {
        pClosedCheckBox->setDisabled(true);
    }
    pEditLayout->addWidget(new QLabel(TR("id_closed_label")), 2, 0);
    pEditLayout->addWidget(pClosedCheckBox, 2, 1);

    // Density
    _pDensitySpinBox = new QDoubleSpinBox();
    _pDensitySpinBox->setFixedWidth(60);
    _pDensitySpinBox->setSingleStep(0.1);
    _pDensitySpinBox->setCorrectionMode(QAbstractSpinBox::CorrectToNearestValue);
    pEditLayout->addWidget(new QLabel(TR("id_density_label")), 3, 0);
    pEditLayout->addWidget(_pDensitySpinBox, 3, 1);

    QBoxLayout* pBtnLayout = new QHBoxLayout();
    pLayout->addLayout(pBtnLayout, 1, 0);

    pBtnLayout->addStretch(1);

    // Ok button
    QPushButton* pButtonOK = new QPushButton(TR("id_ok_btn"), pDlg);
    pButtonOK->setDefault(true);
    pBtnLayout->addWidget(pButtonOK);

    // Cancel button
    QPushButton* pButtonCancel = new QPushButton(TR("id_cancel_btn"), pDlg);
    pButtonCancel->setShortcut(Qt::Key_Escape);
    pBtnLayout->addWidget(pButtonCancel);

    // Connect and validity stuffs(relation between thickness and density)
    QObject::connect(_pThicknessSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateMinimumDensity(double)));
    QObject::connect(pButtonOK, SIGNAL(clicked()), pDlg, SLOT(accept()));
    QObject::connect(pButtonCancel, SIGNAL(clicked()), pDlg, SLOT(reject()));

    // This line will trigger updateMinimumDensity()
    _pThicknessSpinBox->setValue(10);
    _pDensitySpinBox->setValue(_pDensitySpinBox->minimum() * 3.0); // arbitrary density

    // Display the properties dialog
    int ret = pDlg->exec();
    TYApplication::setOverrideCursor(Qt::WaitCursor);

    if (ret == QDialog::Accepted)
        dialogConfirmed(pHeightSpinBox->value(), _pThicknessSpinBox->value(),
                        pClosedCheckBox->isChecked(), _pDensitySpinBox->value(), forceOpened);

    delete pEditLayout;
    delete pBtnLayout;
    TYElement::setIsSavedOk(true);

    showText(false);

    refreshProjectFrame();
    _pModeler->getView()->getRenderer()->updateDisplayList();
    _pModeler->updateView();
    TYApplication::restoreOverrideCursor();
}

void TYBoundaryNoiseMapEditor::dialogConfirmed(double height, double thickness, bool closed, double density, bool forceOpened)
{
    /// XXX Ticket #644892. Check if the polyline hasn't been deleted by the TYPolyLineEditor::close() function
    if (this->getSavedPoints().empty())
    {
        return;
    }
    // We create the BoundaryNoiseMap entity
    TYSiteModelerFrame* pSiteModeler = (TYSiteModelerFrame*) _pModeler;

    LPTYBoundaryNoiseMap pBoundaryNoiseMap = new TYBoundaryNoiseMap();
    pBoundaryNoiseMap->setCanBeClosed(!forceOpened);
    LPTYBoundaryNoiseMapGeoNode pBoundaryNoiseMapGeoNode = new TYBoundaryNoiseMapGeoNode((LPTYElement)pBoundaryNoiseMap);

    TYProjet* pProjet = pSiteModeler->getProjet();
    // XXX What's happen when the pointer is NULL?!
    if (pProjet)
    {
        TYCalcul* pCalcul = pProjet->getCurrentCalcul();
        // XXX What's happen when the pointer is NULL?!
        if (pCalcul)
        {
            pCalcul->addMaillage((LPTYMaillageGeoNode&) pBoundaryNoiseMapGeoNode);

            // Init
            pBoundaryNoiseMap->setHauteur(height);
            pBoundaryNoiseMap->make(this->getSavedPoints(), thickness, closed, density);

            // Add action
            TYAction* pAction = new TYAddMaillageToCalculAction((LPTYMaillageGeoNode&) pBoundaryNoiseMapGeoNode,
                                                                pSiteModeler->getProjet()->getCurrentCalcul(), _pModeler, TR("id_action_add_boundarynoisemap"));
            _pModeler->getActionManager()->addAction(pAction);

            // Compte the noise map altimetry
            pCalcul->updateAltiMaillage(pBoundaryNoiseMapGeoNode);
            pBoundaryNoiseMap->updateGraphicTree();
        }
    }
}

void TYBoundaryNoiseMapEditor::updateMinimumDensity(double thickness)
{
    _pDensitySpinBox->setRange(TYBoundaryNoiseMap::computeMinimumDensity(thickness), std::numeric_limits<double>::max());
}

void TYBoundaryNoiseMapEditor::slotMousePressed(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
{
    if ((button == Qt::LeftButton) && _active)
    {
        if (!getTYApp()->getCurProjet() || !getTYApp()->getCurProjet()->getCurrentCalcul())
        {
            _active = false;

            // Msg "No current TYCalcul...
            QString msg(TR("id_warning_no_curcalcul"));
            writeOutputMsg(msg);
            QMessageBox::warning(_pModeler, "Tympan", msg, QMessageBox::Ok, QMessageBox::NoButton);
        }
    }
}

