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
 * \file TYRouteEditor.h
 * \brief Construit une route a partir des points saisis (fichier header)
 *
 *
 *
 */

#ifndef __TY_ROUTE_EDITOR__
#define __TY_ROUTE_EDITOR__

#include "TYPolyLineEditor.h"
#include "Tympan/MetierSolver/DataManagerCore/TYDefines.h"

/**
 * \class TYRouteEditor
 * \brief Construit une route a partir des points saisis
 */
class TYRouteEditor : public TYPolyLineEditor
{
    Q_OBJECT

public:
    TYRouteEditor(TYModelerFrame* pModeler);
    ~TYRouteEditor();

public slots:
    /**
     * Construit un Route a partir des points saisis.
     */
    void endRoute();

signals:
    void setEditorMode(int);

protected:

};

#endif // __TY_ROUTE_EDITOR__
