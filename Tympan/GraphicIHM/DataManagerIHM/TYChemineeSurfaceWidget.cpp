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
 * \file TYChemineeSurfaceWidget.cpp
 * \brief outil IHM pour une surface de cheminee
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYChemineeSurface.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticRectangleNode.h"

//Added by qt3to4:
#include <QMessageBox>
#include <QGridLayout>

#define TR(id) OLocalizator::getString("TYChemineeSurfaceWidget", (id))


TYChemineeSurfaceWidget::TYChemineeSurfaceWidget(TYChemineeSurface* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(_pParent)
{
    Q_ASSERT(pElement);
    _pElement = pElement;

    resize(300, 640);
    setWindowTitle(TR("id_caption"));
    _pChemineeSurfaceLayout = new QGridLayout();
    setLayout(_pChemineeSurfaceLayout);

    _elmW = new TYAcousticRectangleWidget(pElement, this);
    _elmW->setLayoutSpacing(0, 0);
    //  _elmW->getSurfaceWidget()->getGroupBoxSrcSurf()->hide();

    _pChemineeSurfaceLayout->addWidget(_elmW, 0, 0);
    /*
        QGroupBox * pGroupBoxSrc = new QGroupBox( this );
        pGroupBoxSrc->setTitle( TR( "id_source_cheminee" ) );
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

        _pChemineeSurfaceLayout->addWidget(pGroupBoxSrc, 1, 0);
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

    _pChemineeSurfaceLayout->addWidget(_groupBoxDimensions, 1, 0);

    updateContent();

    //  connect(pPushButtonSrc, SIGNAL(clicked()), this, SLOT(editSource()));
}

TYChemineeSurfaceWidget::~TYChemineeSurfaceWidget()
{
}

void TYChemineeSurfaceWidget::updateContent()
{
    _elmW->updateContent();

    float sizeX, sizeY;
    ((TYRectangle*)_elmW->getElement()->getShape())->getSize(sizeX, sizeY);

    _dimensionsXLineEdit->setText(QString().setNum(sizeX, 'f', 2));
    _dimensionsYLineEdit->setText(QString().setNum(sizeY, 'f', 2));

    //  _pLineEditSrc->setText( getElement()->getSourceCheminee()->getName());
}

void TYChemineeSurfaceWidget::apply()
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
void TYChemineeSurfaceWidget::editSource()
{
    int ret = getElement()->getSourceCheminee()->edit(this);

    if ((ret == QDialog::Accepted) && ( getElement()->getSourceCheminee() != NULL ) ) {
        _pLineEditSrc->setText( getElement()->getSourceCheminee()->getName() );
    }
}
*/
