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
 * \file TYAcousticInterfaceWidget.cpp
 * \brief Outil IHM pour une interface acoustique
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticInterface.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSpectre.h"

#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qcombobox.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>


#define TR(id) OLocalizator::getString("TYAcousticInterfaceWidget", (id))
#define IMG(id) OLocalizator::getPicture("TYAcousticInterfaceWidget", (id))

#include "Tympan/GraphicIHM/DataManagerIHM/TYAcousticInterfaceWidget.h"

TYAcousticInterfaceWidget::TYAcousticInterfaceWidget(TYAcousticInterface* pElement, QWidget* _pParent /*=NULL*/):
    QWidget(_pParent), _pElement(pElement)
{
    _bAddRemRegimeOk = false;
    _modified = false;


    resize(300, 375);
    setWindowTitle(TR("id_caption"));
    _acousticInterfaceLayout = new QGridLayout();
    setLayout(_acousticInterfaceLayout);

    _groupBox = new QGroupBox(this);
    _groupBox->setTitle(TR(""));
    _groupBoxLayout = new QGridLayout();
    _groupBox->setLayout(_groupBoxLayout);

    // Gestion des densites de sources

    // Densite Horizontale
    _groupBoxDensiteH = new QGroupBox(_groupBox);
    _groupBoxDensiteH->setTitle(TR("id_densitessrcsH_label"));
    _groupBoxDensiteHLayout = new QGridLayout();
    _groupBoxDensiteH->setLayout(_groupBoxDensiteHLayout);

    _lineEditDensiteSrcsH = new QLineEdit(_groupBoxDensiteH);
    _groupBoxDensiteHLayout->addWidget(_lineEditDensiteSrcsH, 0, 0);
    QLabel* pUnitDensiteH = new QLabel(_groupBoxDensiteH);
    pUnitDensiteH->setText(TR("id_unite_densite_srcs"));
    _groupBoxDensiteHLayout->addWidget(pUnitDensiteH, 0, 1);

    _groupBoxLayout->addWidget(_groupBoxDensiteH, 0, 0);

    // Densite verticale
    _groupBoxDensiteV = new QGroupBox(_groupBox);
    _groupBoxDensiteV->setTitle(TR("id_densitessrcsV_label"));
    _groupBoxDensiteVLayout = new QGridLayout();
    _groupBoxDensiteV->setLayout(_groupBoxDensiteVLayout);

    _lineEditDensiteSrcsV = new QLineEdit(_groupBoxDensiteV);
    _groupBoxDensiteVLayout->addWidget(_lineEditDensiteSrcsV, 0, 0);
    QLabel* pUnitDensiteV = new QLabel(_groupBoxDensiteV);
    pUnitDensiteV->setText(TR("id_unite_densite_srcs"));
    _groupBoxDensiteVLayout->addWidget(pUnitDensiteV, 0, 1);

    _groupBoxLayout->addWidget(_groupBoxDensiteV, 0, 1);

    // Proprietes acoustiques
    QGroupBox* pgroupBoxPropriete = new QGroupBox(_groupBox);
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

    pgroupBoxProprieteLayout->addWidget(_labeUseAtt, 0, 1);
    pgroupBoxProprieteLayout->addWidget(_checkBoxUseAtt, 0, 2);
    pgroupBoxProprieteLayout->addWidget(_labelIsRayonnant, 0, 3);
    pgroupBoxProprieteLayout->addWidget(_checkBoxIsRayonnant, 0, 4);

    _groupBoxLayout->addWidget(pgroupBoxPropriete, 1, 0);

    // Creation du bouton d'affichage du spectre
    _groupBoxSpectre = new QGroupBox(_groupBox);
    _groupBoxSpectreLayout = new QGridLayout();
    _groupBoxSpectre->setLayout(_groupBoxSpectreLayout);

    _pushButtonShowSpectre = new QPushButton(_groupBoxSpectre);
    _pushButtonShowSpectre->setText(TR("id_button_spectre"));

    _groupBoxSpectreLayout->addWidget(_pushButtonShowSpectre, 0, 0);

    _groupBoxLayout->addWidget(_groupBoxSpectre, 1, 1);

    _buttonGroupTypeDistrib = new QButtonGroup();
    _buttonGroupTypeDistrib->setExclusive(true);
    _pRadioButtonCalculee = new QRadioButton(TR("id_puissance_calculee"));
    _buttonGroupTypeDistrib->addButton(_pRadioButtonCalculee, 0);
    _pRadioButtonImposee = new QRadioButton(TR("id_puissance_imposee"));
    _buttonGroupTypeDistrib->addButton(_pRadioButtonImposee, 1);

    QGridLayout* groupBoxTypeDistribLayout = new QGridLayout();
    groupBoxTypeDistribLayout->addWidget(_pRadioButtonCalculee, 0, 0);
    groupBoxTypeDistribLayout->addWidget(_pRadioButtonImposee, 0, 1);

    QGroupBox* groupBoxTypeDistrib = new QGroupBox();
    groupBoxTypeDistrib->setTitle(TR("id_type_distribution"));
    groupBoxTypeDistrib->setLayout(groupBoxTypeDistribLayout);

    _groupBoxLayout->addWidget(groupBoxTypeDistrib, 2, 0);

    _groupBoxAtt = new QGroupBox(_groupBox);
    _groupBoxAtt->setTitle(TR("id_att"));
    _groupBoxAttLayout = new QGridLayout();
    _groupBoxAtt->setLayout(_groupBoxAttLayout);

    _lineEditNomAtt = new QLineEdit(_groupBoxAtt);
    _lineEditNomAtt->setEnabled(FALSE);
    _groupBoxAttLayout->addWidget(_lineEditNomAtt, 0, 0);

    _pushButtonSpectreAtt = new QPushButton(_groupBoxAtt);
    _pushButtonSpectreAtt->setText(TR("id_proprietes_button"));
    _groupBoxAttLayout->addWidget(_pushButtonSpectreAtt, 0, 1);

    _groupBoxLayout->addWidget(_groupBoxAtt, 2, 1);

    _acousticInterfaceLayout->addWidget(_groupBox, 0, 0);

    // Gestion du choix du regime
    QGroupBox* _buttonGroupBox = new QGroupBox(this);
    _buttonGroupBox->setTitle(TR("id_select_regime"));
    QGridLayout* _buttonGroupBoxLayout = new QGridLayout();
    _buttonGroupBox->setLayout(_buttonGroupBoxLayout);

    _pushButtonRemRegime = new QPushButton(_buttonGroupBox);
    _pushButtonRemRegime->setText(TR("id_supprimer"));
    _buttonGroupBoxLayout->addWidget(_pushButtonRemRegime, 0, 0);

    _comboBoxSelectRegime = new QComboBox(_buttonGroupBox);
    _comboBoxSelectRegime->setEditable(true);
    _buttonGroupBoxLayout->addWidget(_comboBoxSelectRegime, 0, 1);

    _pushButtonNewRegime = new QPushButton(_buttonGroupBox);
    _pushButtonNewRegime->setText(TR("id_nouveau"));
    _buttonGroupBoxLayout->addWidget(_pushButtonNewRegime, 0, 2);

    _acousticInterfaceLayout->addWidget(_buttonGroupBox, 3, 0);
    updateContent();

    connect(_pushButtonNewRegime, SIGNAL(clicked()), this, SLOT(createNewRegime()));
    connect(_pushButtonRemRegime, SIGNAL(clicked()), this, SLOT(deleteRegime()));
    connect(_comboBoxSelectRegime, SIGNAL(activated(int)), this, SLOT(changeRegime(int)));
    connect(_lineEditDensiteSrcsH, SIGNAL(textChanged(const QString&)), this, SLOT(updateDensite()));
    connect(_lineEditDensiteSrcsV, SIGNAL(textChanged(const QString&)), this, SLOT(updateDensite()));
    connect(_pushButtonShowSpectre, SIGNAL(clicked()), this, SLOT(showSpectre()));
    connect(_checkBoxIsRayonnant,  SIGNAL(clicked()), this, SLOT(updateRayonnant()));
    connect(_checkBoxUseAtt, SIGNAL(clicked()), this, SLOT(useAttenuateur()));
    connect(_pushButtonSpectreAtt, SIGNAL(clicked()), this, SLOT(editAtt()));
    connect(_pRadioButtonImposee, SIGNAL(clicked()), this, SLOT(showSpectre()));

    _comboBoxSelectRegime->installEventFilter(this);
}

TYAcousticInterfaceWidget::~TYAcousticInterfaceWidget()
{
}

void TYAcousticInterfaceWidget::updateContent()
{
    QString num;

    _lineEditDensiteSrcsH->setText(num.setNum(getElement()->getDensiteSrcsH(), 'f', 2));
    _lineEditDensiteSrcsV->setText(num.setNum(getElement()->getDensiteSrcsV(), 'f', 2));

    // Mise a jour a partir du regime courant
    updateFromCurrentRegime();

    // Mise a jour du combo des regimes
    updateComboRegime();

    // Inactivation de la possibilite de changer de regime
    _pushButtonRemRegime->setEnabled(getElement()->getIsRegimeChangeAble());
    _comboBoxSelectRegime->setEnabled(getElement()->getIsRegimeChangeAble());
    _pushButtonNewRegime->setEnabled(getElement()->getIsRegimeChangeAble());
}

void TYAcousticInterfaceWidget::apply()
{
    if (_checkBoxUseAtt->isChecked())
    {
        getElement()->setUseAtt(true);
        // Va associer le meme attenuateur aux elements
        // acoustiques composant celui-ci
        getElement()->propagateAtt(getElement()->getAtt());
    }
    else
    {
        getElement()->setUseAtt(false);
    }

    if (_pRadioButtonCalculee->isChecked())
    {
        getElement()->setTypeDistribution(TYAcousticInterface::TY_PUISSANCE_CALCULEE);
    }
    else
    {
        getElement()->setTypeDistribution(TYAcousticInterface::TY_PUISSANCE_IMPOSEE);
    }

    getElement()->setIsRayonnant(_checkBoxIsRayonnant->isChecked());

    if (_modified)
    {
        getElement()->setDensiteSrcsH(_lineEditDensiteSrcsH->text().toDouble());
        getElement()->setDensiteSrcsV(_lineEditDensiteSrcsV->text().toDouble());
    }

    getElement()->updateCurrentRegime();

    TYElement::setIsSavedOk(true);
//    emit modified();
}

void TYAcousticInterfaceWidget::updateDensite()
{
    _modified = true;
}

void TYAcousticInterfaceWidget::updateRayonnant()
{
}

void TYAcousticInterfaceWidget::reject()
{
    // Si l'utilisateur annule, on restitue l'etat du regime courant
    getElement()->setCurRegime(getElement()->getCurRegime());
}

void TYAcousticInterfaceWidget::useAttenuateur()
{
    getElement()->setUseAtt(_checkBoxUseAtt->isChecked());
    _groupBoxAtt->setEnabled(_checkBoxUseAtt->isChecked());

    // Si on selectionne un attenuateur on ouvre immediatement la boite de dialogue de saisie
    if (_checkBoxUseAtt->isChecked()) { editAtt(); }
}

void TYAcousticInterfaceWidget::editAtt()
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

void TYAcousticInterfaceWidget::showSpectre()
{
    TYSpectre& spectre = getElement()->getCurrentSpectre();

    if (_pRadioButtonCalculee->isChecked())
    {
        spectre.setIsReadOnly(true);
    }

    spectre.edit(this);

    spectre.setIsReadOnly(false);
}

void TYAcousticInterfaceWidget::createNewRegime()
{
    // On sauve l'etat du regime courant avant d'en rajouter un autre
    saveCurrentRegime();

    // Creation du nouveau regime
    getElement()->addRegime();

    // Choix du nouveau regime comme regime courant
    getElement()->setCurRegime(-1);

    // Mise a jour depuis le regime courant
    updateFromCurrentRegime();

    // Mise a jour du combo des regimes
    updateComboRegime();
}

void TYAcousticInterfaceWidget::saveCurrentRegime()
{
    bool status = true;
    TYRegime& regime = getElement()->getRegimeNb(getElement()->getCurRegime(), status);

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

    regime._spectre = getElement()->getCurrentSpectre();

    if (regime._useAtt)
    {
        regime._pAtt = getElement()->getAtt();
    }
}

void TYAcousticInterfaceWidget::deleteRegime()
{
    getElement()->remRegime(_comboBoxSelectRegime->currentIndex());

    updateFromCurrentRegime();

    // Mise a jour du combo box
    updateComboRegime();
}

void TYAcousticInterfaceWidget::changeRegime(int regime)
{
    // Sauvegarde du regime courant
    saveCurrentRegime();

    // Changement de regime
    getElement()->setCurRegime(regime);

    updateFromCurrentRegime();

    emit regimeChanged(regime);
}

void TYAcousticInterfaceWidget::renameRegime()
{
    bool status = true;

    TYRegime& regime = getElement()->getRegimeNb(getElement()->getCurRegime(), status);
    QString actualName = regime.getName();

    QLineEdit* LE = _comboBoxSelectRegime->lineEdit();
    QString nomRegime = LE->text();

    // Si le nom n'a pas change, on ne fait rien.
    if (actualName == nomRegime) { return; }

    // Actualisation du nom du regime pour les sous-elements
    getElement()->setRegimeName(nomRegime);

    updateComboRegime();
}


void TYAcousticInterfaceWidget::updateComboRegime()
{
    _comboBoxSelectRegime->clear();

    // Remplissage du comboBox des regimes
    QString nom;
    bool status = true;
    for (short i = 0 ; i < getElement()->getNbRegimes(); i++)
    {
        nom = getElement()->getRegimeNb(i, status).getName();
        _comboBoxSelectRegime->insertItem(i, nom);
    }

    // On affiche le regime courant
    _comboBoxSelectRegime->setCurrentIndex(getElement()->getCurRegime());
}


void TYAcousticInterfaceWidget::updateFromCurrentRegime()
{
    _checkBoxIsRayonnant->setChecked(getElement()->getIsRayonnant());

    _groupBoxAtt->setEnabled(getElement()->getUseAtt());

    if (getElement()->getTypeDistribution() == TYAcousticInterface::TY_PUISSANCE_CALCULEE)
    {
        _pRadioButtonCalculee->setChecked(true);
    }
    else
    {
        _pRadioButtonImposee->setChecked(true);
    }

    if (getElement()->getUseAtt() && getElement()->getAtt())
    {
        _checkBoxUseAtt->setChecked(true);   // Attenuateur ou pas
        _lineEditNomAtt->setText(getElement()->getAtt()->getName());
    }
    else
    {
        _checkBoxUseAtt->setChecked(false);   // Attenuateur ou pas
        _lineEditNomAtt->setText("");
    }
}

void TYAcousticInterfaceWidget::setCheckBoxRayonnantState(const bool& active)
{
    _checkBoxIsRayonnant->setEnabled(active);
}

bool TYAcousticInterfaceWidget::getCheckBoxRayonnantState()
{
    return _checkBoxIsRayonnant->isEnabled();
}


bool TYAcousticInterfaceWidget::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == _comboBoxSelectRegime)
    {
        if (event->type() == QEvent::FocusOut)
        {
            renameRegime();
        }
        else
        {
            return false;
        }
    }
    else
    {
        // pass the event on to the parent class
        return QWidget::eventFilter(obj, event);
    }

    return true;
}
