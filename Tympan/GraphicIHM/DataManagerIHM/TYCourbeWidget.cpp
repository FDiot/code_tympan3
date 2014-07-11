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
 * \file TYCourbeWidget.cpp
 * \brief outil IHM pour une courbe
 *
 *
 *
 */


#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/models/business/acoustique/TYSpectre.h"
#include "TYCourbeWidget.h"
#include "TYSpectreWidget.h"
#include <QPainter>
#include "Tympan/Tools/OLocalizator.h"
#include "TYPrintDialog.h"
#include <QPrinter>
#include <QPrintDialog>

#define TR(id) OLocalizator::getString("TYSpectreWidget", (id))

using namespace Qt;

TYCourbeWidget::TYCourbeWidget(TYSpectreWidget* pSpectreWidget, bool dbType, QWidget* _pParent /*=NULL*/):
    QWidget(_pParent)
{
    _pSpectreWidget = pSpectreWidget;
    _dbType = dbType;
}

TYCourbeWidget::~TYCourbeWidget()
{
}

void TYCourbeWidget::print()
{
    TYPrintDialog* pDialog = new TYPrintDialog(this);
    pDialog->_groupBoxProjet->setEnabled(false);
    pDialog->_groupBoxCalcul->setEnabled(false);
    QPrinter* printer = new QPrinter();
    if (pDialog->exec() == QDialog::Accepted)
    {
        QPrintDialog dialog(printer, this);
        if (dialog.exec())
        {
            QPainter paint(printer);
            QPixmap tmpPix;

            int mid = int(printer->width() / 2) - int(paint.fontMetrics().width(pDialog->_lineEditTete->text()) / 2);
            int midX = int(printer->width() / 2);
            int midY = int(printer->height() / 2);
            paint.drawText(mid, 20, pDialog->_lineEditTete->text());
            mid = int(printer->width() / 2) - int(paint.fontMetrics().width(pDialog->_lineEditPied->text()) / 2);
            paint.drawText(mid, printer->height() - 20, pDialog->_lineEditPied->text());
            paint.setFont(QFont("Helvetica" , 15, QFont::Bold));
            mid = int(printer->width() / 2) - int(paint.fontMetrics().width(pDialog->_lineEditTitre->text()) / 2);
            paint.drawText(mid, printer->height() * 14 / 15, pDialog->_lineEditTitre->text());
            paint.setFont(QFont());

            double Wmetric = printer->width() * 9 / 10;
            double Hmetric = printer->height() * 8 / 9;
            double x = width();
            double y = height();
            double w = Wmetric;
            double h = Wmetric * (y / x);

            if (h > Hmetric)
            {
                h = Hmetric;
                w = Hmetric * (x / y);
            }

            paint.translate(midX - (w / 2), midY - (h / 2));
            paintInto(&paint, w, h);
        }
    }
    delete printer;
}

void TYCourbeWidget::paintEvent(QPaintEvent* event)
{
    int w, h;
    QPainter* p = new QPainter(this);
    w = width();
    h = height();

    paintInto(p, w, h);

    delete p;
}

void TYCourbeWidget::paintInto(QPainter* painter, int w, int h)
{
    int nbFreq;

    LPTYSpectre pSpectre = new TYSpectre();
    _pSpectreWidget->spectre(pSpectre);
    TYTabFreq tabFreq = TYSpectre::getTabFreqNorm(pSpectre->getForm());

    //RNU
    nbFreq = tabFreq.size();

    // Initialisation des parametres
    double width = double(w);
    double height = double(h);
    double value[50], hauteur, L = width * 2 / 3, hzero;
    double hmax = 0, largeur = L / (nbFreq + 1), x = width * 1 / 6 + 10;
    int i, val = -1;
    double hmin = 0;
    QString num;
    double textPos;
    double midFreq = double(nbFreq) / 2;
    bool valid = false;

    for (i = 0; i < nbFreq; i++)
    {
        value[i] = pSpectre->getTabValReel()[i];

        if (value[i] != TY_SPECTRE_DEFAULT_VALUE)
        {
            valid = true;
        }

        if (value[i] >= 0)
        {
            if (value[i] > hmax)
            {
                hmax = value[i];
            }
        }
        else if (value[i] < hmin)
        {
            hmin = value[i];
        }
    }

    double echelle = height * 7 / 10 / (-hmin + hmax);
    hzero = height * 8.5 / 10 + hmin * echelle;
    if ((hmin == 0) && (hmax == 0))
    {
        echelle = 1;
        hzero = height / 2;
    }

    // Courbe
    if (valid)
    {
        double valMax = 0.0;
        hauteur = value[0] * echelle;
        double hprev = hzero - hauteur;
        x += largeur;
        painter->setPen(QPen(red, 2));

        for (i = 1; i < nbFreq; i++)
        {
            hauteur = value[i] * echelle;
            painter->drawLine((int)x, (int)hprev, (int)(x + largeur), (int)(hzero - hauteur));
            hprev = hzero - hauteur;

            if (val != ROUND(value[i]))
            {
                val = ROUND(value[i]);
                valMax = MAX(val, valMax);
            }

            if ((i == int(midFreq)) || (i == (nbFreq - 1)))
            {
                if (hauteur < 0) { textPos = hzero - 5; }
                else { textPos = hzero + 17; }
                painter->drawLine((int)x, (int)(hzero + 5), (int)x, (int)(hzero - 5));
                painter->drawText((int)(x - 10), (int)textPos, num.setNum(tabFreq[i]));
            }

            x += largeur;
        }

        // Val Max
        painter->drawLine((int)(width * 1 / 6 + 5), (int)(hzero - valMax * echelle), (int)(width * 1 / 6 + 15), (int)(hzero - valMax * echelle));
        painter->drawText((int)(width * 1 / 6 - 25), (int)(hzero - valMax * echelle), num.setNum(valMax));
        // Val Max / 2
        painter->drawLine((int)(width * 1 / 6 + 5), (int)(hzero - (valMax / 2)*echelle), (int)(width * 1 / 6 + 15), (int)(hzero - (valMax / 2)*echelle));
        painter->drawText((int)(width * 1 / 6 - 25), (int)(hzero - (valMax / 2)*echelle), num.setNum((valMax / 2)));

        painter->setPen(QPen(black, 2));
        painter->drawLine((int)(width * 1 / 6), (int)(hzero), (int)(width * 5 / 6), (int)hzero);
        painter->drawLine((int)(width * 1 / 6 + 10), (int)(height * 18 / 20), (int)(width * 1 / 6 + 10), (int)(hzero - hmax * echelle - 10));
        painter->drawText((int)(width * 1 / 6), (int)(hzero - hmax * echelle - 20), "dB");
        painter->drawText((int)(width * 5 / 6), (int)hzero, TR("id_abscisse"));
    }
    else
    {
        painter->setPen(QPen(red, 3));
        painter->drawText((int)(width / 10), (int)(height / 10), TR("id_invalid_spectre"));
    }
}
