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
 * \file TYInfrastructureWidget.cpp
 * \brief Outil IHM pour une infrastructure
 *
 *
 */



#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/infrastructure/TYInfrastructure.h"
#include <qmenu.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>


#include "TYInfrastructureWidget.h"

#define TR(id) OLocalizator::getString("TYInfrastructureWidget", (id))


TYInfrastructureWidget::TYInfrastructureWidget(TYInfrastructure* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{

    _elmW = new TYElementWidget(pElement, this);
    resize(300, 620);
    setWindowTitle(TR("id_caption"));
    _infrastructureLayout = new QGridLayout();
    setLayout(_infrastructureLayout);

    _infrastructureLayout->addWidget(_elmW, 0, 0);

    _groupBox = new QGroupBox(this);
    _groupBox->setTitle(TR("id_tabmachinerepere_box"));
    _groupBoxLayout = new QGridLayout();
    _groupBox->setLayout(_groupBoxLayout);

    _listViewList = new QTreeWidget(_groupBox);
    _listViewList->setColumnCount(2);
    QStringList stringList;
    stringList.append(TR(""));
    stringList.append(TR("id_machine"));
    _listViewList->setHeaderLabels(stringList);
    _listViewList->setRootIsDecorated(TRUE);

    _groupBoxLayout->addWidget(_listViewList, 0, 0);

    _infrastructureLayout->addWidget(_groupBox, 1, 0);

    _groupBox_3 = new QGroupBox(this);
    _groupBox_3->setTitle(TR("id_tabrestransprepere_box"));
    _groupBox_3Layout = new QGridLayout();
    _groupBox_3->setLayout(_groupBox_3Layout);

    _listViewList_6 = new QTreeWidget(_groupBox_3);
    _listViewList_6->setColumnCount(2);
    QStringList stringList6;
    stringList6.append(TR(""));
    stringList6.append(TR("id_restransp"));
    _listViewList_6->setHeaderLabels(stringList6);
    _listViewList_6->setRootIsDecorated(TRUE);

    _groupBox_3Layout->addWidget(_listViewList_6, 0, 0);

    _infrastructureLayout->addWidget(_groupBox_3, 3, 0);

    _groupBox_4 = new QGroupBox(this);
    _groupBox_4->setTitle(TR("id_tabrouterepere_box"));
    _groupBox_4Layout = new QGridLayout();
    _groupBox_4->setLayout(_groupBox_4Layout);

    _listViewList_7 = new QTreeWidget(_groupBox_4);
    _listViewList_7->setColumnCount(2);
    QStringList stringList7;
    stringList7.append(TR(""));
    stringList7.append(TR("id_route"));
    _listViewList_7->setHeaderLabels(stringList7);
    _listViewList_7->setRootIsDecorated(TRUE);

    _groupBox_4Layout->addWidget(_listViewList_7, 0, 0);

    _infrastructureLayout->addWidget(_groupBox_4, 4, 0);

    _groupBox_2 = new QGroupBox(this);
    _groupBox_2->setTitle(TR("id_tabbatimentrepere_box"));
    _groupBox_2Layout = new QGridLayout();
    _groupBox_2->setLayout(_groupBox_2Layout);

    _listViewList_5 = new QTreeWidget(_groupBox_2);
    _listViewList_5->setColumnCount(2);
    QStringList stringList5;
    stringList5.append(TR(""));
    stringList5.append(TR("id_batiment"));
    _listViewList_5->setHeaderLabels(stringList5);
    _listViewList_5->setRootIsDecorated(TRUE);

    _groupBox_2Layout->addWidget(_listViewList_5, 0, 0);

    _infrastructureLayout->addWidget(_groupBox_2, 2, 0);

    _groupBox_1 = new QGroupBox(this);
    _groupBox_1->setTitle(TR("id_tabsource_box"));
    _groupBox_1Layout = new QGridLayout();
    _groupBox_1->setLayout(_groupBox_1Layout);

    _listViewList_4 = new QTreeWidget(_groupBox_1);
    _listViewList_4->setColumnCount(2);
    QStringList stringList4;
    stringList4.append(TR(""));
    stringList4.append(TR("id_source"));
    _listViewList_4->setHeaderLabels(stringList4);
    _listViewList_4->setRootIsDecorated(TRUE);

    _groupBox_1Layout->addWidget(_listViewList_4, 0, 0);

    _infrastructureLayout->addWidget(_groupBox_1, 5, 0);

    updateContent();
}

TYInfrastructureWidget::~TYInfrastructureWidget()
{
}

void TYInfrastructureWidget::updateContent()
{
    unsigned int i;
    _elmW->updateContent();

    _listViewList_5->clear();
    for (i = 0; i < getElement()->getListBatiment().size(); i++)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(_listViewList_5);
        item->setText(0, QString().setNum(i));
        item->setText(1, getElement()->getListBatiment()[i]->getElement()->getName());
    }

    _listViewList->clear();
    for (i = 0; i < getElement()->getListMachine().size(); i++)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(_listViewList);
        item->setText(0, QString().setNum(i));
        item->setText(1, getElement()->getListMachine()[i]->getElement()->getName());
    }

    _listViewList_6->clear();
    for (i = 0; i < getElement()->getListResTrans().size(); i++)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(_listViewList_6);
        item->setText(0, QString().setNum(i));
        item->setText(1, getElement()->getListResTrans()[i]->getElement()->getName());
    }
#if WITH_NMPB
    _listViewList_7->clear();
    for (i = 0; i < getElement()->getListRoute().size(); i++)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(_listViewList_7);
        item->setText(0, QString().setNum(i));
        item->setText(1, getElement()->getListRoute()[i]->getElement()->getName());
    }
#endif

    _listViewList_4->clear();
    for (i = 0; i < getElement()->getSrcs().size(); i++)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(_listViewList_4);
        item->setText(0, QString().setNum(i));
        item->setText(1, getElement()->getSrcs()[i]->getElement()->getName());
    }

}

void TYInfrastructureWidget::apply()
{
    _elmW->apply();

    emit modified();
}

void TYInfrastructureWidget::editBatiment(QTreeWidgetItem* item)
{
    int ret = getElement()->getListBatiment()[item->text(0).toInt()]->getElement()->edit(this);

    if (ret == QDialog::Accepted)
    {
        _listViewList_5->clear();
        for (unsigned int i = 0; i < getElement()->getListBatiment().size(); i++)
        {
            QTreeWidgetItem* item = new QTreeWidgetItem(_listViewList_5);
            item->setText(0, QString().setNum(i));
            item->setText(1, getElement()->getListBatiment()[i]->getElement()->getName());
        }
    }

}

void TYInfrastructureWidget::editMachine(QTreeWidgetItem* item)
{
    int ret = getElement()->getListMachine()[item->text(0).toInt()]->getElement()->edit(this);

    if (ret == QDialog::Accepted)
    {
        _listViewList->clear();
        for (unsigned int i = 0; i < getElement()->getListMachine().size(); i++)
        {
            QTreeWidgetItem* item = new QTreeWidgetItem(_listViewList);
            item->setText(0, QString().setNum(i));
            item->setText(1, getElement()->getListMachine()[i]->getElement()->getName());
        }
    }
}

void TYInfrastructureWidget::editResTransp(QTreeWidgetItem* item)
{
    int ret = getElement()->getListResTrans()[item->text(0).toInt()]->getElement()->edit(this);

    if (ret == QDialog::Accepted)
    {
        _listViewList_6->clear();
        for (unsigned int i = 0; i < getElement()->getListResTrans().size(); i++)
        {
            QTreeWidgetItem* item = new QTreeWidgetItem(_listViewList_6);
            item->setText(0, QString().setNum(i));
            item->setText(1, getElement()->getListResTrans()[i]->getElement()->getName());
        }
    }
}

#if WITH_NMPB
void TYInfrastructureWidget::editRoute(QTreeWidgetItem* item)
{
    int ret = getElement()->getListRoute()[item->text(0).toInt()]->getElement()->edit(this);

    if (ret == QDialog::Accepted)
    {
        _listViewList_7->clear();
        for (unsigned int i = 0; i < getElement()->getListRoute().size(); i++)
        {
            QTreeWidgetItem* item = new QTreeWidgetItem(_listViewList_7);
            item->setText(0, QString().setNum(i));
            item->setText(1, getElement()->getListRoute()[i]->getElement()->getName());
        }
    }
}
#endif

void TYInfrastructureWidget::editSource(QTreeWidgetItem* item)
{
    int ret = getElement()->getSrcs()[item->text(0).toInt()]->getElement()->edit(this);

    if (ret == QDialog::Accepted)
    {
        _listViewList_4->clear();
        for (unsigned int i = 0; i < getElement()->getSrcs().size(); i++)
        {
            QTreeWidgetItem* item = new QTreeWidgetItem(_listViewList_4);
            item->setText(0, QString().setNum(i));
            item->setText(1, getElement()->getSrcs()[i]->getElement()->getName());
        }
    }
}

void TYInfrastructureWidget::contextMenuEvent(QContextMenuEvent* e)
{
    //si on trouve un item associe
    QPoint point = _listViewList->mapFrom(this, e->pos());
    QTreeWidgetItem* item = _listViewList->itemAt(point.x(), point.y() - _listViewList->header()->height());
    QPoint point4 = _listViewList_4->mapFrom(this, e->pos());
    QTreeWidgetItem* item4 = _listViewList_4->itemAt(point4.x(), point4.y() - _listViewList_4->header()->height());
    QPoint point5 = _listViewList_5->mapFrom(this, e->pos());
    QTreeWidgetItem* item5 = _listViewList_5->itemAt(point5.x(), point5.y() - _listViewList_5->header()->height());
    QPoint point6 = _listViewList_6->mapFrom(this, e->pos());
    QTreeWidgetItem* item6 = _listViewList_6->itemAt(point6.x(), point6.y() - _listViewList_6->header()->height());
    QPoint point7 = _listViewList_7->mapFrom(this, e->pos());
    QTreeWidgetItem* item7 = _listViewList_7->itemAt(point7.x(), point7.y() - _listViewList_7->header()->height());
    if ((item != NULL) || (item4 != NULL) || (item5 != NULL) || (item6 != NULL) || (item7 != NULL))
    {
        QMenu* pMenu = new QMenu(this);

        QAction* prop = pMenu->addAction(TR("id_property"));
        if (item != NULL)
        {
            QAction* ret = pMenu->exec(_listViewList->mapToGlobal(point));
            if ((ret) && (ret == prop)) { editMachine(item); }
        }
        else if (item4 != NULL)
        {
            QAction* ret = pMenu->exec(_listViewList_4->mapToGlobal(point4));
            if ((ret) && (ret == prop)) { editSource(item4); }
        }
        else if (item5 != NULL)
        {
            QAction* ret = pMenu->exec(_listViewList_5->mapToGlobal(point5));
            if ((ret) && (ret == prop)) { editBatiment(item5); }
        }
        else if (item6 != NULL)
        {
            QAction* ret = pMenu->exec(_listViewList_6->mapToGlobal(point6));
            if ((ret) && (ret == prop)) { editResTransp(item6); }
        }
#if WITH_NMPB
        else if (item7 != NULL)
        {
            QAction* ret = pMenu->exec(_listViewList_7->mapToGlobal(point7));
            if ((ret) && (ret == prop)) { editRoute(item7); }
        }
#endif
    }
}
