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
 * \file TYBatimentModelerFrame.h
 * \brief Modeler specialisee pour l'edition des batiments (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_BATIMENT_MODELER_FRAME__
#define __TY_BATIMENT_MODELER_FRAME__


#include <qwidget.h>
#include "Tympan/models/business/infrastructure/TYBatiment.h"
#include "TYModelerFrame.h"


/**
 * \class TYBatimentModelerFrame
 * \brief Classe Modeler specialisee pour l'edition des batiments.
 */
class TYBatimentModelerFrame : public TYModelerFrame
{
    Q_OBJECT

public:
    /**
     * \fn TYBatimentModelerFrame(LPTYBatiment pBatiment = 0, QWidget* parent = 0, const char* name = 0, Qt::WFlags f = 0)
     * \brief Constructeur par defaut.
     */
    TYBatimentModelerFrame(LPTYBatiment pBatiment = 0, QWidget* parent = 0, const char* name = 0, Qt::WFlags f = 0);
    /**
     * \fn virtual ~TYBatimentModelerFrame()
     * \brief Destructeur.
     */
    virtual ~TYBatimentModelerFrame();

    /**
     * \fn void setBatiment(LPTYBatiment pBatiment)
     * \brief Set/Get du bâtiment a editer.
     */
    void setBatiment(LPTYBatiment pBatiment);
    /**
     * \fn void setBatiment(LPTYBatiment pBatiment)
     * \brief Set/Get du bâtiment a editer.
     */
    LPTYBatiment getBatiment() { return _pBatiment; }

    ///Les differents modes d'edition d'un Batiment.
    enum MachineModelerMode
    {
        EtageMode = NbOfModelerMode,
        SilosMode,
        MachineMode,
        SourceMode,
        NbOfBatimentModelerMode
    };

public slots:
    virtual void setEditorMode(int mode);
    virtual void updatePreferences();
    virtual bool close();

    void calculDistribution();

	virtual void closeEvent(QCloseEvent* pEvent);//closeEvent is overriden

protected:
    ///Un pointeur sur le bâtiment a editer.
    LPTYBatiment _pBatiment;

    ///Editor pour la construction d'etage.
    TYEtageEditor* _pEtageEditor;

    ///Editor pour la construction de silos.
    TYSilosEditor* _pSilosEditor;

    ///Editor pour l'ajout d'une source ponctuelle
    TYSourceEditor* _pSourceEditor;

    ///Nombre d'instance de type TYBatimentModelerFrame.
    static int _nbInstance;
};

#endif //__TY_BATIMENT_MODELER_FRAME__
