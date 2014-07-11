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
 * \file TYTerrainWidget.cpp
 * \brief outil IHM pour un terrain
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/models/business/topography/TYTerrain.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#define TR(id) OLocalizator::getString("TYTerrainWidget", (id))


TYTerrainWidget::TYTerrainWidget(TYTerrain* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{
    unsigned short lnW = 0;

    _elmW = new TYElementWidget(pElement, this);
    _colorW = new TYColorInterfaceWidget(pElement, this);

    resize(300, 330);
    setWindowTitle(TR("id_caption"));
    _terrainLayout = new QGridLayout();
    setLayout(_terrainLayout);

    _terrainLayout->addWidget(_elmW, lnW++, 0);
    _terrainLayout->addWidget(_colorW, lnW++, 0);

    //   _groupBoxEdit = new QGroupBox( this );
    //   _groupBoxEdit->setTitle( TR( "" ) );
    //   _groupBoxEditLayout = new QGridLayout();
    //_groupBoxEdit->setLayout(_groupBoxEditLayout);

    //_labelType = new QLabel( _groupBoxEdit );
    //_labelType->setText( TR( "id_type_label" ) );
    //_groupBoxEditLayout->addWidget( _labelType, 0, 0 );

    //_lineEditType = new QLineEdit( _groupBoxEdit );
    //_groupBoxEditLayout->addWidget( _lineEditType, 0, 1 );

    //_terrainLayout->addWidget( _groupBoxEdit, 2, 0 );

    _tabWidget = new QTabWidget(this);

    _solW = new TYSolWidget(getElement()->getSol(), _tabWidget);

    _tabWidget->insertTab(1, _solW, TR("id_sol"));

    //_groupBoxSol = new QGroupBox( this);
    //   _groupBoxSol->setTitle( TR( "id_sol" ) );
    //   _groupBoxSolLayout = new QGridLayout();
    //_groupBoxSol->setLayout(_groupBoxSolLayout);

    //   _pushButtonSol = new QPushButton( _groupBoxSol );
    //   _pushButtonSol->setText( TR( "id_proprietes_button" ) );
    //   _groupBoxSolLayout->addWidget( _pushButtonSol, 0, 1 );

    //   _lineEditNomSol = new QLineEdit( _groupBoxSol );
    //   _lineEditNomSol->setEnabled( FALSE );
    //   _groupBoxSolLayout->addWidget( _lineEditNomSol, 0, 0 );

    _groupBox = new QGroupBox(_tabWidget);
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

    _groupBoxLayout->addWidget(_listViewTabPt, 0, 0);

    _tabWidget->insertTab(2, _groupBox, TR("id_geometrie"));

    //  _terrainLayout->addWidget( _groupBoxSol, lnW++, 0 );

    _terrainLayout->addWidget(_tabWidget, lnW++, 0);

    updateContent();

    //  connect(_pushButtonSol,SIGNAL(clicked()),this,SLOT(editSol()));
}

TYTerrainWidget::~TYTerrainWidget()
{
}

void TYTerrainWidget::updateContent()
{
    QString num;

    _elmW->updateContent();
    _colorW->updateContent();
    _solW->updateContent();

    //  _lineEditNomSol->setText( getElement()->getSol()->getName() );
    //    _lineEditType->setText( num.setNum(getElement()->getType()) );

    _listViewTabPt->clear();
    for (unsigned int i = 0; i < getElement()->getListPoints().size(); i++)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(_listViewTabPt, 0);
        item->setText(0, num.setNum(getElement()->getListPoints()[i]._x, 'f', 2));
        item->setText(1, num.setNum(getElement()->getListPoints()[i]._y, 'f', 2));
        item->setText(2, num.setNum(getElement()->getListPoints()[i]._z, 'f', 2));
    }
}

void TYTerrainWidget::apply()
{
    _elmW->apply();
    _colorW->apply();
    _solW->apply();

    //  getElement()->setType( _lineEditType->text().toInt( ) );

    emit modified();
}

void TYTerrainWidget::disableSolWidget()
{
    _tabWidget->removeTab(0);
}


//void TYTerrainWidget::editSol()
//{
//  int ret = getElement()->getSol()->edit(this);
//
//  if (ret == QDialog::Accepted) {
//      _lineEditNomSol->setText( getElement()->getSol()->getName() );
//  }
//}


