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
 * \file TYAcousticSurfaceNodeWidget.cpp
 * \brief outil IHM pour un ensemble de surfaces acoustiques
 *
 *
 */





#include <qmenu.h>
#include <qtablewidget.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QHeaderView>

#include "TYAcousticSurfaceNodeWidget.h"
#include "Tympan/gui/widgets/TYColorInterfaceWidget.h"
#include "Tympan/gui/widgets/TYAcousticInterfaceWidget.h"
#include "Tympan/gui/widgets/TYSurfaceInterfaceWidget.h"

#include "Tympan/models/business/geoacoustic/TYAcousticSurfaceNode.h"

#include "Tympan/models/business/OLocalizator.h"
#define TR(id) OLocalizator::getString("TYAcousticSurfaceNodeWidget", (id))


TYAcousticSurfaceNodeWidget::TYAcousticSurfaceNodeWidget(TYAcousticSurfaceNode* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{

    _elmW = new TYElementWidget(pElement, this);
    _colorW = new TYColorInterfaceWidget(dynamic_cast<TYColorInterface*>(pElement), this);
    _interW = new TYAcousticInterfaceWidget(dynamic_cast<TYAcousticInterface*>(pElement), this);
    _surfaceW = new TYSurfaceInterfaceWidget(dynamic_cast<TYSurfaceInterface*>(pElement), this);

    resize(300, 480);
    setWindowTitle(TR("id_caption"));
    _acousticSurfacenodeLayout = new QGridLayout();
    setLayout(_acousticSurfacenodeLayout);

    unsigned short numLine = 0;

    _acousticSurfacenodeLayout->addWidget(_elmW, numLine, 0);
    _acousticSurfacenodeLayout->addWidget(_colorW, ++numLine, 0);
    _acousticSurfacenodeLayout->addWidget(_surfaceW, ++numLine, 0);
    _acousticSurfacenodeLayout->addWidget(_interW, ++numLine, 0);
    /*
        _groupBoxRect = new QGroupBox( this );
        _groupBoxRect->setTitle( TR( "id_rect" ) );
        _groupBoxRect->setColumnLayout(0, Qt::Vertical );
        _groupBoxRect->layout()->setSpacing( 6 );
        _groupBoxRect->layout()->setMargin( 11 );
        _groupBoxRectLayout = new QGridLayout( _groupBoxRect->layout() );
        _groupBoxRectLayout->setAlignment( Qt::AlignTop );

        _lineEditNomRect = new QLineEdit( _groupBoxRect );
        _lineEditNomRect->setEnabled( FALSE );
        _groupBoxRectLayout->addWidget( _lineEditNomRect, 0, 0 );

        _pushButtonRect = new QPushButton( _groupBoxRect );
        _pushButtonRect->setText( TR( "id_proprietes_button" ) );
        _groupBoxRectLayout->addWidget( _pushButtonRect, 0, 1 );

        _acousticSurfacenodeLayout->addWidget( _groupBoxRect, 4, 0 );
    */
    _groupBox = new QGroupBox(this);
    _groupBox->setTitle(TR("id_tabaccsurf_box"));
    _groupBoxLayout = new QGridLayout();
    _groupBox->setLayout(_groupBoxLayout);

    _tableFaces = new QTableWidget();
    _tableFaces->setColumnCount(2);
    _tableFaces->setHorizontalHeaderItem(0, new QTableWidgetItem(TR("id_nom")));
    _tableFaces->setHorizontalHeaderItem(1, new QTableWidgetItem(TR("id_emission")));

    _groupBoxLayout->addWidget(_tableFaces, 0, 0);
    _acousticSurfacenodeLayout->addWidget(_groupBox, ++numLine, 0);

    updateContent();

    //  connect( _pushButtonRect,SIGNAL(clicked()),this,SLOT(editRect()));
}

TYAcousticSurfaceNodeWidget::~TYAcousticSurfaceNodeWidget()
{
}

void TYAcousticSurfaceNodeWidget::updateContent()
{
    _elmW->updateContent();
    _colorW->updateContent();
    _surfaceW->updateContent();

    //  _lineEditNomRect->setText( getElement()->getBoundingRect()->getName() );

    for (int i = 0, row = 0; i < getElement()->getTabAcousticSurf().size() ; i++, row++)
    {
        LPTYAcousticSurface pSurf = getElement()->getAcousticSurf(i);

        if (pSurf->getIsSub())
        {
            _tableFaces->setRowCount(i + 1);
            QTableWidgetItem* pItem = new QTableWidgetItem(pSurf->getName());
            _tableFaces->setItem(row, 0, pItem);
            QTableWidgetItem* pCheckItemEmit = new QTableWidgetItem(" ");
            pCheckItemEmit->setCheckState(pSurf->getIsRayonnant() ? Qt::Checked : Qt::Unchecked);
            _tableFaces->setItem(row, 1, pCheckItemEmit);
        }
    }
}

void TYAcousticSurfaceNodeWidget::apply()
{
    unsigned int i;

    _elmW->apply();
    _colorW->apply();
    _surfaceW->apply();

    TYTabAcousticSurfaceGeoNode* pTab = &getElement()->getTabAcousticSurf();
    for (i = 0; i < pTab->size() ; ++i)
        if (!TYAcousticSurface::safeDownCast(pTab->operator[](i)->getElement())->getIsSub())
        {
            TYAcousticSurface::safeDownCast(pTab->operator[](i)->getElement())->setColor(getElement()->getColor());
        }


    for (i = 0; i < _tableFaces->rowCount(); i++)
    {
        LPTYAcousticSurface pSurf = getElement()->getAcousticSurf(i);
        QTableWidgetItem* pCheck = (QTableWidgetItem*) _tableFaces->item(i, 1);
        pSurf->setIsRayonnant(pCheck->checkState() == Qt::Checked);
        QTableWidgetItem* pText  = (QTableWidgetItem*)_tableFaces->item(i, 0);
        pSurf->setName(pText->text());
    }

    _interW->apply();

    _pElement->setIsAcousticModified(true);

    emit modified();
}

void TYAcousticSurfaceNodeWidget::editAccSurf(const int& item)
{
    LPTYAcousticSurface pSurf = getElement()->getAcousticSurf(item);
    QTableWidgetItem* pCheck = (QTableWidgetItem*) _tableFaces->item(item, 1);
    pSurf->setIsRayonnant(pCheck->checkState() == Qt::Checked);
    QTableWidgetItem* pText  = (QTableWidgetItem*)_tableFaces->item(item, 0);
    pSurf->setName(pText->text());

    int ret = getElement()->getAcousticSurf(item)->edit(this);

    if (ret == QDialog::Accepted)
    {
        pCheck->setCheckState(pSurf->getIsRayonnant() ? Qt::Checked : Qt::Unchecked);
        pText->setText(pSurf->getName());
    }
}

void TYAcousticSurfaceNodeWidget::delAccSurf(const int& item)
{
    LPTYAcousticSurfaceGeoNode pSurfNode = getElement()->getTabAcousticSurf()[item];

    bool ret = getElement()->remAcousticSurf(pSurfNode);

    _tableFaces->removeRow(item);

    if (ret) { updateContent(); }
}
/*
void TYAcousticSurfaceNodeWidget::editRect()
{
    int ret = getElement()->getBoundingRect()->edit(this);

    if (ret == QDialog::Accepted) {
        _lineEditNomRect->setText( getElement()->getBoundingRect()->getName() );
    }
}
*/
/*
void TYAcousticSurfaceNodeWidget::selectionChanged( int index)
{
    switch (index) {
    case 0: resize( 300, 700 );
        _interW->show();
        _groupBoxRect->show();
        _groupBox->show();
        break;
    case 1: resize( 300, 200 );
        _interW->hide();
        _groupBoxRect->show();
        _groupBox->show();
        break;
    case 2: resize( 300, 300);
        _interW->show();
        _groupBoxRect->hide();
        _groupBox->hide();
        break;
    }
}
*/

void TYAcousticSurfaceNodeWidget::contextMenuEvent(QContextMenuEvent* e)
{
    QPoint point = _tableFaces->mapFrom(this, e->pos());

    if ((point.x() >= 0) && (point.y() >= 0) && (point.x() <= _tableFaces->width()) && (point.y() <= _tableFaces->height()))
    {
        QPoint resPoint = QPoint(point.x(), point.y() - _tableFaces->horizontalHeader()->height());
        QTableWidgetItem* item = _tableFaces->itemAt(resPoint);
        if (item)
        {
            QMenu* pMenu = new QMenu(this);

            QAction* prop = pMenu->addAction(TR("id_property"));
            QAction* del = pMenu->addAction(TR("id_delete_surf"));

            QAction* ret = pMenu->exec(_tableFaces->mapToGlobal(point));

            if (ret)
            {
                if (ret == prop) { editAccSurf(_tableFaces->indexAt(resPoint).row()); }
                else if (ret == del) { delAccSurf(_tableFaces->indexAt(resPoint).row()); }
            }
        }
    }
}

