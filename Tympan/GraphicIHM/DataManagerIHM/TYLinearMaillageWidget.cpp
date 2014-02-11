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
 * \file TYLinearMaillageWidget.cpp
 * \brief Outil IHM pour un maillage lineaire
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYLinearMaillage.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

#if defined(WIN32)
#include <crtdbg.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif


#define TR(id) OLocalizator::getString("TYLinearMaillageWidget", (id))


TYLinearMaillageWidget::TYLinearMaillageWidget(TYLinearMaillage* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{

    _maillageW = new TYMaillageWidget(pElement, this);

    resize(300, 200);
    setWindowTitle(TR("id_caption"));
    QGridLayout* pLinearMaillageLayout = new QGridLayout();
    setLayout(pLinearMaillageLayout);

    pLinearMaillageLayout->addWidget(_maillageW, 0, 0);

    QGroupBox* pGroupBox = new QGroupBox(this);
    QGridLayout* pGroupBoxLayout = new QGridLayout();
    pGroupBox->setLayout(pGroupBoxLayout);

    QLabel* pLabelDensite = new QLabel(TR("id_densite"), pGroupBox);
    _lineEditDensite = new QLineEdit(pGroupBox);
    QObject::connect(_lineEditDensite, SIGNAL(textChanged(const QString&)), this, SLOT(updateNbPoints()));
    QLabel* pLabelDensiteUnite = new QLabel(TR("id_densite_unit"), pGroupBox);
    pGroupBoxLayout->addWidget(pLabelDensite, 0, 0);
    pGroupBoxLayout->addWidget(_lineEditDensite, 0, 1);
    pGroupBoxLayout->addWidget(pLabelDensiteUnite, 0, 2);

    pLinearMaillageLayout->addWidget(pGroupBox, 1, 0);

    QGroupBox* pGroupBoxSegment = new QGroupBox(this);
    pGroupBoxSegment->setTitle(TR("id_segment"));
    QGridLayout* pGroupBoxSegmentLayout = new QGridLayout();
    pGroupBoxSegment->setLayout(pGroupBoxSegmentLayout);

    _lineEditNomSegment = new QLineEdit(pGroupBoxSegment);
    _lineEditNomSegment->setEnabled(false);
    pGroupBoxSegmentLayout->addWidget(_lineEditNomSegment, 0, 0);

    QLabel* pLongueurLabel = new QLabel(TR("id_longueur"), pGroupBoxSegment);
    pGroupBoxSegmentLayout->addWidget(pLongueurLabel, 0, 1);
    _lineEditLongueur = new QLineEdit(pGroupBoxSegment);
    _lineEditLongueur->setReadOnly(true);
    pGroupBoxSegmentLayout->addWidget(_lineEditLongueur, 0, 2);

    QPushButton* pPushButtonSegment = new QPushButton(TR("id_proprietes_button"), pGroupBoxSegment);
    pGroupBoxSegmentLayout->addWidget(pPushButtonSegment, 0, 3);

    pLinearMaillageLayout->addWidget(pGroupBoxSegment, 2, 0);

    updateContent();

    connect(pPushButtonSegment, SIGNAL(clicked()), this, SLOT(editSegment()));
}

TYLinearMaillageWidget::~TYLinearMaillageWidget()
{
}

void TYLinearMaillageWidget::updateContent()
{
    _maillageW->updateContent();

    if (getElement()->isLocked())
    {
        _lineEditDensite->setEnabled(false);
    }

    if (getElement()->getSegment())
    {
        _lineEditNomSegment->setText(getElement()->getSegment()->getName());
        _lineEditLongueur->setText(QString().setNum(getElement()->getSegment()->longueur(), 'f', 2));
    }

    _lineEditDensite->setText(QString().setNum(getElement()->getDensite(), 'f', 2));
}

void TYLinearMaillageWidget::updateNbPoints()
{
    double longueur = _lineEditLongueur->text().toDouble();
    double densite = _lineEditDensite->text().toDouble();

    _maillageW->updateNbPoints(longueur * densite);
}

void TYLinearMaillageWidget::apply()
{
    _maillageW->apply();

    // Si la densite est differente
    if ((getElement()->getDensite() != _lineEditDensite->text().toDouble()) && getElement()->getSegment())
    {
        getElement()->make(getElement()->getSegment(), _lineEditDensite->text().toDouble());
    }

    emit modified();
}

void TYLinearMaillageWidget::editSegment()
{
    int ret = getElement()->getSegment()->edit(this);

    if (ret == QDialog::Accepted)
    {
        _lineEditNomSegment->setText(getElement()->getSegment()->getName());
    }
}
