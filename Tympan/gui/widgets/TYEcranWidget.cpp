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
 * \file TYEcranWidget.cpp
 * \brief Outil IHM pour un ecran
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/infrastructure/TYEcran.h"
#include <qradiobutton.h>
#include <qbuttongroup.h>
//Added by qt3to4:
#include <QGridLayout>


#define TR(id) OLocalizator::getString("TYEcranWidget", (id))


TYEcranWidget::TYEcranWidget(TYEcran* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{

    _elmW = new TYAcousticFaceSetWidget(pElement, this);

    resize(300, 810);
    setWindowTitle(TR("id_caption"));
    _ecranLayout = new QGridLayout();
    setLayout(_ecranLayout);

    _ecranLayout->addWidget(_elmW, 0, 0);

    // Parametre hauteur et epaisseur...
    _widgetProp = new QWidget(this);
    QGridLayout* _groupBoxPropLayout = new QGridLayout();
    _widgetProp->setLayout(_groupBoxPropLayout);

    _groupBoxHauteur = new QGroupBox(_widgetProp);
    _groupBoxHauteur->setTitle(TR("id_hauteur_label"));
    _groupBoxHauteurLayout = new QGridLayout();
    _groupBoxHauteur->setLayout(_groupBoxHauteurLayout);

    _groupBoxEpaisseur = new QGroupBox(_widgetProp);
    _groupBoxEpaisseur->setTitle(TR("id_epaisseur_label"));
    _groupBoxEpaisseurLayout = new QGridLayout();
    _groupBoxEpaisseur->setLayout(_groupBoxEpaisseurLayout);

    // Presentation du volume
    _lineEditHauteur = new QLineEdit(_groupBoxHauteur);
    _lineEditHauteur->setEnabled(true);
    _groupBoxHauteurLayout->addWidget(_lineEditHauteur, 0, 0);

    // Presentation de la surface
    _lineEditEpaisseur = new QLineEdit(_groupBoxEpaisseur);
    _lineEditEpaisseur->setEnabled(true);
    _groupBoxEpaisseurLayout->addWidget(_lineEditEpaisseur, 0, 0);

    //Insertion dans la boite de dialogue
    _groupBoxPropLayout->addWidget(_groupBoxHauteur, 0, 0);
    _groupBoxPropLayout->addWidget(_groupBoxEpaisseur, 0, 1);

    _ecranLayout->addWidget(_widgetProp, 1, 0);

    updateContent();
}

TYEcranWidget::~TYEcranWidget()
{
}

void TYEcranWidget::updateContent()
{
    _elmW->updateContent();

    _lineEditHauteur->setText(QString().setNum(((TYEcran*)getElement())->getHauteur()));
    _lineEditEpaisseur->setText(QString().setNum(((TYEcran*)getElement())->getEpaisseur()));
}

void TYEcranWidget::apply()
{
    _elmW->apply();

    float hauteur, epaisseur;
    float new_hauteur, new_epaisseur;

    hauteur = ((TYEcran*)getElement())->getHauteur();
    epaisseur = ((TYEcran*)getElement())->getEpaisseur();

    QString strHauteur = _lineEditHauteur->text();
    QString strEpaisseur = _lineEditEpaisseur->text();
    bool bOk;

    new_hauteur = strHauteur.toFloat(&bOk);
    if (bOk && (new_hauteur != hauteur))
    {
        ((TYEcran*)getElement())->setHauteur(new_hauteur);
    }

    new_epaisseur = strEpaisseur.toFloat(&bOk);
    if (bOk && (new_epaisseur != epaisseur))
    {
        ((TYEcran*)getElement())->setEpaisseur(new_epaisseur);
    }

    ((TYEcran*)getElement())->updateGeometry(); // update geometry to take in account changes in height or width...

    emit modified();
}
