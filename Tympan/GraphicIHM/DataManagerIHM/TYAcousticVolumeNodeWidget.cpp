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
 * \file TYAcousticVolumeNodeWidget.cpp
 * \brief outil IHM pour un ensemble de volumes acoustiques
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "TYAcousticVolumeNodeWidget.h"

#include "Tympan/GraphicIHM/DataManagerIHM/TYColorInterfaceWidget.h"
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticVolumeNode.h"

#include "Tympan/GraphicIHM/DataManagerIHM/TYAcousticInterfaceWidget.h"

#include <qmenu.h>
#include <qtablewidget.h>
#include <qmessagebox.h>
//Added by qt3to4:
#include <QHBoxLayout>
#include <QBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QVBoxLayout>
#include <QHeaderView>

#define TR(id) OLocalizator::getString("TYAcousticVolumeNodeWidget", (id))


TYAcousticVolumeNodeWidget::TYAcousticVolumeNodeWidget(TYAcousticVolumeNode* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent), _isColorModified(false)
{

    _elmW = new TYElementWidget(pElement, this);
    _colorW = new TYColorInterfaceWidget(dynamic_cast<TYColorInterface*>(pElement), this);

    _interW = new TYAcousticInterfaceWidget(dynamic_cast<TYAcousticInterface*>(pElement), this);

	//  _volumeW = new TYVolumeInterfaceWidget(pElement, this);

    // Pour les element principaux, on desactive la possibilite de changer l'etat
    // (cette propriete est pilotee directement par le calcul)
    _interW->setCheckBoxRayonnantState(false);

    resize(300, 140);
    setWindowTitle(TR("id_caption"));
    _acousticVolumeNodeLayout = new QGridLayout();
    setLayout(_acousticVolumeNodeLayout);

    unsigned int iln = 0;

    _acousticVolumeNodeLayout->addWidget(_elmW, iln, 0);
    _acousticVolumeNodeLayout->addWidget(_colorW, ++iln, 0);
    //  _acousticVolumeNodeLayout->addWidget(_volumeW, ++iln, 0);
    _acousticVolumeNodeLayout->addWidget(_interW, ++iln, 0);

    QGroupBox* pGroupBoxAccVols = new QGroupBox(this);
    pGroupBoxAccVols->setTitle(TR("id_tabaccvol_box"));
    QGridLayout* pGroupBoxAccVolsLayout = new QGridLayout();
    pGroupBoxAccVols->setLayout(pGroupBoxAccVolsLayout);

    QLineEdit* pLineEditNomAccVols = new QLineEdit(pGroupBoxAccVols);
    pLineEditNomAccVols->setEnabled(FALSE);
    pGroupBoxAccVolsLayout->addWidget(pLineEditNomAccVols, 0, 0);

    QPushButton* pPushButtonAccVols = new QPushButton(pGroupBoxAccVols);
    pPushButtonAccVols->setText(TR("id_proprietes_button"));
    pGroupBoxAccVolsLayout->addWidget(pPushButtonAccVols, 0, 1);

    _acousticVolumeNodeLayout->addWidget(pGroupBoxAccVols, ++iln, 0);

    // CREATION DE LA BOITE DE DIALOGUE DE SELECTION DES VOLUMES ACOUSTIQUES

    _pAccVolsWidget = new QWidget(this, Qt::Dialog);
    _pAccVolsWidget->setWindowModality(Qt::WindowModal);
    _pAccVolsWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    QBoxLayout* pLayoutAccVols = new QVBoxLayout(_pAccVolsWidget);
    _pAccVolsWidget->setLayout(pLayoutAccVols);
    _pAccVolsWidget->setWindowTitle(TR("id_tabaccvol_box"));

    _groupBox = new QGroupBox(_pAccVolsWidget);
    _groupBoxLayout = new QGridLayout();
    _groupBox->setLayout(_groupBoxLayout);

    _tableFaces = new QTableWidget();
    _tableFaces->setColumnCount(2);
    _tableFaces->setHorizontalHeaderItem(0, new QTableWidgetItem(TR("id_nom")));
    _tableFaces->setHorizontalHeaderItem(1, new QTableWidgetItem(TR("id_emission")));

    _groupBoxLayout->addWidget(_tableFaces, 0, 0);
    pLayoutAccVols->addWidget(_groupBox);

    pLayoutAccVols->addSpacing(5);
    QBoxLayout* pBtnLayoutAccVols = new QHBoxLayout();
    pBtnLayoutAccVols->setMargin(10);
    pLayoutAccVols->addLayout(pBtnLayoutAccVols);

    pBtnLayoutAccVols->addStretch();

    QPushButton* pButtonOKAccVols = new QPushButton(TR("id_ok_btn"), _pAccVolsWidget);
    pButtonOKAccVols->setDefault(true);
    connect(pButtonOKAccVols, SIGNAL(clicked()), this, SLOT(hideAccVolWidget()));
    pBtnLayoutAccVols->addWidget(pButtonOKAccVols);

    // FIN DE CREATION DE LA BOITE DE DIALOGUE DES VOLUMES ACOUSTIQUES

    updateContent();

    connect(pPushButtonAccVols, SIGNAL(clicked()), _pAccVolsWidget, SLOT(show()));
    connect(_interW, SIGNAL(regimeChanged(int)), this, SLOT(propagateRegime(int)));
    connect(_colorW, SIGNAL(colorModified()), this, SLOT(modifyColor()));
    connect(_pAccVolsWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(contextMenuRequested(const QPoint&)));
}

TYAcousticVolumeNodeWidget::~TYAcousticVolumeNodeWidget()
{
}

void TYAcousticVolumeNodeWidget::updateContent()
{
    _elmW->updateContent();
    _colorW->updateContent();
    //  _volumeW->updateContent();
    _interW->updateContent();

    updateFromSubVol();
}

void TYAcousticVolumeNodeWidget::apply()
{
    _elmW->apply();
    _colorW->apply();
    //  _volumeW->apply();
    _interW->apply();

    applyToSubVol();

    _pElement->setIsAcousticModified(true);

    emit modified();
}

void TYAcousticVolumeNodeWidget::editAccVol(const int& item)
{
    LPTYAcousticVolume pVol = getElement()->getAcousticVol(item);
    QTableWidgetItem* pCheck = (QTableWidgetItem*) _tableFaces->item(item, 1);
    pVol->setIsRayonnant(pCheck->checkState() == Qt::Checked, false);
    QTableWidgetItem* pText  = (QTableWidgetItem*)_tableFaces->item(item, 0);
    pVol->setName(pText->text());

    int ret = pVol->edit(_pAccVolsWidget);

    if (ret == QDialog::Accepted)
    {
        pCheck->setCheckState(pVol->getIsRayonnant() ? Qt::Checked : Qt::Unchecked);
        pText->setText(pVol->getName());
    }
}

void TYAcousticVolumeNodeWidget::remAccVol(const int& item)
{
    int ret = QMessageBox::warning(this, TR("id_attention"), TR("id_are_you_sure"), TR("id_button_oui"), TR("id_button_non"), QString::null, 0, 1);

    if (ret == 0)
    {
        LPTYAcousticVolume pVol = getElement()->getAcousticVol(item);

        LPTYAcousticVolumeNode pVolNode = TYAcousticVolumeNode::safeDownCast(pVol->getParent());

        if (pVolNode) { pVolNode->remAcousticVol(pVol); }

        updateContent();

        emit modified(); // Il y a eu modification
    }

}

void TYAcousticVolumeNodeWidget::contextMenuRequested(const QPoint& pt)
{
    QPoint point = _tableFaces->mapFrom(_pAccVolsWidget, pt);

    if ((point.x() >= 0) && (point.y() >= 0) && (point.x() <= _tableFaces->width()) && (point.y() <= _tableFaces->height()))
    {
        QPoint resPoint = QPoint(point.x(), point.y() - _tableFaces->horizontalHeader()->height());
        QTableWidgetItem* item = _tableFaces->itemAt(resPoint);
        if (item)
        {
            QMenu* pMenu = new QMenu(this);

            QAction* prop = pMenu->addAction(TR("id_property"));
            QAction* del  = pMenu->addAction(TR("id_delete"));

            QAction* ret = pMenu->exec(_tableFaces->mapToGlobal(point));

            if (ret)
            {
                if (ret == prop) { editAccVol(_tableFaces->indexAt(resPoint).row()); }
                else if (ret == del) { remAccVol(_tableFaces->indexAt(resPoint).row()); }
            }
        }
    }
}

void TYAcousticVolumeNodeWidget::propagateRegime(int regime)
{
    // On met a jour la boite de dialogue des volumes
    updateFromSubVol();

    emit regimeChanged(regime);
}


void TYAcousticVolumeNodeWidget::hideAccVolWidget()
{
    // On met a jour les sous volumes qui ont pu etre modifies
    applyToSubVol();

    _pAccVolsWidget->hide();
}


void TYAcousticVolumeNodeWidget::applyToSubVol()
{
    for (int i = 0; i < _tableFaces->rowCount(); i++)
    {
        LPTYAcousticVolume pVol = getElement()->getAcousticVol(i);
        QTableWidgetItem* pCheck = (QTableWidgetItem*) _tableFaces->item(i, 1);
        pVol->setIsRayonnant(pCheck->checkState() == Qt::Checked, false);
        QTableWidgetItem* pText  = (QTableWidgetItem*)_tableFaces->item(i, 0);
        pVol->setName(pText->text());

        if (_isColorModified)
        {
            // Couleur
            TYTabAcousticSurfaceGeoNode tab = pVol->acousticFaces();
            pVol->setColor(getElement()->getColor());
            for (unsigned int i = 0; i < tab.size() ; ++i)
            {
                LPTYAcousticSurface pAccSurf = TYAcousticSurface::safeDownCast(tab[i]->getElement());
                if (!pAccSurf->getIsSub())
                {
                    pAccSurf->setColor(getElement()->getColor());
                }
            }
        }
    }
}

void TYAcousticVolumeNodeWidget::updateFromSubVol()
{
    for (unsigned int i = 0, row = 0; i < getElement()->getTabAcousticVol().size() ; i++, row++)
    {
        LPTYAcousticVolume pVol = getElement()->getAcousticVol(i);
        _tableFaces->setRowCount(i + 1);
        QTableWidgetItem* pItem = new QTableWidgetItem(pVol->getName());
        _tableFaces->setItem(row, 0, pItem);
        QTableWidgetItem* pCheckItemEmit = new QTableWidgetItem(" ");
        pCheckItemEmit->setCheckState(pVol->getIsRayonnant() ? Qt::Checked : Qt::Unchecked);
        _tableFaces->setItem(row, 1, pCheckItemEmit);
    }
}

void TYAcousticVolumeNodeWidget::modifyColor()
{
    _isColorModified = true;
}
