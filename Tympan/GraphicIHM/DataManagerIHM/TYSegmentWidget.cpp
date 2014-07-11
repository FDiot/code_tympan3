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
 * \file TYSegmentWidget.cpp
 * \brief outil IHM pour un segment
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

//Added by qt3to4:
#include <QGridLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#define TR(id) OLocalizator::getString("TYSegmentWidget", (id))

#include "Tympan/GraphicIHM/DataManagerIHM/TYSegmentWidget.h"
#include "Tympan/GraphicIHM/DataManagerIHM/TYColorInterfaceWidget.h"
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/models/business/geometry/TYSegment.h"


TYSegmentWidget::TYSegmentWidget(TYSegment* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{

    _elmW = new TYElementWidget(pElement, this);
    _colorW = new TYColorInterfaceWidget(pElement, this);

    resize(300, 200);
    setWindowTitle(TR("id_caption"));
    _segmentLayout = new QGridLayout();
    setLayout(_segmentLayout);

    _segmentLayout->addWidget(_elmW, 0, 0);
    _segmentLayout->addWidget(_colorW, 1, 0);

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

    QTreeWidgetItem* itemA = new QTreeWidgetItem(_listViewTabPt, 0);
    itemA->setText(0, QString().setNum(getElement()->_ptA._x, 'f', 2));
    itemA->setText(1, QString().setNum(getElement()->_ptA._y, 'f', 2));
    itemA->setText(2, QString().setNum(getElement()->_ptA._z, 'f', 2));
    QTreeWidgetItem* itemB = new QTreeWidgetItem(_listViewTabPt, 0);
    itemB->setText(0, QString().setNum(getElement()->_ptB._x, 'f', 2));
    itemB->setText(1, QString().setNum(getElement()->_ptB._y, 'f', 2));
    itemB->setText(2, QString().setNum(getElement()->_ptB._z, 'f', 2));
    _listViewTabPt->setEnabled(false);

    _listViewTabPt->setRootIsDecorated(TRUE);
    _groupBoxLayout->addWidget(_listViewTabPt, 0, 0);

    _segmentLayout->addWidget(_groupBox, 2, 0);
}

TYSegmentWidget::~TYSegmentWidget()
{
}

void TYSegmentWidget::updateContent()
{
    _elmW->updateContent();
    _colorW->updateContent();

    _listViewTabPt->clear();
    QTreeWidgetItem* itemA = new QTreeWidgetItem(_listViewTabPt, 0);
    itemA->setText(0, QString().setNum(getElement()->_ptA._x, 'f', 2));
    itemA->setText(1, QString().setNum(getElement()->_ptA._y, 'f', 2));
    itemA->setText(2, QString().setNum(getElement()->_ptA._z, 'f', 2));
    QTreeWidgetItem* itemB = new QTreeWidgetItem(_listViewTabPt, 0);
    itemB->setText(0, QString().setNum(getElement()->_ptB._x, 'f', 2));
    itemB->setText(1, QString().setNum(getElement()->_ptB._y, 'f', 2));
    itemB->setText(2, QString().setNum(getElement()->_ptB._z, 'f', 2));
}

void TYSegmentWidget::apply()
{
    _elmW->apply();
    _colorW->apply();

    emit modified();
}
