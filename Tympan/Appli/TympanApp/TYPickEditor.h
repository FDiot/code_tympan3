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
 * \file TYPickEditor.h
 * \brief gestion de l'element actionne par picking (fichier header)
 *
 *
 *
 *
 *
 *
 */


#ifndef __TY_PICK_EDITOR__
#define __TY_PICK_EDITOR__


#include "TYAbstractSceneEditor.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYGeometryNode.h"
#include "TYElementPicker.h"
#include <QPoint>

/**
 * \class TYPickEditor
 * \brief gestion de l'element actionne par picking
 */
class TYPickEditor : public TYAbstractSceneEditor
{
    Q_OBJECT

public:
    TYPickEditor(TYModelerFrame* pModeler);
    ~TYPickEditor();

    /**
     * \fn TYElementPicker * getPicker()
     * \brief Methode qui retourne le picker.
     */
    TYElementPicker* getPicker() { return _pPicker; }

    /**
     * Set/Get de la tolerance du picking.
     */
    void setPickTolerance(float tolerance) { _pickTolerance = tolerance; }
    /**
     * Set/Get de la tolerance du picking.
     */
    float getPickTolerance() { return _pickTolerance; }

    /**
     * Set/Get de la tolerance du rollover.
     */
    void setRolloverTolerance(float tolerance) { _rolloverTolerance = tolerance; }
    /**
     * Set/Get de la tolerance du rollover.
     */
    float getRolloverTolerance() { return _rolloverTolerance; }


public slots:
    virtual void slotMousePressed(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state);
    virtual void slotMouseReleased(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state);
    virtual void slotMouseMoved(int x, int y, Qt::MouseButtons button, Qt::KeyboardModifiers state);
    virtual void slotWheeled(int x, int y, int delta, Qt::KeyboardModifiers state);
    virtual void slotKeyPressed(int key);
    virtual void slotMouseLeave();

    /**
     * Utilise ou pas le highlight lors d'un clic gauche.
     */
    void useHighlight(bool state) { _useHighlight = state; }
    /**
     * Utilise ou pas le popup lors d'un clic droit.
     */
    void usePopup(bool state) { _usePopup = state; }

    /**
     * Reinitialise le picker.
     */
    void resetPicker() { _pPicker->reset(); }


protected slots:
    virtual void showPopupMenu(std::shared_ptr<LPTYElementArray> pElts);
    void showPositionDialog(TYGeometryNode* pGeoNode);
    void showRotationDialog(TYGeometryNode* pGeoNode);
    void showDimensionsDialog(TYAcousticVolume* pAccVol);
    void showPanel(TYElement* pElt);
    void hidePanel(TYElement* pElt);


protected:
    ///Le picker du modeler.
    TYElementPicker* _pPicker;

    ///Derniere position de la souris
    QPoint _lastPressedCurPos;
    QPoint _lastMovedCurPos;

    // Dernier element rollover
    TYElement* _pLastRolloverElt;

    ///Flag pour l'activation du highlight.
    bool _useHighlight;
    ///Flag pour l'activation du popup menu.
    bool _usePopup;

    ///Tolerance du picking.
    float _pickTolerance;
    float _rolloverTolerance;
};

#endif // __TY_PICK_EDITOR__
