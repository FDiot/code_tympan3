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
 * \file OSplashScreen.h
 * \brief Splash screen widget (fichier header)
 */

#ifndef __O_SPLASH_SCREEN__
#define __O_SPLASH_SCREEN__


#include <qwidget.h>
#include <qpixmap.h>
//Added by qt3to4:
#include <QMouseEvent>


/**
 * Splash screen widget.
 */
class OSplashScreen : public QWidget
{
public:
    /**
     * \fn OSplashScreen(const QPixmap &pixmap, const QString & msgText)
     * \brief Constructeur par defaut.
     */
    OSplashScreen(const QPixmap& pixmap, const QString& msgText);
    /**
     * \fn virtual ~OSplashScreen()
     * \brief Destructeur
     */
    virtual ~OSplashScreen();

    /**
     * \fn void setStatus(const QString & message, int alignment = Qt::AlignLeft, const QColor & color = Qt::black)
     * \brief Affiche un nouveau message indiquant le status de l'application.
     */
    void setStatus(const QString& message, int alignment = Qt::AlignLeft, const QColor& color = Qt::black);

    /**
     * \fn void finish(QWidget * pMainWin)
     * \brief Indique que le chargement de l'application est termine.
     */
    void finish(QWidget* pMainWin);

    /**
     * \fn void repaint()
     * \brief Mise a jour.
     */
    void repaint();


protected:
    void mousePressEvent(QMouseEvent* pEvent);


private:
    ///L'image du splash screen.
    QPixmap _pix;

    // CLM-NT33: gestion du changement de version
    QLabel* _msgLabel;
};


#endif //__O_SPLASH_SCREEN__
