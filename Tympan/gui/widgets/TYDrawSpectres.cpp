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
 * \file TYDrawSpectres.cpp
 * \brief outil IHM pour l'affichage des spectres
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include <qprinter.h>
#include <qpainter.h>
#include <qpixmap.h>

//Added by qt3to4:
#include <QHBoxLayout>
#include <QBoxLayout>
#include <QPaintEvent>
#include <QGridLayout>
#include <QMouseEvent>
#include <QScrollBar>
#include <QPrintDialog>


#include "TYDrawSpectres.h"

#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/TYPreferenceManager.h"
#include "Tympan/gui/widgets/TYPrintDialog.h"

#define NBSPECTRESMAX 5

using namespace Qt;

#define TR(id) OLocalizator::getString("TYDrawSpectres", (id))

QString TYDrawSpectresColor[] =
{
    "red", "green", "blue", "yellow", "black"
};

TYScrollView::TYScrollView(QWidget* parent, const char* name, Qt::WFlags f):
    QScrollArea(parent)
{
    setObjectName(name);

    _posY = 0;
    _zoom = 1;
    _isRight = false;

    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(repaint()));
}

TYScrollView::~TYScrollView()
{
}

void TYScrollView::contentsMousePressEvent(QMouseEvent* e)
{
    _posY = e->y();
    if (e->button() == Qt::RightButton)
    {
        _isRight = true;
    }
}

void TYScrollView::contentsMouseReleaseEvent(QMouseEvent* e)
{
    _isRight = false;
}

void TYScrollView::contentsMouseMoveEvent(QMouseEvent* e)
{
    if (_isRight)
    {
        if (((e->y() - _posY) > 10) && (_zoom > 1))
        {
            _zoom -= 1;
            _posY = e->y();
            emit updateGraph();
        }
        else if ((_posY - e->y()) > 10)
        {
            _zoom += 1;
            _posY = e->y();
            emit updateGraph();
        }
    }
}

void TYScrollView::paintEvent(QPaintEvent* e /*= NULL*/)
{
    emit updateGraph();
}

TYDrawSpectres::TYDrawSpectres(QWidget* parent, const char* name, Qt::WFlags f):
    QWidget(parent, f)
{
    // On recupere les settings
    TYPreferenceManager::loadGeometryFromPreferences(metaObject()->className(), this);

    setWindowTitle(TR("id_caption"));
    resize(300, 400);
    _isFitMode = true;

    _gridLayout = new QGridLayout(this);
    _gridLayout->setMargin(10);
    _gridLayout->setSpacing(10);
    _scrollView = new TYScrollView(this);
    _graphic = new QWidget(_scrollView);
    //QBoxLayout * pLayout = new QHBoxLayout();
    _scrollView->setWidget(_graphic);
    //_scrollView->widget()->setLayout(pLayout);

    //pLayout->addWidget(_graphic);
    _gridLayout->addWidget(_scrollView, 0, 0, 1, 4);
    _pushButtonPrint = new QPushButton(this);
    _pushButtonPrint->setText(TR("id_print"));
    _gridLayout->addWidget(_pushButtonPrint, 1, 2);
    _pushButtonClose = new QPushButton(this);
    _pushButtonClose->setText(TR("id_close"));
    _gridLayout->addWidget(_pushButtonClose, 1, 3);
    QPushButton* pushButtonFit = new QPushButton(this);
    pushButtonFit->setText(TR("id_fit"));
    pushButtonFit->setCheckable(true);
    pushButtonFit->setChecked(true);
    _gridLayout->addWidget(pushButtonFit, 1, 1);

    QPalette palette;
    palette.setColor(_graphic->backgroundRole(), white);
    _graphic->setPalette(palette);
    QPalette palette2;
    palette2.setColor(_scrollView->widget()->backgroundRole(), white);
    _scrollView->widget()->setPalette(palette2);
    //_scrollView->setResizePolicy(QScrollView::AutoOne );

    _scrollView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _scrollView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    _printer = new QPrinter();
    _printer->setFullPage(true);
    _printer->setPageSize(QPrinter::A4);

    setLayout(_gridLayout);

    _painter = new QPainter(_graphic);

    _graphic->resize(WIDTH, (int)(HEIGHT * _scrollView->getZoom()));

    connect(_pushButtonClose, SIGNAL(clicked()), this, SLOT(close()));
    connect(pushButtonFit, SIGNAL(toggled(bool)), this, SLOT(changeMode(bool)));
    connect(_pushButtonPrint, SIGNAL(clicked()), this, SLOT(print()));
    connect(_scrollView, SIGNAL(contentsMoving(int, int)), this, SLOT(draw()));
    connect(_scrollView, SIGNAL(updateGraph()), this, SLOT(draw()));
}

TYDrawSpectres::~TYDrawSpectres()
{
    // On sauve les settings
    TYPreferenceManager::saveGeometryToPreferences(metaObject()->className(), this);

    delete _printer;
    delete _painter;
}

int TYDrawSpectres::addOrRemoveSpectre(LPTYSpectre pSpectre, bool rem /*=false*/)
{
    bool found = false;

    // Suppression de l'element s'il est deja dans la liste
    TYTabLPSpectre::iterator iter;
    for (iter = _listSpectre.begin(); iter < _listSpectre.end(); iter++)
    {
        if ((TYSpectre*)(*iter) == (TYSpectre*)pSpectre)
        {
            found = true;
            _listSpectre.erase(iter);
            draw();
        }
    }

    if (rem) { found = true; }

    // Sinon ajout de l'element a la liste
    if ((!found) && (_listSpectre.size() < NBSPECTRESMAX))
    {
        _listSpectre.push_back(pSpectre);
        draw();
        return static_cast<uint32>(_listSpectre.size());
    }

    if (!rem) { show(); }

    return -1;
}

void TYDrawSpectres::print()
{
    TYPrintDialog* pDialog = new TYPrintDialog(this);
    pDialog->_groupBoxProjet->setEnabled(false);
    pDialog->_groupBoxCalcul->setEnabled(false);

    if (pDialog->exec() == QDialog::Accepted)
    {
        QPrintDialog dialog(_printer, this);
        if (dialog.exec())
        {
            QPainter paint(_printer);

            double Wmetric = _printer->width();
            double Hmetric = _printer->height() * 4 / 5;
            int w = int(double(_graphic->width() * Wmetric) / double(_scrollView->frameRect().width()));
            int h = int(double(_graphic->height() * Hmetric) / double(_scrollView->frameRect().height()));

            QPixmap tmpPix;
            tmpPix = tmpPix.copy(0, 0, w, h);
            tmpPix.fill(white);
            QPainter paint0(&tmpPix);
            draw(&paint0, w, h);

            int mid = int(_printer->width() / 2) - int(paint.fontMetrics().width(pDialog->_lineEditTete->text()) / 2);
            paint.drawText(mid, 20, pDialog->_lineEditTete->text());

            //int x = int(double(w*_scrollView->contentsX())/double(_graphic->width()));
            //int y = int(double(h*_scrollView->contentsY())/double(_graphic->height()));
            int x = 0;//int(double(w*_scrollView->contentsX())/double(_graphic->width()));
            int y = 0;//int(double(h*_scrollView->contentsY())/double(_graphic->height()));
            paint.drawPixmap(0, 30, tmpPix, x, y, (int)Wmetric, (int)Hmetric);

            mid = int(_printer->width() / 2) - int(paint.fontMetrics().width(pDialog->_lineEditPied->text()) / 2);
            paint.drawText(mid, _printer->height() - 20, pDialog->_lineEditPied->text());
            paint.setFont(QFont("Helvetica" , 15, QFont::Bold));
            mid = int(_printer->width() / 2) - int(paint.fontMetrics().width(pDialog->_lineEditTitre->text()) / 2);
            paint.drawText(mid, _printer->height() * 14 / 15, pDialog->_lineEditTitre->text());
            paint.setFont(QFont());

        }
    }
}

void TYDrawSpectres::draw(QPainter* paint, int w, int h)
{
    if (paint == NULL) { paint = _painter; }
    if (w == 0) { w = WIDTH; }
    if (h == 0)
    {
        h = (int)(HEIGHT * _scrollView->getZoom());
        _graphic->resize(WIDTH, (int)(HEIGHT * _scrollView->getZoom()));
    }

    if (_isFitMode)
    {
        w = _scrollView->frameRect().width();
        h = _scrollView->frameRect().height();
        _graphic->resize(w, h);
    }

    // Initialisation des parametres
    double width = double(w);
    double height = double(h);
    double value[50], hauteur, L = width * 2 / 3, hzero;
    double hmax = 0, largeur = 0.0, x;
    int val = -1;
    unsigned int i, j;
    double hmin = 0, seuil = -20;
    QString num;
    double textPos;
    double midFreq;

    for (j = 0; j < _listSpectre.size(); j++)
    {
        largeur = L / (_listSpectre[0]->getNbValues() + 1);
        for (i = 0; i < _listSpectre[j]->getNbValues(); i++)
        {
            value[i] = _listSpectre[j]->getTabValReel()[i];
            if (value[i] < seuil) { value[i] = 0; }
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
    }
    double echelle = height * 7 / 10 / (-hmin + hmax);
    hzero = height * 8.5 / 10 + hmin * echelle;
    if ((hmin == 0) && (hmax == 0))
    {
        hzero = height / 2;
    }

    // Courbe
    for (j = 0; j < _listSpectre.size(); j++)
    {
        x = width * 1 / 6 + 10;
        for (i = 0; i < _listSpectre[j]->getNbValues(); i++)
        {
            value[i] = _listSpectre[j]->getTabValReel()[i];
            if (value[i] < seuil) { value[i] = 0; }
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
        midFreq = double(_listSpectre[j]->getNbValues()) / 2;

        if ((hmin != 0) || (hmax != 0))
        {
            hauteur = value[0] * echelle;
            double hprev = hzero - hauteur;
            x += largeur;
            paint->setPen(QPen(QBrush(QColor(TYDrawSpectresColor[j])), 2)) ;

            for (int k = 1; (int)k < _listSpectre[j]->getNbValues(); k++)
            {
                hauteur = value[k] * echelle;
                paint->drawLine((int)x, (int)hprev, (int)(x + largeur), (int)(hzero - hauteur));
                hprev = hzero - hauteur;

                if (val != ROUND(value[k]))
                {
                    paint->setPen(QPen(Qt::black, 2)) ;
                    val = ROUND(value[k]);
                    paint->drawLine((int)(width * 1 / 6 + 5), (int)(hzero - val * echelle),
                                    (int)(width * 1 / 6 + 15), (int)(hzero - val * echelle));
                    paint->drawText((int)(width * 1 / 6 - 25), (int)(hzero - val * echelle), num.setNum(val));
                    paint->setPen(QPen(QBrush(QColor(TYDrawSpectresColor[j])), 2)) ;
                }

                if (((k == int(midFreq)) || (k == (_listSpectre[j]->getNbValues() - 1))) && (j == 0))
                {
                    paint->setPen(QPen(Qt::black, 2)) ;
                    if (hauteur < 0)
                    {
                        textPos = hzero - 5;
                    }
                    else
                    {
                        textPos = hzero + 17;
                    }
                    paint->drawLine((int)x, (int)(hzero + 5), (int)x, (int)(hzero - 5));
                    paint->drawText((int)(x - 8), (int)textPos, num.setNum(_listSpectre[j]->getTabFreqNorm()[k]));
                    paint->setPen(QPen(QBrush(QColor(TYDrawSpectresColor[j])), 2)) ;
                }
                x += largeur;
            }
            paint->setPen(QPen(Qt::black, 2));
            paint->drawLine((int)(width * 1 / 6), (int)hzero, (int)(width * 5 / 6), (int)hzero);
            paint->drawLine((int)(width * 1 / 6 + 10), (int)(height * 18 / 20),
                            (int)(width * 1 / 6 + 10), (int)(hzero - hmax * echelle - 10));
            paint->drawText((int)(width * 1 / 6), (int)(hzero - hmax * echelle - 20), "dB");
            paint->drawText((int)(width * 5.1 / 6), (int)hzero, TR("id_abscisse"));
        }
    }
}

int TYDrawSpectres::isInList(LPTYSpectre pSpectre)
{
    TYTabLPSpectre::iterator iter;
    int i = 0;
    for (iter = _listSpectre.begin(); iter < _listSpectre.end(); iter++)
    {
        if ((*iter) == pSpectre)
        {
            return i;
        }
        i++;
    }

    return -1;
}

void TYDrawSpectres::changeMode(bool mode)
{
    _isFitMode = mode;
    _scrollView->setZoom(1);
    if (mode)
    {
        _scrollView->setWidgetResizable(true);
    }
    else
    {
        _scrollView->setWidgetResizable(false);
    }

    _scrollView->update();
}
