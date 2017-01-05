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
 * \file YPrintDialog.cpp
 * \brief Boite de dialogue des parametres d'impression
 */


#include <qvariant.h>
#include <qcheckbox.h>
#include <qtooltip.h>
#include <QGroupBox>
#include <QPushButton>
#include <QLineEdit>
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

#include "Tympan/models/business/OLocalizator.h"
#include "TYPrintDialog.h"

#define TR(id) OLocalizator::getString("TYPrintDialog", (id))


TYPrintDialog::TYPrintDialog(QWidget* parent,  const char* name, bool modal, Qt::WFlags fl)
    : QDialog(parent, fl)
{
    setModal(modal);
    if (!name)
    {
        setObjectName("_printDialog");
    }
    resize(371, 210);
    setWindowTitle(TR("id_printer"));

    QGridLayout* layout = new QGridLayout(this);
    layout->setMargin(7);
    layout->setSpacing(10);
    setLayout(layout);

    _groupBox = new QGroupBox(this);
    _groupBoxLayout = new QGridLayout();
    _groupBox->setLayout(_groupBoxLayout);

    _groupBoxProjet = new QGroupBox(_groupBox);
    _groupBoxProjet->setTitle(TR("id_projet"));
    _groupBoxProjetLayout = new QGridLayout();
    _groupBoxProjet->setLayout(_groupBoxProjetLayout);

    _checkBoxNomProjet = new QCheckBox(_groupBoxProjet);
    _checkBoxNomProjet->setText(TR("id_nom"));
    _groupBoxProjetLayout->addWidget(_checkBoxNomProjet, 0, 0);

    _checkBoxAuteurProjet = new QCheckBox(_groupBoxProjet);
    _checkBoxAuteurProjet->setText(TR("id_auteur"));
    _groupBoxProjetLayout->addWidget(_checkBoxAuteurProjet, 0, 1);

    _checkBoxDateProjet = new QCheckBox(_groupBoxProjet);
    _checkBoxDateProjet->setText(TR("id_date"));
    _groupBoxProjetLayout->addWidget(_checkBoxDateProjet, 1, 0);

    _checkBoxCommentProjet = new QCheckBox(_groupBoxProjet);
    _checkBoxCommentProjet->setText(TR("id_comment"));
    _groupBoxProjetLayout->addWidget(_checkBoxCommentProjet, 1, 1);

    _groupBoxLayout->addWidget(_groupBoxProjet, 0, 0, 1, 2);

    _groupBoxCalcul = new QGroupBox(_groupBox);
    _groupBoxCalcul->setTitle(TR("id_calcul"));
    _groupBoxCalculLayout = new QGridLayout();
    _groupBoxCalcul->setLayout(_groupBoxCalculLayout);

    _checkBoxNomCalcul = new QCheckBox(_groupBoxCalcul);
    _checkBoxNomCalcul->setText(TR("id_nom"));
    _groupBoxCalculLayout->addWidget(_checkBoxNomCalcul, 0, 0);

    _checkBoxDateCalcul = new QCheckBox(_groupBoxCalcul);
    _checkBoxDateCalcul->setText(TR("id_date"));
    _groupBoxCalculLayout->addWidget(_checkBoxDateCalcul, 1, 0);

    _checkBoxCommentCalcul = new QCheckBox(_groupBoxCalcul);
    _checkBoxCommentCalcul->setText(TR("id_comment"));
    _groupBoxCalculLayout->addWidget(_checkBoxCommentCalcul, 1, 1);

    _groupBoxLayout->addWidget(_groupBoxCalcul, 0, 2);

    _groupBoxMachine = new QGroupBox(_groupBox);
    _groupBoxMachine->setTitle(TR("id_machine"));
    _groupBoxMachineLayout = new QGridLayout();
    _groupBoxMachine->setLayout(_groupBoxMachineLayout);

    _checkBoxNomMachine = new QCheckBox(_groupBoxMachine);
    _checkBoxNomMachine->setText(TR("id_nom"));
    _groupBoxMachineLayout->addWidget(_checkBoxNomMachine, 0, 0);

    _checkBoxConstrMachine = new QCheckBox(_groupBoxMachine);
    _checkBoxConstrMachine->setText(TR("id_constr"));
    _groupBoxMachineLayout->addWidget(_checkBoxConstrMachine, 0, 1);

    _checkBoxModelMachine = new QCheckBox(_groupBoxMachine);
    _checkBoxModelMachine->setText(TR("id_model"));
    _groupBoxMachineLayout->addWidget(_checkBoxModelMachine, 1, 0);

    _checkBoxCatMachine = new QCheckBox(_groupBoxMachine);
    _checkBoxCatMachine->setText(TR("id_categorie"));
    _groupBoxMachineLayout->addWidget(_checkBoxCatMachine, 1, 1);

    _checkBoxCommentMachine = new QCheckBox(_groupBoxMachine);
    _checkBoxCommentMachine->setText(TR("id_comment"));
    _groupBoxMachineLayout->addWidget(_checkBoxCommentMachine, 2, 0);

    _groupBoxLayout->addWidget(_groupBoxMachine, 1, 0, 1, 3);
    _groupBoxMachine->hide();

    _groupBoxBatiment = new QGroupBox(_groupBox);
    _groupBoxBatiment->setTitle(TR("id_batiment"));
    _groupBoxBatimentLayout = new QGridLayout();
    _groupBoxBatiment->setLayout(_groupBoxBatimentLayout);

    _checkBoxNomBatiment = new QCheckBox(_groupBoxBatiment);
    _checkBoxNomBatiment->setText(TR("id_nom"));
    _groupBoxBatimentLayout->addWidget(_checkBoxNomBatiment, 0, 0);

    _groupBoxLayout->addWidget(_groupBoxBatiment, 1, 0, 1, 3);
    _groupBoxBatiment->hide();

    _groupBoxSite = new QGroupBox(_groupBox);
    _groupBoxSite->setTitle(TR("id_site"));
    _groupBoxSiteLayout = new QGridLayout();
    _groupBoxSite->setLayout(_groupBoxSiteLayout);

    _checkBoxNomSite = new QCheckBox(_groupBoxSite);
    _checkBoxNomSite->setText(TR("id_nom"));
    _groupBoxSiteLayout->addWidget(_checkBoxNomSite, 0, 0);

    _checkBoxDimSite = new QCheckBox(_groupBoxSite);
    _checkBoxDimSite->setText(TR("id_dim"));
    _groupBoxSiteLayout->addWidget(_checkBoxDimSite, 0, 1);

    _groupBoxLayout->addWidget(_groupBoxSite, 1, 0, 1, 3);
    _groupBoxSite->hide();

    _labelPied = new QLabel(_groupBox);
    _labelPied->setText(TR("id_pied_page"));

    _groupBoxLayout->addWidget(_labelPied, 3, 0);

    _labelTete = new QLabel(_groupBox);
    _labelTete->setText(TR("id_en_tete"));

    _groupBoxLayout->addWidget(_labelTete, 2, 0);

    _lineEditTete = new QLineEdit(_groupBox);

    _groupBoxLayout->addWidget(_lineEditTete, 2, 1, 1, 2);

    _lineEditPied = new QLineEdit(_groupBox);
    //_lineEditPied->setFrameShape( QLineEdit::LineEditPanel );
    //_lineEditPied->setFrameShadow( QLineEdit::Sunken );

    _groupBoxLayout->addWidget(_lineEditPied, 3, 1, 1, 2);

    _labelTitre = new QLabel(_groupBox);
    _labelTitre->setText(TR("id_titre"));

    _groupBoxLayout->addWidget(_labelTitre, 4, 0);

    _lineEditTitre = new QLineEdit(_groupBox);
    //    _lineEditTitre->setFrameShape( QLineEdit::LineEditPanel );
    //  _lineEditTitre->setFrameShadow( QLineEdit::Sunken );

    _groupBoxLayout->addWidget(_lineEditTitre, 4, 1, 1, 2);

    _pushButtonPrint = new QPushButton(this);
    _pushButtonPrint->setGeometry(QRect(210, 170, 71, 31));
    _pushButtonPrint->setText(TR("id_print"));

    _pushButtonCancel = new QPushButton(this);
    _pushButtonCancel->setGeometry(QRect(290, 170, 71, 31));
    _pushButtonCancel->setText(TR("id_cancel"));

    layout->addWidget(_groupBox, 0, 0, 1, 4);
    layout->addWidget(_pushButtonPrint, 1, 2);
    layout->addWidget(_pushButtonCancel, 1, 3);

    connect(_pushButtonCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(_pushButtonPrint, SIGNAL(clicked()), this, SLOT(accept()));
}

/*
 *  Destroys the object and frees any allocated resources
 */
TYPrintDialog::~TYPrintDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

