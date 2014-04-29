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
 * \file TYParoiWidget.cpp
 * \brief outil IHM pour une paroi
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include <qfiledialog.h>
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYParoi.h"
#include "Tympan/MetierSolver/DataManagerCore/TYXMLManager.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>


#define TR(id) OLocalizator::getString("TYParoiWidget", (id))


TYParoiWidget::TYParoiWidget(TYParoi* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{
    QString num;


    _elmW = new TYElementWidget(pElement, this);
    resize(300, 350);
    setWindowTitle(TR("id_caption"));
    _paroiLayout = new QGridLayout();
    setLayout(_paroiLayout);

    _paroiLayout->addWidget(_elmW, 0, 0);

    _groupBox4 = new QGroupBox(this);
    _groupBox4->setTitle(TR(""));
    _groupBox4Layout = new QGridLayout();
    _groupBox4->setLayout(_groupBox4Layout);

    _labelEpaisseur = new QLabel(_groupBox4);
    _labelEpaisseur->setText(TR("id_epaisseur_label"));
    _groupBox4Layout->addWidget(_labelEpaisseur, 0, 0);

    _lineEditEpaisseur = new QLineEdit(_groupBox4);
    _lineEditEpaisseur->setText(num.setNum(getElement()->getEpaisseur(), 'f', 2));
    _groupBox4Layout->addWidget(_lineEditEpaisseur, 0, 1);
    QLabel* pUnitEpaisseur = new QLabel(_groupBox4);
    pUnitEpaisseur->setText(TR("id_unite_epaisseur"));
    _groupBox4Layout->addWidget(pUnitEpaisseur, 0, 2);

    _paroiLayout->addWidget(_groupBox4, 1, 0);

    _groupBoxMatStruct = new QGroupBox(this);
    _groupBoxMatStruct->setTitle(TR("id_matstruct"));
    _groupBoxMatStructLayout = new QGridLayout();
    _groupBoxMatStruct->setLayout(_groupBoxMatStructLayout);

    _pushButtonMatStruct = new QPushButton(_groupBoxMatStruct);
    _pushButtonMatStruct->setText(TR("id_proprietes_button"));
    _groupBoxMatStructLayout->addWidget(_pushButtonMatStruct, 0, 1);

    _lineEditNomMatstruct = new QLineEdit(_groupBoxMatStruct);
    _lineEditNomMatstruct->setEnabled(FALSE);
    _lineEditNomMatstruct->setText(getElement()->getMatStruct()->getName());
    _groupBoxMatStructLayout->addWidget(_lineEditNomMatstruct, 0, 0);

    _paroiLayout->addWidget(_groupBoxMatStruct, 2, 0);

    _groupBoxMatFace1 = new QGroupBox(this);
    _groupBoxMatFace1->setTitle(TR("id_matface1"));
    _groupBoxMatFace1Layout = new QGridLayout();
    _groupBoxMatFace1->setLayout(_groupBoxMatFace1Layout);

    _pushButtonMatFace1 = new QPushButton(_groupBoxMatFace1);
    _pushButtonMatFace1->setText(TR("id_proprietes_button"));
    _groupBoxMatFace1Layout->addWidget(_pushButtonMatFace1, 0, 1);

    _lineEditNomMatFace1 = new QLineEdit(_groupBoxMatFace1);
    _lineEditNomMatFace1->setEnabled(FALSE);
    _lineEditNomMatFace1->setText(getElement()->getMatFace1()->getName());
    _groupBoxMatFace1Layout->addWidget(_lineEditNomMatFace1, 0, 0);

    _paroiLayout->addWidget(_groupBoxMatFace1, 3, 0);

    _groupBoxMatFace2 = new QGroupBox(this);
    _groupBoxMatFace2->setTitle(TR("id_matface2"));
    _groupBoxMatFace2Layout = new QGridLayout();
    _groupBoxMatFace2->setLayout(_groupBoxMatFace2Layout);

    _lineEditNomMatFace2 = new QLineEdit(_groupBoxMatFace2);
    _lineEditNomMatFace2->setEnabled(FALSE);
    _lineEditNomMatFace2->setText(getElement()->getMatFace2()->getName());
    _groupBoxMatFace2Layout->addWidget(_lineEditNomMatFace2, 0, 0);

    _pushButtonMatFace2 = new QPushButton(_groupBoxMatFace2);
    _pushButtonMatFace2->setText(TR("id_proprietes_button"));
    _groupBoxMatFace2Layout->addWidget(_pushButtonMatFace2, 0, 1);

    _paroiLayout->addWidget(_groupBoxMatFace2, 4, 0);

    QGroupBox* buttonGroupBox = new QGroupBox(this);
    buttonGroupBox->setTitle(TR(""));
    QGridLayout* buttonGroupBoxLayout = new QGridLayout();
    buttonGroupBox->setLayout(buttonGroupBoxLayout);

    _pushButtonGetFromXML = new QPushButton(buttonGroupBox);
    _pushButtonGetFromXML->setText(TR("id_get_from_xml"));
    buttonGroupBoxLayout->addWidget(_pushButtonGetFromXML, 0, 1);

    _paroiLayout->addWidget(buttonGroupBox, 6, 0);

    connect(_pushButtonGetFromXML, SIGNAL(clicked()), this, SLOT(getFromXML()));
    connect(_pushButtonMatStruct, SIGNAL(clicked()), this, SLOT(editMatStruct()));
    connect(_pushButtonMatFace1, SIGNAL(clicked()), this, SLOT(editMatFace1()));
    connect(_pushButtonMatFace2, SIGNAL(clicked()), this, SLOT(editMatFace2()));

    updateContent();
}

TYParoiWidget::~TYParoiWidget()
{
}

void TYParoiWidget::updateContent()
{
    QString num;

    _elmW->updateContent();

    _lineEditNomMatFace2->setText(getElement()->getMatFace2()->getName());
    _lineEditNomMatFace1->setText(getElement()->getMatFace1()->getName());
    _lineEditNomMatstruct->setText(getElement()->getMatStruct()->getName());
    _lineEditEpaisseur->setText(num.setNum(getElement()->getEpaisseur(), 'f', 2));
}

void TYParoiWidget::apply()
{
    _elmW->apply();

    getElement()->setEpaisseur(_lineEditEpaisseur->text().toDouble());

    emit modified();
}

void TYParoiWidget::editMatStruct()
{
    int ret = getElement()->getMatStruct()->getSpectreTransm().edit(this);

    if (ret == QDialog::Accepted)
    {
        _lineEditNomMatstruct->setText(getElement()->getMatStruct()->getName());
    }
}

void TYParoiWidget::editMatFace1()
{
    // On augmente la precision d'affichage car les valeurs sont faibles
    int oldPrecision = TYSpectre::getXMLPrecision();
    TYSpectre::setXMLPrecision(3);

    int ret = getElement()->getMatFace1()->getSpectreAbso().edit(this);

    if (ret == QDialog::Accepted)
    {
        _lineEditNomMatFace1->setText(getElement()->getMatFace1()->getName());
    }

    // Le spectre est borne entre 0.001 et 200 pour eviter les plantage
    TYSpectre spectre = getElement()->getMatFace1()->getSpectreAbso().seuillage(0.001);
    getElement()->getMatFace1()->setSpectreAbso(spectre);


    // On remet la precision d'affichage a la valeur anterieure
    TYSpectre::setXMLPrecision(oldPrecision);
}

void TYParoiWidget::editMatFace2()
{
    // On augmente la precision d'affichage car les valeurs sont faibles
    int oldPrecision = TYSpectre::getXMLPrecision();
    TYSpectre::setXMLPrecision(3);

    int ret = getElement()->getMatFace2()->getSpectreAbso().edit(this);

    if (ret == QDialog::Accepted)
    {
        _lineEditNomMatFace2->setText(getElement()->getMatFace2()->getName());
    }

    // Le spectre est borne entre 0.001 et 0.999 pour eviter les plantage (10*log10(1-alpha))...
    TYSpectre spectre = getElement()->getMatFace2()->getSpectreAbso().seuillage(0.001, 0.999);
    getElement()->getMatFace2()->setSpectreAbso(spectre);


    // On remet la precision d'affichage a la valeur anterieure
    TYSpectre::setXMLPrecision(oldPrecision);
}

void TYParoiWidget::getFromXML()
{
    QString fileName = QFileDialog::getOpenFileName(this, "", "", "XML (*.xml)");

    if (!fileName.isEmpty())
    {
        TYXMLManager xmlManager;
        TYElement* pElt = NULL;
        LPTYElementArray eltList;

        LPTYParoi pParoi = NULL;

        xmlManager.load(fileName, eltList);

        size_t count = eltList.size();
        for (size_t i = 0 ; i < count; i++)
        {
            pElt = eltList.at(i);

            if ((pElt != NULL) && (pElt->isA("TYParoi")))
            {
                pParoi = (TYParoi*) pElt;

                if (pParoi != NULL)
                {
                    (*getElement()) = (*pParoi);
                    updateContent();
                }
            }
        }

        pElt = NULL;
    }
}

