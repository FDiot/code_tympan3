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
 * \file YEtatsWidget.cpp
 * \brief objet IHM pour les etats
 *
 *
 */


#include <qtablewidget.h>
#include <qtabwidget.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qmenu.h>
#include <qstringlist.h>
//Added by qt3to4:
#include <QHBoxLayout>
#include <QBoxLayout>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHeaderView>

#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/TYCalcul.h"
#include "Tympan/models/business/TYProjet.h"

#if defined(WIN32)
#include <crtdbg.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif


#include "TYEtatsWidget.h"

#define TR(id) OLocalizator::getString("TYCalculWidget", (id))


TYEtatsWidget::TYEtatsWidget(TYCalcul* pElement, QWidget* _pParent /*=NULL*/): QDialog(_pParent), _pElement(pElement)
{

    this->setWindowTitle(TR("id_etat_title"));


    resize(400, 400);

    _elmW = new TYElementWidget(pElement, this);

    // Creation du widget de gestion des etats
    QBoxLayout* pLayoutEtats = new QVBoxLayout(this);
    setLayout(pLayoutEtats);

    pLayoutEtats->addWidget(_elmW);

    QGroupBox* groupBox5 = new QGroupBox(this);
    groupBox5->setTitle(TR("id_map_box"));
    QGridLayout* groupBox5Layout = new QGridLayout();
    groupBox5->setLayout(groupBox5Layout);

    _tabWidget = new QTabWidget(groupBox5);

    // Table des sources
    _tableSource = new QTableWidget();
    _tableSource->setColumnCount(3);
    _tableSource->setHorizontalHeaderItem(0, new QTableWidgetItem(TR("id_nom")));
    _tableSource->setHorizontalHeaderItem(1, new QTableWidgetItem(TR("id_emission")));
    _tableSource->setHorizontalHeaderItem(2, new QTableWidgetItem(TR("id_regime")));

    _tabWidget->insertTab(0, _tableSource, TR("id_etat_source"));

    // Table des machines
    _tableMachine = new QTableWidget();
    _tableMachine->setColumnCount(3);
    _tableMachine->setHorizontalHeaderItem(0, new QTableWidgetItem(TR("id_nom")));
    _tableMachine->setHorizontalHeaderItem(1, new QTableWidgetItem(TR("id_emission")));
    _tableMachine->setHorizontalHeaderItem(2, new QTableWidgetItem(TR("id_regime")));

    _tabWidget->insertTab(1, _tableMachine, TR("id_etat_machine"));

    // Table des batiments
    _tableBatiment = new QTableWidget();
    _tableBatiment->setColumnCount(3);
    _tableBatiment->setHorizontalHeaderItem(0, new QTableWidgetItem(TR("id_nom")));
    _tableBatiment->setHorizontalHeaderItem(1, new QTableWidgetItem(TR("id_emission")));
    _tableBatiment->setHorizontalHeaderItem(2, new QTableWidgetItem(TR("id_regime")));

    _tabWidget->insertTab(2, _tableBatiment, TR("id_etat_batiment"));

    // Table des sources lineique
    _tableSourceLin = new QTableWidget();
    _tableSourceLin->setColumnCount(3);
    _tableSourceLin->setHorizontalHeaderItem(0, new QTableWidgetItem(TR("id_nom")));
    _tableSourceLin->setHorizontalHeaderItem(1, new QTableWidgetItem(TR("id_emission")));
    _tableSourceLin->setHorizontalHeaderItem(2, new QTableWidgetItem(TR("id_regime")));

    _tabWidget->insertTab(3, _tableSourceLin, TR("id_etat_source_lin"));

    groupBox5Layout->addWidget(_tabWidget, 0, 0);

    pLayoutEtats->addWidget(groupBox5);

    pLayoutEtats->addSpacing(5);
    QBoxLayout* pBtnLayoutEtats = new QHBoxLayout();
    pBtnLayoutEtats->setMargin(10);
    pLayoutEtats->addLayout(pBtnLayoutEtats);
    pBtnLayoutEtats->addStretch();


    QPushButton* pButtonOKEtats = new QPushButton(TR("id_ok_btn"), this);
    pButtonOKEtats->setDefault(true);
    pBtnLayoutEtats->addWidget(pButtonOKEtats);
    pBtnLayoutEtats->insertSpacing(2, 4);
    QPushButton* pButtonCancelEtats = new QPushButton(TR("id_cancel_btn"), this);
    pButtonCancelEtats->setDefault(false);
    pBtnLayoutEtats->addWidget(pButtonCancelEtats);

    connect(pButtonOKEtats, SIGNAL(clicked()), this, SLOT(apply()));
    connect(pButtonCancelEtats, SIGNAL(clicked()), this, SLOT(reject()));

    updateContent();
}

TYEtatsWidget::~TYEtatsWidget()
{
}

void TYEtatsWidget::updateContent()
{
    int row = 0;

    //if (_pElement && (_pElement->getState()==TYCalcul::Locked)) // DTn : Desactive 20110915
    //{
    //  _tableSource->setEnabled(false);
    //  _tableMachine->setEnabled(false);
    //  _tableBatiment->setEnabled(false);
    //  _tableSourceLin->setEnabled(false);
    //}

    TYMapPtrElementBool& mapEtat = _pElement->getEmitAcVolNode();
    TYMapPtrElementInt& mapRegime = _pElement->getMapElementRegime();
    TYMapPtrElementBool::iterator iterEmit;

    // Update des bâtiments
    mapRegime.begin();
    for (iterEmit = mapEtat.begin(); iterEmit != mapEtat.end(); iterEmit++)
    {
        TYElement* pEmetteur = (*iterEmit).first;
        if (strcmp(pEmetteur->getClassName(), "TYBatiment") == 0)
        {
            _tabPtrBatiment.push_back(pEmetteur);

            _tableBatiment->setRowCount(_tableBatiment->rowCount() + 1);
            QTableWidgetItem* pItemEmit = new QTableWidgetItem((*iterEmit).first->getName());
            _tableBatiment->setItem(row, 0, pItemEmit);
            QTableWidgetItem* pCheckItemEmit = new QTableWidgetItem("");
            pCheckItemEmit->setCheckState((*iterEmit).second ? Qt::Checked : Qt::Unchecked);
            _tableBatiment->setItem(row, 1, pCheckItemEmit);

            // Ajout des comboBox
            QStringList listRegimes;  // Liste des regimes pour "peupler" le combo
            TYAcousticVolumeNode* pVolNode = TYAcousticVolumeNode::safeDownCast(pEmetteur);
            QString nom;
            bool status = true;
            for (short i = 0 ; i < pVolNode->getNbRegimes(); i++)
            {
                nom = pVolNode->getRegimeNb(i, status).getName();
                listRegimes.append(nom);
            }

            QComboBox* pComboRegime = new QComboBox();
            pComboRegime->addItems(listRegimes);
            pComboRegime->setCurrentIndex(mapRegime[pEmetteur]);
            _tableBatiment->setCellWidget(row, 2, pComboRegime);

            row++;
        }
    }

    // Update des Machines
    row = 0;
    mapRegime.begin();
    for (iterEmit = mapEtat.begin(); iterEmit != mapEtat.end(); iterEmit++)
    {
        TYElement* pEmetteur = (*iterEmit).first;
        if (strcmp(pEmetteur->getClassName(), "TYMachine") == 0)
        {
            _tabPtrMachine.push_back(pEmetteur);

            _tableMachine->setRowCount(_tableMachine->rowCount() + 1);
            QTableWidgetItem* pItemEmit = new QTableWidgetItem((*iterEmit).first->getName());
            _tableMachine->setItem(row, 0, pItemEmit);
            QTableWidgetItem* pCheckItemEmit = new QTableWidgetItem("");
            pCheckItemEmit->setCheckState((*iterEmit).second ? Qt::Checked : Qt::Unchecked);
            _tableMachine->setItem(row, 1, pCheckItemEmit);

            // Ajout des comboBox
            QStringList listRegimes;  // Liste des regimes pour "peupler" le combo
            TYAcousticVolumeNode* pVolNode = TYAcousticVolumeNode::safeDownCast(pEmetteur);
            QString nom;
            bool status = true;
            for (short i = 0 ; i < pVolNode->getNbRegimes(); i++)
            {
                nom = pVolNode->getRegimeNb(i, status).getName();
                listRegimes.append(nom);
            }

            QComboBox* pComboRegime = new QComboBox();
            pComboRegime->addItems(listRegimes);
            pComboRegime->setCurrentIndex(mapRegime[pEmetteur]);
            _tableMachine->setCellWidget(row, 2, pComboRegime);

            row++;
        }
    }

    // Update des Sources ponctuelles
    row = 0;
    for (iterEmit = mapEtat.begin(); iterEmit != mapEtat.end(); iterEmit++)
    {
        TYElement* pEmetteur = (*iterEmit).first;
        if (strcmp(pEmetteur->getClassName(), "TYUserSourcePonctuelle") == 0)
        {
            _tabPtrSourcePonctuelle.push_back(pEmetteur);

            _tableSource->setRowCount(_tableSource->rowCount() + 1);
            QTableWidgetItem* pItemEmit = new QTableWidgetItem((*iterEmit).first->getName());
            _tableSource->setItem(row, 0, pItemEmit);
            QTableWidgetItem* pCheckItemEmit = new QTableWidgetItem("");
            pCheckItemEmit->setCheckState((*iterEmit).second ? Qt::Checked : Qt::Unchecked);
            _tableSource->setItem(row, 1, pCheckItemEmit);

            // Ajout des comboBox
            QStringList listRegimes;  // Liste des regimes pour "peupler" le combo
            TYUserSourcePonctuelle* pSource = TYUserSourcePonctuelle::safeDownCast(pEmetteur);
            QString nom;
            for (short i = 0 ; i < pSource->getNbRegimes(); i++)
            {
                nom = pSource->getRegimeNb(i).getName();
                listRegimes.append(nom);
            }

            QComboBox* pComboRegime = new QComboBox();
            pComboRegime->addItems(listRegimes);
            pComboRegime->setCurrentIndex(mapRegime[pEmetteur]);
            _tableSource->setCellWidget(row, 2, pComboRegime);

            row++;
        }
    }

    // Update des Sources Lineiques
    row = 0;
    mapRegime.begin();
    for (iterEmit = mapEtat.begin(); iterEmit != mapEtat.end(); iterEmit++)
    {
        TYElement* pEmetteur = (*iterEmit).first;
        TYAcousticLine* pSource = dynamic_cast<TYAcousticLine*>(pEmetteur);
        if (pSource != nullptr)
        {
            _tabPtrSourceLin.push_back(pEmetteur);

            _tableSourceLin->setRowCount(_tableSourceLin->rowCount() + 1);
            QTableWidgetItem* pItemEmit = new QTableWidgetItem((*iterEmit).first->getName());
            _tableSourceLin->setItem(row, 0, pItemEmit);
            QTableWidgetItem* pCheckItemEmit = new QTableWidgetItem("");
            pCheckItemEmit->setCheckState((*iterEmit).second ? Qt::Checked : Qt::Unchecked);
            _tableSourceLin->setItem(row, 1, pCheckItemEmit);

            // Ajout des comboBox
            QStringList listRegimes;  // Liste des regimes pour "peupler" le combo
            QString nom;
            for (short i = 0 ; i < pSource->getNbRegimes(); i++)
            {
                nom = pSource->getRegimeNb(i).getName();
                listRegimes.append(nom);
            }

            QComboBox* pComboRegime = new QComboBox();
            pComboRegime->addItems(listRegimes);
            pComboRegime->setCurrentIndex(mapRegime[pEmetteur]);
            _tableSourceLin->setCellWidget(row, 2, pComboRegime);

            row++;
        }
    }
}

void TYEtatsWidget::apply()
{
    TYMapPtrElementBool& mapEtat = _pElement->getEmitAcVolNode();
    TYMapPtrElementInt& mapRegime = _pElement->getMapElementRegime();
    TYMapPtrElementBool::iterator iterEmit;
    TYMapPtrElementInt::iterator iterRegime;
    int row;

    // Mis a jour des bâtiments
    for (row = 0; row < _tableBatiment->rowCount(); row++)
    {
        iterRegime = mapRegime.begin();
        for (iterEmit = mapEtat.begin(); iterEmit != mapEtat.end(); iterEmit++)
        {
            if (_tabPtrBatiment[row] == (*iterEmit).first)
            {
                QTableWidgetItem* pCheck = (QTableWidgetItem*)_tableBatiment->item(row, 1);
                (*iterEmit).second = (pCheck->checkState() == Qt::Checked);

                // Recup choix regime
                QComboBox* pComboBox = (QComboBox*)_tableBatiment->cellWidget(row, 2);
                (*iterRegime).second = pComboBox->currentIndex();
            }

            iterRegime++;
        }
    }

    // Mis a jour des machines
    for (row = 0; row < _tableMachine->rowCount(); row++)
    {
        iterRegime = mapRegime.begin();
        for (iterEmit = mapEtat.begin(); iterEmit != mapEtat.end(); iterEmit++)
        {
            if (_tabPtrMachine[row] == (*iterEmit).first)
            {
                QTableWidgetItem* pCheck = (QTableWidgetItem*)_tableMachine->item(row, 1);
                (*iterEmit).second = (pCheck->checkState() == Qt::Checked);

                // Recup choix regime
                QComboBox* pComboBox = (QComboBox*)_tableMachine->cellWidget(row, 2);
                (*iterRegime).second = pComboBox->currentIndex();
            }

            iterRegime++;
        }
    }

    // Mis a jour des sources ponctuelles
    for (row = 0; row < _tableSource->rowCount(); row++)
    {
        iterRegime = mapRegime.begin();
        for (iterEmit = mapEtat.begin(); iterEmit != mapEtat.end(); iterEmit++)
        {
            if (_tabPtrSourcePonctuelle[row] == (*iterEmit).first)
            {
                QTableWidgetItem* pCheck = (QTableWidgetItem*)_tableSource->item(row, 1);
                (*iterEmit).second = (pCheck->checkState() == Qt::Checked);

                // Recup choix regime
                QComboBox* pComboBox = (QComboBox*)_tableSource->cellWidget(row, 2);
                (*iterRegime).second = pComboBox->currentIndex();
            }

            iterRegime++;
        }
    }

    // Mis a jour des sources lineique
    for (row = 0; row < _tableSourceLin->rowCount(); row++)
    {
        iterRegime = mapRegime.begin();
        for (iterEmit = mapEtat.begin(); iterEmit != mapEtat.end(); iterEmit++)
        {
            if (_tabPtrSourceLin[row] == (*iterEmit).first)
            {
                QTableWidgetItem* pCheck = (QTableWidgetItem*)_tableSourceLin->item(row, 1);
                (*iterEmit).second = (pCheck->checkState() == Qt::Checked);

                // Recup choix regime
                QComboBox* pComboBox = (QComboBox*)_tableSourceLin->cellWidget(row, 2);
                (*iterRegime).second = pComboBox->currentIndex();
            }

            iterRegime++;
        }
    }

    TYElement::setIsSavedOk(true);
    emit modified();
    accept();
}

void TYEtatsWidget::contextMenuEvent(QContextMenuEvent* e)
{
    QTableWidget* _pCurrentTable = (QTableWidget*)_tabWidget->currentWidget();
    QPoint point = _pCurrentTable->mapFrom(this, e->pos());

    if ((point.x() >= 0) && (point.y() >= 0) && (point.x() <= _pCurrentTable->width()) && (point.y() <= _pCurrentTable->height()))
    {
        QPoint resPoint = QPoint(point.x(), point.y() - _pCurrentTable->horizontalHeader()->height());
        QTableWidgetItem* item = _pCurrentTable->itemAt(resPoint);
        if (item)
        {
            QMenu* pMenu = new QMenu(this);

            QAction* selectAll = pMenu->addAction(TR("id_popup_select_all"));
            QAction* unselectAll = pMenu->addAction(TR("id_popup_unselect_all"));
            QAction* ret = pMenu->exec(_pCurrentTable->mapToGlobal(point));

            if (ret)
            {
                Qt::CheckState checkedState;
                if (ret == selectAll) { checkedState = Qt::Checked; }
                if (ret == unselectAll) { checkedState = Qt::Unchecked; }

                // Update de la table active
                for (int row = 0; row < _pCurrentTable->rowCount(); row++)
                {
                    _pCurrentTable->item(row, 1)->setCheckState(checkedState);
                }

            }
        }
    }
}
