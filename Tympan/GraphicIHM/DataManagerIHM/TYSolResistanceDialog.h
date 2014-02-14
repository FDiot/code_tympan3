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
 * \file TYSolResistanceDialog.h
 * \brief Outil IHM pour la saisie de la resisitivite du sol (fichier header)
 *
 *
 *
 *
 *
 */

#ifndef __TY_SOL_RESISTANCE__DIALOG_
#define __TY_SOL_RESISTANCE__DIALOG_


#include <qvariant.h>
#include <qdialog.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QFrame;
class QLabel;
class QLineEdit;
class QPushButton;
class QSlider;
class QToolButton;
class QButtonGroup;

/**
 * \class TYSolResistanceDialog
 * \brief Objet IHM pour la saisie de la resisitivite du sol
 */

class TYSolResistanceDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * Constructeur.
     */
    TYSolResistanceDialog(QWidget* parent = 0, double res = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0);
    /**
     * Destructeur.
     */
    ~TYSolResistanceDialog();

    /**
     * Get de la resistivite.
     */
    double getResistivite() {return _res;}

protected slots:

    /**
     * Gere les mouvements du slider.
     */
    void sliderChanged(int value);
    /**
     * Gere les changements de valeur dans la line edit.
     */
    void lineEditChanged(const QString& sValue);
    /**
     * Gere les pressions sur les boutons.
     */
    void buttonValue(int button);

protected :

    QFrame* _frame6;
    QFrame* _line1_2_4;
    QFrame* _line1_2_11;
    QLabel* _textLabel5_8_6;
    QFrame* _line1_2_13;
    QFrame* _line1_2_6;
    QLabel* _textLabel5_8_2;
    QFrame* _line1_2;
    QLabel* _textLabel5_8;
    QFrame* _line1_2_5;
    QFrame* _line1_2_2;
    QFrame* _line1_2_7;
    QFrame* _line1_2_3;
    QLabel* _textLabel5_8_3;
    QLabel* _textLabel5_8_7;
    QLabel* _textLabel5_8_4;
    QLabel* _textLabel5;
    QLabel* _textLabel5_8_11;
    QLabel* _textLabel5_8_5;
    QFrame* _line1;
    QLabel* _textLabel3;
    QLineEdit* _lineEdit1;
    QLabel* _textLabel6;
    QPushButton* _pushButton1;
    QPushButton* _pushButton2;
    QSlider* _slider1;
    QLabel* _textLabel4;
    QLabel* _textLabel4_2;
    QLabel* _textLabel4_3;
    QLabel* _textLabel4_4;
    QLabel* _textLabel4_5;
    QLabel* _textLabel4_6;
    QLabel* _textLabel4_7;
    QLabel* _textLabel4_8;
    QLabel* _textLabel4_9;
    QLabel* _textLabel4_10;
    QLabel* _textLabel4_11;
    QLabel* _textLabel4_12;
    QToolButton* _toolButton1;
    QToolButton* _toolButton1_8_4;
    QToolButton* _toolButton1_8_6;
    QToolButton* _toolButton1_8_5;
    QToolButton* _toolButton1_8_3;
    QToolButton* _toolButton1_8_11;
    QToolButton* _toolButton1_8;
    QToolButton* _toolButton1_8_13;
    QToolButton* _toolButton1_8_2;
    QToolButton* _toolButton1_8_7;
    QLabel* _textLabel5_8_13;
    QButtonGroup* _pButtonGroup;

    QVBoxLayout* _layout1;

    int _mode;

    double _res;
};



#endif // __TY_SOL_RESISTANCE__DIALOG_
