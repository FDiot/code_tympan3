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
 * \file TYRectangularMaillageWidget.cpp
 * \brief outil IHM pour un maillage rectangulaire
 */


//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/TYRectangularMaillage.h"
#include "Tympan/models/business/TYCalcul.h"
#include "Tympan/gui/widgets/TYMaillageWidget.h"
#include "TYRectangularMaillageWidget.h"

#if defined(WIN32)
  #include <crtdbg.h>
  #ifdef _DEBUG
    #undef THIS_FILE
    static char THIS_FILE[] = __FILE__;
    #define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
  #endif
#endif

#define TR(id) OLocalizator::getString("TYRectangularMaillageWidget", (id))


TYRectangularMaillageWidget::TYRectangularMaillageWidget(TYRectangularMaillage* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{

    _maillageW = new TYMaillageWidget(pElement, this);

    resize(300, 200);
    setWindowTitle(TR("id_caption"));
    QGridLayout* pRectangularMaillageLayout = new QGridLayout();
    setLayout(pRectangularMaillageLayout);

    pRectangularMaillageLayout->addWidget(_maillageW, 0, 0);

    QGroupBox* pGroupBox = new QGroupBox(this);
    QGridLayout* pGroupBoxLayout = new QGridLayout();
    pGroupBox->setLayout(pGroupBoxLayout);

    QLabel* pLabelDensiteX = new QLabel(TR("id_densite_x"), pGroupBox);
    _lineEditDensiteX = new QLineEdit(pGroupBox);
    QObject::connect(_lineEditDensiteX, SIGNAL(textChanged(const QString&)), this, SLOT(updateNbPoints()));
    QLabel* pLabelDensiteUniteX = new QLabel(TR("id_densite_unit"), pGroupBox);
    pGroupBoxLayout->addWidget(pLabelDensiteX, 0, 0);
    pGroupBoxLayout->addWidget(_lineEditDensiteX, 0, 1);
    pGroupBoxLayout->addWidget(pLabelDensiteUniteX, 0, 2);

    QLabel* pLabelDensiteY = new QLabel(TR("id_densite_y"), pGroupBox);
    _lineEditDensiteY = new QLineEdit(pGroupBox);
    QObject::connect(_lineEditDensiteY, SIGNAL(textChanged(const QString&)), this, SLOT(updateNbPoints()));
    QLabel* pLabelDensiteUniteY = new QLabel(TR("id_densite_unit"), pGroupBox);
    pGroupBoxLayout->addWidget(pLabelDensiteY, 1, 0);
    pGroupBoxLayout->addWidget(_lineEditDensiteY, 1, 1);
    pGroupBoxLayout->addWidget(pLabelDensiteUniteY, 1, 2);

    pRectangularMaillageLayout->addWidget(pGroupBox, 1, 0);

    QGroupBox* pGroupBoxRectangle = new QGroupBox(this);
    pGroupBoxRectangle->setTitle(TR("id_rectangle"));
    QGridLayout* pGroupBoxRectangleLayout = new QGridLayout();
    pGroupBoxRectangle->setLayout(pGroupBoxRectangleLayout);

    _lineEditNomRectangle = new QLineEdit(pGroupBoxRectangle);
    _lineEditNomRectangle->setEnabled(false);
    pGroupBoxRectangleLayout->addWidget(_lineEditNomRectangle, 0, 0);

    QLabel* pSurfaceLabel = new QLabel(TR("id_surface"), pGroupBoxRectangle);
    pGroupBoxRectangleLayout->addWidget(pSurfaceLabel, 0, 1);
    _lineEditSurface = new QLineEdit(pGroupBoxRectangle);
    _lineEditSurface->setReadOnly(true);
    pGroupBoxRectangleLayout->addWidget(_lineEditSurface, 0, 2);

    QPushButton* pPushButtonRectangle = new QPushButton(TR("id_proprietes_button"), pGroupBoxRectangle);
    pGroupBoxRectangleLayout->addWidget(pPushButtonRectangle, 0, 3);

    pRectangularMaillageLayout->addWidget(pGroupBoxRectangle, 2, 0);

    updateContent();

    connect(pPushButtonRectangle, SIGNAL(clicked()), this, SLOT(editRectangle()));
}

TYRectangularMaillageWidget::~TYRectangularMaillageWidget()
{
}

void TYRectangularMaillageWidget::updateContent()
{
    _maillageW->updateContent();

    if (getElement()->isLocked())
    {
        _lineEditDensiteX->setEnabled(false);
        _lineEditDensiteY->setEnabled(false);
    }

    if (getElement()->getRectangle())
    {
        _lineEditNomRectangle->setText(getElement()->getRectangle()->getName());
        _lineEditSurface->setText(QString().setNum(getElement()->getRectangle()->surface(), 'f', 2));
    }

    _lineEditDensiteX->setText(QString().setNum(getElement()->getDensiteX(), 'f', 4));
    _lineEditDensiteY->setText(QString().setNum(getElement()->getDensiteY(), 'f', 4));
}

void TYRectangularMaillageWidget::updateNbPoints()
{
    double surface = _lineEditSurface->text().toDouble();
    double densiteX = _lineEditDensiteX->text().toDouble();
    double densiteY = _lineEditDensiteY->text().toDouble();

    _maillageW->updateNbPoints((int)(surface * densiteX * densiteY));
}

void TYRectangularMaillageWidget::apply()
{
    _maillageW->apply();

    // Si les densites sont differentes
    double densiteX = _lineEditDensiteX->text().toDouble();
    double densiteY = _lineEditDensiteY->text().toDouble();

    if (((getElement()->getDensiteX() != densiteX) || (getElement()->getDensiteY() != densiteY)) && getElement()->getRectangle())
    {
        getElement()->make(getElement()->getRectangle(), densiteX, densiteY);

        // La densite a changee, il faut mettre a jour l'altimetrie
        LPTYCalcul pCalcul = TYCalcul::safeDownCast(getElement()->getParent());
        pCalcul->updateAltiRecepteurs();
    }

    emit modified();
}

void TYRectangularMaillageWidget::editRectangle()
{
    int ret = getElement()->getRectangle()->edit(this);

    if ((ret == QDialog::Accepted) && (getElement()->getRectangle()))
    {
        _lineEditNomRectangle->setText(getElement()->getRectangle()->getName());

    }
}
