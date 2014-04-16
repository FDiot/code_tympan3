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
 * \file TYMachineModelerFrame.cpp
 * \brief Modeler specialisee pour l'edition des machines
 *
 *
 */





#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/GraphicIHM/DataManagerGraphic/TYMachineGraphic.h"

#include <qlayout.h>
#include <qpushbutton.h>
#include <qcursor.h>

#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerCore/TYPreferenceManager.h"

#define TR(id) OLocalizator::getString("TYMachineModelerFrame", (id))
#define IMG(id) OLocalizator::getPicture("TYMachineModelerFrame", (id))


int TYMachineModelerFrame::_nbInstance = 0;


TYMachineModelerFrame::TYMachineModelerFrame(LPTYMachine pMachine, QWidget* parent, const char* name, Qt::WFlags f) :
    TYModelerFrame(parent, name, f)
{
    _nbInstance++;
    setWindowTitle(TR("id_caption") + " " + QString("%1").arg(_nbInstance));

    _pCtrlLayout->addSpacing(10);

    // Btn Calculer
    QPushButton* pCalculBtn = new QPushButton(QPixmap(IMG("id_icon_calcul_btn")),  "",/*TR("id_calcul_btn"),*/ this);
    pCalculBtn->setFixedSize(24, 24);
    _pCtrlLayout->addWidget(pCalculBtn, 0);
    _pCtrlLayout->addStretch(1);
    connect(pCalculBtn, SIGNAL(clicked()), this, SLOT(calculDistribution()));

    //Editors
    _pBoxEditor = new TYBoxEditor(this);
    _pCylinderEditor = new TYCylinderEditor(this);
    _pSemiCylinderEditor = new TYSemiCylinderEditor(this);

    // Cadrage
    _pOGLCameras[TopView]->setDefaultZoomFactor(0.3);
    _pOGLCameras[FrontView]->setDefaultZoomFactor(0.3);
    _pOGLCameras[LeftView]->setDefaultZoomFactor(0.3);
    _pOGLCameras[PerspView]->setDefaultZoomFactor(0.7);


    if (pMachine)
    {
        setMachine(pMachine);
    }
    else
    {
        setMachine(new TYMachine());
    }

    updatePreferences();
}

TYMachineModelerFrame::~TYMachineModelerFrame()
{
    _nbInstance--;

    if (_pMachine)
    {
        _pMachine->drawGraphic(false);
    }

    delete _pBoxEditor;
    delete _pCylinderEditor;
    delete _pSemiCylinderEditor;
    getView()->updateGL();
}

bool TYMachineModelerFrame::close()
{
    return TYModelerFrame::close();
}

void TYMachineModelerFrame::setMachine(LPTYMachine pMachine)
{
    QString caption(TR("id_caption") + " " + QString("%1").arg(_nbInstance));

    if (_pMachine)
    {
        _pMachine->drawGraphic(false);
    }

    _pMachine = pMachine;

    if (_pMachine)
    {
        if (!_pMachine->getName().isEmpty())
        {
            caption += QString(" : %1").arg(_pMachine->getName());
        }

        // On affiche la machine dans ce renderer
        _pMachine->drawGraphic();

        // On recadre
        fit();
    }

    _pElement = _pMachine;
    setWindowTitle(caption);

    _pView->getRenderer()->setElement((LPTYElement&)_pMachine);
    getView()->getRenderer()->updateDisplayList();
}

void TYMachineModelerFrame::setEditorMode(int mode)
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
            case AddBoxMode:
                _pCurrentEditor = _pBoxEditor;
                break;
            case AddCylMode:
                _pCurrentEditor = _pCylinderEditor;
                break;
            case AddSemiCylMode:
                _pCurrentEditor = _pSemiCylinderEditor;
                break;
            default:
                _editorModeAccepted = false;
        }
    }

    TYModelerFrame::setEditorMode(mode);
}

void TYMachineModelerFrame::calculDistribution()
{
    if (_pMachine)
    {
        getTYApp()->getCalculManager()->updateAcoustic(_pMachine);
    }
}

void TYMachineModelerFrame::updatePreferences()
{
    // Grille
    _gridDimX = 100.0f;
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "GridDimXMachine"))
    {
        _gridDimX = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, "GridDimXMachine");
    }
    else
    {
        TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, "GridDimXMachine", _gridDimX);
    }

    _gridDimY = 100.0f;
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "GridDimYMachine"))
    {
        _gridDimY = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, "GridDimYMachine");
    }
    else
    {
        TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, "GridDimYMachine", _gridDimY);
    }

    _gridStep = 5.0f;
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "GridStepMachine"))
    {
        _gridStep = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, "GridStepMachine");
    }
    else
    {
        TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, "GridStepMachine", _gridStep);
    }

    _gridMagnStep = 1.0f;
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "GridMagnStepMachine"))
    {
        _gridMagnStep = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, "GridMagnStepMachine");
    }
    else
    {
        TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, "GridMagnStepMachine", _gridMagnStep);
    }

    resizeGrid();
    setGridLinesActorsVisibility((_curViewType == FrontView), (_curViewType == TopView) || (_curViewType == PerspView), (_curViewType == LeftView));

    TYModelerFrame::updatePreferences();
}

