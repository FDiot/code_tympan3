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
 * \file TYPlanEauEditor.h
 * \brief  Construit un plan d'eau a partir des points saisis (fichier header)
 *
 *
 *
 *
 *
 *
 */


#ifndef __TY_PLAN_EAU_EDITOR__
#define __TY_PLAN_EAU_EDITOR__

#include "Tympan/MetierSolver/DataManagerCore/TYDefines.h"
#include "TYPolyLineEditor.h"

/**
 * \class TYPlanEauEditor
 * \brief Construit un plan d'eau a partir des points saisis
 */
class TYPlanEauEditor : public TYPolyLineEditor
{
    Q_OBJECT

public:
    TYPlanEauEditor(TYModelerFrame* pModeler);
    ~TYPlanEauEditor();

public slots:
    virtual void slotKeyPressed(int key);

    /**
     * Construit un plan d'eau a partir des points saisis.
     */
    void endPlanEau();


protected:
    ///Le dernier plan d'eau cree.
    LPTYPlanEau _pPlanEau;
};

#endif // __TY_PLAN_EAU_EDITOR__
