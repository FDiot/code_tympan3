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
 * \file TYToolButton.cpp
 * \brief outil IHM pour la gestion de la couleur des ToolButton
 *
 *
 */



#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include <qcolordialog.h>
#include <QPainter>
#include <QWindowsStyle>
#include "TYToolButton.h"

using namespace Qt;

TYToolButton::TYToolButton(QWidget* parent, const char* name):
    QToolButton(parent)
{
    setAutoFillBackground(true);
    setObjectName(name);
    setToolButtonStyle(Qt::ToolButtonTextOnly);
    _color = white;
    QObject::connect(this, SIGNAL(clicked()), this, SLOT(selectColor()));

#if TY_PLATFORM == TY_PLATFORM_WIN32 || TY_PLATFORM == TY_PLATFORM_WIN64
    QWindowsStyle* wStyle = new QWindowsStyle();
    setStyle(wStyle);
#endif
}

TYToolButton::~TYToolButton()
{
}

void TYToolButton::selectColor()
{
    QColor c = QColorDialog::getColor(_color);

    if (c.isValid())
    {
        _color = c;
        updateColor();

        emit colorAccepted(_color);
    }
    else
    {
        emit colorRejected();
    }
}

void TYToolButton::updateColor()
{
    QPalette palette = this->palette();
    palette.setColor(QPalette::Button, _color);
    palette.setColor(QPalette::Window, _color);
    setPalette(palette);
}
