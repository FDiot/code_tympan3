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
 * \file TYMachineModelerFrame.h
 * \brief Modeler specialisee pour l'edition des machines (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_MACHINE_MODELER_FRAME__
#define __TY_MACHINE_MODELER_FRAME__


#include <qwidget.h>
#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYMachine.h"
#include "TYModelerFrame.h"


/**
 * \class TYMachineModelerFrame
 * \brief Classe Modeler specialisee pour l'edition des machines.
 */
class TYMachineModelerFrame : public TYModelerFrame
{
    Q_OBJECT

public:
    /**
     * Constructeur par defaut.
     */
    TYMachineModelerFrame(LPTYMachine pMachine = 0, QWidget* parent = 0, const char* name = 0, Qt::WFlags f = 0);
    /**
     * Destructeur.
     */
    virtual ~TYMachineModelerFrame();

    /**
     * Set/Get de la machine a editer.
     */
    void setMachine(LPTYMachine pMachine);
    /**
     * Set/Get de la machine a editer.
     */
    LPTYMachine getMachine() { return _pMachine; }


    ///Les differents modes d'edition d'une Machine.
    enum MachineModelerMode
    {
        AddBoxMode = NbOfModelerMode,
        AddCylMode,
        AddSemiCylMode,
        NbOfMachineModelerMode
    };


public slots:
    virtual void setEditorMode(int mode);
    virtual void updatePreferences();
    virtual bool close();

    void calculDistribution();


protected:
    ///Un pointeur sur la machine a editer.
    LPTYMachine _pMachine;

    ///Box editor.
    TYBoxEditor* _pBoxEditor;
    ///Cylinder editor.
    TYCylinderEditor* _pCylinderEditor;
    ///Semi cylinder editor.
    TYSemiCylinderEditor* _pSemiCylinderEditor;

    ///Nombre d'instance de type TYMachineModelerFrame.
    static int _nbInstance;
};

#endif //__TY_MACHINE_MODELER_FRAME__
