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
 * \file TYCameraStepEditor.h
 * \brief gestion de la camera pas a pas (fichier obsolete)(fichier header)
 *
 *
 *
 *
 *
 */

#ifndef __TY_CAMERA_STEP_EDITOR__
#define __TY_CAMERA_STEP_EDITOR__

#include "TYAppDefines.h"
#include "TYAbstractSceneEditor.h"

class QDialog;
class QLineEdit;
/**
 * \class TYCameraStepEditor
 * \brief gestion de la camera pas a pas (obsolete)
 */
class TYCameraStepEditor : public TYAbstractSceneEditor
{
    Q_OBJECT

public:
    TYCameraStepEditor(TYModelerFrame* pModeler);
    ~TYCameraStepEditor();


public slots:
    virtual void init();
    virtual void cancel();
    virtual void close();
    virtual void slotViewTypeChanged(int view);
    void slotZoomStepChanged();
    void slotTranslateChanged();
    void slotRotateChanged();

protected:
    ///Indique si cet editor est actif.
    bool _active;

    ///Boite de dialogue pour les deplacements camera
    QDialog* _pDlg;

    ///Editor de camera du modeleur
    TYCameraEditor* _pCameraEditor;

    QLineEdit* _pZoomStepLineEdit;
    QLineEdit* _pTranslateStepLineEdit;
    QLineEdit* _pRotateStepLineEdit;
};

#endif // __TY_CAMERA_STEP_EDITOR__
