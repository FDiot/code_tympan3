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
 * \file TYPositionEditor.h
 * \brief gestion de la position selon les modes 'moving', 'rotation', 'edition' (fichier header)
 *
 *
 *
 *
 *
 */


#ifndef __TY_POSITION_EDITOR__
#define __TY_POSITION_EDITOR__

#include <limits>

#include "TYAbstractSceneEditor.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYGeometryNode.h"
#include "TYAction.h"

class TYElement;
class QTimer;
class OGLLineElement;

/**
 * \class TYPositionEditor
 * \brief gestion de la position selon les modes 'moving', 'rotation', 'edition'
 */
class TYPositionEditor : public TYAbstractSceneEditor
{
    Q_OBJECT

public:
    /**
     * Constructeur.
     *
     * @param mode Le mode d'edition (Moving, Rotation, Edition).
     */
    TYPositionEditor(TYModelerFrame* pModeler, int mode = Moving);
    /**
     * Destructeur.
     */
    virtual ~TYPositionEditor();

    /**
     * Set/Get du temps d'interaction (step du timer).
     */
    void setInteractionTime(int time) { _interactionTime = time; }
    /**
     * Set/Get du temps d'interaction (step du timer).
     */
    int getInteractionTime() { return _interactionTime; }

    /**
     * Set/Get du facteur de rotation.
     */
    void setSensitivity(float t) { _trackballFactor = t; }
    /**
     * Set/Get du facteur de rotation.
     */
    float getSensitivity() { return _trackballFactor; }

    /**
     * Set/Get du mode d'edition.
     */
    void setMode(int mode) { _mode = mode; }
    /**
     * Set/Get du mode d'edition.
     */
    int getMode() { return _mode; }

    /**
     * Set/Get du pas d'angle pour la rotation.
     */
    void setAngleStep(float step) { _angleStep = step; }
    /**
     * Set/Get du pas d'angle pour la rotation.
     */
    float getAngleStep() { return _angleStep; }

    /**
     * Set/Get de la precision pour la detection d'un point clique.
     */
    void setPickPointPrecision(float precision) { _pickPointPrecision = precision; }
    /**
     * Set/Get de la precision pour la detection d'un point clique.
     */
    float getPickPointPrecision() { return _pickPointPrecision; }

    /**
     * On affiche seulement la bounding box ou pas lors de l'edition.
     */
    bool getOnlyHighlight() { return _viewOnlyHighlight; }

    /**
     * Permet d'eviter de deplacer un element hors de son contexte
     */
    bool selectElement(TYElement* pElt);


    ///Les modes pour cet editor.
    enum { Moving, Rotation, Edition };


public slots:
    virtual void connect();
    virtual void disconnect();
    virtual void init();
    virtual void cancel();
    virtual void close();
    virtual void slotMousePressed(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state);
    virtual void slotMouseReleased(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state);
    virtual void slotMouseMoved(int x, int y, Qt::MouseButtons button, Qt::KeyboardModifiers state);
    virtual void slotKeyPressed(int key);
    virtual void slotKeyReleased(int key);
    virtual void slotViewTypeChanged(int view);

    /**
     * On affiche seulement la bounding box ou pas lors de l'edition.
     */
    void viewOnlyHighlight(bool state) { _viewOnlyHighlight = state; }


protected slots:
    void initEditSiteNode(TYElement* pElt);
    void initEditVolume(TYElement* pElt);
    void initEditMachine(TYElement* pElt);
    //void initEditEtage(TYElement* pElt);
    void initEditBatiment(TYElement* pElt);
    void initEditRoute(TYElement* pElt);
    void initEditResTrans(TYElement* pElt);
    void initEditCrbNiv(TYElement* pElt);
    void initEditCrsEau(TYElement* pElt);
    void initEditPlanEau(TYElement* pElt);
    void initEditTerrain(TYElement* pElt);
    void initEditSrcPonct(TYElement* pElt);
    void initEditPointControl(TYElement* pElt);
    void initEditMaillage(TYElement* pElt);
    void initEditEcran(TYElement* pElt);

    /**
     * Mets a jour le repere de _pEditGeoNode.
     * Applique une rotation, rotate() ou un deplacement, move(),
     * en fonction du mode choisi et du deplacement de la souris.
     */
    void editGeoNode();

    /**
     * Deplace _pEditPoint selon le deplacement de la souris.
     */
    void editPoint();

    /**
     * Deplace _pEditPoint, un point d'un etage, selon le
     * deplacement de la souris.
     */
    void editPointEtage();

    /**
     * Oriente la source ponctuelle pickee.
     */
    void orienteSrcPonct();


protected:
    /**
     * Modifie les coordonnees du point passe en fonction
     * du deplacement effectue avec la souris.
     *
     * @param pPt Le point a deplacer.
     * @param enableX Autorise ou interdit la translation sur X.
     * @param enableY Autorise ou interdit la translation sur Y.
     * @param enableZ Autorise ou interdit la translation sur Z.
     *
     * @return <code>true</code> si le point a ete modifie;
     *         <code>false</code> sinon.
     */
    bool move(OPoint3D* pPt, bool enableX = true, bool enableY = true, bool enableZ = true);

    /**
     * Modifie l'orientation repere passe en fonction
     * du deplacement effectue avec la souris.
     * La position du point d'origine n'est pas modifiee.
     *
     * @param pRepere Le repere a modifier.
     * @param enableX Autorise ou interdit la rotation autour de X.
     * @param enableY Autorise ou interdit la rotation autour de Y.
     * @param enableZ Autorise ou interdit la rotation autour de Z.
     *
     * @return <code>true</code> si le repere a ete modifie;
     *         <code>false</code> sinon.
     */
    bool rotate(ORepere3D* pRepere, bool enableX = true, bool enableY = true, bool enableZ = true);

    /**
     * Recherche si le point passe (en coordonnees locales a l'objet) est
     * present dans la liste de point passee.
     * Le test se fait avec une certaine tolerance (PickPointPrecision).
     *
     * @param ptToFind Le point a chercher dans la liste.
     * @param pInPoints La collection de point pour rechercher le point.
     *
     * @return L'indice du point trouve dans la collection de point ou -1 sinon.
     */
    int findPoint(float* ptToFind, TYTabPoint* pInPoints);

    /**
     * Mets a jour la position des moving lines.
     */
    void updateMovingLines(TYTabPoint* pPoints, int centralPointId);
    /**
     * Mets a jour la position du point central des moving lines.
     */
    void updateMovingPoint(const TYPoint& point);

    /**
     * Changement de repere pour calculer le delta de la souris lors des  translations et rotations.
     */
    void PointOfGeoNodeInCurrentSiteCoordinates(TYGeometryNode* pGeoNode, OPoint3D& centralPointGN);
    /**
     * Changement de repere pour calculer le delta de la souris lors des  translations et rotations.
     */
    void PointOfCurrentSiteInGeoNodeCoordinates(TYGeometryNode* pGeoNode, OPoint3D& centralPointGN);
    /**
     * Changement de repere pour calculer le delta de la souris lors des  translations et rotations.
     */
    void PointOfGeoNodeInCurrentSiteCoordinatesOrPointOfCurrentSiteInGeoNodeCoordinates(TYGeometryNode* pGeoNode, OPoint3D& centralPointGN, bool bPointOfGeoNodeInCurrentSiteCoordinates);
    /**
     * \brief Edit a polyline
     * \param pElt The entity which contains the polyline
     * \param pts Points of the polyline
     * \param closed If the polyline is closed
     * \param pAlti The altimetry (if available)
     */
    void editPolyLine(LPTYElement pElt, TYTabPoint& pts, bool close, LPTYAltimetrie pAlti = NULL);

    /**
     * \brief Add a new point on the closest line to the mouse position
     * \param pts Points of the polyline.
     * \param newPoint The newly created point (for undo/redo purpose)
     * \param idInsert The id of the newly created point (for undo/redo purpose)
     */
    void insertNewPoint(TYTabPoint& pts, TYPoint& newPoint, int& idInsert, LPTYAltimetrie pAlti = NULL);

    /**
     * \brief Delete the closest point of a polyline to the mouse position
     * \param pts The points of the polyline
     * \param deletedPoint The deleted point (for undo/redo purpose)
     * \param idDelete The id of the deleted point (for undo/redo purpose)
     */
    void deletePoint(TYTabPoint& pts, TYPoint& deletedPoint, int& idDelete);

    /**
     * \brief Find the closest segment of a polyline to the mouse position
     * \param pts The points of the polyline
     * \return The id of the first point of the closest segment
     */
    int findClosestSegmentToMouse(const TYTabPoint& pts);

    /**
     * \brief Return the id of the closest point of a polyline to the mouse position
     * \param pts The points of the polyline
     * \return The id of the closest point
     */
    int findClosestPointToMouse(const TYTabPoint& pts);

    /**
     * \bref Shortest distance (squared) between a segment and a point
     * \param mx Point to project (x)
     * \param my Point to project (y)
     * \param vx First point of the segment (x)
     * \param vy First point of the segment (y)
     * \param wy Second point of the segment (x)
     * \param wy Second point of the segment (y)
     * \return The squared distance between the point and the segment
     */
    double distSegmentSqr(double mx, double my, double vx, double vy, double wx, double wy) const;

protected:
    ///Le mode pour cet editor.
    int _mode;

    ///Indique si cet editor est actif.
    bool _active;

    ///Indiquateur pour l'init de calculs.
    bool _firstCall;

    ///Indique si une action est deja en cours.
    bool _inUse;

    ///Indique si on affiche que la bounding box lors de l'edition.
    bool _viewOnlyHighlight;

    ///Le picker du modeler.
    TYElementPicker* _pPicker;

    ///Un CameraEditor associe a cet editor.
    TYCameraEditor* _pCamEditor;

    ///Indique si le bouton Shift est enfonce.
    bool _shiftOn;

    ///If the A key is pressed.
    bool _keyAOn;

    ///If the D key is pressed.
    bool _keyDOn;

    ///L'element que l'on edite.
    TYElement* _pEditElement;

    ///Le GeoNode associe a l'element que l'on edite.
    TYGeometryNode* _pEditGeoNode;

    ///Le point a deplacer.
    TYPoint* _pEditPoint;

    ///Le contour de l'etage a editer.
    TYTabPoint _editContour;

    ///Timer pour miniser le nombre de refresh.
    QTimer* _pStepTimer;
    ///La duree de step pour le timer.
    int     _interactionTime;

    ///Indique si l'editor est en pleine action.
    bool    _mouseEventActive;

    ///La position courante du curseur.
    int     _currentMousePos[2];
    ///La derniere position connue du curseur.
    int     _lastMousePos[2];

    ///Facteur de rotation.
    float _trackballFactor;

    ///Pour la representation de l'edition d'un point.
    OGLLineElement* _pOGLLineElementMoving1;
    OGLLineElement* _pOGLLineElementMoving2;

private:
    /// Indicateur d'edition dans un modeler frame
    bool _bSiteModelerFrame;

    ///Pour l'affichage de l'angle de rotation a appliquer.
    float _angleInfo;

    ///La derniere action (pour le undo).
    TYAction* _pLastAction;

    ///Pas d'angle pour la rotation.
    float _angleStep;

    ///Precision pour la detection d'un point clique.
    float _pickPointPrecision;


};

#endif // __TY_POSITION_EDITOR__
