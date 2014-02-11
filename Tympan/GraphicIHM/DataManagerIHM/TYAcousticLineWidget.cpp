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
 * \file TYAcousticLineWidget.cpp
 * \brief Outil IHM pour une ligne acoustique
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticLine.h"
#include <qcombobox.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>


#define TR(id) OLocalizator::getString("TYAcousticLineWidget", (id))


TYAcousticLineWidget::TYAcousticLineWidget(TYAcousticLine* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{

    _elmW = new TYSourceWidget(pElement, this);

    resize(300, 480);
    setWindowTitle(TR("id_caption"));
    _acousticLineLayout = new QGridLayout();
    setLayout(_acousticLineLayout);

    _acousticLineLayout->addWidget(_elmW, 0, 0);

    // Saisie de la densite de sources
    _groupBoxDensite = new QGroupBox(this);
    _groupBoxDensite->setTitle(TR("id_densitessrcs_label"));
    _groupBoxDensiteLayout = new QGridLayout();
    _groupBoxDensite->setLayout(_groupBoxDensiteLayout);

    _lineEditDensiteSrcs = new QLineEdit(_groupBoxDensite);
    _groupBoxDensiteLayout->addWidget(_lineEditDensiteSrcs, 0, 0);
    QLabel* pUnitDensite = new QLabel(_groupBoxDensite);
    pUnitDensite->setText(TR("id_unite_densite_srcs"));
    _groupBoxDensiteLayout->addWidget(pUnitDensite, 0, 1);

    _acousticLineLayout->addWidget(_groupBoxDensite, 1, 0);


    // Gestion de la distribution des sources
    _buttonGroupTypeDistrib = new QButtonGroup();
    _buttonGroupTypeDistrib->setExclusive(true);
    _pRadioButtonCalculee = new QRadioButton(TR("id_puissance_calculee"));
    _buttonGroupTypeDistrib->addButton(_pRadioButtonCalculee, 0);
    _pRadioButtonImposee = new QRadioButton(TR("id_puissance_imposee"));
    _buttonGroupTypeDistrib->addButton(_pRadioButtonImposee);

    QGridLayout* groupBoxTypeDistribLayout = new QGridLayout();
    groupBoxTypeDistribLayout->addWidget(_pRadioButtonCalculee, 0, 0);
    groupBoxTypeDistribLayout->addWidget(_pRadioButtonImposee, 0, 1);

    QGroupBox* groupBoxTypeDistrib = new QGroupBox();
    groupBoxTypeDistrib->setTitle(TR("id_type_distribution"));
    groupBoxTypeDistrib->setLayout(groupBoxTypeDistribLayout);


    _acousticLineLayout->addWidget(groupBoxTypeDistrib, 2, 0, 1, 1);

    // Definition des proprietes acoustiques
    QGroupBox* pgroupBoxPropriete = new QGroupBox(this);
    QGridLayout* pgroupBoxProprieteLayout = new QGridLayout();
    pgroupBoxPropriete->setLayout(pgroupBoxProprieteLayout);

    _labeUseAtt = new QLabel(pgroupBoxPropriete);
    _labeUseAtt->setText(TR("id_useatt_label"));
    _checkBoxUseAtt = new QCheckBox(pgroupBoxPropriete);
    _checkBoxUseAtt->setText(TR(""));

    _labelIsRayonnant = new QLabel(pgroupBoxPropriete);
    _labelIsRayonnant->setText(TR("id_israyonnant_label"));
    _checkBoxIsRayonnant = new QCheckBox(pgroupBoxPropriete);
    _checkBoxIsRayonnant->setText(TR(""));

    _checkBoxIsRayonnant->setEnabled(false); //L'etat du calcul pilote le rayonnement ou non d'une source

    pgroupBoxProprieteLayout->addWidget(_labeUseAtt, 0, 0);
    pgroupBoxProprieteLayout->addWidget(_checkBoxUseAtt, 0, 1);
    pgroupBoxProprieteLayout->addWidget(_labelIsRayonnant, 0, 2);
    pgroupBoxProprieteLayout->addWidget(_checkBoxIsRayonnant, 0, 4);

    _acousticLineLayout->addWidget(pgroupBoxPropriete, 3, 0);

    // Gestion de l'attenuateur
    _groupBoxAtt = new QGroupBox(this);
    _groupBoxAtt->setTitle(TR("id_att"));
    _groupBoxAttLayout = new QGridLayout();
    _groupBoxAtt->setLayout(_groupBoxAttLayout);

    _lineEditNomAtt = new QLineEdit(_groupBoxAtt);
    _lineEditNomAtt->setEnabled(FALSE);
    _groupBoxAttLayout->addWidget(_lineEditNomAtt, 0, 0);

    _pushButtonSpectreAtt = new QPushButton(_groupBoxAtt);
    _pushButtonSpectreAtt->setText(TR("id_proprietes_button"));
    _groupBoxAttLayout->addWidget(_pushButtonSpectreAtt, 0, 1);

    _acousticLineLayout->addWidget(_groupBoxAtt, 4, 0);

    // Gestion du choix du regime
    QGroupBox* buttonGroupBox = new QGroupBox(this);
    buttonGroupBox->setTitle(TR("id_select_regime"));
    QGridLayout* buttonGroupBoxLayout = new QGridLayout();
    buttonGroupBox->setLayout(buttonGroupBoxLayout);

    _pushButtonRemRegime = new QPushButton(buttonGroupBox);
    _pushButtonRemRegime->setText(TR("id_supprimer"));
    buttonGroupBoxLayout->addWidget(_pushButtonRemRegime, 0, 0);

    _comboBoxSelectRegime = new QComboBox(buttonGroupBox);
    _comboBoxSelectRegime->setEditable(true);
    buttonGroupBoxLayout->addWidget(_comboBoxSelectRegime, 0, 1);

    _pushButtonNewRegime = new QPushButton(buttonGroupBox);
    _pushButtonNewRegime->setText(TR("id_nouveau"));
    buttonGroupBoxLayout->addWidget(_pushButtonNewRegime, 0, 2);

    _acousticLineLayout->addWidget(buttonGroupBox, 5, 0);

    _groupBoxLine = new QGroupBox(this);
    _groupBoxLine->setTitle(TR(""));
    _groupBoxLineLayout = new QGridLayout();
    _groupBoxLine->setLayout(_groupBoxLineLayout);

    _labelLargeur = new QLabel(_groupBoxLine);
    _labelLargeur->setText(TR("id_largeur_label"));
    _groupBoxLineLayout->addWidget(_labelLargeur, 0, 0);

    _lineEditLargeur = new QLineEdit(_groupBoxLine);
    _groupBoxLineLayout->addWidget(_lineEditLargeur, 0, 1);
    QLabel* pUnitLargeur = new QLabel(_groupBoxLine);
    pUnitLargeur->setText(TR("id_unite_largeur"));
    _groupBoxLineLayout->addWidget(pUnitLargeur, 0, 2);

    _acousticLineLayout->addWidget(_groupBoxLine, 6, 0);

    updateContent();

    connect(_pushButtonNewRegime, SIGNAL(clicked()), this, SLOT(createNewRegime()));
    connect(_pushButtonRemRegime, SIGNAL(clicked()), this, SLOT(deleteRegime()));
    connect(_comboBoxSelectRegime, SIGNAL(activated(int)), this, SLOT(changeRegime(int)));
    //  connect(_checkBoxIsRayonnant,  SIGNAL(clicked()), this, SLOT(updateRayonnant()));
    connect(_checkBoxUseAtt, SIGNAL(clicked()), this, SLOT(useAttenuateur()));
    connect(_pushButtonSpectreAtt, SIGNAL(clicked()), this, SLOT(editAtt()));
    connect(_pRadioButtonImposee, SIGNAL(clicked()), this, SLOT(showSpectre()));
    connect(_pRadioButtonCalculee, SIGNAL(clicked()), this, SLOT(setSpectreToReadOnly()));
}

TYAcousticLineWidget::~TYAcousticLineWidget()
{
}

void TYAcousticLineWidget::updateContent()
{
    _elmW->updateContent();

    updateFromCurrentRegime();

    updateComboRegime();

    _lineEditLargeur->setText(QString().setNum(getElement()->getLargeur(), 'f', 2));
    _lineEditDensiteSrcs->setText(QString().setNum(getElement()->getSrcLineic()->getDensiteSrcs(), 'f', 2));

    // Inactivation de la possibilite de changer de regime
    _pushButtonRemRegime->setEnabled(getElement()->getIsRegimeChangeAble());
    _comboBoxSelectRegime->setEnabled(true);
    _pushButtonNewRegime->setEnabled(getElement()->getIsRegimeChangeAble());
}

void TYAcousticLineWidget::apply()
{
    _elmW->apply();
    getElement()->setLargeur(_lineEditLargeur->text().toDouble());
    getElement()->getSrcLineic()->setDensiteSrcs(_lineEditDensiteSrcs->text().toDouble());
    //  getElement()->setIsRayonnant( _checkBoxIsRayonnant->isChecked() );
    getElement()->setUseAtt(_checkBoxUseAtt->isChecked());

    if (_pRadioButtonCalculee->isChecked())
    {
        getElement()->setTypeDistribution(TYAcousticInterface::TY_PUISSANCE_CALCULEE);
    }
    else
    {
        getElement()->setTypeDistribution(TYAcousticInterface::TY_PUISSANCE_IMPOSEE);
    }

    getElement()->updateCurrentRegime();

    _pElement->setIsAcousticModified(true);

    emit modified();
}

void TYAcousticLineWidget::editSrcLin()
{
    int ret = getElement()->getSrcLineic()->edit(this);

    if (ret == QDialog::Accepted)
    {
        _lineEditNomSrcLin->setText(getElement()->getSrcLineic()->getName());
    }
}

void TYAcousticLineWidget::createNewRegime()
{
    // Creation du nouveau regime
    getElement()->addRegime(getElement()->buildRegime());

    getElement()->setCurRegime(-1); // On bascule sur le regime nouvellement cree

    // Mise a jour du combo des regimes
    updateComboRegime();
}

void TYAcousticLineWidget::deleteRegime()
{
    getElement()->remRegime(_comboBoxSelectRegime->currentIndex());

    // Mise a jour du combo box
    updateComboRegime();
}

void TYAcousticLineWidget::changeRegime(int regime)
{
    saveCurrentRegime(); // Save current before changing

    // Quand on change le nom d'un regime, le systeme considere qu'il s'agit
    // d'un nouveau regime et le numero retourne est egal au nombre d'entree
    // dans la comboBox + 1. Identifier le bon regime pour qu'il n'ay ai pas
    // de probleme.
    size_t nbRegime = getElement()->getNbRegimes();
    if (regime >= nbRegime) { regime = getElement()->getCurRegime(); }

    QLineEdit* LE = _comboBoxSelectRegime->lineEdit();
    QString nomRegime = LE->text();

    getElement()->setCurRegime(regime);
    getElement()->setRegimeName(nomRegime);

    updateFromCurrentRegime();
}

void TYAcousticLineWidget::saveCurrentRegime()
{
    bool status = true;
    TYRegime& regime = getElement()->getRegimeNb(getElement()->getCurRegime());

    regime._isRayonnant = _checkBoxIsRayonnant->isChecked();
    regime._useAtt = _checkBoxUseAtt->isChecked();

    if (_pRadioButtonCalculee->isChecked())
    {
        getElement()->setTypeDistribution(TYAcousticInterface::TY_PUISSANCE_CALCULEE);
    }
    else
    {
        getElement()->setTypeDistribution(TYAcousticInterface::TY_PUISSANCE_IMPOSEE);
    }

    regime._spectre = *(getElement()->getCurrentSpectre());

    if (regime._useAtt)
    {
        regime._pAtt = getElement()->getAtt();
    }
}

void TYAcousticLineWidget::renameRegime(const QString& nom)
{
    _comboBoxSelectRegime->setEditText(nom);
    getElement()->getRegimeNb(_comboBoxSelectRegime->currentIndex()).setName(nom);
}

void TYAcousticLineWidget::updateComboRegime()
{
    _comboBoxSelectRegime->clear();

    // Remplissage du comboBox des regimes
    QString nom;
    for (short i = 0 ; i < getElement()->getNbRegimes(); i++)
    {
        nom = getElement()->getRegimeNb(i).getName();
        _comboBoxSelectRegime->insertItem(i, nom);
    }

    // On affiche le regime courant
    _comboBoxSelectRegime->setCurrentIndex(getElement()->getCurRegime());
}


void TYAcousticLineWidget::updateFromCurrentRegime()
{
    _checkBoxUseAtt->setChecked(getElement()->getUseAtt());   // Attenuateur ou pas
    _checkBoxIsRayonnant->setChecked(getElement()->getIsRayonnant());
    _groupBoxAtt->setEnabled(getElement()->getUseAtt());
    if (getElement()->getUseAtt() && getElement()->getAtt())
    {
        _lineEditNomAtt->setText(getElement()->getAtt()->getName());
    }
    else
    {
        _lineEditNomAtt->setText("");
    }

    getElement()->getSpectre()->setIsReadOnly(false);
    if (getElement()->getTypeDistribution() == TYAcousticInterface::TY_PUISSANCE_CALCULEE)
    {
        _pRadioButtonCalculee->setChecked(true);
        getElement()->getSpectre()->setIsReadOnly(true);
    }
    else
    {
        _pRadioButtonImposee->setChecked(true);
        getElement()->getSpectre()->setIsReadOnly(false);
    }
}

void TYAcousticLineWidget::showSpectre()
{
    LPTYSpectre spectre = getElement()->getSpectre();

    if (_pRadioButtonCalculee->isChecked())
    {
        spectre->setIsReadOnly(true);
    }

    spectre->setIsReadOnly(false);

    spectre->edit(this);

}

void TYAcousticLineWidget::setSpectreToReadOnly()
{
    if (_pRadioButtonCalculee->isChecked())
    {
        getElement()->getSpectre()->setIsReadOnly(true);
    }
}

void TYAcousticLineWidget::editAtt()
{
    if (getElement()->getAtt() == NULL) // Si l'attenuateur n'existe pas, on le cree
    {
        LPTYAttenuateur pAtt = new TYAttenuateur();
        getElement()->setAtt(pAtt);
    }

    int ret = getElement()->getAtt()->edit(this);

    if (ret == QDialog::Accepted)
    {
        _lineEditNomAtt->setText(getElement()->getAtt()->getName());
    }
}


void TYAcousticLineWidget::useAttenuateur()
{
    _groupBoxAtt->setEnabled(_checkBoxUseAtt->isChecked());
}
