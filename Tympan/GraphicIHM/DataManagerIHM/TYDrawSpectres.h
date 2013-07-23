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
 * \file TYDrawSpectres.h
 * \brief outil IHM pour l'affichage des spectres (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_DRAW_SPECTRES_FRAME__
#define __TY_DRAW_SPECTRES_FRAME__


#include <qwidget.h>
#include <qscrollarea.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QMouseEvent>
#include <QPaintEvent>
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSpectre.h"

class QPainter;
class QPrinter;
class QGridLayout;
class QPushButton;

#define WIDTH 400
#define HEIGHT 600

/**
 * \class TYScrollView
 * \brief objet ScrollView pour Tympan (utile au DrawSpectre)
 */
class TYScrollView : public QScrollArea
{
    Q_OBJECT

public:
    /**
     * Constructeur par defaut.
     */
    TYScrollView(QWidget* parent = 0, const char* name = 0, Qt::WFlags f = 0);
    /**
     * Destructeur.
     */
    virtual ~TYScrollView();

    /**
     * Set/Get du zoom.
     */
    double getZoom() { return _zoom;}
    /**
     * Set/Get du zoom.
     */
    void setZoom(double val) { _zoom = val;}

signals:
    /**
     * Signal entrainant la mise a jour de la fenetre graphique.
     */
    void updateGraph();

protected:
    /**
     * Surcharge des fonctions de gestion de la souris.
     */
    virtual void contentsMousePressEvent(QMouseEvent*);
    /**
     * Surcharge des fonctions de gestion de la souris.
     */
    virtual void contentsMouseReleaseEvent(QMouseEvent*);
    /**
     * Surcharge des fonctions de gestion de la souris.
     */
    virtual void contentsMouseMoveEvent(QMouseEvent*);

    /**
     * Gere la mise a jour de la fenetre graphique.
     */
    void paintEvent(QPaintEvent* e = NULL);

protected:
    ///Position en Y precedente de la souris.
    int _posY;
    ///Facteur de zoom.
    double _zoom;
    ///Indique si le bouton droit de la souris est enfonce.
    bool _isRight;

};


/**
 * \class TYDrawSpectres
 * \brief classe de l'objet IHM pour l'affichage des spectres
 */
class TYDrawSpectres : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructeur par defaut.
     */
    TYDrawSpectres(QWidget* parent = 0, const char* name = 0, Qt::WFlags f = 0);
    /**
     * Destructeur.
     */
    virtual ~TYDrawSpectres();

    /**
     * Set/Get de la liste de spectres.
     */
    TYTabLPSpectre getListspectre() { return _listSpectre;}
    /**
     * Set/Get de la liste de spectres.
     */
    void setListSpectre(TYTabLPSpectre list) {_listSpectre = list;}

    /**
     * Test si un spectre est present dans la liste.
     */
    int isInList(LPTYSpectre pSpectre);

public slots:
    /**
     * Dessine les spectres.
     */
    void draw(QPainter* paint = NULL, int widht = 0, int height = 0);

    /**
     * Gere l'impression.
     */
    void print();

    /**
     * Ajout/Suppression d'un element de la liste.
     *
     * @param pSpectre Spectre a ajouter ou a supprimer s'il est deja dans la liste.
     * @param rem True pour supprimer uniquement.
     */
    int addOrRemoveSpectre(LPTYSpectre pSpectre, bool rem = false);

    /**
     * Change le mode d'affichage.
     *
     * @param mode 1 pour le mode fit, 0 pour le mode zoom.
     */
    void changeMode(bool mode);

protected:
    ///Liste des spectres.
    TYTabLPSpectre _listSpectre;

    ///Outil d'impression
    QPrinter* _printer;

    QGridLayout* _gridLayout;
    QPushButton* _pushButtonClose;
    QPushButton* _pushButtonPrint;
    TYScrollView* _scrollView;
    QWidget* _graphic;
    QPainter* _painter;

    ///Mode d'affichage.
    bool _isFitMode;
};

#endif //__TY_DRAW_SPECTRES_FRAME__
