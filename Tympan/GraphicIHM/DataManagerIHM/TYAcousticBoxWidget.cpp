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
 * \file TYAcousticBoxWidget.cpp
 * \brief outil IHM pour une boite acoustique
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include <qmenu.h>
#include <qtablewidget.h>
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticBox.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QHeaderView>


#define TR(id) OLocalizator::getString("TYAcousticBoxWidget", (id))


TYAcousticBoxWidget::TYAcousticBoxWidget(TYAcousticBox* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent), _isColorModified(false)
{

    _elmW = new TYAcousticVolumeWidget(pElement, this);

    resize(300, 500);
    setWindowTitle(TR("id_caption"));
    _acousticBoxLayout = new QGridLayout();
    setLayout(_acousticBoxLayout);

    _acousticBoxLayout->addWidget(_elmW, 0, 0);

    _groupBox = new QGroupBox(this);
    _groupBox->setTitle(TR(""));
    _groupBoxLayout = new QGridLayout();
    _groupBox->setLayout(_groupBoxLayout);

    _tableFaces = new QTableWidget();
    _tableFaces->setColumnCount(2);
    _tableFaces->setHorizontalHeaderItem(0, new QTableWidgetItem(TR("id_nom")));
    _tableFaces->setHorizontalHeaderItem(1, new QTableWidgetItem(TR("id_emission")));

    _groupBoxLayout->addWidget(_tableFaces, 0, 0);
    _acousticBoxLayout->addWidget(_groupBox, 1, 0);

    _groupBoxDimensions = new QGroupBox(this);
    _groupBoxDimensions->setTitle(TR("id_title_dimensions"));
    _groupBoxDimensionsLayout = new QGridLayout();
    _groupBoxDimensions->setLayout(_groupBoxDimensionsLayout);

    _dimensionsXLabel = new QLabel("labelName");
    _dimensionsXLabel->setText("X : ");
    _dimensionsXLineEdit = new QLineEdit("");

    _dimensionsYLabel = new QLabel("labelName");
    _dimensionsYLabel->setText("Y : ");
    _dimensionsYLineEdit = new QLineEdit("");

    _dimensionsZLabel = new QLabel("labelName");
    _dimensionsZLabel->setText("Z : ");
    _dimensionsZLineEdit = new QLineEdit("");

    _groupBoxDimensionsLayout->addWidget(_dimensionsXLabel, 0, 0);
    _groupBoxDimensionsLayout->addWidget(_dimensionsXLineEdit, 0, 1);
    _groupBoxDimensionsLayout->addWidget(_dimensionsYLabel, 0, 2);
    _groupBoxDimensionsLayout->addWidget(_dimensionsYLineEdit, 0, 3);
    _groupBoxDimensionsLayout->addWidget(_dimensionsZLabel, 0, 4);
    _groupBoxDimensionsLayout->addWidget(_dimensionsZLineEdit, 0, 5);

    _acousticBoxLayout->addWidget(_groupBoxDimensions, 2, 0);

    updateContent();

    connect(_elmW, SIGNAL(colorModified()), this, SLOT(modifyColor()));
}

TYAcousticBoxWidget::~TYAcousticBoxWidget()
{
}

void TYAcousticBoxWidget::updateContent()
{
    _elmW->updateContent();

    for (int i = 0, row = 0; i < 6 ; i++, row++)
    {
        LPTYAcousticRectangleNode pRectNode = TYAcousticRectangleNode::safeDownCast(getElement()->getFace(i)->getElement());
        _tableFaces->setRowCount(i + 1);
        QTableWidgetItem* pItem = new QTableWidgetItem(pRectNode->getName());
        QTableWidgetItem* pCheckItemEmit = new QTableWidgetItem(" ");
        pCheckItemEmit->setCheckState(pRectNode->getIsRayonnant() ? Qt::Checked : Qt::Unchecked);
        _tableFaces->setItem(row, 0, pItem);
        _tableFaces->setItem(row, 1, pCheckItemEmit);
    }

    float sizeX, sizeY, sizeZ;
    ((TYAcousticBox*)_pElement)->getDimension(sizeX, sizeY, sizeZ);
    _dimensionsXLineEdit->setText(QString().setNum(sizeX, 'f', 2));
    _dimensionsYLineEdit->setText(QString().setNum(sizeY, 'f', 2));
    _dimensionsZLineEdit->setText(QString().setNum(sizeZ, 'f', 2));
}

void TYAcousticBoxWidget::apply()
{

    unsigned int i = 0;
    if (_isColorModified)
    {
        for (i = 0; i < 6; ++i)
        {
            TYAcousticRectangleNode::safeDownCast(getElement()->getFace(i)->getElement())->setColor(getElement()->getColor());
        }

        _isColorModified = false;
    }

    for (i = 0; i < _tableFaces->rowCount(); i++)
    {
        LPTYAcousticRectangleNode pRectNode = TYAcousticRectangleNode::safeDownCast(getElement()->getFace(i)->getElement());
        QTableWidgetItem* pCheck = (QTableWidgetItem*) _tableFaces->item(i, 1);
        pRectNode->setIsRayonnant(pCheck->checkState() == Qt::Checked);
        QTableWidgetItem* pText  = (QTableWidgetItem*)_tableFaces->item(i, 0);
        pRectNode->setName(pText->text());
    }

    _elmW->apply();

    float sizeX = _dimensionsXLineEdit->text().toDouble();
    float sizeY = _dimensionsYLineEdit->text().toDouble();
    float sizeZ = _dimensionsZLineEdit->text().toDouble();
    ((TYAcousticBox*)_pElement)->setDimension(sizeX, sizeY, sizeZ);

    emit modified();
}

void TYAcousticBoxWidget::reject()
{
    _elmW->reject();

    _isColorModified = false;
}

void TYAcousticBoxWidget::editFace(const int& item)
{
    LPTYAcousticRectangleNode pRectNode = TYAcousticRectangleNode::safeDownCast(getElement()->getFace(item)->getElement());
    QTableWidgetItem* pCheck = (QTableWidgetItem*) _tableFaces->item(item, 1);
    pRectNode->setIsRayonnant(pCheck->checkState() == Qt::Checked);
    QTableWidgetItem* pText  = (QTableWidgetItem*)_tableFaces->item(item, 0);
    pRectNode->setName(pText->text());

    int ret = pRectNode->edit(this);

    if (ret == QDialog::Accepted)
    {
        pCheck->setCheckState(pRectNode->getIsRayonnant() ? Qt::Checked : Qt::Unchecked);
        pText->setText(pRectNode->getName());
    }
}

void TYAcousticBoxWidget::contextMenuEvent(QContextMenuEvent* e)
{
    QPoint point = _tableFaces->mapFrom(this, e->pos());

    if ((point.x() >= 0) && (point.y() >= 0) && (point.x() <= _tableFaces->width()) && (point.y() <= _tableFaces->height()))
    {
        QPoint resPoint = QPoint(point.x(), point.y() - _tableFaces->horizontalHeader()->height());
        QTableWidgetItem* item = _tableFaces->itemAt(resPoint);
        if (item)
        {
            QMenu* pMenu = new QMenu(this);

            QAction* prop = pMenu->addAction(TR("id_proprietes_button"));
            QAction* ret = pMenu->exec(_tableFaces->mapToGlobal(point));

            if ((ret) && (ret == prop)) { editFace(_tableFaces->indexAt(resPoint).row()); }
        }
    }
}

void TYAcousticBoxWidget::modifyColor()
{
    _isColorModified = true;

}
