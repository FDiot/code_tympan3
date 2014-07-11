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
 * \file TYSolEditor.h
 * \brief Construit un sol a partir des points saisis (fichier header)
 *
 *
 *
 */

/**
 * \class TYSolEditor
 * \brief Construit un sol a partir des points saisis
 */
#ifndef __TY_SOL_EDITOR__
#define __TY_SOL_EDITOR__


#include "TYPolyLineEditor.h"
#include "Tympan/models/business/TYDefines.h"


class TYSolEditor : public TYPolyLineEditor
{
    Q_OBJECT

public:
    TYSolEditor(TYModelerFrame* pModeler);
    ~TYSolEditor();

public slots:
    /**
     * Construit un sol a partir des points saisis.
     */
    void endSol();

protected:

};

#endif // __TY_SOL_EDITOR__
