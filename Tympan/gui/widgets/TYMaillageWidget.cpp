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
 * \file TYMaillageWidget.cpp
 * \brief Outil IHM pour un maillage
 *
 *
 */




#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/TYMaillage.h"
#include "Tympan/models/business/TYProjet.h"
#include "Tympan/models/business/TYCalcul.h"

#include "TYLookupTableWidget.h"

#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qcombobox.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QFrame>
#include <QLabel>



#include "TYMaillageWidget.h"

#define TR(id) OLocalizator::getString("TYMaillageWidget", (id))


TYMaillageWidget::TYMaillageWidget(TYMaillage* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{
    _dataFreq = 16.0;

    _elmW = new TYElementWidget(pElement, this);

    resize(300, 200);
    setWindowTitle(TR("id_caption"));
    _maillageLayout = new QGridLayout();
    setLayout(_maillageLayout);

    _maillageLayout->addWidget(_elmW, 0, 0);

    // Affichage du min et du max des valeurs
    QGroupBox* pGroupBoxVal = new QGroupBox(this);
    pGroupBoxVal->setTitle(TR("id_valeurs"));
    QGridLayout* pGroupBoxValLayout = new QGridLayout();
    pGroupBoxVal->setLayout(pGroupBoxValLayout);

    QLabel* pDataValMinLabel = new QLabel(TR("id_datavalmin"), pGroupBoxVal);
    pGroupBoxValLayout->addWidget(pDataValMinLabel, 0, 0);
    _lineEditDataValMin = new QLineEdit(pGroupBoxVal);
    _lineEditDataValMin->setReadOnly(true);
    pGroupBoxValLayout->addWidget(_lineEditDataValMin, 0, 1);

    QLabel* pDataValMaxLabel = new QLabel(TR("id_datavalmax"), pGroupBoxVal);
    pGroupBoxValLayout->addWidget(pDataValMaxLabel, 0, 2);
    _lineEditDataValMax = new QLineEdit(pGroupBoxVal);
    _lineEditDataValMax->setReadOnly(true);
    pGroupBoxValLayout->addWidget(_lineEditDataValMax, 0, 3);

    _maillageLayout->addWidget(pGroupBoxVal, 1, 0);

    // Choix des valeurs (dBLin, dBA, freq)
    QGroupBox* pGroupBoxData = new QGroupBox(this);
    pGroupBoxData->setTitle(TR("id_datatype"));
    QGridLayout* pGroupBoxDataLayout = new QGridLayout();
    pGroupBoxData->setLayout(pGroupBoxDataLayout);

    pGroupBoxDataLayout->setColumnStretch(0, 1);
    pGroupBoxDataLayout->setColumnStretch(1, 0);

    _buttonGroupDataType = new QButtonGroup();
    QObject::connect(_buttonGroupDataType, SIGNAL(buttonClicked(int)), this, SLOT(updateValMinMax()));
    _buttonGroupDataType->setExclusive(true);
    _pRadioButtondBA = new QRadioButton(TR("id_dba"));
    _buttonGroupDataType->addButton(_pRadioButtondBA, TYMaillage::ValGlobalDBA);
    _pRadioButtondBLin = new QRadioButton(TR("id_dblin"));
    _buttonGroupDataType->addButton(_pRadioButtondBLin, TYMaillage::ValGlobalDBLin);
    _pRadioButtonDataFreq = new QRadioButton(TR("id_datafreq"));
    _buttonGroupDataType->addButton(_pRadioButtonDataFreq, TYMaillage::DataFreq);

    QGridLayout* groupBoxDataTypeLayout = new QGridLayout();
    groupBoxDataTypeLayout->addWidget(_pRadioButtondBA, 0, 0);
    groupBoxDataTypeLayout->addWidget(_pRadioButtondBLin, 0, 1);
    groupBoxDataTypeLayout->addWidget(_pRadioButtonDataFreq, 0, 2);

    QGroupBox* groupBoxDataType = new QGroupBox();
    groupBoxDataType->setTitle(TR(""));
    groupBoxDataType->setLayout(groupBoxDataTypeLayout);


    pGroupBoxDataLayout->addWidget(groupBoxDataType, 0, 0);

    // Choix de la frequence
    _comboBoxFreq = new QComboBox(pGroupBoxData);
    pGroupBoxDataLayout->addWidget(_comboBoxFreq, 0, 1);
    updateFreqList();

    connect(_comboBoxFreq, SIGNAL(activated(int)), this, SLOT(setFrequency(int)));

    _maillageLayout->addWidget(pGroupBoxData, 2, 0);

    // Definition de la palette de couleur
    QGroupBox* pGroupBoxColor = new QGroupBox(this);
    pGroupBoxColor->setTitle(TR("id_palette"));

    QGridLayout* pGroupBoxColorLayout = new QGridLayout();
    pGroupBoxColor->setLayout(pGroupBoxColorLayout);

    // Nom de la palette
    _lineEditPaletteName = new QLineEdit(pGroupBoxColor);
    _lineEditPaletteName->setEnabled(false);
    pGroupBoxColorLayout->addWidget(_lineEditPaletteName, 0, 0);

    // Bouton d'edition de la palette
    _pushButtonEditPalette = new QPushButton(pGroupBoxColor);
    _pushButtonEditPalette->setText(TR("id_edit_palette"));
    pGroupBoxColorLayout->addWidget(_pushButtonEditPalette, 0, 1);
    connect(_pushButtonEditPalette, SIGNAL(clicked()), this, SLOT(editPalette()));

    QWidget* dummy = new QWidget();
    _lookupTableWidget = new TYLabeledLookupTableWidget(getElement()->getPalette(), dummy);
    pGroupBoxColorLayout->addWidget(dummy, 1, 0, 1, 2);

    _maillageLayout->addWidget(pGroupBoxColor, 3, 0);

    //Indication du nombre de points
    _groupBox = new QGroupBox(this);
    _groupBox->setTitle("");
    _groupBoxLayout = new QGridLayout();
    _groupBox->setLayout(_groupBoxLayout);

    QLabel* pNbPointsLabel = new QLabel(TR("id_nbpoint"), _groupBox);
    _groupBoxLayout->addWidget(pNbPointsLabel, 0, 0);
    _lineEditNbPoints = new QLineEdit(_groupBox);
    _lineEditNbPoints->setReadOnly(true);
    _groupBoxLayout->addWidget(_lineEditNbPoints, 0, 1);

    _maillageLayout->addWidget(_groupBox, 4, 0);

    // Hauteur du maillage
    QGroupBox* pGroupBoxHaut = new QGroupBox(this);
    pGroupBoxHaut->setTitle("");
    QGridLayout* pGroupBoxHautLayout = new QGridLayout();
    pGroupBoxHaut->setLayout(pGroupBoxHautLayout);

    QLabel* pHautLabel = new QLabel(TR("id_hauteur"), pGroupBoxHaut);
    pGroupBoxHautLayout->addWidget(pHautLabel, 0, 0);
    _lineEditHauteur = new QLineEdit(pGroupBoxHaut);
    pGroupBoxHautLayout->addWidget(_lineEditHauteur, 0, 1);
    QLabel* pHautUnitLabel = new QLabel(TR("id_unite_hauteur"), pGroupBoxHaut);
    pGroupBoxHautLayout->addWidget(pHautUnitLabel, 0, 2);

    _maillageLayout->addWidget(pGroupBoxHaut, 5, 0);

    updateContent();

    connect(_buttonGroupDataType, SIGNAL(buttonClicked(int)), this, SLOT(dataTypeChanged(int)));
}

TYMaillageWidget::~TYMaillageWidget()
{

}

void TYMaillageWidget::updateContent()
{
    if (getElement()->isLocked())
    {
        _elmW->setEnabled(false);
        _lineEditHauteur->setEnabled(false);
    }

    _elmW->updateContent();

    // Positionne la valeur du comboBox;
    _dataFreq = getElement()->getDataFreq();
    int indFreq = TYSpectre::getIndice(_dataFreq);
    _comboBoxFreq->setCurrentIndex(indFreq);


    _buttonGroupDataType->button(getElement()->getDataType())->setChecked(true);
    dataTypeChanged(getElement()->getDataType());

    _lineEditHauteur->setText(QString().setNum(getElement()->getHauteur()));

    updateNbPoints(static_cast<uint32>(getElement()->getPtsCalcul().size()));
    updateValMinMax();
    updatePalette();
}

void TYMaillageWidget::updateNbPoints(unsigned int nbPoints)
{
    _lineEditNbPoints->setText(QString().setNum(nbPoints));
}

void TYMaillageWidget::updateValMinMax()
{
    double val = 0.0;
    double valMin = +10000.0;
    double valMax = -10000.0;
    TYPointCalcul* pPoint = NULL;
    TYCalcul* pCalcul = static_cast<TYCalcul*>(getElement()->getParent());

    int dataType = _buttonGroupDataType->checkedId();

    for (unsigned int i = 0; i < getElement()->getPtsCalcul().size(); i++)
    {
        pPoint = getElement()->getPtsCalcul()[i];
        if (pPoint->etat() == false) { continue ; } // On ne prend pas en compte les points inactifs

        switch (dataType)
        {
            case TYMaillage::ValGlobalDBA:
                val = pPoint->getValA(); break;
            case TYMaillage::ValGlobalDBLin:
                val = pPoint->getValLin(); break;
            case TYMaillage::DataFreq:
                val = pPoint->getSpectre()->getValueReal(_dataFreq); break;
        }

        valMin = val < valMin ? val : valMin ;
        valMax = val > valMax ? val : valMax ;
    }

    _lineEditDataValMin->setText(QString().setNum(valMin, 'f', 2));
    _lineEditDataValMax->setText(QString().setNum(valMax, 'f', 2));
}

void TYMaillageWidget::updatePalette()
{
    _lineEditPaletteName->setText(getElement()->getPalette()->getName());
    _lookupTableWidget->update(getElement()->getPalette());
}

void TYMaillageWidget::apply()
{
    _elmW->apply();

    // Si la hauteur est differente
    if (getElement()->getHauteur() != _lineEditHauteur->text().toDouble())
    {
        getElement()->setHauteur(_lineEditHauteur->text().toDouble());
        TYCalcul* pCalcul = (TYCalcul*)getElement()->getParent();
        if (pCalcul && pCalcul->getSite()->getAltimetry()->containsData())
        {
            pCalcul->updateAltiRecepteurs();
        }
    }

    getElement()->setDataFreq(_dataFreq);
    getElement()->setDataType(_buttonGroupDataType->checkedId());

    emit modified();
}

void TYMaillageWidget::dataTypeChanged(int selected)
{
    if (selected == 2)
    {
        _comboBoxFreq->setEnabled(true);
    }
    else
    {
        _comboBoxFreq->setEnabled(false);
    }
}

void TYMaillageWidget::setFrequency(int freq)
{
    _dataFreq = _comboBoxFreq->currentText().toFloat();
    updateValMinMax();
}

void TYMaillageWidget::editPalette()
{
    LPTYPalette palette = getElement()->getPalette();
    if (palette->edit(this))
    {
        updatePalette();
    }
}

void TYMaillageWidget::updateFreqList()
{
    TYTabFreq tabFreq = TYSpectre::getTabFreqNorm();
    for (unsigned int i = 0 ; i < tabFreq.size() ; i++)
    {
        QString str;
        str.setNum(tabFreq[i], 'f', 1);
        _comboBoxFreq->insertItem(i, str);
    }
}
