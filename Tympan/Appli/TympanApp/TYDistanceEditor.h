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
 * \file TYDistanceEditor.h
 * \brief outil de mesure des distances (fichier header)
 *
 *
 *
 *
 *
 */


#ifndef __TY_DISTANCE_EDITOR__
#define __TY_DISTANCE_EDITOR__


#include "TYAppDefines.h"
#include "TYAbstractSceneEditor.h"
#include "Tympan/MetierSolver/DataManagerCore/TYDefines.h"

class OGLLineElement;

/**
 * \class TYDistanceEditor
 * \brief outil de mesure des distances
 */
class TYDistanceEditor : public TYAbstractSceneEditor
{
    Q_OBJECT

public:
    TYDistanceEditor(TYModelerFrame* pModeler);
    virtual ~TYDistanceEditor();

public slots:
    virtual void connect();
    virtual void disconnect();
    virtual void init();
    virtual void cancel();
    virtual void close();
    virtual void slotMousePressed(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state);
    virtual void slotMouseMoved(int x, int y, Qt::MouseButtons button, Qt::KeyboardModifiers state);
    virtual void slotMouseReleased(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state);
    virtual void slotKeyPressed(int key);
    virtual void slotKeyReleased(int key);
    virtual void slotViewTypeChanged(int view);

protected slots:
    /**
     * \fn bool updateMovingLine()
     * \brief Mets a jour la ligne courante.
     */
    bool updateMovingLine();


protected:
    ///Indique si cet editor est actif.
    bool _active;

    ///Un CameraEditor associe a cet editor.
    TYCameraEditor* _pCamEditor;

    ///La ligne courante.
    OGLLineElement* _pOGLLineElement;

    ///Indique si le bouton Shift est enfonce.
    bool _shiftOn;
};

#endif // __TY_DISTANCE_EDITOR__
