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
 * \file TYSetCameraCoordinates.cpp
 * \brief Repositionnement de la camera en fonction de coordonnees specifiees
 *
 *
 */




#include "TYSetCameraCoordinates.h"

#include "Tympan/models/business/OLocalizator.h"

#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qcursor.h>
#include <qgroupbox.h>
#include <qpixmap.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

#if defined(WIN32)
#include <crtdbg.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif


#define TR(id) OLocalizator::getString("TYSetCameraCoordinates", (id))


TYSetCameraCoordinates::TYSetCameraCoordinates(TYModelerFrame* pModeler) :
    QDialog(pModeler)
{
    setModal(true);

    setWindowTitle(TR("id_caption"));
    setFixedSize(220, 150);

    QGridLayout* pLayout = new QGridLayout();
    pLayout->setMargin(10);
    setLayout(pLayout);

    QLabel* pLabelMainText = new QLabel(this);
    pLabelMainText->setText(TR("id_maintext"));
    pLayout->addWidget(pLabelMainText, 0, 0, 1, 2);

    QLabel* pLabelCoordX = new QLabel(this);
    pLabelCoordX->setText(TR("id_coordX"));
    pLayout->addWidget(pLabelCoordX, 1, 0);

    _pXCoordLineEdit = new QLineEdit(this);
    _pXCoordLineEdit->setFixedSize(50, 20);
    QObject::connect(_pXCoordLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(slotXCoordChanged()));
    pLayout->addWidget(_pXCoordLineEdit, 1, 1, Qt::AlignRight);

    QLabel* pLabelCoordY = new QLabel(this);
    pLabelCoordY->setText(TR("id_coordY"));
    pLayout->addWidget(pLabelCoordY, 2, 0);

    _pYCoordLineEdit = new QLineEdit(this);
    _pYCoordLineEdit->setFixedSize(50, 20);
    QObject::connect(_pYCoordLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(slotYCoordChanged()));
    pLayout->addWidget(_pYCoordLineEdit, 2, 1, Qt::AlignRight);

    QLabel* pLabelCoordZ = new QLabel(this);
    pLabelCoordZ->setText(TR("id_coordZ"));
    pLayout->addWidget(pLabelCoordZ, 3, 0);

    _pZCoordLineEdit = new QLineEdit(this);
    _pZCoordLineEdit->setFixedSize(50, 20);
    QObject::connect(_pZCoordLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(slotZCoordChanged()));
    pLayout->addWidget(_pZCoordLineEdit, 3, 1, Qt::AlignRight);

    QPushButton* pOKButton = new QPushButton(TR("id_oktext"), this);
    QObject::connect(pOKButton, SIGNAL(clicked()), this, SLOT(accept()));
    pLayout->addWidget(pOKButton, 4, 0);

    QPushButton* pCancelButton = new QPushButton(TR("id_canceltext"), this);
    QObject::connect(pCancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    pLayout->addWidget(pCancelButton, 4, 1);

    _xCoord = _yCoord = _zCoord = 0;
}

TYSetCameraCoordinates::~TYSetCameraCoordinates()
{
}

void TYSetCameraCoordinates::slotXCoordChanged()
{
    _xCoord = _pXCoordLineEdit->text().toInt();
}

void TYSetCameraCoordinates::slotYCoordChanged()
{
    _yCoord = _pYCoordLineEdit->text().toInt();
}

void TYSetCameraCoordinates::slotZCoordChanged()
{
    _zCoord = _pZCoordLineEdit->text().toInt();
}

void TYSetCameraCoordinates::setXCoord(int xCoord)
{
    _xCoord = xCoord;
    _pXCoordLineEdit->setText(QString("%1").arg(_xCoord));
}

void TYSetCameraCoordinates::setYCoord(int yCoord)
{
    _yCoord = yCoord;
    _pYCoordLineEdit->setText(QString("%1").arg(_yCoord));
}

void TYSetCameraCoordinates::setZCoord(int zCoord)
{
    _zCoord = zCoord;
    _pZCoordLineEdit->setText(QString("%1").arg(_zCoord));
}
