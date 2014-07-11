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
 * \file TYCourbeNiveauEditor.h
 * \brief Construit une courbe de niveau a partir des points saisis (fichier header)
 *
 *
 *
 *
 *
 *
 */


#ifndef __TY_COURBE_NIVEAU_EDITOR__
#define __TY_COURBE_NIVEAU_EDITOR__


#include "TYPolyLineEditor.h"
#include "Tympan/models/business/TYDefines.h"

/**
 * \class TYCourbeNiveauEditor
 * \brief Construit une courbe de niveau a partir des points saisis
 */
class TYCourbeNiveauEditor : public TYPolyLineEditor
{
    Q_OBJECT

public:
    TYCourbeNiveauEditor(TYModelerFrame* pModeler);
    ~TYCourbeNiveauEditor();

public slots:
    virtual void init();
    virtual void slotKeyPressed(int key);

    /**
     * \fn void endCourbeNiveau()
     * \brief Construit une courbe de niveau a partir des points saisis.
     */
    void endCourbeNiveau();


protected:
    ///La derniere courbe de niveau creee.
    LPTYCourbeNiveau _pCrbNiv;
};

#endif // __TY_COURBE_NIVEAU_EDITOR__
