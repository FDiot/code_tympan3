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
 * \file TYRepereWidget.cpp
 * \brief Outil IHM pour un repere
 *
 *
 */



#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include <stdlib.h>
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYRepere.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>


#define TR(id) OLocalizator::getString("TYRepereWidget", (id))


TYRepereWidget::TYRepereWidget(TYRepere* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(_pParent)
{
    QString buf;

    Q_ASSERT(pElement);
    _pElement = pElement;

    elmW = new TYElementWidget(getElement(), this);

    resize(300, 200);
    setWindowTitle(TR("id_caption"));
    RepereLayout = new QGridLayout();
    setLayout(RepereLayout);

    RepereLayout->addWidget(elmW, 0, 0, 1, 2);

    GroupBoxOrigine = new QGroupBox(this);
    GroupBoxOrigine->setTitle(TR("id_origine"));
    GroupBoxOrigineLayout = new QGridLayout();
    GroupBoxOrigine->setLayout(GroupBoxOrigineLayout);

    labelX = new QLabel(GroupBoxOrigine);
    labelX->setText(TR("id_x_label"));
    GroupBoxOrigineLayout->addWidget(labelX, 0, 0);

    labelZ = new QLabel(GroupBoxOrigine);
    labelZ->setText(TR("id_z_label"));
    GroupBoxOrigineLayout->addWidget(labelZ, 2, 0);

    labelY = new QLabel(GroupBoxOrigine);
    labelY->setText(TR("id_y_label"));
    GroupBoxOrigineLayout->addWidget(labelY, 1, 0);

    lineEditX = new QLineEdit(GroupBoxOrigine);
    lineEditX->setText(buf.setNum(getElement()->_origin._x, 'g', 10));
    GroupBoxOrigineLayout->addWidget(lineEditX, 0, 1);

    lineEditY = new QLineEdit(GroupBoxOrigine);
    lineEditY->setText(buf.setNum(getElement()->_origin._y, 'g', 10));
    GroupBoxOrigineLayout->addWidget(lineEditY, 1, 1);

    lineEditZ = new QLineEdit(GroupBoxOrigine);
    lineEditZ->setText(buf.setNum(getElement()->_origin._z, 'g', 10));
    GroupBoxOrigineLayout->addWidget(lineEditZ, 2, 1);

    RepereLayout->addWidget(GroupBoxOrigine, 1, 0);

    GroupBoxVecI = new QGroupBox(this);
    GroupBoxVecI->setTitle(TR("id_veci"));
    GroupBoxVecILayout = new QGridLayout();
    GroupBoxVecI->setLayout(GroupBoxVecILayout);

    labelX_2 = new QLabel(GroupBoxVecI);
    labelX_2->setText(TR("id_x_label"));
    GroupBoxVecILayout->addWidget(labelX_2, 0, 0);

    labelZ_2 = new QLabel(GroupBoxVecI);
    labelZ_2->setText(TR("id_z_label"));
    GroupBoxVecILayout->addWidget(labelZ_2, 2, 0);

    labelY_2 = new QLabel(GroupBoxVecI);
    labelY_2->setText(TR("id_y_label"));
    GroupBoxVecILayout->addWidget(labelY_2, 1, 0);

    lineEditX_2 = new QLineEdit(GroupBoxVecI);
    lineEditX_2->setText(buf.setNum(getElement()->_vecI._x, 'g', 10));
    GroupBoxVecILayout->addWidget(lineEditX_2, 0, 1);

    lineEditY_2 = new QLineEdit(GroupBoxVecI);
    lineEditY_2->setText(buf.setNum(getElement()->_vecI._y, 'g', 10));
    GroupBoxVecILayout->addWidget(lineEditY_2, 1, 1);

    lineEditZ_2 = new QLineEdit(GroupBoxVecI);
    lineEditZ_2->setText(buf.setNum(getElement()->_vecI._z, 'g', 10));
    GroupBoxVecILayout->addWidget(lineEditZ_2, 2, 1);

    RepereLayout->addWidget(GroupBoxVecI, 1, 1);

    GroupBoxVecK = new QGroupBox(this);
    GroupBoxVecK->setTitle(TR("id_vecj"));
    GroupBoxVecKLayout = new QGridLayout();
    GroupBoxVecK->setLayout(GroupBoxVecKLayout);

    labelX_4 = new QLabel(GroupBoxVecK);
    labelX_4->setText(TR("id_x_label"));
    GroupBoxVecKLayout->addWidget(labelX_4, 0, 0);

    labelZ_4 = new QLabel(GroupBoxVecK);
    labelZ_4->setText(TR("id_z_label"));
    GroupBoxVecKLayout->addWidget(labelZ_4, 2, 0);

    labelY_4 = new QLabel(GroupBoxVecK);
    labelY_4->setText(TR("id_y_label"));
    GroupBoxVecKLayout->addWidget(labelY_4, 1, 0);

    lineEditX_4 = new QLineEdit(GroupBoxVecK);
    lineEditX_4->setText(buf.setNum(getElement()->_vecJ._x, 'g', 10));
    GroupBoxVecKLayout->addWidget(lineEditX_4, 0, 1);

    lineEditY_4 = new QLineEdit(GroupBoxVecK);
    lineEditY_4->setText(buf.setNum(getElement()->_vecJ._y, 'g', 10));
    GroupBoxVecKLayout->addWidget(lineEditY_4, 1, 1);

    lineEditZ_4 = new QLineEdit(GroupBoxVecK);
    lineEditZ_4->setText(buf.setNum(getElement()->_vecJ._z, 'g', 10));
    GroupBoxVecKLayout->addWidget(lineEditZ_4, 2, 1);

    RepereLayout->addWidget(GroupBoxVecK, 2, 1);

    GroupBoxVecJ = new QGroupBox(this);
    GroupBoxVecJ->setTitle(TR("id_veck"));
    GroupBoxVecJLayout = new QGridLayout();
    GroupBoxVecJ->setLayout(GroupBoxVecJLayout);

    labelX_3 = new QLabel(GroupBoxVecJ);
    labelX_3->setText(TR("id_x_label"));
    GroupBoxVecJLayout->addWidget(labelX_3, 0, 0);

    labelZ_3 = new QLabel(GroupBoxVecJ);
    labelZ_3->setText(TR("id_z_label"));
    GroupBoxVecJLayout->addWidget(labelZ_3, 2, 0);

    labelY_3 = new QLabel(GroupBoxVecJ);
    labelY_3->setText(TR("id_y_label"));
    GroupBoxVecJLayout->addWidget(labelY_3, 1, 0);

    lineEditX_3 = new QLineEdit(GroupBoxVecJ);
    lineEditX_3->setText(buf.setNum(getElement()->_vecK._x, 'g', 10));
    GroupBoxVecJLayout->addWidget(lineEditX_3, 0, 1);

    lineEditY_3 = new QLineEdit(GroupBoxVecJ);
    lineEditY_3->setText(buf.setNum(getElement()->_vecK._y, 'g', 10));
    GroupBoxVecJLayout->addWidget(lineEditY_3, 1, 1);

    lineEditZ_3 = new QLineEdit(GroupBoxVecJ);
    lineEditZ_3->setText(buf.setNum(getElement()->_vecK._z, 'g', 10));
    GroupBoxVecJLayout->addWidget(lineEditZ_3, 2, 1);

    RepereLayout->addWidget(GroupBoxVecJ, 2, 0);
}

TYRepereWidget::~TYRepereWidget()
{
}


void TYRepereWidget::updateContent()
{
    QString buf;

    elmW->updateContent();

    lineEditZ_3->setText(buf.setNum(getElement()->_vecK._z, 'g', 10));
    lineEditX_3->setText(buf.setNum(getElement()->_vecK._x, 'g', 10));
    lineEditY_3->setText(buf.setNum(getElement()->_vecK._y, 'g', 10));

    lineEditZ_4->setText(buf.setNum(getElement()->_vecJ._z, 'g', 10));
    lineEditX_4->setText(buf.setNum(getElement()->_vecJ._x, 'g', 10));
    lineEditY_4->setText(buf.setNum(getElement()->_vecJ._y, 'g', 10));

    lineEditZ_2->setText(buf.setNum(getElement()->_vecI._z, 'g', 10));
    lineEditX_2->setText(buf.setNum(getElement()->_vecI._x, 'g', 10));
    lineEditY_2->setText(buf.setNum(getElement()->_vecI._y, 'g', 10));

    lineEditZ->setText(buf.setNum(getElement()->_origin._z, 'g', 10));
    lineEditX->setText(buf.setNum(getElement()->_origin._x, 'g', 10));
    lineEditY->setText(buf.setNum(getElement()->_origin._y, 'g', 10));

}

void TYRepereWidget::apply()
{
    elmW->apply();

    getElement()->_origin._x = lineEditX->text().toDouble();
    getElement()->_origin._y = lineEditY->text().toDouble();
    getElement()->_origin._z = lineEditZ->text().toDouble();

    getElement()->_vecI._x = lineEditX_2->text().toDouble();
    getElement()->_vecI._y = lineEditY_2->text().toDouble();
    getElement()->_vecI._z = lineEditZ_2->text().toDouble();

    getElement()->_vecJ._x = lineEditX_4->text().toDouble();
    getElement()->_vecJ._y = lineEditY_4->text().toDouble();
    getElement()->_vecJ._z = lineEditZ_4->text().toDouble();

    getElement()->_vecK._x = lineEditX_3->text().toDouble();
    getElement()->_vecK._y = lineEditY_3->text().toDouble();
    getElement()->_vecK._z = lineEditZ_3->text().toDouble();

    emit modified();
}
