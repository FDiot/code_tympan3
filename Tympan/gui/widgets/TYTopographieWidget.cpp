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
 * \file TYTopographieWidget.cpp
 * \brief outil IHM pour une topographie
 *
 *
 */





#include "Tympan/TYVersion.h"
#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/infrastructure/TYTopographie.h"
#include <qmenu.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>


#define TR(id) OLocalizator::getString("TYTopographieWidget", (id))


TYTopographieWidget::TYTopographieWidget(TYTopographie* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{

    _elmW = new TYElementWidget(pElement, this);
    resize(300, 797);
    setWindowTitle(TR("id_caption"));
    _topographieLayout = new QGridLayout();
    setLayout(_topographieLayout);

    _topographieLayout->addWidget(_elmW, 0, 0);

    // Terrain par defaut
    _groupBoxTerrainDef = new QGroupBox(this);
    _groupBoxTerrainDef->setTitle(TR("id_terraindef"));
    _groupBoxTerrainDefLayout = new QGridLayout();
    _groupBoxTerrainDef->setLayout(_groupBoxTerrainDefLayout);

    _pushButtonTerrainDef = new QPushButton(_groupBoxTerrainDef);
    _pushButtonTerrainDef->setText(TR("id_proprietes_button"));
    _groupBoxTerrainDefLayout->addWidget(_pushButtonTerrainDef, 0, 1);

    _lineEditNomTerrainDef = new QLineEdit(_groupBoxTerrainDef);
    _lineEditNomTerrainDef->setEnabled(FALSE);
    _groupBoxTerrainDefLayout->addWidget(_lineEditNomTerrainDef, 0, 0);

    _topographieLayout->addWidget(_groupBoxTerrainDef, 2, 0);


    // Cours d'eau
    _groupBox_2 = new QGroupBox(this);
    _groupBox_2->setTitle(TR("id_tabcrseaurepere_box"));
    _groupBox_2Layout = new QGridLayout();
    _groupBox_2->setLayout(_groupBox_2Layout);

    _listView_5 = new QTreeWidget(_groupBox_2);
    _listView_5->setColumnCount(2);
    QStringList stringList;
    stringList.append(TR(""));
    stringList.append(TR("id_crseau"));
    _listView_5->setHeaderLabels(stringList);
    _listView_5->setRootIsDecorated(TRUE);

    _groupBox_2Layout->addWidget(_listView_5, 0, 0);

    _topographieLayout->addWidget(_groupBox_2, 3, 0);


    // Plan d'eau
    _groupBoxPlanEau = new QGroupBox(this);
    _groupBoxPlanEau->setTitle(TR("id_tabplaneaurepere_box"));
    _groupBoxPlanEauLayout = new QGridLayout();
    _groupBoxPlanEau->setLayout(_groupBoxPlanEauLayout);

    _listViewPlanEau = new QTreeWidget(_groupBoxPlanEau);
    _listViewPlanEau->setColumnCount(2);
    QStringList stringList2;
    stringList2.append(TR(""));
    stringList2.append(TR("id_planeau"));
    _listViewPlanEau->setHeaderLabels(stringList2);
    _listViewPlanEau->setRootIsDecorated(TRUE);

    _groupBoxPlanEauLayout->addWidget(_listViewPlanEau, 0, 0);

    _topographieLayout->addWidget(_groupBoxPlanEau, 4, 0);


    // Courbes de niveaux
    _groupBox_3 = new QGroupBox(this);
    _groupBox_3->setTitle(TR("id_tabcrbnivrepere_box"));
    _groupBox_3Layout = new QGridLayout();
    _groupBox_3->setLayout(_groupBox_3Layout);

    _listView_6 = new QTreeWidget(_groupBox_3);
    _listView_6->setColumnCount(2);
    QStringList stringList3;
    stringList3.append(TR(""));
    stringList3.append(TR("id_crbniv"));
    _listView_6->setHeaderLabels(stringList3);
    _listView_6->setRootIsDecorated(TRUE);

    _groupBox_3Layout->addWidget(_listView_6, 0, 0);

    _topographieLayout->addWidget(_groupBox_3, 5, 0);

    // Terrains
    _groupBox_4 = new QGroupBox(this);
    _groupBox_4->setTitle(TR("id_tabterrainrepere_box"));
    _groupBox_4Layout = new QGridLayout();
    _groupBox_4->setLayout(_groupBox_4Layout);

    _listView_7 = new QTreeWidget(_groupBox_4);
    _listView_7->setColumnCount(2);
    QStringList stringList4;
    stringList4.append(TR(""));
    stringList4.append(TR("id_terrain"));
    _listView_7->setHeaderLabels(stringList4);
    _listView_7->setRootIsDecorated(TRUE);

    _groupBox_4Layout->addWidget(_listView_7, 0, 0);

    _topographieLayout->addWidget(_groupBox_4, 6, 0);

    updateContent();

    connect(_pushButtonTerrainDef, SIGNAL(clicked()), this, SLOT(editDefTerrain()));
}

TYTopographieWidget::~TYTopographieWidget()
{
}

void TYTopographieWidget::updateContent()
{
    unsigned int i;
    _elmW->updateContent();

    _lineEditNomTerrainDef->setText(getElement()->getDefTerrain()->getName());

    _listView_7->clear();
    for (i = 0; i < getElement()->getListTerrain().size(); i++)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(_listView_7, 0);
        item->setText(0,  QString().setNum(i));
        item->setText(1, getElement()->getListTerrain()[i]->getElement()->getName());
    }

    _listView_5->clear();
    for (i = 0; i < getElement()->getListCrsEau().size(); i++)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(_listView_5, 0);
        item->setText(0,  QString().setNum(i));
        item->setText(1, getElement()->getListCrsEau()[i]->getElement()->getName());
    }

    _listViewPlanEau->clear();
    for (i = 0; i < getElement()->getListPlanEau().size(); i++)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(_listViewPlanEau, 0);
        item->setText(0,  QString().setNum(i));
        item->setText(1, getElement()->getPlanEau(i)->getName());
    }

    _listView_6->clear();
    for (i = 0; i < getElement()->getListCrbNiv().size(); i++)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(_listView_6, 0);
        item->setText(0,  QString().setNum(i));
        item->setText(1, getElement()->getListCrbNiv()[i]->getElement()->getName());
    }
}

void TYTopographieWidget::apply()
{
    _elmW->apply();

    getElement()->updateGraphic();

    emit modified();
}

void TYTopographieWidget::editDefTerrain()
{
    int ret = getElement()->getDefTerrain()->edit(this);

    if (ret == QDialog::Accepted)
    {
        _lineEditNomTerrainDef->setText(getElement()->getDefTerrain()->getName());
    }
}

void TYTopographieWidget::editTerrain(QTreeWidgetItem* item)
{
    int ret = getElement()->getTerrain(item->text(0).toInt())->edit(this);

    if (ret == QDialog::Accepted)
    {
        _listView_7->clear();
        for (unsigned int i = 0; i < getElement()->getListTerrain().size(); i++)
        {
            QTreeWidgetItem* item = new QTreeWidgetItem(_listView_7, 0);
            item->setText(0,  QString().setNum(i));
            item->setText(1, getElement()->getListTerrain()[i]->getElement()->getName());
        }
    }
}

void TYTopographieWidget::editCrbNiv(QTreeWidgetItem* item)
{
    int ret = getElement()->getCrbNiv(item->text(0).toInt())->edit(this);

    if (ret == QDialog::Accepted)
    {
        _listView_6->clear();
        for (unsigned int i = 0; i < getElement()->getListCrbNiv().size(); i++)
        {
            QTreeWidgetItem* item = new QTreeWidgetItem(_listView_6, 0);
            item->setText(0,  QString().setNum(i));
            item->setText(1, getElement()->getListCrbNiv()[i]->getElement()->getName());
        }
    }

}

void TYTopographieWidget::editCrsEau(QTreeWidgetItem* item)
{
    int ret = getElement()->getCrsEau(item->text(0).toInt())->edit(this);

    if (ret == QDialog::Accepted)
    {
        _listView_5->clear();
        for (unsigned int i = 0; i < getElement()->getListCrsEau().size(); i++)
        {
            QTreeWidgetItem* item = new QTreeWidgetItem(_listView_5, 0);
            item->setText(0,  QString().setNum(i));
            item->setText(1, getElement()->getListCrsEau()[i]->getElement()->getName());
        }
    }

}

void TYTopographieWidget::editPlanEau(QTreeWidgetItem* item)
{
    int ret = getElement()->getPlanEau(item->text(0).toInt())->edit(this);

    if (ret == QDialog::Accepted)
    {
        _listViewPlanEau->clear();
        for (unsigned int i = 0; i < getElement()->getListPlanEau().size(); i++)
        {
            QTreeWidgetItem* item = new QTreeWidgetItem(_listViewPlanEau, 0);
            item->setText(0,  QString().setNum(i));
            item->setText(1, getElement()->getPlanEau(i)->getName());
        }
    }
}

void TYTopographieWidget::contextMenuEvent(QContextMenuEvent* e)
{
    //si on trouve un item associe
    QPoint point = _listViewPlanEau->mapFrom(this, e->pos());
    QTreeWidgetItem* item = _listViewPlanEau->itemAt(point.x(), point.y() - _listViewPlanEau->header()->height());
    QPoint point5 = _listView_5->mapFrom(this, e->pos());
    QTreeWidgetItem* item5 = _listView_5->itemAt(point5.x(), point5.y() - _listView_5->header()->height());
    QPoint point6 = _listView_6->mapFrom(this, e->pos());
    QTreeWidgetItem* item6 = _listView_6->itemAt(point6.x(), point6.y() - _listView_6->header()->height());
    QPoint point7 = _listView_7->mapFrom(this, e->pos());
    QTreeWidgetItem* item7 = _listView_7->itemAt(point7.x(), point7.y() - _listView_7->header()->height());
    if ((item != NULL) || (item5 != NULL) || (item6 != NULL) || (item7 != NULL))
    {
        QMenu* pMenu = new QMenu(this);

        QAction* prop = pMenu->addAction(TR("id_property"));
        if (item != NULL)
        {
            QAction* ret = pMenu->exec(_listViewPlanEau->mapToGlobal(point));
            if ((ret) && (ret == prop)) { editPlanEau(item); }
        }
        else if (item5 != NULL)
        {
            QAction* ret = pMenu->exec(_listView_5->mapToGlobal(point5));
            if ((ret) && (ret == prop)) { editCrsEau(item5); }
        }
        else if (item6 != NULL)
        {
            QAction* ret = pMenu->exec(_listView_6->mapToGlobal(point6));
            if ((ret) && (ret == prop)) { editCrbNiv(item6); }
        }
        else if (item7 != NULL)
        {
            QAction* ret = pMenu->exec(_listView_7->mapToGlobal(point7));
            if ((ret) && (ret == prop)) { editTerrain(item7); }
        }
    }
}
