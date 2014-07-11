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
 * \file TYFaceModelerFrame.h
 * \brief Classe Modeler specialisee pour l'edition des faces (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_FACE_MODELER_FRAME__
#define __TY_FACE_MODELER_FRAME__


#include <qwidget.h>
//Added by qt3to4:
#include <QFocusEvent>
#include "Tympan/models/business/geoacoustic/TYAcousticRectangleNode.h"
#include "TYModelerFrame.h"


/**
 * \class TYFaceModelerFrame
 * \brief Classe Modeler specialisee pour l'edition des faces.
 */
class TYFaceModelerFrame : public TYModelerFrame
{
    Q_OBJECT

public:
    /**
     * Constructeur par defaut.
     */
    TYFaceModelerFrame(LPTYAcousticRectangleNode pFace = 0, QWidget* parent = 0, const char* name = 0, Qt::WFlags f = 0);
    /**
     * Destructeur.
     */
    virtual ~TYFaceModelerFrame();

    /**
     * Set/Get de la face a decouper.
     */
    void setFace(LPTYAcousticRectangleNode pFace);
    /**
     * Set/Get de la face a decouper.
     */
    LPTYAcousticRectangleNode getFace() { return _pFace; }

    ///Les differents modes d'edition d'une face.
    enum FaceModelerMode
    {
        BoucheMode = NbOfModelerMode,
        ChemineeMode,
        FenetreMode,
        NbOfFaceModelerMode
    };


public slots:
    virtual void setEditorMode(int mode);
    virtual void updatePreferences();
    virtual bool close();


protected slots:
    /**
     * Termine l'edition de la face.
     * Si ok est vrai les modifications sont retenues sinon elles
     * sont annulees.
     *
     * @param ok Ok ou Annuler.
     */
    void done(bool ok);

    /**
     * Accepte les modifications faites sur la face.
     */
    void accept();

    /**
     * Rejete les modifications faites sur la face.
     */
    void reject();

    /**
     * Mets a jour les outils en fonction du type de la face.
     */
    void updateToolActivation();


protected:
    void focusInEvent(QFocusEvent* pEvent);


protected:
    ///Un pointeur sur la face a decouper.
    LPTYAcousticRectangleNode _pFace;

    ///Rectangle editor.
    TYRectangleEditor* _pRectangleEditor;

    ///Nombre d'instance de type TYFaceModelerFrame.
    static int _nbInstance;
};

#endif //__TY_FACE_MODELER_FRAME__
