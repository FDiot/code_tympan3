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
 * \file TYFaceModelerFrame.cpp
 * \brief Classe Modeler specialisee pour l'edition des faces
 */


#include <qcombobox.h>
#include <qlayout.h>
#include <qtoolbutton.h>
#include <qpushbutton.h>
//Added by qt3to4:
#include <QFocusEvent>

#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/TYPreferenceManager.h"
#include "Tympan/gui/gl/TYAcousticRectangleNodeGraphic.h"
#include "Tympan/gui/app/TYRectangleEditor.h"
#include "Tympan/gui/app/TYPickEditor.h"
#include "Tympan/gui/app/TYFaceModelerPickEditor.h"
#include "Tympan/gui/app/TYAbstractSceneEditor.h"
#include "Tympan/gui/app/TYFaceToolbar.h"
#include "Tympan/gui/app/TYApplication.h"
#include "Tympan/gui/app/TYMainWindow.h"
#include "TYFaceModelerFrame.h"

#if defined(WIN32)
  #include <crtdbg.h>
  #ifdef _DEBUG
    #undef THIS_FILE
    static char THIS_FILE[] = __FILE__;
    #define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
  #endif
#endif


#define TR(id) OLocalizator::getString("TYFaceModelerFrame", (id))
#define IMG(id) OLocalizator::getPicture("TYFaceModelerFrame", (id))


int TYFaceModelerFrame::_nbInstance = 0;


TYFaceModelerFrame::TYFaceModelerFrame(LPTYAcousticRectangleNode pFace, QWidget* parent, const char* name, Qt::WFlags f) :
    TYModelerFrame(parent, name, f)
{
    _nbInstance++;
    setWindowTitle(TR("id_caption") + " " + QString("%1").arg(_nbInstance));

    _pCtrlLayout->addStretch(1);

    //QPushButton * pButtonOK = new QPushButton(TR("id_ok_btn"), this);
    //QObject::connect(pButtonOK, SIGNAL( clicked() ), this, SLOT( accept() ));
    //_pCtrlLayout->addWidget(pButtonOK);

    //_pViewTypeBox->setEnabled(false);

    // Vue de dessus
    setViewType(TopView);

    // Rectangle Editor
    _pRectangleEditor = new TYRectangleEditor(this);

    // Le pick editor
    delete _pPickEditor;
    _pPickEditor = new TYFaceModelerPickEditor(this);
    _pPickEditor->connect();

    // Cadrage
    _pOGLCameras[TopView]->setDefaultZoomFactor(0.3);
    _pOGLCameras[FrontView]->setDefaultZoomFactor(0.3);
    _pOGLCameras[LeftView]->setDefaultZoomFactor(0.3);
    _pOGLCameras[PerspView]->setDefaultZoomFactor(0.7);


    // Attribution de la face a editer
    if (pFace)
    {
        setFace(pFace);
    }
    else
    {
        setFace(new TYAcousticRectangleNode());
    }

    updatePreferences();

    showMaximized();
}

TYFaceModelerFrame::~TYFaceModelerFrame()
{
    _nbInstance--;

    if (_pFace)
    {
        _pFace->drawGraphic(false);
    }

    delete _pRectangleEditor;
    getView()->updateGL();
}

bool TYFaceModelerFrame::close()
{
    return TYModelerFrame::close();
}

void TYFaceModelerFrame::setFace(LPTYAcousticRectangleNode pFace)
{
    if (_pFace)
    {
        _pFace->drawGraphic(false);
    }

    _pFace = pFace;

    if (_pFace)
    {
        // On affiche la face
        _pFace->drawGraphic();

        // On recadre
        fit();
    }

    _pElement = _pFace;

    _pView->getRenderer()->setElement((LPTYElement&)_pFace);

    getView()->getRenderer()->updateDisplayList();

    updateToolActivation();
}

void TYFaceModelerFrame::setEditorMode(int mode)
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
            case BoucheMode:
                _pRectangleEditor->setMode(TYRectangleEditor::Bouche);
                _pCurrentEditor = _pRectangleEditor;
                break;
            case ChemineeMode:
                _pRectangleEditor->setMode(TYRectangleEditor::Cheminee);
                _pCurrentEditor = _pRectangleEditor;
                break;
            case FenetreMode:
                _pRectangleEditor->setMode(TYRectangleEditor::Fenetre);
                _pCurrentEditor = _pRectangleEditor;
                break;
            default:
                _editorModeAccepted = false;
        }
    }

    TYModelerFrame::setEditorMode(mode);
}

void TYFaceModelerFrame::done(bool ok)
{
    if (ok)
    {
    }
    else
    {
    }

    close();
    getTYMainWnd()->updateModelers(false, false);//az++ : permet de rafraichir le modeler machine & de site apres avoir ajoute une/des faces
}

void TYFaceModelerFrame::accept()
{
    done(true);
}

void TYFaceModelerFrame::reject()
{
    done(false);
}

void TYFaceModelerFrame::updatePreferences()
{
    // Grille
    _gridDimX = 20.0f;
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "GridDimXFace"))
    {
        _gridDimX = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, "GridDimXFace");
    }
    else
    {
        TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, "GridDimXFace", _gridDimX);
    }

    _gridDimY = 20.0f;
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "GridDimYFace"))
    {
        _gridDimY = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, "GridDimYFace");
    }
    else
    {
        TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, "GridDimYFace", _gridDimY);
    }

    _gridStep = 1.0f;
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "GridStepFace"))
    {
        _gridStep = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, "GridStepFace");
    }
    else
    {
        TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, "GridStepFace", _gridStep);
    }

    _gridMagnStep = 1.0f;
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "GridMagnStepFace"))
    {
        _gridMagnStep = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, "GridMagnStepFace");
    }
    else
    {
        TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, "GridMagnStepFace", _gridMagnStep);
    }

    resizeGrid();
    setGridLinesActorsVisibility((_curViewType == FrontView), (_curViewType == TopView) || (_curViewType == PerspView), (_curViewType == LeftView));

    TYModelerFrame::updatePreferences();
}

void TYFaceModelerFrame::updateToolActivation()
{
    if (_pFace)
    {
        // Activation des outils selon le type de la face
        if (dynamic_cast<TYMur*>(_pFace._pObj) != nullptr)
        {
            // Les sous-faces sont des fenetres
            getTYMainWnd()->getFaceToolbar()->activeBouche(false);
            getTYMainWnd()->getFaceToolbar()->activeCheminee(false);
            getTYMainWnd()->getFaceToolbar()->activeFenetre(true);
        }
        else
        {
            // Les sous-faces sont des bouches ou cheminees
            getTYMainWnd()->getFaceToolbar()->activeBouche(true);
            getTYMainWnd()->getFaceToolbar()->activeCheminee(true);
            getTYMainWnd()->getFaceToolbar()->activeFenetre(false);
        }
    }
    else
    {
        // Desactive tous les outils
        getTYMainWnd()->getFaceToolbar()->activeButtons(false);
    }
}

void TYFaceModelerFrame::focusInEvent(QFocusEvent* pEvent)
{
    updateToolActivation();
    TYModelerFrame::focusInEvent(pEvent);
}
