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
 * \file TYHistoWidget.h
 * \brief outil IHM pour un histogramme (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_HISTO_WIDGET__
#define __TY_HISTO_WIDGET__


#include "TYHistoWidget.h"
#include <QWidget>

class TYSpectreWidget;

/**
 * \class TYHistoWidget
 * \brief classe de l'outil IHM pour un histogramme
 */
class TYHistoWidget : public QWidget
{
    Q_OBJECT

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYHistoWidget(TYSpectreWidget* pSpectreWidget, bool dbType, QWidget* _pParent = NULL);

    /**
     * Destructeur.
     */
    virtual ~TYHistoWidget();

    /**
     * Modifier le type de db (db ou db/km)
     */
    void setDbType(bool dbType) {_dbType = dbType;};

    /**
     * Imprimer l'histogramme
     */
    void print();

protected:

    /**
     * Reimplementation du paintEvent
     */
    virtual void paintEvent(QPaintEvent* event);

    /**
     * Dessiner dans
     */
    void paintInto(QPainter* painter, int w, int h);

private:
    TYSpectreWidget* _pSpectreWidget;
    bool _dbType;
};


#endif // __TY_HISTO_WIDGET__
