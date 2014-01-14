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
 * \file TYLookupTableWidget.cpp
 * \brief Outil IHM utile a l'affichage de la palette de couleur
 *
 *
 */


#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "TYLookupTableWidget.h"

#include <cassert>
#include <limits>

#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/math/special_functions/fpclassify.hpp>

#include <qpainter.h>
#include <qrect.h>

#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYPalette.h"


TYLookupTableWidget::TYLookupTableWidget(const TYPalette* palette, QWidget* parent, const char* name)
    : _palette(palette), QWidget(parent), _rectPal(0)
{
    setObjectName(name);

    setFixedHeight(30);
    _rectPal = new QRect(0, 0, width(), height());
}

TYLookupTableWidget::~TYLookupTableWidget()
{

}

void TYLookupTableWidget::paintEvent(QPaintEvent*)
{
    QPainter paint(this);
    drawPalette(&paint);
}

void TYLookupTableWidget::resizeEvent(QResizeEvent* e)
{
    _rectPal->setSize(e->size());
}

void TYLookupTableWidget::drawPalette(QPainter* painter)
{
    size_t nbColors = _palette->getNbColors();
    if (nbColors==0) return;

    const int bounds_width = 0.1  * _rectPal->width();
    const int width_but_bounds = _rectPal->width() - 2*bounds_width;

    int x = 0;
    TYPalette::values_type prev_value = - std::numeric_limits<float>::infinity();

    TYPalette::values_type value;
    OColor color;
    BOOST_FOREACH(boost::tie(value, color), _palette->getColorMap() ) {
    	// Handle specially first and last iteration
    	const int dx =  boost::math::isinf(value - prev_value) ? bounds_width :
    			(_palette->normalize(value) - _palette->normalize(prev_value)) * width_but_bounds;
    	prev_value = value;
    	painter->fillRect(x, _rectPal->top(), x+dx, _rectPal->bottom(), toQColor(color));
    	x += dx;
    }
}



TYLabeledLookupTableWidget::TYLabeledLookupTableWidget(const TYPalette* palette, QWidget* parent, const char* name)
    : QWidget(parent)
{
	if (!parent)
		parent = this;

    QGridLayout* pPreviewLayout = new QGridLayout();
    parent->setLayout(pPreviewLayout);

	p_scale = new TYLookupTableWidget(palette, this, name);
	p_minBound = new QDoubleSpinBox();
	p_maxBound = new QDoubleSpinBox();

	p_minBound->setDecimals(1);
	p_minBound->setReadOnly(true);
	p_minBound->setButtonSymbols(QAbstractSpinBox::NoButtons);
	p_maxBound->setDecimals(1);
	p_maxBound->setReadOnly(true);
	p_maxBound->setButtonSymbols(QAbstractSpinBox::NoButtons);

    pPreviewLayout->addWidget(p_scale, 1, 0, 1, 3);
    pPreviewLayout->addWidget(p_minBound, 0, 0, Qt::AlignLeft);
    pPreviewLayout->addWidget(p_maxBound, 0, 2, Qt::AlignRight);

    update(palette);
}

TYLabeledLookupTableWidget::~TYLabeledLookupTableWidget() {}


void TYLabeledLookupTableWidget::update(const TYPalette* palette)
{
	assert(p_scale->_palette == palette && "Inconsistent use of the update slot");
	p_minBound->setValue(p_scale->_palette->getValueMin());
	p_maxBound->setValue(p_scale->_palette->getValueMax());
	p_scale->repaint();
	QWidget::update();
}
