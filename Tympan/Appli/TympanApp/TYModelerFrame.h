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
 * \file TYModelerFrame.h
 * \brief Classe generique pour une fenetre de modeleur (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_MODELER_FRAME__
#define __TY_MODELER_FRAME__


#include <qwidget.h>
//Added by qt3to4:
#include <QCloseEvent>
#include <QBoxLayout>
#include <QShowEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QGridLayout>
#include <QFocusEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QEvent>

#include "TYAppDefines.h"
#include "TYRenderWindowInteractor.h"
#include "TYActionManager.h"
#include "Tympan/MetierSolver/DataManagerCore/TYElement.h"

class QGridLayout;
class QBoxLayout;
class QComboBox;
class QStatusBar;
class QToolButton;
class OGLTextElement;
class OGLScalarBarElement;
class OGLLineElement;
class OGLGridElement;
class OGLLightElement;
class OGLCamera;


/**
 * \class TYModelerFrame
 * \brief Classe generique pour une fenetre de modeleur
 */
class TYModelerFrame : public QWidget
{
    Q_OBJECT

public:
    /**
     * Les 5 vues du modeler :
     *  - la vue de dessus,
     *  - la vue de gauche,
     *  - la vue de face,
     *  - la vue en perspective.
     *  - la vue en camera mobile.
     */
    enum ModelerViewType
    {
        TopView,
        LeftView,
        FrontView,
        PerspView,
        FreeView,
        NbOfViews
    };

    /**
     * Les modes de rendu.
     */
    enum RenderMode
    {
        Points,
        Wireframe,
        Surface,
        Flat,
        Gouraud,
        Phong,
        NbOfRenderMode
    };

    /**
     * Les differents modes d'edition d'un site.
     */
    enum ModelerMode
    {
        NoMode = -1,
        CameraMode,
        CameraZoneMode,
        CameraStepMode,
        DistanceMode,
        MovingMode,
        RotationMode,
        EditionMode,
        NbOfModelerMode
    };


    /**
     * Constructeur par defaut.
     */
    TYModelerFrame(QWidget* parent = 0, const char* name = 0, Qt::WFlags f = 0);
    /**
     * Destructeur.
     */
    virtual ~TYModelerFrame();

    /**
     * Retourne la vue graphique.
     */
    TYRenderWindowInteractor* getView() { return _pView; }

    /**
     * Retourne le renderer associe a la vue.
     */
    TYOpenGLRenderer* getRenderer() { return _pView->getRenderer(); }

    /**
     * Retourne le type de la vue courante.
     */
    int getCurrentView() { return _curViewType; }

    /**
     * Retourne le mode de rendu actif.
     */
    int getRenderMode() { return _curRenderMode; }

    /**
     * Retourne l'element picker.
     *
     * @return L'element picker.
     */
    TYElementPicker* getElementPicker() { return _pPicker; }
    /**
     * Retourne un pointeur sur le pick editor associe a ce modeler.
     */
    TYPickEditor* getPickEditor() { return _pPickEditor; }

    /**
     * Retourne un pointeur sur l'editor courant de ce modeler.
     */
    TYAbstractSceneEditor* getCurrentEditor() { return _pCurrentEditor; }

    /**
     * Retourne un pointeur sur l'editor de la camera.
     */
    TYCameraEditor* getCameraEditor() { return _pCameraEditor; }

    /**
     * Retourne un pointeur sur la barre d'etat.
     */
    QStatusBar* statusBar() { return _pStatusBar; }

    /**
     * Retourne l'etat d'activation de la grille magnetique.
     */
    bool getSnapGridActive() { return _snapGridActive; }

    /**
     * Retourne le gestionnaire d'action (historique).
     */
    TYActionManager* getActionManager() { return &_actionManager; }

    /**
     * Indique si on affiche ou pas les sources ponctuelles.
     */
    bool getShowSources() { return _showSources; }

    /**
     * Indique l'etat de visualisation de la grille
     */
    bool getShowGrid() { return _showGrid; }

    /**
     * Retourne l'etat du mode de rendu special lors du deplacement de
     * la camera.
     *
     * @see _wireframeOnMovingCamera
     */
    bool getWireframeOnMovingCamera() { return _wireframeOnMovingCamera; }

    /**
     * Set/Get de l'element a editer.
     */
    void setElement(LPTYElement pElement) { _pElement = pElement;}
    /**
     * Set/Get de l'element a editer.
     */
    LPTYElement getElement() { return _pElement; }

    /**
     * Indique si l'element edite fait parti de la selection du calcul
     * courant du projet courant.
     */
    bool isElementInCurrentCalcul();
    /**
     * Indique si l'element edite fait parti du projet courant.
     * La recherche se fait par la paternite.
     */
    bool isElementInCurrentProjet();
    /**
     * Indique l'utilisateur accepte d'effacer le resultat (si necessaire)
     * pour effectuer l'action.
     */
    bool askForResetResultat();

    /**
     * Calcule les coordonees dans la scene 3D a partir de la
     * position du curseur.
     * Cette methode peut etre surchagee pour la calcul de
     * l'altitude.
     */
    virtual bool computeCurPos(int x, int y, float* pos);

public slots:
    /**
     * Selectionne le type vue.
     */
    virtual void setViewType(int view);

    /**
     * Selectionne le mode de rendu.
     */
    virtual void setRenderModeSlot(int mode);

    /**
     * Selectionne le mode de rendu.
     */
    virtual void setRenderMode(int mode, bool bUpdateGL);

    /**
     * Active un mode d'edition.
     */
    virtual void setEditorMode(int mode);
    /**
     * Active le mode camera.
     */
    void setEditorModeToCamera() { setEditorMode(CameraMode); }

    /**
     * Affiche ou non la grille.
     */
    void showGrid(bool show);

    /**
     * Affiche ou masque les normals.
     * Fait un updateView().
     */
    void showNormals(bool show);

    /**
     * Affiche ou masque le plafond des batiments.
     * Fait un updateView().
     */
    void showPlafond(bool show);

    /**
     * Affiche ou pas les sources ponctuelles.
     */
    void showSources(bool show);

    /**
     * Affichage des rayons
     */
    void showRays(bool show);
    /**
     * Active ou non la grille magnetique.
     */
    void setSnapGridActive(bool state);

    /**
     * Pour specifier les coordonnees de la camera (en mode "mobile")
     */
    void setCameraCoordinates();

    /**
     * Effectue un snapshot de la vue 3D pour la sauvegarder
     * dans un fichier image.
     */
    void screenShot();

    /**
     * Mets a jour la vue graphique ainsi que les axes et la
     * grille.
     *
     * @param clipping Pour mettre a jour le clipping range de la camera.
     * @param axesAndGrid Pour mettre a jour les axes et la grille.
     */
    virtual void updateView(bool clipping = true, bool axesAndGrid = true);

    /**
     * Mets a jour l'orientation des axes a partir de la position
     * de la camera.
     */
    void updateAxes();

    /**
     * Mets a jour l'information de position du curseur.
     */
    void updateCurPosInfo();

    /**
     * Construit la grille.
     */
    void updateGrid();

    /**
     * Mets a jour la structure graphique de l'element associe a ce modeler.
     *
     * @param force Force la mise a jour meme si l'element n'est pas marque
     *              comme modifie.
     */
    void updateElementGraphic(bool force = false);

    /**
     * Lance l'impression de la fenetre en cours.
     */
    void print();

    /**
     * Effectue un snapshot de la vue 3D pour la copier
     * dans le presse papier.
     */
    void copy();

    /**
     * Affiche la boite d'edition des proprietes de l'element edite.
     */
    void editElement();

    /**
     * Cadre la vue afin de voir l'ensemble des objets graphiques
     * presents dans la scene.
     */
    void fit();

    /**
     * Mise a jour des parametres des preferences.
     */
    virtual void updatePreferences();

    /**
     * Mise a jour de la grille.
     */
    void resizeGrid();

    /**
     * Surcharge de la fonction close de QWidget.
     * Propose la sauvegarde a la fermeture.
     */
    virtual bool close();

    /**
     * Active le mode de rendu special lors du deplacement de
     * la camera.
     *
     * @see _wireframeOnMovingCamera
     */
    void setWireframeOnMovingCamera(bool state) { _wireframeOnMovingCamera = state; }

    /**
     * Passe en mode de rendu special lors du deplacement de
     * la camera.
     *
     * @see _wireframeOnMovingCamera
     */
    void startMovingRenderMode();
    /**
     * Repasse en mode de rendu normal apres un deplacement de
     * la camera.
     *
     * @see _wireframeOnMovingCamera
     */
    void stopMovingRenderMode();

    /**
     * Mets l'echelle a jour.
     */
    void updateScale();

    /**
     * Affiche ou pas l'echelle.
     */
    void showScale(bool show);


signals:
    /**
     * Indique que le type de vue a change.
     */
    void viewTypeChanged(int);

    /**
     * Indique que le mode d'edition a change.
     */
    void editorModeChanged(int mode);

    /**
     * Indique que l'element a ete modifie.
     */
    void eltModified(LPTYElement pElt);

    /**
     * Indique que le curseur de souris entre dans la widget.
     */
    void mouseEnter();

    /**
     * Indique que le curseur de souris sort dans la widget.
     */
    void mouseLeave();

    /**
     * Indique que la frame a ete maximisee.
     * Attention: un slot nomme isMaximized existe deja
     */
    void frameResized();

protected:
    virtual void keyPressEvent(QKeyEvent* pEvent);
    virtual void keyReleaseEvent(QKeyEvent* pEvent);
    virtual void mouseMoveEvent(QMouseEvent* pEvent);
    virtual void wheelEvent(QWheelEvent* pEvent);
    virtual void resizeEvent(QResizeEvent* pEvent);
    virtual void focusInEvent(QFocusEvent* pEvent);
    virtual void showEvent(QShowEvent* pEvent);
    virtual void closeEvent(QCloseEvent* pEvent);
    virtual void enterEvent(QEvent* pEvent);
    virtual void leaveEvent(QEvent* pEvent);

    /**
     * Definit la visibilite des actors constituant les grilles.
     */
    void setGridLinesActorsVisibility(bool showGridXY, bool showGridXZ, bool showGridZY);

    OBox getGlobalBoundingBox();

private:
    ///Le layout de cette frame.
    QGridLayout* _pLayout;

    ///Combo box pour le mode de rendu.
    QComboBox* _pRenderModeBox;

    ///Bouton d'activation de la grille magnetique.
    QToolButton* _pSnapGridBtn;

    ///Indique si on active ou non la grille magnetique.
    bool _snapGridActive;

    ///Bouton pour effectuer une capture d'ecran.
    QToolButton* _pScreenShotBtn;

    ///Bouton pour l'affichage des sources ponctuelles.
    QToolButton* _pShowSourcesBtn;

    ///Indique si on affiche ou pas les sources ponctuelles.
    bool _showSources;

    ///Bouton pour l'affichage des normals.
    QToolButton* _pShowNormalsBtn;

    ///Indique si on affiche ou masque les normals.
    bool _showNormals;

    ///Bouton pour l'affichage des normals.
    QToolButton* _pShowPlafondBtn;

    ///Indique si on affiche ou masque les normals.
    bool _showPlafond;

    ///Bouton pour l'affichage des rayons calcules par le lance de rayons.
    QToolButton* _pShowRaysBtn;

    ///Indique si on affiche ou pas les rayons.
    bool _showRays;

    ///Barre d'etat.
    QStatusBar* _pStatusBar;

    ///Le mode de rendu courant.
    int _curRenderMode;

    ///Indique si on passe en rendu wireframe lors de deplacement de camera.
    bool _wireframeOnMovingCamera;

    ///Le dernier mode de rendu courant.
    int _lastRenderMode;

    ///Pour conserver le dernier mode d'edition.
    int _lastEditorMode;

    ///Pour le picking.
    TYElementPicker* _pPicker;

    ///Gere la camera.
    TYCameraEditor* _pCameraEditor;

    ///Outils pour mesurer.
    TYDistanceEditor* _pDistanceEditor;

    ///Editor pour deplacer les elements.
    TYPositionEditor* _pPositionEditor;

    ///Editor pour definir une zone de zoom pour la camera.
    TYCameraZoneEditor* _pCameraZoneEditor;

    ///Editor pour deplacer la camera par step.
    TYCameraStepEditor* _pCameraStepEditor;

    ///Label 2D pour afficher le type de la vue.
    OGLTextElement* _pOGLTextElement;

    ///La geometrie des Axes.
    OGLLineElement* _pOGLLineElementAxeX;
    OGLLineElement* _pOGLLineElementAxeY;
    OGLLineElement* _pOGLLineElementAxeZ;
    OGLTextElement* _pOGLTextElementLabelX;
    OGLTextElement* _pOGLTextElementLabelY;
    OGLTextElement* _pOGLTextElementLabelZ;

    ///Bouton d'activation de la grille.
    QToolButton* _pGridBtn;

    ///Axes X et Y de la grille
    OGLLineElement* _pOGLLineElementX;
    OGLLineElement* _pOGLLineElementY;


    ///Indique si on affiche ou non la grille.
    bool _showGrid;

    ///Pour la gestion de l'historique.
    TYActionManager _actionManager;

    ///La lumiere par default.
    OGLLightElement* _pLightElement;

    //Les collections de lignes pour la grille.
    OGLGridElement* _pOGLGridElement;

    ///Objet graphique pour la representation de l'echelle.
    OGLScalarBarElement* _pOGLScalarBarElement;

    ///Bouton pour l'affichage de l'echelle.
    QToolButton* _pShowScale;

    ///Indique si on affiche ou pas l'echelle.
    bool _showScale;

    bool _firstTimeShown;

protected:
    ///Le layout ou se trouvent les boutons, etc.
    QBoxLayout* _pCtrlLayout;

    ///Combo box pour la selection de la camera courante.
    QComboBox* _pViewTypeBox;

    ///La fenetre graphique.
    TYRenderWindowInteractor* _pView;

    ///Le type de la vue courante.
    int _curViewType;

    ///Indique si le mode d'edition a ete traite.
    bool _editorModeAccepted;

    ///L'editor courant.
    TYAbstractSceneEditor* _pCurrentEditor;

    ///Gere le menu contextuel.
    TYPickEditor* _pPickEditor;

    ///Les cameras pour chaque type de vue.
    OGLCamera* _pOGLCameras[NbOfViews];

    ///Pas de la grille.
    float _gridStep;
    ///Pas de la grille magnetique.
    float _gridMagnStep;
    ///Dimension de la grille en X.
    float _gridDimX;
    ///Dimension de la grille en Y.
    float _gridDimY;

    ///Bouton de specification des coordonnees de la camera.
    QToolButton* _pSetCameraCoordinatesBtn;

    ///Un pointeur sur l'element a editer.
    LPTYElement _pElement;
};

#endif //__TY_MODELER_FRAME__
