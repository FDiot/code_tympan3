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
 * \file TYCustomPopupMenu.cpp
 * \brief Definit un popup menu, necessaire pour maitriser l'ouverture automtique apres un createPopupMenu
 *
 *
 */


#include "TYCustomPopupMenu.h"

// CLM-NT35: Gestion MDI avec QT4.7
#ifdef USE_QMDIAREA
#include <QMdiArea>
#else
#include <QWorkspace>
#endif
// CLM-NT35 End
TYCustomPopupMenu::TYCustomPopupMenu(QWidget* parent)
    : QMenu(parent)
{
    QObject::connect(this, SIGNAL(aboutToShow()), this, SLOT(onAboutToShow()));
}

TYCustomPopupMenu::TYCustomPopupMenu(const QString& title, QWidget* parent)
    : QMenu(title, parent)
{
    QObject::connect(this, SIGNAL(aboutToShow()), this, SLOT(onAboutToShow()));
}


void TYCustomPopupMenu::onAboutToShow()
{
    int toto = 0;
    toto++;
}

void TYCustomPopupMenu::aboutToShow()
{
    QMenu::aboutToShow();
}

void TYCustomPopupMenu::setVisible(bool visible)
{
    QPoint point = pos();
    QPoint point2;

    if (visible)
    {
        int w = getTYMainWnd()->getWorkspace()->width();
        int h = getTYMainWnd()->getWorkspace()->height();
        point2 = getTYMainWnd()->getWorkspace()->mapFromGlobal(point);
        if ((point2.x() >= 0) && (point2.y() >= 0) &&
            (point2.x() <= w) &&
            (point2.y() <= h))
        {
            return;
        }
        QMenu::setVisible(visible);
    }
    else
    {
        QMenu::setVisible(visible);
    }
}
