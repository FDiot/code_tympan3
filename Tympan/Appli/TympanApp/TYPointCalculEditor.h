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
 * \file TYPointCalculEditor.h
 * \brief gestion de l'edition d'un point de calcul (fichier header)
 *
 *
 *
 *
 *
 *
 */


#ifndef __TY_POINT_CALCUL_EDITOR__
#define __TY_POINT_CALCUL_EDITOR__


#include "TYAbstractSceneEditor.h"

/**
 * \class TYPointCalculEditor
 * \brief gestion de l'edition d'un point de calcul
 */
class TYPointCalculEditor : public TYAbstractSceneEditor
{
    Q_OBJECT

public:
    TYPointCalculEditor(TYSiteModelerFrame* pModeler);
    ~TYPointCalculEditor();

public slots:
    virtual void undo();
    virtual void slotMouseReleased(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state);
    virtual void slotViewTypeChanged(int view);

protected:
    ///Indique si cet editor est actif.
    bool _active;
};

#endif // __TY_POINT_CALCUL_EDITOR__
