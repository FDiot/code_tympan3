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
 * \file OSplashScreen.cpp
 * \brief Splash screen widget
 *
 *
 */



#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
//Added by qt3to4:
#include <QMouseEvent>
#include <QPixmap>

#include <qapplication.h>
#include <qpainter.h>
#include <QDesktopWidget>
#include <QMessageBox>

#if defined(WIN32)
#include <crtdbg.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif


// WStyle_Splash est dispo que depuis Qt 3.1, il est equivalent a : WStyle_NoBorder|WStyle_StaysOnTop|WX11BypassWM
OSplashScreen::OSplashScreen(const QPixmap& pixmap, const QString& msgText) :
    QWidget(0, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint),
    _pix(pixmap)
{
    // Defini l'image
    QPalette _palette = palette();
    _palette.setBrush(backgroundRole(), QBrush(_pix));
    setPalette(_palette);
    resize(_pix.size());


    // Defini le message texte
    _msgLabel = new QLabel(msgText, this); // "<font color=\"#09367A\"><i><b>TYMPAN "+version+"</i></b></font>", this);
    _msgLabel->setFont(QFont("Times", 30));     // la police du label
    _msgLabel->setGeometry(80, 480, 300, 60);

    // Recupere la taille de l'ecran
    QRect scr = QApplication::desktop()->screenGeometry();
    // Centre le splash screen
    move(scr.center() - rect().center());

    show();
    repaint();
}

OSplashScreen::~OSplashScreen()
{
}

void OSplashScreen::setStatus(const QString& message, int alignment, const QColor& color)
{
    QPixmap textPix = _pix;
    QPainter painter(&textPix);
    painter.setPen(color);
    QFont tmpFont = painter.font();
    //RNU : pb de version QT : en version 4.1, il faut donner un petit chiffre, mais pas en version 4.5
#if TY_PLATFORM == TY_PLATFORM_LINUX
    tmpFont.setPointSize(3);
#else
    tmpFont.setPointSize(10);
#endif
    painter.setFont(tmpFont);

    QRect r = rect();
    r.setRect(r.x() + 10, r.y() + 10, r.width() - 20, r.height() - 20);

    painter.drawText(r, alignment, message);

    QPalette _palette = palette();

    _palette.setBrush(backgroundRole(), QBrush(textPix));
    setPalette(_palette);

    repaint();
}

#if defined(Q_WS_X11)
//void qt_wait_for_window_manager(QWidget * widget);
#endif

void OSplashScreen::finish(QWidget* mainWin)
{
#if defined(Q_WS_X11)
    //  qt_wait_for_window_manager(mainWin);
#endif

    close();
}

void OSplashScreen::repaint()
{
    QWidget::repaint();
    QApplication::flush();
}

void OSplashScreen::mousePressEvent(QMouseEvent* pEvent)
{
    hide();
}
