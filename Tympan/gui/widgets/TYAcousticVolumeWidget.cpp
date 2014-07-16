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
 * \file TYAcousticVolumeWidget.cpp
 * \brief outil IHM pour un volume acoustique
 *
 *
 */



#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/geoacoustic/TYAcousticVolume.h"
#include "Tympan/models/business/infrastructure/TYEtage.h"
//Added by qt3to4:
#include <QGridLayout>


#define TR(id) OLocalizator::getString("TYAcousticVolumeWidget", (id))


TYAcousticVolumeWidget::TYAcousticVolumeWidget(TYAcousticVolume* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent), _isColorModified(false)
{

    resize(300, 425);
    setWindowTitle(TR("id_caption"));
    _acousticVolumeLayout = new QGridLayout();
    setLayout(_acousticVolumeLayout);

    _elmW = new TYElementWidget(pElement, this);
    _colorW = new TYColorInterfaceWidget(dynamic_cast<TYColorInterface*>(pElement), this);
    _interW = new TYAcousticInterfaceWidget(dynamic_cast<TYAcousticInterface*>(pElement), this);
    //  _volumeW = new TYVolumeInterfaceWidget(pElement, this);

    unsigned int iln = 0;

    _acousticVolumeLayout->addWidget(_elmW, iln, 0);
    _acousticVolumeLayout->addWidget(_colorW, ++iln, 0);
    //  _acousticVolumeLayout->addWidget(_volumeW, ++iln2, 0);
    _acousticVolumeLayout->addWidget(_interW, ++iln, 0);

    _groupBoxMat = new QGroupBox(this);

    if (_pElement->getParent() != NULL)
    {
        if (_pElement->getParent()->isA("TYMachine"))
        {
            _groupBoxMat->setTitle(TR("id_mat"));
        }
        else // _pParent->isA("TYBatiment");
        {
            _groupBoxMat->setTitle(TR("id_paroi"));
        }
    }
    else // _pParent != NULL
    {
        _groupBoxMat->setTitle(TR("id_paroi"));
    }

    _groupBoxMatLayout = new QGridLayout();
    _groupBoxMat->setLayout(_groupBoxMatLayout);

    _lineEditNomMat = new QLineEdit(_groupBoxMat);
    _lineEditNomMat->setEnabled(FALSE);
    _groupBoxMatLayout->addWidget(_lineEditNomMat, 0, 0);

    _pushButtonMat = new QPushButton(_groupBoxMat);
    _pushButtonMat->setText(TR("id_proprietes_button"));
    _groupBoxMatLayout->addWidget(_pushButtonMat, 0, 1);

    if (getElement()->getMateriau() != NULL)
    {
        _lineEditNomMat->setText(getElement()->getMateriau()->getName());
    }
    else
    {
        _groupBoxMat->setEnabled(FALSE);
    }

    _acousticVolumeLayout->addWidget(_groupBoxMat, ++iln, 0);

    connect(_colorW, SIGNAL(colorModified()), this, SLOT(modifyColor()));
    connect(_pushButtonMat, SIGNAL(clicked()), this, SLOT(editMat()));
}

TYAcousticVolumeWidget::~TYAcousticVolumeWidget()
{
}

void TYAcousticVolumeWidget::updateContent()
{
    _elmW->updateContent();
    _colorW->updateContent();
    _interW->updateContent();
    //  _volumeW->updateContent();
}

void TYAcousticVolumeWidget::apply()
{
    _elmW->apply();
    _colorW->apply();
    //  _volumeW->apply();

    if (_isColorModified)
    {
        TYTabAcousticSurfaceGeoNode tab = getElement()->acousticFaces();
        for (unsigned int i = 0; i < tab.size() ; ++i)
        {
            LPTYAcousticSurface pAccSurf = TYAcousticSurface::safeDownCast(tab[i]->getElement());
            if (!pAccSurf->getIsSub())
            {
                pAccSurf->setColor(getElement()->getColor());
            }
        }

        _isColorModified = false;
    }

    _interW->apply();

    _pElement->setIsAcousticModified(true);

    emit modified();
}

void TYAcousticVolumeWidget::reject()
{
    _isColorModified = false;

    // Si l'utilisateur annule, on restitue l'etat du regime courant
    //  getElement()->setCurRegime(getElement()->getCurRegime());
}

void TYAcousticVolumeWidget::editMat()
{
    if ((_pElement->getParent()->isA("TYMachine")) ||
        (dynamic_cast<TYAcousticCylinder*>(_pElement) != nullptr))
    {
        int ret = getElement()->getMateriau()->edit(this);

        if ((ret == QDialog::Accepted) && (getElement()->getMateriau() != NULL))
        {
            _lineEditNomMat->setText(getElement()->getMateriau()->getName());
        }
    }
    else // _pElement->getParent()->isA("TYBatiment")
    {
        int ret = TYEtage::safeDownCast(_pElement)->getParoi()->edit(this);

        if ((ret == QDialog::Accepted) && (TYEtage::safeDownCast(_pElement)->getParoi() != NULL))
        {
            _lineEditNomMat->setText(TYEtage::safeDownCast(_pElement)->getParoi()->getName());

            TYEtage::safeDownCast(_pElement)->updateParoi();
        } //*/
    }
}

void TYAcousticVolumeWidget::modifyColor()
{
    _isColorModified = true;

    emit colorModified();
}
