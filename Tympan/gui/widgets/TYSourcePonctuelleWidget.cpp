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
 * \file TYSourcePonctuelleWidget.cpp
 * \brief outil IHM pour une source ponctuelle
 */


#include <qcombobox.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/acoustic/TYSourcePonctuelle.h"
#include "Tympan/gui/widgets/TYColorInterfaceWidget.h"
#include "Tympan/gui/widgets/TYSourceWidget.h"
#include "Tympan/gui/widgets/TYPointWidget.h"
#include "TYSourcePonctuelleWidget.h"

#define TR(id) OLocalizator::getString("TYSourcePonctuelleWidget", (id))


TYSourcePonctuelleWidget::TYSourcePonctuelleWidget(TYSourcePonctuelle* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{

    _elmW = new TYSourceWidget(pElement, this);
    _colorW = new TYColorInterfaceWidget(pElement, this);


    unsigned short wln = 0;

    resize(300, 550);
    setWindowTitle(TR("id_caption"));
    _sourcePonctuelleLayout = new QGridLayout();

    _sourcePonctuelleLayout->addWidget(_elmW, wln++, 0);

    _pPointW = new TYPointWidget(getElement()->getPos(), this);

    _sourcePonctuelleLayout->addWidget(_pPointW, wln++, 0);

    // Gestion hauteur
    _groupBoxType = new QGroupBox(this);
    _groupBoxType->setTitle(TR(""));
    _groupBoxTypeLayout = new QGridLayout();
    _groupBoxType->setLayout(_groupBoxTypeLayout);

    QLabel* pLabelHauteur = new QLabel(TR("id_hauteur_label"), _groupBoxType);
    _groupBoxTypeLayout->addWidget(pLabelHauteur, 0, 0);

    _pLineEditHauteur = new QLineEdit(_groupBoxType);
    _groupBoxTypeLayout->addWidget(_pLineEditHauteur, 0, 1);

    _sourcePonctuelleLayout->addWidget(_groupBoxType, wln++, 0);

    // Choix de l'object geometrique et couleur
    QGroupBox* groupBoxObject = new QGroupBox(this);
    groupBoxObject->setTitle(TR(""));
    QGridLayout* groupBoxLayoutObject = new QGridLayout();
    groupBoxObject->setLayout(groupBoxLayoutObject);

    // Gestion de la forme
    QGroupBox* groupBoxForme = new QGroupBox(groupBoxObject);
    QGridLayout* groupBoxFormeLayout = new QGridLayout();
    groupBoxForme->setLayout(groupBoxFormeLayout);

    _labelSelectObject = new QLabel(groupBoxForme);
    _labelSelectObject->setText(TR("id_objet_label"));

    groupBoxFormeLayout->addWidget(_labelSelectObject, 0, 0);

    _comboBoxSelectObject = new QComboBox(groupBoxObject);
    groupBoxFormeLayout->addWidget(_comboBoxSelectObject, 0, 1);

    groupBoxLayoutObject->addWidget(groupBoxForme, 0, 0);
    groupBoxLayoutObject->addWidget(_colorW, 0, 1);

    _sourcePonctuelleLayout->addWidget(groupBoxObject, wln++, 0);

    // Gestion du type de rayonnement
    // Retrait de la possibilité de mettre des sources hémispheriques. Cette option pourra être réintroduite avec une meilleure modélisation.
	// Puisque toutes les sources sont sphérique, le choix du type de rayonnement est enlevé
	//     _buttonGroupTypeRaynt = new QButtonGroup(this);
    //     _buttonGroupTypeRaynt->setExclusive(true);
    //     _pRadioButtonSpheric = new QRadioButton(TR("id_spherique"));
    //     _buttonGroupTypeRaynt->addButton(_pRadioButtonSpheric, 0);
	//     _pRadioButtonHemi = new QRadioButton(TR("id_hemispherique"));
    //     _buttonGroupTypeRaynt->addButton(_pRadioButtonHemi, 1);
    //    _pRadioButtonForce = new QRadioButton(TR("id_forcee"));
    //    _buttonGroupTypeRaynt->addButton(_pRadioButtonForce, 3);

    /*QGridLayout* groupBoxTypeRayntLayout = new QGridLayout();
    groupBoxTypeRayntLayout->addWidget(_pRadioButtonSpheric, 0, 0);*/
    //    groupBoxTypeRayntLayout->addWidget(_pRadioButtonHemi, 0, 1);
    //    groupBoxTypeRayntLayout->addWidget(_pRadioButtonForce, 0, 3);

    //QGroupBox* groupBoxTypeRaynt = new QGroupBox();
    //groupBoxTypeRaynt->setTitle(TR("id_type_raynt_label"));
    //groupBoxTypeRaynt->setLayout(groupBoxTypeRayntLayout);

    //_sourcePonctuelleLayout->addWidget(groupBoxTypeRaynt, wln++, 0);


    //_sourcePonctuelleLayout->setMargin(0);
    setLayout(_sourcePonctuelleLayout);

    updateContent();

    //connect(_buttonGroupTypeRaynt, SIGNAL(buttonClicked(int)), this, SLOT(changeTypeRaynt(int)));
    connect(_comboBoxSelectObject, SIGNAL(activated(int)), this, SLOT(changeObject(int)));
}

TYSourcePonctuelleWidget::~TYSourcePonctuelleWidget()
{
}

void TYSourcePonctuelleWidget::updateComboObject()
{
    _comboBoxSelectObject->clear();

    // Remplissage du comboBox des objects
    _comboBoxSelectObject->insertItem(0, TR("id_objet_cube"));
    _comboBoxSelectObject->insertItem(1, TR("id_objet_pyramide"));
    _comboBoxSelectObject->insertItem(2, TR("id_objet_sphere"));
    _comboBoxSelectObject->insertItem(3, TR("id_objet_etoile"));

    // On affiche la forme d'objet courante
    _comboBoxSelectObject->setCurrentIndex(getElement()->getObject());
}

void TYSourcePonctuelleWidget::updateContent()
{
    _elmW->updateContent();
    _colorW->updateContent();

    // Mise a jour des boutons radio de type de rayonnement (uniquement si source utilisateur)
    if (getElement()->getType() == TYSourcePonctuelle::TypeUser)
    {
        //_pRadioButtonSpheric->setEnabled(true);
        ////       _pRadioButtonHemi->setEnabled(true);
        ////        _pRadioButtonForce->setEnabled(false);
        //switch (getElement()->getTypeRaynt())
        //{
        //    case(0):
        //    {
        //        _pRadioButtonSpheric->setChecked(true);
        //    }
        //    break;
        //    //case(1):
        //    //{
        //    //    _pRadioButtonHemi->setChecked(true);
        //    //}
        //    //break;
        //    //case(2):
        //    //{
        //    //    _pRadioButtonForce->setChecked(true);
        //    //}
        //    //            break;
        //}
        _pLineEditHauteur->setEnabled(true);
        _pLineEditHauteur->setText(QString().setNum(getElement()->getHauteur()));
    }
    else
    {
        //_pRadioButtonSpheric->setEnabled(false);
        ////        _pRadioButtonHemi->setEnabled(false);
        ////        _pRadioButtonForce->setEnabled(false);
        _pLineEditHauteur->setEnabled(false);
    }

    // Mise a jour de la position du point et desactivation du Z;
    _pPointW->update();
    _pPointW->disableZ();

    // Mise a jour du combo des objets
    updateComboObject();
}

void TYSourcePonctuelleWidget::updateDir()
{
}

void TYSourcePonctuelleWidget::apply()
{
    _elmW->apply();
    _colorW->apply();


    getElement()->setHauteur(_pLineEditHauteur->text().toDouble());

    //    getElement()->setTypeRaynt((TYTypeRaynt)_buttonGroupTypeRaynt->checkedId());

    _pPointW->apply();

    emit modified();
}

void TYSourcePonctuelleWidget::editPosition()
{
    int ret = getElement()->getPos()->edit(this);

    if (ret == QDialog::Accepted)
    {
        //      _lineEditNomPosition->setText( getElement()->getPos()->getName());
    }
}

//void TYSourcePonctuelleWidget::changeTypeRaynt(int id)
//{
//    emit typeRayntChanged(id);
//}

void TYSourcePonctuelleWidget::changeObject(int object)
{
    getElement()->setObject(object);
}
