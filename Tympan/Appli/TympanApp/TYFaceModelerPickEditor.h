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
 * \file TYFaceModelerPickEditor.h
 * \brief gestion du picking des faces (fichier header)
 *
 *
 *
 *
 *
 */


#ifndef __TY_FACE_MODELER_PICK_EDITOR__
#define __TY_FACE_MODELER_PICK_EDITOR__


#include "TYPickEditor.h"

/**
 * \class TYFaceModelerPickEditor
 * \brief gestion du picking des faces
 */
class TYFaceModelerPickEditor : public TYPickEditor
{
    Q_OBJECT

public:
    TYFaceModelerPickEditor(TYModelerFrame* pModeler);
    ~TYFaceModelerPickEditor();


protected slots:
    virtual void showPopupMenu(LPTYElementCollection pElts);
    void showDimensionsDialog(TYRectangle* pRect, TYAcousticRectangleNode* pParent = NULL);


protected:

};

#endif // __TY_FACE_MODELER_PICK_EDITOR__
