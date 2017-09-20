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
 * \file TYRectangleWidget.cpp
 * \brief outil IHM pour un rectangle
 *
 *
 */





//Added by qt3to4:
#include <QGridLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "Tympan/models/business/OLocalizator.h"
#include "TYRectangleWidget.h"

#define TR(id) OLocalizator::getString("TYRectangleWidget", (id))

#include "Tympan/models/business/geometry/TYRectangle.h"
#include "Tympan/gui/widgets/TYSurfaceInterfaceWidget.h"


TYRectangleWidget::TYRectangleWidget(TYRectangle* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{

    _elmW = new TYElementWidget(pElement, this);
    _surfaceW = new TYSurfaceInterfaceWidget(pElement, this);

    resize(300, 350);
    setWindowTitle(TR("id_caption"));
    _rectangleLayout = new QGridLayout();
    setLayout(_rectangleLayout);
    _rectangleLayout->addWidget(_elmW, 0, 0);
    _rectangleLayout->addWidget(_surfaceW, 1, 0);

    _groupBox = new QGroupBox(this);
    _groupBox->setTitle(TR("id_pts"));
    _groupBoxLayout = new QGridLayout();
    _groupBox->setLayout(_groupBoxLayout);

    _listViewTabPt = new QTreeWidget(_groupBox);
    QStringList stringList;
    stringList.append(TR("id_x"));
    stringList.append(TR("id_y"));
    stringList.append(TR("id_z"));
    _listViewTabPt->setColumnCount(3);
    _listViewTabPt->setHeaderLabels(stringList);
    _listViewTabPt->setSelectionMode(QTreeWidget::NoSelection);
    _listViewTabPt->setRootIsDecorated(TRUE);

    _groupBoxLayout->addWidget(_listViewTabPt, 0, 0);

    _rectangleLayout->addWidget(_groupBox, 2, 0);

    updateContent();
}

TYRectangleWidget::~TYRectangleWidget()
{
}

void TYRectangleWidget::updateContent()
{
    _elmW->updateContent();
    _surfaceW->updateContent();

    _listViewTabPt->clear();
    for (int i = 0; i < 4; i++)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(_listViewTabPt, 0);
        item->setText(0, QString().setNum(getElement()->_pts[i]._x , 'f', 2));
        item->setText(1, QString().setNum(getElement()->_pts[i]._y , 'f', 2));
        item->setText(2, QString().setNum(getElement()->_pts[i]._z , 'f', 2));
    }
}

void TYRectangleWidget::apply()
{
    _elmW->apply();
    _surfaceW->apply();

    emit modified();
}

