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
 * \file TYSetCameraCoordinates.h
 * \brief Repositionnement de la camera en fonction de coordonnees specifiees (fichier header)
 *
 *
 *
 *
 *
 */


#ifndef __TY_SET_CAMERA_COORDINATES__
#define __TY_SET_CAMERA_COORDINATES__


#include "TYAppDefines.h"
#include <qdialog.h>

class QLineEdit;
/**
 * \class TYSetCameraCoordinates
 * \brief Repositionnement de la camera en fonction de coordonnees specifiees
 */
class TYSetCameraCoordinates : public QDialog
{
    Q_OBJECT

public:
    TYSetCameraCoordinates(TYModelerFrame* pModeler);
    ~TYSetCameraCoordinates();

    /**
     * Retourne la coordonnee X.
     */
    int getXCoord() { return _xCoord; }
    /**
     * Set la coordonnee X.
     */
    void setXCoord(int xCoord);

    /**
     * Retourne la coordonnee Y.
     */
    int getYCoord() { return _yCoord; }
    /**
     * Set la coordonnee Y.
     */
    void setYCoord(int yCoord);

    /**
     * Retourne la coordonnee Z.
     */
    int getZCoord() { return _zCoord; }
    /**
     * Set la coordonnee Z.
     */
    void setZCoord(int zCoord);

public slots:
    void slotXCoordChanged();
    void slotYCoordChanged();
    void slotZCoordChanged();

protected:
    int _xCoord;
    int _yCoord;
    int _zCoord;

    QLineEdit* _pXCoordLineEdit;
    QLineEdit* _pYCoordLineEdit;
    QLineEdit* _pZCoordLineEdit;
};

#endif // __TY_SET_CAMERA_COORDINATES__
