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
 * \file TYDirectivityWidget.cpp
 * \brief outil IHM pourla directivite
 *
 *
 */


#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/acoustic/TYDirectivity.h"

#include <qmenu.h>
#include <qtablewidget.h>

//Added by qt3to4:
#include <QGridLayout>
#include <QHeaderView>


#include "TYDirectivityWidget.h"

#define TR(id) OLocalizator::getString("TYDirectivityWidget", (id))


TYDirectivityWidget::TYDirectivityWidget(TYDirectivity* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{
    resize(300, 240);
    setWindowTitle(TR("id_caption"));

    _elmW = new TYElementWidget(pElement, this);
    _table = new QTableWidget();
    _table->setColumnCount(3);
    _table->setContextMenuPolicy(Qt::CustomContextMenu);
    _table->setHorizontalHeaderItem(0, new QTableWidgetItem(TR("id_theta")));
    _table->setHorizontalHeaderItem(1, new QTableWidgetItem(TR("id_phi")));
    _table->setHorizontalHeaderItem(2, new QTableWidgetItem(TR("id_value")));

    QGridLayout* groupBoxLayout = new QGridLayout();
    groupBoxLayout->addWidget(_table, 1, 0);

    QGroupBox* groupBox = new QGroupBox();
    groupBox->setTitle(TR("id_tabangleval_box"));
    groupBox->setLayout(groupBoxLayout);

    _directiviteLayout = new QGridLayout();
    _directiviteLayout->addWidget(_elmW, 0, 0);
    _directiviteLayout->addWidget(groupBox, 1, 0);
    setLayout(_directiviteLayout);

    updateContent();

    connect(_table, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(showContextMenu(const QPoint&)));
    connect(_table, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(apply()));
}


TYDirectivityWidget::~TYDirectivityWidget()
{
}

void TYDirectivityWidget::updateContent()
{
    _elmW->updateContent();

    for (int i = 0; i < _table->rowCount(); i++)
    {
        _table->takeItem(i, 0);
        _table->takeItem(i, 1);
    }
    _table->setRowCount(0);
}

void TYDirectivityWidget::apply()
{
    _elmW->apply();
    emit modified();
}

void TYDirectivityWidget::showContextMenu(const QPoint& pos)
{
    QTableWidgetItem* item = _table->itemAt(pos);
    if (item)
    {
        int row = _table->row(item);
        QMenu* pMenu = new QMenu(this);
        QAction* pAddAction = pMenu->addAction(TR("id_add"));

        //QAction* pDelAction = NULL;

        if (row >= 0)
        {
            pMenu->addSeparator();
            //pDelAction = pMenu->addAction(TR("id_del"));
            pMenu->addAction(TR("id_del"));
        }

        QAction* pRet = pMenu->exec(pos);
    }
}

