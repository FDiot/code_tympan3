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
 * \file TYLinearMaillageEditor.h
 * \brief Indique si cet editor est actif (fichier header)
 *
 *
 *
 *
 *
 */


#ifndef __TY_LINEAR_MAILLAGE_EDITOR__
#define __TY_LINEAR_MAILLAGE_EDITOR__


#include "TYAbstractSceneEditor.h"

class QTimer;
class OGLLineElement;

/**
 * \class TYLinearMaillageEditor
 * \brief Indique si cet editor est actif
 */
class TYLinearMaillageEditor : public TYAbstractSceneEditor
{
    Q_OBJECT

public:
    TYLinearMaillageEditor(TYSiteModelerFrame* pModeler);
    virtual ~TYLinearMaillageEditor();


public slots:
    virtual void init();
    virtual void cancel();
    virtual void close();
    virtual void slotMousePressed(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state);
    virtual void slotMouseMoved(int x, int y, Qt::MouseButtons button, Qt::KeyboardModifiers state);
    virtual void slotMouseReleased(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state);
    virtual void slotKeyPressed(int key);
    virtual void slotViewTypeChanged(int view);


protected:
    ///Indique si cet editor est actif.
    bool _active;

    ///Indique l'absence de calcul courant.
    bool _noCalcul;

    ///La ligne courante.
    OGLLineElement* _pOGLLineElement;
};

#endif // __TY_LINEAR_MAILLAGE_EDITOR__
