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
 * \file YPrintDialog.h
 * \brief Boite de dialogue des parametres d'impression (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef _TY_PRINT_DIALOG_H
#define _TY_PRINT_DIALOG_H

#include <qvariant.h>
#include <qdialog.h>
//Added by qt3to4:
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QCheckBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;

/**
 * \class TYPrintDialog
 * \brief classe pour une boite de dialogue des parametres d'impression.
 */

class TYPrintDialog : public QDialog
{
    Q_OBJECT

public:
    TYPrintDialog(QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0);
    ~TYPrintDialog();

    QPushButton* _pushButtonCancel;
    QGroupBox* _groupBox;
    QGroupBox* _groupBoxProjet;
    QCheckBox* _checkBoxCommentProjet;
    QCheckBox* _checkBoxAuteurProjet;
    QCheckBox* _checkBoxDateProjet;
    QCheckBox* _checkBoxNomProjet;
    QGroupBox* _groupBoxCalcul;
    QCheckBox* _checkBoxCommentCalcul;
    QCheckBox* _checkBoxDateCalcul;
    QCheckBox* _checkBoxNomCalcul;
    QCheckBox* _checkBoxCommentMachine;
    QCheckBox* _checkBoxConstrMachine;
    QCheckBox* _checkBoxNomMachine;
    QCheckBox* _checkBoxCatMachine;
    QCheckBox* _checkBoxModelMachine;
    QCheckBox* _checkBoxNomSite;
    QCheckBox* _checkBoxDimSite;
    QCheckBox* _checkBoxNomBatiment;
    QGroupBox* _groupBoxMachine;
    QGroupBox* _groupBoxBatiment;
    QGroupBox* _groupBoxSite;
    QLabel* _labelPied;
    QLabel* _labelTete;
    QLineEdit* _lineEditPied;
    QLineEdit* _lineEditTete;
    QLabel* _labelTitre;
    QLineEdit* _lineEditTitre;
    QPushButton* _pushButtonPrint;
    QPushButton* _pushButtonSetUp;


protected:
    QGridLayout* _groupBoxLayout;
    QGridLayout* _groupBoxProjetLayout;
    QGridLayout* _groupBoxCalculLayout;
    QGridLayout* _groupBoxMachineLayout;
    QGridLayout* _groupBoxBatimentLayout;
    QGridLayout* _groupBoxSiteLayout;
};

#endif // _TY_PRINT_DIALOG_H
