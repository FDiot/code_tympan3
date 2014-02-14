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
 * \file YEtatsWidget.h
 * \brief objet IHM pour les etats (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_ETATS_WIDGET__
#define __TY_ETATS_WIDGET__


#include "TYWidget.h"
#include <qdialog.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

#include <vector>
using std::vector;
typedef vector<TYElement*> tabPtrElement;

class TYCalcul;
class QLineEdit;
class QGridLayout;
class QLabel;
class QGroupBox;
class QPushButton;
class QCheckBox;
class QPoint;
class QTextEdit;
class QDateEdit;
class TYElementWidget;
class QTableWidget;
class QTabWidget;
class QRadioButton;

/**
 * \class TYEtatsWidget
 * \brief classe de l'outil IHM pour les Etats
 */
class TYEtatsWidget : public QDialog
{
    Q_OBJECT

public:
    /**
     * Constructeur.
     */
    TYEtatsWidget(TYCalcul* pElement, QWidget* _pParent /*=NULL*/);

    /**
     * Destructeur.
     */
    virtual ~TYEtatsWidget();

public slots:
    virtual void updateContent();
    virtual void apply();

    /**
     * Affiche un menu contextuel.
     */
    virtual void contextMenuEvent(QContextMenuEvent* e);

signals:
    /**
     * Ce signal est emis lorsque les modification (slot apply())
     * ont ete effectuees.
     */
    void modified();

private:
    TYCalcul* _pElement;
    TYElementWidget* _elmW;

    QTabWidget* _tabWidget;

    QTableWidget* _tableSource;
    tabPtrElement _tabPtrSourcePonctuelle;// Permet de memoriser l'element associe a la ligne dans le tableau

    QTableWidget* _tableMachine;
    tabPtrElement _tabPtrMachine;// Permet de memoriser l'element associe a la ligne dans le tableau

    QTableWidget* _tableBatiment;
    tabPtrElement _tabPtrBatiment; // Permet de memoriser l'element associe a la ligne dans le tableau

    QTableWidget* _tableSourceLin;
    tabPtrElement _tabPtrSourceLin;// Permet de memoriser l'element associe a la ligne dans le tableau
};

#endif // __TY_ETATS_WIDGET__
