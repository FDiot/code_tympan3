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
 * \file TYBoucheSurfaceWidget.cpp
 * \brief outil IHM pour une bouche de ventilation
 *
 *
 */




#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/geoacoustic/TYBoucheSurface.h"
#include "Tympan/models/business/geoacoustic/TYAcousticRectangleNode.h"
//Added by qt3to4:

#include <QMessageBox>
#include <QGridLayout>


#define TR(id) OLocalizator::getString("TYBoucheSurfaceWidget", (id))


TYBoucheSurfaceWidget::TYBoucheSurfaceWidget(TYBoucheSurface* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{

    resize(300, 640);
    setWindowTitle(TR("id_caption"));
    _pBoucheSurfaceLayout = new QGridLayout();
    setLayout(_pBoucheSurfaceLayout);

    _elmW = new TYAcousticRectangleWidget(pElement, this);
    _elmW->setLayoutSpacing(0, 0);
    //  _elmW->getSurfaceWidget()->getGroupBoxSrcSurf()->hide();

    _pBoucheSurfaceLayout->addWidget(_elmW, 0, 0);
    /*
        QGroupBox * pGroupBoxSrc = new QGroupBox( this );
        pGroupBoxSrc->setTitle( TR( "id_source_bafflee" ) );
        pGroupBoxSrc->setColumnLayout(0, Qt::Vertical );
        pGroupBoxSrc->layout()->setSpacing( 6 );
        pGroupBoxSrc->layout()->setMargin( 11 );
        QGridLayout * pGroupBoxSrcLayout = new QGridLayout( pGroupBoxSrc->layout() );
        pGroupBoxSrcLayout->setAlignment( Qt::AlignTop );

        _pLineEditSrc = new QLineEdit( pGroupBoxSrc );
        _pLineEditSrc->setEnabled( FALSE );
        pGroupBoxSrcLayout->addWidget( _pLineEditSrc, 0, 0 );

        QPushButton * pPushButtonSrc = new QPushButton( pGroupBoxSrc );
        pPushButtonSrc->setText( TR( "id_proprietes_button" ) );
        pGroupBoxSrcLayout->addWidget( pPushButtonSrc, 0, 1 );

        _pBoucheSurfaceLayout->addWidget(pGroupBoxSrc, 1, 0);
    */

    //Dimensions
    _groupBoxDimensions = new QGroupBox(this);
    _groupBoxDimensions->setTitle(TR("id_title_dimensions"));
    _groupBoxDimensionsLayout = new QGridLayout();
    _groupBoxDimensions->setLayout(_groupBoxDimensionsLayout);

    _dimensionsXLabel = new QLabel("labelName");
    _dimensionsXLabel->setText("X :");
    _dimensionsXLineEdit = new QLineEdit("");

    _dimensionsYLabel = new QLabel("labelName");
    _dimensionsYLabel->setText("Y :");
    _dimensionsYLineEdit = new QLineEdit("");

    _groupBoxDimensionsLayout->addWidget(_dimensionsXLabel, 0, 0);
    _groupBoxDimensionsLayout->addWidget(_dimensionsXLineEdit, 0, 1);
    _groupBoxDimensionsLayout->addWidget(_dimensionsYLabel, 0, 2);
    _groupBoxDimensionsLayout->addWidget(_dimensionsYLineEdit, 0, 3);

    _pBoucheSurfaceLayout->addWidget(_groupBoxDimensions, 1, 0);

    updateContent();

    //  connect(pPushButtonSrc, SIGNAL(clicked()), this, SLOT(editSource()));
}

TYBoucheSurfaceWidget::~TYBoucheSurfaceWidget()
{
}

void TYBoucheSurfaceWidget::updateContent()
{
    _elmW->updateContent();

    float sizeX, sizeY;
    ((TYRectangle*)_elmW->getElement()->getShape())->getSize(sizeX, sizeY);

    _dimensionsXLineEdit->setText(QString().setNum(sizeX, 'f', 2));
    _dimensionsYLineEdit->setText(QString().setNum(sizeY, 'f', 2));

    //  _pLineEditSrc->setText( getElement()->getSourceBafflee()->getName());
}

void TYBoucheSurfaceWidget::apply()
{
    _elmW->apply();

    double sizeX = _dimensionsXLineEdit->text().toDouble();
    double sizeY = _dimensionsYLineEdit->text().toDouble();

    if (sizeX <= 1E-4 || sizeY <= 1.E-4) // Eviter les surfaces nulles
    {
        QMessageBox::warning(this, "Tympan", TR("id_warning_size_not_ok"), QMessageBox::Yes);//, QMessageBox::No);
        return;
    }

    ((TYRectangle*)_elmW->getElement()->getShape())->setSize(sizeX , sizeY);

    TYAcousticRectangleNode* pRectNode = TYAcousticRectangleNode::safeDownCast(getElement()->getParent());
    if (pRectNode) { pRectNode->updateGrid(); }

    _pElement->setIsAcousticModified(true);

    emit modified();
}
/*
void TYBoucheSurfaceWidget::editSource()
{
    int ret = getElement()->getSourceBafflee()->edit(this);

    if ((ret == QDialog::Accepted) && ( getElement()->getSourceBafflee() != NULL ) ) {
        _pLineEditSrc->setText( getElement()->getSourceBafflee()->getName() );
    }
}
*/
