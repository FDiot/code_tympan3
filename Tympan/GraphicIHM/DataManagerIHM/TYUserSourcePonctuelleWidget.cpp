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
 * \file TYUserSourcePonctuelleWidget.cpp
 * \brief outil IHM pour une source ponctuelle
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include <qcombobox.h>
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYUserSourcePonctuelle.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

#define TR(id) OLocalizator::getString("TYSourcePonctuelleWidget", (id))


TYUserSourcePonctuelleWidget::TYUserSourcePonctuelleWidget(TYUserSourcePonctuelle* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(_pParent)
{
    Q_ASSERT(pElement);

    _pElement = pElement;

    _elmW = new TYSourcePonctuelleWidget(pElement, this);

    resize(300, 550);
    setWindowTitle(TR("id_caption"));
    _sourcePonctuelleLayout = new QGridLayout();
    setLayout(_sourcePonctuelleLayout);
    _sourcePonctuelleLayout->addWidget(_elmW, 0, 0);

    // Proprietes acoustiques
    QGroupBox* pgroupBoxPropriete = new QGroupBox(this);

    QGridLayout* pgroupBoxProprieteLayout = new QGridLayout();
    pgroupBoxPropriete->setLayout(pgroupBoxProprieteLayout);

    _labelIsRayonnant = new QLabel(pgroupBoxPropriete);
    _labelIsRayonnant->setText(TR("id_israyonnant_label"));
    _checkBoxIsRayonnant = new QCheckBox(pgroupBoxPropriete);
    _checkBoxIsRayonnant->setText(TR(""));

    _labeUseAtt = new QLabel(pgroupBoxPropriete);
    _labeUseAtt->setText(TR("id_useatt_label"));
    _checkBoxUseAtt = new QCheckBox(pgroupBoxPropriete);
    _checkBoxUseAtt->setText(TR(""));

    pgroupBoxProprieteLayout->addWidget(_labelIsRayonnant, 0, 1);
    pgroupBoxProprieteLayout->addWidget(_checkBoxIsRayonnant, 0, 2);
    pgroupBoxProprieteLayout->addWidget(_labeUseAtt, 0, 3);
    pgroupBoxProprieteLayout->addWidget(_checkBoxUseAtt, 0, 4);

    _sourcePonctuelleLayout->addWidget(pgroupBoxPropriete, 1, 0);

    // Attenuateur et directivite
    _groupBoxAtt = new QGroupBox(this);
    _groupBoxAtt->setTitle(TR("id_att"));
    _groupBoxAttLayout = new QGridLayout();
    _groupBoxAtt->setLayout(_groupBoxAttLayout);

    _pushButtonAtt = new QPushButton(_groupBoxAtt);
    _pushButtonAtt->setText(TR("id_proprietes_button"));
    _groupBoxAttLayout->addWidget(_pushButtonAtt, 0, 1);

    _lineEditNomAtt = new QLineEdit(_groupBoxAtt);
    _lineEditNomAtt->setEnabled(FALSE);
    _groupBoxAttLayout->addWidget(_lineEditNomAtt, 0, 0);

    _sourcePonctuelleLayout->addWidget(_groupBoxAtt, 2, 0);

    // Gestion de la directivite
    //_groupBoxDir = new QGroupBox(this);
    //_groupBoxDir->setTitle(TR("id_tabdir_box"));
    //_groupBoxDirLayout = new QGridLayout();
    //_groupBoxDir->setLayout(_groupBoxDirLayout);

    //_pushButtonDir = new QPushButton(_groupBoxDir);
    //_pushButtonDir->setText(TR("id_proprietes_button"));
    //_groupBoxDirLayout->addWidget(_pushButtonDir, 0, 1);

    //_lineEditNomDir = new QLineEdit(_groupBoxDir);
    //_lineEditNomDir->setEnabled(FALSE);
    //_groupBoxDirLayout->addWidget(_lineEditNomDir, 0, 0);

    //_sourcePonctuelleLayout->addWidget(_groupBoxDir, 3, 0);

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

    _sourcePonctuelleLayout->addWidget(buttonGroupBox, 3, 0);

    updateContent();

    connect(_pushButtonNewRegime, SIGNAL(clicked()), this, SLOT(createNewRegime()));
    connect(_pushButtonRemRegime, SIGNAL(clicked()), this, SLOT(deleteRegime()));
    connect(_comboBoxSelectRegime, SIGNAL(activated(int)), this, SLOT(changeRegime(int)));
    connect(_pushButtonAtt, SIGNAL(clicked()), this, SLOT(editAttenuateur()));
    connect(_elmW, SIGNAL(typeRayntChanged(int)), this, SLOT(changeTypeRaynt(int)));
    connect(_checkBoxUseAtt, SIGNAL(clicked()), this, SLOT(useAttenuateur()));

    _comboBoxSelectRegime->installEventFilter(this);
}

TYUserSourcePonctuelleWidget::~TYUserSourcePonctuelleWidget()
{
}

void TYUserSourcePonctuelleWidget::updateContent()
{
    _elmW->updateContent();

    // Mise a jour a partir du regime courant
    updateFromCurrentRegime();

    // Mise a jour du combo des regimes
    updateComboRegime();
}

void TYUserSourcePonctuelleWidget::updateFromCurrentRegime()
{
    _checkBoxUseAtt->setChecked(getElement()->getUseAtt());   // Attenuateur ou pas
    _checkBoxIsRayonnant->setChecked(getElement()->getIsRayonnant());

    // On ne peut desactiver une source dans son widget
    // Cette propriete est pilotee par le calcul
    _checkBoxIsRayonnant->setEnabled(false);

    _groupBoxAtt->setEnabled(getElement()->getUseAtt());

    if (getElement()->getUseAtt() && getElement()->getAttenuateur())
    {
        _lineEditNomAtt->setText(getElement()->getAttenuateur()->getName());
    }
    else
    {
        _lineEditNomAtt->setText("");
    }

    //_groupBoxDir->setEnabled(false);
    // Si la source est a directivite "solid" ou "utilisateur"
    //if ((getElement()->getTypeRaynt() == SOLID) ||
    //    (getElement()->getTypeRaynt() == FORCED))
    //{
    //    _groupBoxDir->setEnabled(true);
    //}
}

void TYUserSourcePonctuelleWidget::apply()
{
    _elmW->apply();

    if (getElement()->getAttenuateur() != NULL)
    {
        getElement()->setUseAtt(_checkBoxUseAtt->isChecked());
    }
    else
    {
        getElement()->setUseAtt(false);
    }

    // Mise a jour du regime courant
    getElement()->updateCurrentRegime();

    emit modified();
}

//void TYUserSourcePonctuelleWidget::editDirectivite()
//{
//    getElement()->getDirectivite()->edit(this);
//}

void TYUserSourcePonctuelleWidget::createNewRegime()
{
    // On commence par sauver le regime courant
    saveCurrentRegime();

    // Creation du nouveau regime
    getElement()->addRegime(getElement()->buildRegime());

    // Basculement sur le regime nouvellement cree
    getElement()->setCurrentRegime(-1);

    // Mise a jour du combo des regimes
    updateComboRegime();
}

void TYUserSourcePonctuelleWidget::saveCurrentRegime()
{
    TYUserSrcRegime& regime = getElement()->getRegimeNb(getElement()->getCurrentRegime());

    regime._isRayonnant = _checkBoxIsRayonnant->isChecked();
    regime._useAtt = _checkBoxUseAtt->isChecked();
    regime._typeDistribution = 1;  // TY_PUISSANCE_IMPOSEE
    regime._spectre = *getElement()->getCurrentSpectre();

    if (regime._useAtt)
    {
        regime._pAtt = getElement()->getAttenuateur();
    }

//    regime._pDirectivite = getElement()->getDirectivite();
}

void TYUserSourcePonctuelleWidget::deleteRegime()
{
    getElement()->remRegime(_comboBoxSelectRegime->currentIndex());

    // Mise a jour sur la base du nouveau regime par defaut
    updateFromCurrentRegime();

    // Mise a jour du combo box
    updateComboRegime();
}

void TYUserSourcePonctuelleWidget::changeRegime(int regime)
{
    // Sauvegarde du regime courant
    saveCurrentRegime();

    // Changement de regime (au niveau de l'element)
    getElement()->setCurrentRegime(regime);

    // Mise a jour de la boite de dialogue
    updateFromCurrentRegime();
}

void TYUserSourcePonctuelleWidget::renameRegime()
{
    TYUserSrcRegime& regime = getElement()->getRegimeNb(getElement()->getCurrentRegime());
    QString actualName = regime.getName();

    QLineEdit* LE = _comboBoxSelectRegime->lineEdit();
    QString nomRegime = LE->text();

    // Si le nom n'a pas change, on ne fait rien.
    if (actualName == nomRegime) { return; }

    regime.setName(nomRegime);
    updateComboRegime();
}

void TYUserSourcePonctuelleWidget::updateComboRegime()
{
    _comboBoxSelectRegime->clear();

    // Remplissage du comboBox des regimes
    QString nom;
    for (short i = 0 ; i < getElement()->getNbRegimes(); i++)
    {
        nom = getElement()->getRegimeNb(i).getName();
        _comboBoxSelectRegime->insertItem(i, nom);
    }

    _comboBoxSelectRegime->setCurrentIndex(getElement()->getCurrentRegime());
}

void TYUserSourcePonctuelleWidget::useAttenuateur()
{
    getElement()->setUseAtt(_checkBoxUseAtt->isChecked());

    _groupBoxAtt->setEnabled(_checkBoxUseAtt->isChecked());

    // Si on selectionne un attenuateur on ouvre immediatement la boite de dialogue de saisie
    if (_checkBoxUseAtt->isChecked()) { editAttenuateur(); }
}

void TYUserSourcePonctuelleWidget::editAttenuateur()
{
    // Si l'element n'a pas d'attenuateur, on le cree
    if (getElement()->getAttenuateur() == NULL)
    {
        getElement()->setAttenuateur(new TYAttenuateur());
    }
    int ret = getElement()->getAttenuateur()->edit(this);

    if (ret == QDialog::Accepted)
    {
        _lineEditNomAtt->setText(getElement()->getAttenuateur()->getName());
    }
}

//void TYUserSourcePonctuelleWidget::changeTypeRaynt(int typeRaynt)
//{
//    if ((typeRaynt == SOLID) || (typeRaynt == FORCED))
//    {
//        _groupBoxDir->setEnabled(true);
//    }
//    else
//    {
//        _groupBoxDir->setEnabled(false);
//    }
//}

bool TYUserSourcePonctuelleWidget::eventFilter(QObject* obj, QEvent* event)
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
