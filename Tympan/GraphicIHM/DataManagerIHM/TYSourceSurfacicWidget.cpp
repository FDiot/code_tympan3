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
 * \file TYSourceSurfacicWidget.cpp
 * \brief outil IHM pour une source surfacique
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/models/business/acoustique/TYSourceSurfacic.h"
#include <qmenu.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>

#define TR(id) OLocalizator::getString("TYSourceSurfacicWidget", (id))


TYSourceSurfacicWidget::TYSourceSurfacicWidget(TYSourceSurfacic* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{

    _elmW = new TYSourceWidget(pElement, this);

    resize(300, 463);
    setWindowTitle(TR("id_caption"));
    _sourceSurfacicLayout = new QGridLayout();
    setLayout(_sourceSurfacicLayout);

    _sourceSurfacicLayout->addWidget(_elmW, 0, 0);

    _groupBox = new QGroupBox(this);
    _groupBox->setTitle(TR(""));
    _groupBoxLayout = new QGridLayout();
    _groupBox->setLayout(_groupBoxLayout);

    _labeSrcsCor = new QLabel(_groupBox);
    _labeSrcsCor->setText(TR("id_srcscor_label"));
    _groupBoxLayout->addWidget(_labeSrcsCor, 2, 0);

    _checkBoxSrcsCor = new QCheckBox(_groupBox);
    _checkBoxSrcsCor->setText(TR(""));
    _groupBoxLayout->addWidget(_checkBoxSrcsCor, 2, 1);

    _labelDensiteSrcsH = new QLabel(_groupBox);
    _labelDensiteSrcsH->setText(TR("id_densitessrcsH_label"));
    _groupBoxLayout->addWidget(_labelDensiteSrcsH, 0, 0);

    _lineEditDensiteSrcsH = new QLineEdit(_groupBox);
    _groupBoxLayout->addWidget(_lineEditDensiteSrcsH, 0, 1);
    QLabel* pUnitDensiteH = new QLabel(_groupBox);
    pUnitDensiteH->setText(TR("id_unite_densite_srcs"));
    _groupBoxLayout->addWidget(pUnitDensiteH, 0, 2);

    _lineEditDensiteV = new QLineEdit(_groupBox);
    _groupBoxLayout->addWidget(_lineEditDensiteV, 1, 1);
    QLabel* pUnitDensiteV = new QLabel(_groupBox);
    pUnitDensiteV->setText(TR("id_unite_densite_srcs"));
    _groupBoxLayout->addWidget(pUnitDensiteV, 1, 2);

    _labelDensiteSrcsV = new QLabel(_groupBox);
    _labelDensiteSrcsV->setText(TR("id_densitessrcsV_label"));
    _groupBoxLayout->addWidget(_labelDensiteSrcsV, 1, 0);

    _sourceSurfacicLayout->addWidget(_groupBox, 1, 0);

    _groupBoxListSrc = new QGroupBox(this);
    _groupBoxListSrc->setTitle(TR("id_tabsrc_box"));
    _groupBoxListSrcLayout = new QGridLayout();
    _groupBoxListSrc->setLayout(_groupBoxListSrcLayout);

    _listViewListSrc = new QTreeWidget(_groupBoxListSrc);
    QStringList stringList;
    stringList.append(TR(""));
    stringList.append(TR("id_source"));
    _listViewListSrc->setColumnCount(2);
    _listViewListSrc->setHeaderLabels(stringList);
    _listViewListSrc->setRootIsDecorated(TRUE);

    _groupBoxListSrcLayout->addWidget(_listViewListSrc, 0, 0);

    _sourceSurfacicLayout->addWidget(_groupBoxListSrc, 2, 0);

    updateContent();
}


TYSourceSurfacicWidget::~TYSourceSurfacicWidget()
{
}

void TYSourceSurfacicWidget::updateContent()
{
    QString num;

    _elmW->updateContent();

    _lineEditDensiteV->setText(num.setNum(getElement()->getDensiteSrcsV(), 'f', 2));
    _lineEditDensiteSrcsH->setText(num.setNum(getElement()->getDensiteSrcsH(), 'f', 2));
    _checkBoxSrcsCor->setChecked(getElement()->getSrcsCorr());

    _listViewListSrc->clear();
    for (unsigned int i = 0; i < getElement()->getNbSrcs(); i++)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(_listViewListSrc, 0);
        item->setText(0,  QString().setNum(i));
        item->setText(1, getElement()->getSrcs().at(i)->getName());
    }

}

void TYSourceSurfacicWidget::apply()
{
    _elmW->apply();

    getElement()->setSrcsCorr(_checkBoxSrcsCor->isChecked());
    getElement()->setDensiteSrcsV(_lineEditDensiteV->text().toDouble());
    getElement()->setDensiteSrcsH(_lineEditDensiteSrcsH->text().toDouble());

    emit modified();
}

void TYSourceSurfacicWidget::editSource(QTreeWidgetItem* item)
{
    int ret = getElement()->getSrc(item->text(0).toInt())->edit(this);

    if (ret == QDialog::Accepted)
    {
        updateContent();
    }
}

void TYSourceSurfacicWidget::contextMenuEvent(QContextMenuEvent* e)
{
    //si on trouve un item associe
    QPoint point = _listViewListSrc->mapFrom(this, e->pos());
    QTreeWidgetItem* item = _listViewListSrc->itemAt(point.x(), point.y() - _listViewListSrc->header()->height());

    if (item != NULL)
    {
        QMenu* pMenu = new QMenu(this);

        QAction* prop = pMenu->addAction(TR("id_property"));
        QAction* ret = pMenu->exec(_listViewListSrc->mapToGlobal(point));

        if (ret == prop)
        {
            editSource(item);
        }
    }
}
