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
 * \file TYPointControlWidget.cpp
 * \brief outil IHM pour un point de controle
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYPointControl.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYCalcul.h"
#include <qradiobutton.h>
#include <qbuttongroup.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>


#define TR(id) OLocalizator::getString("TYPointControlWidget", (id))


TYPointControlWidget::TYPointControlWidget(TYPointControl* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{
    unsigned short pos = 0;

    _pPointBack = new TYPointControl();
    _pPointBack->deepCopy(pElement, true);

    resize(300, 200);
    setWindowTitle(TR("id_caption"));
    QGridLayout* pPointCalculLayout = new QGridLayout();
    setLayout(pPointCalculLayout);

    _ptCalculW = new TYPointCalculWidget(pElement, this);
    _ptCalculW->setLayoutSpacing(0, 0);
    pPointCalculLayout->addWidget(_ptCalculW, pos++, 0);

    QGroupBox* pGroupBoxHauteur = new QGroupBox(this);
    pGroupBoxHauteur->setTitle(TR("id_maillage"));
    QGridLayout* groupBoxHauteurLayout = new QGridLayout();
    pGroupBoxHauteur->setLayout(groupBoxHauteurLayout);

    QLabel* pLabelHauteur = new QLabel(TR("id_hauteur"), pGroupBoxHauteur);
    groupBoxHauteurLayout->addWidget(pLabelHauteur, 0, 0);
    _lineEditHauteur = new QLineEdit(pGroupBoxHauteur);
    groupBoxHauteurLayout->addWidget(_lineEditHauteur, 0, 1);

    pPointCalculLayout->addWidget(pGroupBoxHauteur, pos++, 0);

    // Choix du systeme de coordonnees
    QGroupBox* pGroupBoxSIG = new QGroupBox(this);
    pGroupBoxSIG->setTitle(TR("id_SIG"));
    QGridLayout* groupBoxSIGLayout = new QGridLayout();
    pGroupBoxSIG->setLayout(groupBoxSIGLayout);

    _labelRepere = new QLabel(TR("id_choixSIG"), pGroupBoxSIG);
    groupBoxSIGLayout->addWidget(_labelRepere, 0, 0);

    _lineEditRepere = new QLineEdit(pGroupBoxSIG);
    groupBoxSIGLayout->addWidget(_lineEditRepere, 0, 1);

    _buttonGroupSysCoord = new QButtonGroup();
    _buttonGroupSysCoord->setExclusive(true);
    QRadioButton* pRadioButtonTympan = new QRadioButton(TR("id_tympan"));
    _buttonGroupSysCoord->addButton(pRadioButtonTympan, 0);
    QRadioButton* pRadioButtonSIG = new QRadioButton(TR("id_sigle_SIG"));
    _buttonGroupSysCoord->addButton(pRadioButtonSIG, 1);

    QGridLayout* groupBoxSysCoordLayout = new QGridLayout();
    groupBoxSysCoordLayout->addWidget(pRadioButtonTympan, 0, 0);
    groupBoxSysCoordLayout->addWidget(pRadioButtonSIG, 0, 1);

    QGroupBox* groupBoxSysCoord = new QGroupBox();
    groupBoxSysCoord->setTitle(TR("id_repere"));
    groupBoxSysCoord->setLayout(groupBoxSysCoordLayout);

    groupBoxSIGLayout->addWidget(groupBoxSysCoord, 0, 2);

    pPointCalculLayout->addWidget(pGroupBoxSIG, pos++, 0);

    // Choix de l'object geometrique et couleur
    QGroupBox* groupBoxAspect = new QGroupBox(this);
    groupBoxAspect->setTitle(TR(""));
    QGridLayout* groupBoxLayoutAspect = new QGridLayout();
    groupBoxAspect->setLayout(groupBoxLayoutAspect);

    // Choix de l'object geometrique
    QGroupBox* groupBoxObject = new QGroupBox(groupBoxAspect);
    groupBoxObject->setTitle(TR(""));
    QGridLayout* groupBoxLayoutObject = new QGridLayout();
    groupBoxObject->setLayout(groupBoxLayoutObject);

    _labelSelectObject = new QLabel(groupBoxObject);
    _labelSelectObject->setText(TR("id_objet_label"));
    groupBoxLayoutObject->addWidget(_labelSelectObject, 0, 0);

    _comboBoxSelectObject = new QComboBox(groupBoxObject);
    groupBoxLayoutObject->addWidget(_comboBoxSelectObject, 0, 1);

    groupBoxLayoutAspect->addWidget(groupBoxObject, 0, 0);

    // Choix de la couleur
    _colorW = new TYColorInterfaceWidget(pElement, groupBoxAspect);
    groupBoxLayoutAspect->addWidget(_colorW, 0, 1);

    pPointCalculLayout->addWidget(groupBoxAspect, pos++, 0);



    updateContent();

    connect(_comboBoxSelectObject, SIGNAL(activated(int)), this, SLOT(changeObject(int)));
    QObject::connect(_buttonGroupSysCoord, SIGNAL(buttonClicked(int)), this, SLOT(updateCoord(int)));

}

TYPointControlWidget::~TYPointControlWidget()
{
}

void TYPointControlWidget::updateContent()
{
    if (getElement()->isLocked())
    {
        _lineEditHauteur->setEnabled(false);
    }

    _colorW->updateContent();
    QString strSIG;
    int SIGType = getElement()->getSIGType();

    switch (SIGType)
    {
        case 1 :
            strSIG = "LAMBERT IIe";
            break;
        case 2 :
            strSIG = "GPS";
            break;

        case 0 :
        default :
            strSIG = "TYMPAN";
            break;
    }

    _buttonGroupSysCoord->button(0)->setChecked(true);

    _lineEditRepere->setText(strSIG);
    _lineEditRepere->setEnabled(false); // On ne peut pas choisir ici son systeme de coordonnees

    _ptCalculW->updateContent();

    _lineEditHauteur->setText(QString().setNum(getElement()->getHauteur()));

    updateComboObject();
}

void TYPointControlWidget::apply()
{
    _ptCalculW->apply();
    _colorW->apply();

    getElement()->fromSIG(); // Pas de danger car le point controle son etat

    getElement()->setHauteur(_lineEditHauteur->text().toDouble());

    delete _pPointBack;
    _pPointBack = NULL;

    emit modified();
}

void TYPointControlWidget::reject()
{
    _pElement->deepCopy(_pPointBack, true);
    getElement()->fromSIG(); // Pas de danger car le point controle son etat
    delete _pPointBack;
    _pPointBack = NULL;
}

void TYPointControlWidget::updateComboObject()
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

void TYPointControlWidget::changeObject(int object)
{
    getElement()->setObject(object);
}

void TYPointControlWidget::updateCoord(int selected)
{
    _ptCalculW->apply();

    if (selected == 0)
    {
        getElement()->fromSIG();
    }
    else
    {
        getElement()->toSIG();
    }

    _ptCalculW->updateContent();
}
