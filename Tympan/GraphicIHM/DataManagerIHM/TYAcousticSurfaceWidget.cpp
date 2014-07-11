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
 * \file TYAcousticSurfaceWidget.cpp
 * \brief outil IHM pour une surface acoustique
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/models/business/geoacoustic/TYAcousticSurface.h"
#include "Tympan/models/business/material/TYMateriauConstruction.h"
//Added by qt3to4:
#include <QGridLayout>


#define TR(id) OLocalizator::getString("TYAcousticSurfaceWidget", (id))


TYAcousticSurfaceWidget::TYAcousticSurfaceWidget(TYAcousticSurface* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{

    _elmW = new TYElementWidget(pElement, this);
    _colorW = new TYColorInterfaceWidget(dynamic_cast<TYColorInterface*>(pElement), this);
    _interW = new TYAcousticInterfaceWidget(dynamic_cast<TYAcousticInterface*>(pElement), this);
    _surfaceW = new TYSurfaceInterfaceWidget(dynamic_cast<TYSurfaceInterface*>(pElement), this);

    resize(300, 570);
    setWindowTitle(TR("id_caption"));
    _acousticSurfaceLayout = new QGridLayout();
    setLayout(_acousticSurfaceLayout);

    unsigned short numLine = 0;

    _acousticSurfaceLayout->addWidget(_elmW, numLine, 0);
    _acousticSurfaceLayout->addWidget(_colorW, ++numLine, 0);
    _acousticSurfaceLayout->addWidget(_surfaceW, ++numLine, 0);
    _acousticSurfaceLayout->addWidget(_interW, ++numLine, 0);

    /*  _groupBoxSrcSurf = new QGroupBox( this, "_groupBoxSrcSurf" );
        _groupBoxSrcSurf->setTitle( TR( "id_srcSurf" ) );
        _groupBoxSrcSurf->setColumnLayout(0, Qt::Vertical );
        _groupBoxSrcSurf->layout()->setSpacing( 6 );
        _groupBoxSrcSurf->layout()->setMargin( 11 );
        _groupBoxSrcSurfLayout = new QGridLayout( _groupBoxSrcSurf->layout() );
        _groupBoxSrcSurfLayout->setAlignment( Qt::AlignTop );

        _lineEditNomSrcsurf = new QLineEdit( _groupBoxSrcSurf, "_lineEditNomSrcsurf" );
        _lineEditNomSrcsurf->setText( getElement()->getSrcSurf()->getName() );
        _lineEditNomSrcsurf->setEnabled( FALSE );
        _groupBoxSrcSurfLayout->addWidget( _lineEditNomSrcsurf, 0, 0 );

        _pushButtonSrcSurf = new QPushButton( _groupBoxSrcSurf, "_pushButtonSrcSurf" );
        _pushButtonSrcSurf->setText( TR( "id_proprietes_button" ) );
        _groupBoxSrcSurfLayout->addWidget( _pushButtonSrcSurf, 0, 1 );

        _acousticSurfaceLayout->addWidget( _groupBoxSrcSurf, 4, 0 );
    */
    _groupBoxMat = new QGroupBox(this);
    _groupBoxMat->setTitle(TR("id_mat"));
    _groupBoxMatLayout = new QGridLayout();
    _groupBoxMat->setLayout(_groupBoxMatLayout);

    _lineEditNomMat = new QLineEdit(_groupBoxMat);
    _lineEditNomMat->setEnabled(FALSE);
    _groupBoxMatLayout->addWidget(_lineEditNomMat, 0, 0);

    _pushButtonMat = new QPushButton(_groupBoxMat);
    _pushButtonMat->setText(TR("id_proprietes_button"));
    _groupBoxMatLayout->addWidget(_pushButtonMat, 0, 1);

    if (getElement()->getMateriau() != NULL)
    {
        _lineEditNomMat->setText(getElement()->getMateriau()->getName());
    }
    else
    {
        _groupBoxMat->setEnabled(FALSE);
    }

    _acousticSurfaceLayout->addWidget(_groupBoxMat, ++numLine, 0);
    /*
        _groupBoxRect = new QGroupBox( this, "_groupBoxRect" );
        _groupBoxRect->setTitle( TR( "id_rect" ) );
        _groupBoxRect->setColumnLayout(0, Qt::Vertical );
        _groupBoxRect->layout()->setSpacing( 6 );
        _groupBoxRect->layout()->setMargin( 11 );
        _groupBoxRectLayout = new QGridLayout( _groupBoxRect->layout() );
        _groupBoxRectLayout->setAlignment( Qt::AlignTop );

        _lineEditNomRect = new QLineEdit( _groupBoxRect, "_lineEditNomRect" );
        _lineEditNomRect->setText( getElement()->getBoundingRect()->getName() );
        _lineEditNomRect->setEnabled( FALSE );
        _groupBoxRectLayout->addWidget( _lineEditNomRect, 0, 0 );

        _pushButtonRect = new QPushButton( _groupBoxRect, "_pushButtonRect" );
        _pushButtonRect->setText( TR( "id_proprietes_button" ) );
        _groupBoxRectLayout->addWidget( _pushButtonRect, 0, 1 );

        _acousticSurfaceLayout->addWidget( _groupBoxRect, 6, 0 );
    */
    //  connect(_pushButtonRect,SIGNAL(clicked()),this,SLOT(editRect()));
    connect(_pushButtonMat, SIGNAL(clicked()), this, SLOT(editMat()));
    //  connect(_pushButtonSrcSurf,SIGNAL(clicked()),this,SLOT(editSrcSurf()));
}

TYAcousticSurfaceWidget::~TYAcousticSurfaceWidget()
{
}

void TYAcousticSurfaceWidget::updateContent()
{
    _elmW->updateContent();
    _colorW->updateContent();
    _interW->updateContent();
    _surfaceW->updateContent();

    if (getElement()->getMateriau() != NULL)
    {
        _lineEditNomMat->setText(getElement()->getMateriau()->getName());
    }
    //  _lineEditNomSrcsurf->setText( getElement()->getSrcSurf()->getName() );
    //  _lineEditNomRect->setText( getElement()->getBoundingRect()->getName() );
}

void TYAcousticSurfaceWidget::apply()
{
    _elmW->apply();
    _colorW->apply();
    _interW->apply();
    _surfaceW->apply();

    _pElement->setIsAcousticModified(true);

    emit modified();
}
/*
void TYAcousticSurfaceWidget::editRect()
{
    int ret = getElement()->getBoundingRect()->edit(this);

    if (ret == QDialog::Accepted) {
        _lineEditNomRect->setText( getElement()->getBoundingRect()->getName() );
    }
}

void TYAcousticSurfaceWidget::editSrcSurf()
{
    int ret = getElement()->getSrcSurf()->edit(this);

    if (ret == QDialog::Accepted) {
//      _lineEditNomSrcsurf->setText( getElement()->getSrcSurf()->getName() );
    }
}
*/
void TYAcousticSurfaceWidget::editMat()
{
    int ret = getElement()->getMateriau()->edit(this);

    if ((ret == QDialog::Accepted) && (getElement()->getMateriau() != NULL))
    {
        _lineEditNomMat->setText(getElement()->getMateriau()->getName());
    }
}
