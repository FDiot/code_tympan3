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
 * \file TYPolyLineEditor.h
 * \brief gestion de l'edition d'une polyligne (fichier header)
 *
 *
 *
 *
 *
 */


#ifndef __TY_POLYLINE_EDITOR__
#define __TY_POLYLINE_EDITOR__


#include "TYAbstractSceneEditor.h"
#include "Tympan/MetierSolver/DataManagerCore/TYDefines.h"
#include <QList>

class OGLLineElement;
class OGLSphereElement;

/**
 * \class TYPolyLineEditor
 * \brief gestion de l'edition d'une polyligne
 */
class TYPolyLineEditor : public TYAbstractSceneEditor
{
    Q_OBJECT

public:
    TYPolyLineEditor(TYModelerFrame* pModeler);
    ~TYPolyLineEditor();

    /**
     * Retourne le tableau de points cree dans le mode
     * SavePoints.
     */
    TYTabPoint& getSavedPoints() { return _savedPoints; }

    /**
     * Set/Get de l'indicateur d'utilisation des angles pleins.
     */
    void setUseAngleStep(bool state) { _useAngleStep = state; }
    /**
     * Set/Get de l'indicateur d'utilisation des angles pleins.
     */
    bool getUseAngleStep() { return _useAngleStep; }

    /**
     * Set/Get de l'indicateur d'affichage de la distance.
     */
    void setDispDist(bool state) { _dispDist = state; }
    /**
     * Set/Get de l'indicateur d'affichage de la distance.
     */
    bool getDispDist() { return _dispDist; }

    /**
     * Set/Get de la taille pour la representation des points.
     */
    void setPointSize(float size) { _pointSize = size; }
    /**
     * Set/Get de la taille pour la representation des points.
     */
    float getPointSize() { return _pointSize; }


public slots:
    virtual void connect();
    virtual void disconnect();
    virtual void init();
    virtual void cancel();
    virtual void close();
    virtual void slotMouseReleased(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state);
    virtual void slotMouseMoved(int x, int y, Qt::MouseButtons button, Qt::KeyboardModifiers state);
    virtual void slotKeyPressed(int key);
    virtual void slotKeyReleased(int key);
    virtual void slotViewTypeChanged(int view);


protected slots:
    /**
     * Ajoute un nouveau point a la liste.
     */
    void addPoint(int x, int y);

    /**
     * Supprime le dernier point ajoute, s'il existe.
     */
    bool removeLastPoint();

    /**
     * Mets a jour la ligne courante.
     */
    bool updateMovingLine(int x, int y);

    /**
     * Modifie les coordonnees du point passe si necessaire
     * en fonction des options (grille magnetique, angle pleins).
     */
    void validatePoint(TYPoint& pt);


signals:
    void endedSavingPoints();


protected:
    ///Indique si cet editor est actif.
    bool _active;

    ///Indique si cet editor gere ou non les angles pleins.
    bool _useAngleStep;

    ///Indique si l'information de distance doit etre affichee ou non.
    bool _dispDist;

    ///Un CameraEditor associe a cet editor.
    TYCameraEditor* _pCamEditor;

    ///Tableau pour la sauvegarde des points du mode Save points.
    TYTabPoint _savedPoints;

    ///Lignes dans la vue 3D.
    QList<OGLLineElement*> _OGLLineElementList;

    ///Table de points openGL
    QList<OGLSphereElement*> _OGLSphereElementList;

    ///La ligne courante.
    OGLLineElement* _pOGLLineElement;

    ///Taille pour la representation des points.
    float _pointSize;

    ///Indique si le bouton Shift est enfonce.
    bool _shiftOn;
};

#endif // __TY_POLYLINE_EDITOR__
