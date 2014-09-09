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
 * \file TYLookupTableWidget.h
 * \brief Outil IHM utile a l'affichage de la palette de couleur (fichier header)
 *
 *
 *
 *
 *
 */

#ifndef __TY_LOOKUPTABLE_WIDGET__
#define __TY_LOOKUPTABLE_WIDGET__

#include <qwidget.h>
//Added by qt3to4:
#include <QPaintEvent>
#include <QResizeEvent>
#include <QColor>

#include "Tympan/core/color.h"

inline QColor toQColor(const OColor& color) {return QColor(color.r * 255.0f, color.g * 255.0f, color.b * 255.0f);}

inline OColor toOColor(const QColor& color) {return OColor(color.red() / 255.0f, color.green() / 255.0f, color.blue() / 255.0f);}


class QRect;
class TYPalette;
class QDoubleSpinBox;

/**
 * \class TYLookupTableWidget
 * \brief Outil IHM utile a l'affichage de la palette de couleur
 */

class TYLookupTableWidget : public QWidget
{
    Q_OBJECT

    friend class TYLabeledLookupTableWidget;

public:
    TYLookupTableWidget(const TYPalette* palette, QWidget* parent = 0, const char* name = 0);
    virtual ~TYLookupTableWidget();

protected:
    void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent*);
    void drawPalette(QPainter* painter);

    const TYPalette* _palette;
    QRect* _rectPal;
};


class TYLabeledLookupTableWidget : public QWidget
{
    Q_OBJECT

public:
    TYLabeledLookupTableWidget(const TYPalette* palette, QWidget* parent = 0, const char* name = 0);
    virtual ~TYLabeledLookupTableWidget();


public slots:
    void update(const TYPalette* palette);

protected:
    TYLookupTableWidget* p_scale;
    QDoubleSpinBox* p_minBound;
    QDoubleSpinBox* p_maxBound;
};

#endif // __TY_LOOKUPTABLE_WIDGET__
