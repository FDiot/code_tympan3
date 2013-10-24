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
 * \file TYAcousticSurfaceWidget.h
 * \brief outil IHM pour une surface acoustique (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_ACOUSTICSURFACE_WIDGET__
#define __TY_ACOUSTICSURFACE_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYAcousticSurface;
class QLineEdit;
class QGridLayout;
class QLabel;
class QGroupBox;
class QPushButton;
class TYAcousticInterfaceWidget;
class TYElementWidget;
class TYColorInterfaceWidget;
class TYSurfaceInterfaceWidget;

/**
 * \class TYAcousticSurfaceWidget
 * \brief Classe de l'objet IHM pour une surface acoustique
 */
class TYAcousticSurfaceWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYAcousticSurface)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYAcousticSurfaceWidget(TYAcousticSurface* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYAcousticSurfaceWidget();

    /**
     * Get du groupbox materiau.
     */
    QGroupBox* getGroupBoxMat() {return _groupBoxMat;}

    /**
     * Get du groupbox source surfacique.
     */
    //  QGroupBox * getGroupBoxSrcSurf() {return _groupBoxSrcSurf;}

public slots:
    virtual void updateContent();
    virtual void apply();

    /**
     * Edite la widget du bounding rect.
     */
    //  void editRect();
    /**
     * Edite la widget de la source surfacique.
     */
    //  void editSrcSurf();
    /**
     * Edite la widget du materiau.
     */
    void editMat();


    // Membres
protected:

    //  QGroupBox* _groupBoxSrcSurf;
    //    QLineEdit* _lineEditNomSrcsurf;
    //    QPushButton* _pushButtonSrcSurf;
    QGroupBox* _groupBoxMat;
    QLineEdit* _lineEditNomMat;
    QPushButton* _pushButtonMat;
    //    QGroupBox* _groupBoxRect;
    //    QLineEdit* _lineEditNomRect;
    //    QPushButton* _pushButtonRect;


protected:
    QGridLayout* _acousticSurfaceLayout;
    //    QGridLayout* _groupBoxSrcSurfLayout;
    QGridLayout* _groupBoxMatLayout;
    //    QGridLayout* _groupBoxRectLayout;

    TYAcousticInterfaceWidget* _interW;
    TYSurfaceInterfaceWidget* _surfaceW;
    TYColorInterfaceWidget* _colorW;
    TYElementWidget* _elmW;
};


#endif // __TY_ACOUSTICSURFACE_WIDGET__
