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
 * \file TYReseauTransportEditor.h
 * \brief Construit un reseau de transport a partir des points saisis (fichier header)
 *
 *
 *
 */


#ifndef __TY_RESEAU_TRANSPORT_EDITOR__
#define __TY_RESEAU_TRANSPORT_EDITOR__

#include "TYPolyLineEditor.h"
#include "Tympan/MetierSolver/DataManagerCore/TYDefines.h"

/**
 * \class TYReseauTransportEditor
 * \brief Construit un reseau de transport a partir des points saisis
 */
class TYReseauTransportEditor : public TYPolyLineEditor
{
    Q_OBJECT

public:
    TYReseauTransportEditor(TYModelerFrame* pModeler);
    ~TYReseauTransportEditor();

public slots:
    /**
     * Construit un ReseauTransport a partir des points saisis.
     */
    void endReseauTransport();

protected:

};

#endif // __TY_RESEAU_TRANSPORT_EDITOR__
