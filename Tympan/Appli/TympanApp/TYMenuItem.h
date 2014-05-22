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

/*
 *
 *
 */

/**
 * \file TYMenuItem.h
 * \brief gestion des items de menu avec une police et des styles particuliers (gras, italique...) (fichier header)
 * \author Projet_Tympan
 */


#ifndef __TY_MENU_ITEM__
#define __TY_MENU_ITEM__


#include "TYAppDefines.h"
#include <qpainter.h>


/**
 * \class TYMenuItem
 * \brief gestion des items de menu avec une police et des styles particuliers (gras, italique...)
 */
class TYMenuItem : public QAction
{
public:
    TYMenuItem(const QString& s, const QFont& f)
        : QAction(NULL), _string(s), _font(f) { setObjectName(s); };
    ~TYMenuItem() { }

    void paint(QPainter* p, const QColorGroup& /*cg*/, bool /*act*/, bool /*enabled*/, int x, int y, int w, int h)
    {
        p->setFont(_font);
        p->drawText(x, y, w, h, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextDontClip, _string);
    }

    QSize sizeHint()
    {
        return QFontMetrics(_font).size(Qt::AlignLeft | Qt::AlignVCenter | Qt::TextDontClip, _string);
    }

private:
    QString _string;
    QFont _font;
};

#endif // __TY_MENU_ITEM__
