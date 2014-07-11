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
 * \file TYSourceLineicWidget.cpp
 * \brief outil IHM pour une source lineique
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/models/business/acoustique/TYSourceLineic.h"
#include <qmenu.h>

//Added by qt3to4:
#include <QGridLayout>
#include <QTreeWidget>
#include <QLabel>
#include <QHeaderView>

#define TR(id) OLocalizator::getString("TYSourceLineicWidget", (id))


TYSourceLineicWidget::TYSourceLineicWidget(TYSourceLineic* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{

    _elmW = new TYSourceWidget(pElement, this);

    resize(300, 444);
    setWindowTitle(TR("id_caption"));
    _sourceLineicLayout = new QGridLayout();

    _sourceLineicLayout->addWidget(_elmW, 0, 0);

    _groupBoxEdit = new QGroupBox(this);
    _groupBoxEdit->setTitle(TR(""));
    _groupBoxEditLayout = new QGridLayout();
    _groupBoxEdit->setLayout(_groupBoxEditLayout);

    _lineEditDensiteSrcs = new QLineEdit(_groupBoxEdit);
    _groupBoxEditLayout->addWidget(_lineEditDensiteSrcs, 0, 1);
    QLabel* pUnitDensite = new QLabel(_groupBoxEdit);
    pUnitDensite->setText(TR("id_unite_densite_srcs"));
    _groupBoxEditLayout->addWidget(pUnitDensite, 0, 2);

    _labelDensiteSrcs = new QLabel(_groupBoxEdit);
    _labelDensiteSrcs->setText(TR("id_densitessrcs_label"));
    _groupBoxEditLayout->addWidget(_labelDensiteSrcs, 0, 0);

    _labeSrcsCor = new QLabel(_groupBoxEdit);
    _labeSrcsCor->setText(TR("id_srcscor_label"));
    _groupBoxEditLayout->addWidget(_labeSrcsCor, 1, 0);

    _checkBoxSrcsCor = new QCheckBox(_groupBoxEdit);
    _checkBoxSrcsCor->setText(TR(""));
    _groupBoxEditLayout->addWidget(_checkBoxSrcsCor, 1, 1);

    _sourceLineicLayout->addWidget(_groupBoxEdit, 1, 0);

    _groupBoxListSrc = new QGroupBox(this);
    _groupBoxListSrc->setTitle(TR("id_tabsrc_box"));
    _groupBoxListSrcLayout = new QGridLayout();
    _groupBoxListSrc->setLayout(_groupBoxListSrcLayout);

    _listViewListSrc = new QTreeWidget(_groupBoxListSrc);
    _listViewListSrc->setColumnCount(2);
    QStringList list;
    list.append(TR(""));
    list.append(TR("id_source"));
    _listViewListSrc->setHeaderLabels(list);
    //   _listViewListSrc->addColumn( TR( "" ) );
    //   _listViewListSrc->addColumn( TR( "id_source" ) );

    _listViewListSrc->setRootIsDecorated(TRUE);

    _groupBoxListSrcLayout->addWidget(_listViewListSrc, 0, 0);

    _sourceLineicLayout->addWidget(_groupBoxListSrc, 2, 0);

    updateContent();

    setLayout(_sourceLineicLayout);
}

TYSourceLineicWidget::~TYSourceLineicWidget()
{
}

void TYSourceLineicWidget::updateContent()
{
    QString num;

    _elmW->updateContent();

    _lineEditDensiteSrcs->setText(num.setNum(getElement()->getDensiteSrcs(), 'f', 2));
    _checkBoxSrcsCor->setChecked(getElement()->getSrcsCorr());

    _listViewListSrc->clear();
    for (unsigned int i = 0; i < getElement()->getNbSrcs(); i++)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(_listViewListSrc, 0);
        item->setText(0,  num.setNum(i));
        item->setText(1, getElement()->getSrc(i)->getName());
    }
}

void TYSourceLineicWidget::apply()
{
    _elmW->apply();

    getElement()->setSrcsCorr(_checkBoxSrcsCor->isChecked());
    getElement()->setDensiteSrcs(_lineEditDensiteSrcs->text().toDouble());

    emit modified();
}

void TYSourceLineicWidget::editSource(QTreeWidgetItem* item)
{
    int ret = getElement()->getSrc(item->text(0).toInt())->edit(this);

    if (ret == QDialog::Accepted)
    {
        _listViewListSrc->clear();
        for (unsigned int i = 0; i < getElement()->getNbSrcs(); i++)
        {
            QTreeWidgetItem* item = new QTreeWidgetItem(_listViewListSrc, 0);
            item->setText(0,  QString().setNum(i));
            item->setText(1, getElement()->getSrc(i)->getName());
        }
    }

}

void TYSourceLineicWidget::contextMenuEvent(QContextMenuEvent* e)
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
