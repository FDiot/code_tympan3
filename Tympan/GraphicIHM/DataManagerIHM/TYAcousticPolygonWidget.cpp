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
 * \file TYAcousticPolygonWidget.cpp
 * \brief outil IHM pour un polygone acoustique
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticPolygon.h"
//Added by qt3to4:
#include <QGridLayout>


#define TR(id) OLocalizator::getString("TYAcousticPolygonWidget", (id))


TYAcousticPolygonWidget::TYAcousticPolygonWidget(TYAcousticPolygon* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(_pParent)
{
    Q_ASSERT(pElement);
    _pElement = pElement;

    _accSurfW = new TYAcousticSurfaceWidget(pElement, this);

    resize(300, 680);
    setWindowTitle(TR("id_caption"));
    _accPolygonLayout = new QGridLayout();
    setLayout(_accPolygonLayout);

    _accPolygonLayout->addWidget(_accSurfW, 1, 0);

    /*
        _groupBoxPolygon = new QGroupBox( this );
        _groupBoxPolygon->setTitle( TR( "id_polygon" ) );
        _groupBoxPolygon->setColumnLayout(0, Qt::Vertical );
        _groupBoxPolygon->layout()->setSpacing( 6 );
        _groupBoxPolygon->layout()->setMargin( 11 );
        _groupBoxPolygonLayout = new QGridLayout( _groupBoxPolygon->layout() );
        _groupBoxPolygonLayout->setAlignment( Qt::AlignTop );

        _lineEditNomPolygon = new QLineEdit( _groupBoxPolygon );
        _lineEditNomPolygon->setEnabled( FALSE );
        _groupBoxPolygonLayout->addWidget( _lineEditNomPolygon, 0, 0 );

        _pushButtonPolygon = new QPushButton( _groupBoxPolygon );
        _pushButtonPolygon->setText( TR( "id_proprietes_button" ) );
        _groupBoxPolygonLayout->addWidget( _pushButtonPolygon, 0, 1 );

        _accPolygonLayout->addWidget( _groupBoxPolygon, 2, 0 );

        connect(_pushButtonPolygon, SIGNAL( clicked() ), this, SLOT( editPolygon() ));
    */
    updateContent();
}

TYAcousticPolygonWidget::~TYAcousticPolygonWidget()
{
}

void TYAcousticPolygonWidget::updateContent()
{
    _accSurfW->updateContent();

    //  _lineEditNomPolygon->setText( getElement()->getPolygon()->getName() );
}

void TYAcousticPolygonWidget::apply()
{
    _accSurfW->apply();

    emit modified();
}

void TYAcousticPolygonWidget::hideMat()
{
    _accSurfW->getGroupBoxMat()->hide();
}

/*
void TYAcousticPolygonWidget::editPolygon()
{
    int ret = getElement()->getPolygon()->edit(this);

    if (ret == QDialog::Accepted) {
        _lineEditNomPolygon->setText( getElement()->getPolygon()->getName() );
    }
}
*/
