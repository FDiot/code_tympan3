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
 * \file TYMateriauConstructionWidget.cpp
 * \brief Outil IHM pour des materiaux de construction
 *
 *
 */



#include <qfiledialog.h>
#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/material/TYMateriauConstruction.h"
#include "Tympan/models/business/TYXMLManager.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

#include "TYMateriauConstructionWidget.h"

#define TR(id) OLocalizator::getString("TYMateriauConstructionWidget", (id))


TYMateriauConstructionWidget::TYMateriauConstructionWidget(TYMateriauConstruction* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{
    QString num;


    _elmW = new TYElementWidget(pElement, this);

    resize(300, 200);
    setWindowTitle(TR("id_caption"));
    _matConstrLayout = new QGridLayout();
    setLayout(_matConstrLayout);
    _matConstrLayout->addWidget(_elmW, 0, 0);

    _groupBox = new QGroupBox(this);
    _groupBox->setTitle(TR(""));
    _groupBoxLayout = new QGridLayout();
    _groupBox->setLayout(_groupBoxLayout);

    _lineEditMasseVol = new QLineEdit(_groupBox);
    _lineEditMasseVol->setText(num.setNum(getElement()->getMasseVol(), 'f', 2));
    _groupBoxLayout->addWidget(_lineEditMasseVol, 0, 1);
    QLabel* pUnitMassVol = new QLabel(_groupBox);
    pUnitMassVol->setText(TR("id_unite_massvol"));
    _groupBoxLayout->addWidget(pUnitMassVol, 0, 2);

    _labelMasseVol = new QLabel(_groupBox);
    _labelMasseVol->setText(TR("id_massevol_label"));
    _groupBoxLayout->addWidget(_labelMasseVol, 0, 0);

    _matConstrLayout->addWidget(_groupBox, 1, 0);

    _groupBoxSpectreAbs = new QGroupBox(this);
    _groupBoxSpectreAbs->setTitle(TR("id_spectreabs"));
    _groupBoxSpectreAbsLayout = new QGridLayout();
    _groupBoxSpectreAbs->setLayout(_groupBoxSpectreAbsLayout);

    _pushButtonSpectreAbs = new QPushButton(_groupBoxSpectreAbs);
    _pushButtonSpectreAbs->setText(TR("id_proprietes_button"));
    _groupBoxSpectreAbsLayout->addWidget(_pushButtonSpectreAbs, 0, 1);

    _lineEditNomSpectreAbs = new QLineEdit(_groupBoxSpectreAbs);
    _lineEditNomSpectreAbs->setText(getElement()->getSpectreAbso().getName());
    _lineEditNomSpectreAbs->setEnabled(FALSE);

    _groupBoxSpectreAbsLayout->addWidget(_lineEditNomSpectreAbs, 0, 0);

    _matConstrLayout->addWidget(_groupBoxSpectreAbs, 2, 0);

    _groupBoxSpectreTransm = new QGroupBox(this);
    _groupBoxSpectreTransm->setTitle(TR("id_spectretransm"));
    _groupBoxSpectreTransmLayout = new QGridLayout();
    _groupBoxSpectreTransm->setLayout(_groupBoxSpectreTransmLayout);

    _pushButtonSpectreTransm = new QPushButton(_groupBoxSpectreTransm);
    _pushButtonSpectreTransm->setText(TR("id_proprietes_button"));

    _groupBoxSpectreTransmLayout->addWidget(_pushButtonSpectreTransm, 0, 1);

    _lineEditNomSpectreTransm = new QLineEdit(_groupBoxSpectreTransm);
    _lineEditNomSpectreTransm->setText(getElement()->getSpectreTransm().getName());
    _lineEditNomSpectreTransm->setEnabled(FALSE);
    _groupBoxSpectreTransmLayout->addWidget(_lineEditNomSpectreTransm, 0, 0);

    _matConstrLayout->addWidget(_groupBoxSpectreTransm, 3, 0);

    QGroupBox* buttonGroupBox = new QGroupBox(this);
    buttonGroupBox->setTitle(TR(""));
    QGridLayout* buttonGroupBoxLayout = new QGridLayout();
    buttonGroupBox->setLayout(buttonGroupBoxLayout);

    _pushButtonGetFromXML = new QPushButton(buttonGroupBox);
    _pushButtonGetFromXML->setText(TR("id_get_from_xml"));
    buttonGroupBoxLayout->addWidget(_pushButtonGetFromXML, 0, 1);

    _matConstrLayout->addWidget(buttonGroupBox, 4, 0);

    connect(_pushButtonGetFromXML, SIGNAL(clicked()), this, SLOT(getFromXML()));
    connect(_pushButtonSpectreTransm, SIGNAL(clicked()), this, SLOT(editSpectreT()));
    connect(_pushButtonSpectreAbs, SIGNAL(clicked()), this, SLOT(editSpectreA()));

    updateContent();
}

TYMateriauConstructionWidget::~TYMateriauConstructionWidget()
{
}

void TYMateriauConstructionWidget::updateContent()
{
    QString num;

    _elmW->updateContent();

    _lineEditMasseVol->setText(num.setNum(getElement()->getMasseVol(), 'f', 2));
    _lineEditNomSpectreAbs->setText(getElement()->getSpectreAbso().getName());
    _lineEditNomSpectreTransm->setText(getElement()->getSpectreTransm().getName());
}

void TYMateriauConstructionWidget::apply()
{
    _elmW->apply();

    getElement()->setMasseVol(_lineEditMasseVol->text().toDouble());

    emit modified();
}

void TYMateriauConstructionWidget::editSpectreT()
{
    int ret = getElement()->getSpectreTransm().edit(this);

    if (ret == QDialog::Accepted)
    {
        _lineEditNomSpectreTransm->setText(getElement()->getSpectreTransm().getName());
    }
}

void TYMateriauConstructionWidget::editSpectreA()
{
    int ret = getElement()->getSpectreAbso().edit(this);

    if (ret == QDialog::Accepted)
    {
        _lineEditNomSpectreAbs->setText(getElement()->getSpectreAbso().getName());
    }
}

void TYMateriauConstructionWidget::getFromXML()
{
    QString fileName = QFileDialog::getOpenFileName(this, "", "", "XML (*.xml)");

    if (!fileName.isEmpty())
    {
        TYXMLManager xmlManager;
        TYElement* pElt = NULL;
        LPTYElementArray eltList;

        LPTYMateriauConstruction pMat = NULL;

        xmlManager.load(fileName, eltList);

        size_t count = eltList.size();
        for (size_t i = 0 ; i < count; i++)
        {
            pElt = eltList.at(i);

            if ((pElt != NULL) && (pElt->isA("TYMateriauConstruction")))
            {
                pMat = (TYMateriauConstruction*) pElt;

                if (pMat != NULL)
                {
                    (*getElement()) = (*pMat);
                    updateContent();
                }
            }
        }

        pElt = NULL;
    }
}

