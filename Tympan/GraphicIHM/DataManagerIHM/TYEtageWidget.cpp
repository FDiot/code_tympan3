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
 * \file TYEtageWidget.cpp
 * \brief outil IHM pour un etage
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYEtage.h"
#include <qmenu.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qtabwidget.h>
#include <qtablewidget.h>
//Added by qt3to4:
#include <QHBoxLayout>
#include <QBoxLayout>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QTreeWidgetItem>
#include <QHeaderView>

using namespace Qt;

#define TR(id) OLocalizator::getString("TYEtageWidget", (id))


TYEtageWidget::TYEtageWidget(TYEtage* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(_pParent)
{
    Q_ASSERT(pElement);
    _pElement = pElement;

    _elmW = new TYAcousticVolumeWidget(pElement, this);

    resize(300, 810);
    setWindowTitle(TR("id_caption"));
    _etageLayout = new QGridLayout(this);
    setLayout(_etageLayout);
    _etageLayout->addWidget(_elmW, 0, 0);


    // Boutons d'appel de : boite de dialogue des murs et atmosphere
    _tabWidget = new QTabWidget(this);

    // On definit d'abord l'onglet des murs, sol et plafond

    // CREATION D'UN WIDGET POUR LA PRESENTATION DES MURS

    // Presentation des murs
    _groupBoxListMur = new QGroupBox(_tabWidget);
    _groupBoxListMur->setTitle(TR("id_tabmurrepere_box"));
    _groupBoxListMurLayout = new QGridLayout(_groupBoxListMur);
    _groupBoxListMur->setLayout(_groupBoxListMurLayout);

    _tableFaces = new QTableWidget(_groupBoxListMur);
    _tableFaces->setColumnCount(2);
    _tableFaces->setHorizontalHeaderItem(0, new QTableWidgetItem(TR("id_nom")));
    _tableFaces->setHorizontalHeaderItem(1, new QTableWidgetItem(TR("id_emission")));

    _groupBoxListMurLayout->addWidget(_tableFaces, 0, 0);

    //      LE SOL
    _groupBoxSol = new QGroupBox(_groupBoxListMur);
    _groupBoxSol->setTitle(TR("id_sol"));
    _groupBoxSolLayout = new QGridLayout(_groupBoxSol);
    _groupBoxSol->setLayout(_groupBoxSolLayout);

    _checkBoxSol = new QCheckBox(_groupBoxSol);
    _groupBoxSolLayout->addWidget(_checkBoxSol, 0, 0);

    _lineEditNomSol = new QLineEdit(_groupBoxSol);
    _lineEditNomSol->setEnabled(FALSE);
    _groupBoxSolLayout->addWidget(_lineEditNomSol, 0, 1);

    _pushButtonSol = new QPushButton(_groupBoxSol);
    _pushButtonSol->setText(TR("id_proprietes_button"));
    _groupBoxSolLayout->addWidget(_pushButtonSol, 0, 2);

    _groupBoxListMurLayout->addWidget(_groupBoxSol, 1, 0);

    //      LE PLAFOND
    _groupBoxPlafond = new QGroupBox(_groupBoxListMur);
    _groupBoxPlafond->setTitle(TR("id_plafond"));
    _groupBoxPlafondLayout = new QGridLayout(_groupBoxPlafond);
    _groupBoxPlafond->setLayout(_groupBoxPlafondLayout);

    _checkBoxPlafond = new QCheckBox(_groupBoxPlafond);
    _groupBoxPlafondLayout->addWidget(_checkBoxPlafond, 0, 0);

    _lineEditNomPlafond = new QLineEdit(_groupBoxPlafond);
    _lineEditNomPlafond->setEnabled(FALSE);
    _groupBoxPlafondLayout->addWidget(_lineEditNomPlafond, 0, 1);

    _pushButtonPlafond = new QPushButton(_groupBoxPlafond);
    _pushButtonPlafond->setText(TR("id_proprietes_button"));
    _groupBoxPlafondLayout->addWidget(_pushButtonPlafond, 0, 2);

    _groupBoxListMurLayout->addWidget(_groupBoxPlafond, 2, 0);

    _tabWidget->insertTab(0, _groupBoxListMur, TR("id_opt_murs"));

    // FIN DE CREATION DE LA BOITE DE DIALOGUE DES VOLUMES ACOUSTIQUES

    // CREATION DE L'ONGLET DES SOURCES/MACHINES
    // Gestion des etats des machines et des sources
    _groupBoxListMachine = new QGroupBox(_tabWidget);
    _groupBoxListMachine->setTitle(TR("id_tabmachinerepere_box"));
    _groupBoxListMachineLayout = new QGridLayout();
    _groupBoxListMachine->setLayout(_groupBoxListMachineLayout);

    _tabWidgetSources = new QTabWidget(_groupBoxListMachine);



    // Table des machines
    _tableMachine = new QTableWidget(_tabWidgetSources);
    _tableMachine->setColumnCount(3);
    _tableMachine->setHorizontalHeaderItem(0, new QTableWidgetItem(TR("id_nom")));
    _tableMachine->setHorizontalHeaderItem(1, new QTableWidgetItem(TR("id_emission")));
    _tableMachine->setHorizontalHeaderItem(2, new QTableWidgetItem(TR("id_regime")));
    _tabWidgetSources->insertTab(0, _tableMachine, TR("id_etat_machine"));

    // Table des sources
    _tableSource = new QTableWidget(_tabWidgetSources);
    _tableSource->setColumnCount(3);
    _tableSource->setHorizontalHeaderItem(0, new QTableWidgetItem(TR("id_nom")));
    _tableSource->setHorizontalHeaderItem(1, new QTableWidgetItem(TR("id_emission")));
    _tableSource->setHorizontalHeaderItem(2, new QTableWidgetItem(TR("id_regime")));
    _tabWidgetSources->insertTab(1, _tableSource, TR("id_etat_source"));

    _groupBoxListMachineLayout->addWidget(_tabWidgetSources, 0, 0);

    _tabWidget->insertTab(1, _groupBoxListMachine, TR("id_opt_sources"));

    // CREATION D'UN ONGLET POUR LA GEOMETRIE
    _groupBoxDimensions = new QGroupBox(_tabWidget);
    _groupBoxDimensionsLayout = new QGridLayout();
    _groupBoxDimensions->setLayout(_groupBoxDimensionsLayout);

    _dimensionsHauteurLabel = new QLabel("labelName");
    _dimensionsHauteurLabel->setText(TR("id_hauteur_label"));
    _dimensionsHauteurLineEdit = new QLineEdit("");

    _groupBoxDimensionsLayout->addWidget(_dimensionsHauteurLabel, 0, 0);
    _groupBoxDimensionsLayout->addWidget(_dimensionsHauteurLineEdit, 0, 1);

    // Ajout dans la boite de dialogue globale
    _tabWidget->insertTab(2, _groupBoxDimensions, TR("id_title_dimensions"));

    // CREATION D'UN ONGLET DES PROPRIETES ACOUSTIQUE
    // Choix du mode de calcul du champ reverbere
    QGroupBox* pGroupBox2 = new QGroupBox(_tabWidget);
    QGridLayout* pGroupBox2Layout = new QGridLayout(pGroupBox2);
    pGroupBox2->setLayout(pGroupBox2Layout);

    // A droite acces au TR et au champ reverbere
    QPushButton* pPushButtonTR = new QPushButton(pGroupBox2);
    pPushButtonTR->setText(TR("id_tr_button"));
    pGroupBox2Layout->addWidget(pPushButtonTR, 0, 0);

    QPushButton* pPushButtonReverb = new QPushButton(pGroupBox2);
    pPushButtonReverb->setText(TR("id_reverb_button"));
    pGroupBox2Layout->addWidget(pPushButtonReverb, 0, 1);

    // Ajout dans la boite de dialogue globale
    _tabWidget->insertTab(3, pGroupBox2, TR("id_prop_acous"));

    _etageLayout->addWidget(_tabWidget);

    updateContent();

    connect(_pushButtonSol, SIGNAL(clicked()), this, SLOT(editSol()));
    connect(_pushButtonPlafond, SIGNAL(clicked()), this, SLOT(editPlafond()));
    connect(pPushButtonReverb, SIGNAL(clicked()), this, SLOT(showReverb()));
    connect(pPushButtonTR, SIGNAL(clicked()), this, SLOT(showTR()));

    connect(_elmW, SIGNAL(regimeChanged(int)), this, SLOT(propagateRegime(int)));
}

TYEtageWidget::~TYEtageWidget()
{
}

void TYEtageWidget::updateContent()
{
    _elmW->updateContent();

    // Mise a jour de l'etat des murs
    updateFromMurs();

    // Gestion des regimes des machines et des sources
    int currentRegime = getElement()->getCurRegime();
    propagateRegime(currentRegime);

    // Affichage de la hauteur de l'etage
    _dimensionsHauteurLineEdit->setText(QString().setNum(getElement()->getHauteur(), 'f', 2));
}

void TYEtageWidget::apply()
{
    _elmW->apply();

    // Mise a jour des murs
    applyToMurs();

    // Mis a jour des machines
    int row;

    int currentRegime = getElement()->getCurRegime();
    TYMapPtrElementInt& currentMap = getElement()->getMapRegimes(currentRegime);
    TYMapPtrElementInt::iterator iterRegime ;

    TYMapPtrElementBool& currentEtat = getElement()->getMapEtats(currentRegime);
    iterRegime = currentMap.begin();

    for (row = 0; row < _tableMachine->rowCount(); row++)
    {
        if (iterRegime == currentMap.end())
        {
            break; // Faut pas depasser les bornes des limites !
        }

        // On recherche la premiere machine suivante a partir de la position courante
        while ((iterRegime != currentMap.end()) && (!((*iterRegime).first->isA("TYMachine")))) { iterRegime++; }

        // On lui applique le bon regime
        QTableWidgetItem* pCheck = (QTableWidgetItem*)_tableMachine->item(row, 1);
        QComboBox* pComboItem = (QComboBox*)_tableMachine->cellWidget(row, 2);

        // On convertit l'element en machine
        TYMachine* pMachine = TYMachine::safeDownCast((*iterRegime).first);

        if (pMachine)
        {
            pMachine->setCurRegime(pComboItem->currentIndex());
            if (pCheck->checkState() == Qt::Checked)
            {
                pMachine->setIsRayonnant(true);
            }
            else
            {
                pMachine->setIsRayonnant(false);
            }
        }

        // Enregistrement du regime dans le map
        (*iterRegime).second = pComboItem->currentIndex();

        // Enregistrement de l'etat dans le map
        if (pCheck->checkState() == Qt::Checked)
        {
            currentEtat[pMachine] = true;
        }
        else
        {
            currentEtat[pMachine] = false;
        }

        // On avance a l'element suivant
        iterRegime++;
    }

    // Mis a jour des sources ponctuelles
    iterRegime = currentMap.begin();
    for (row = 0; row < _tableSource->rowCount(); row++)
    {
        if (iterRegime == currentMap.end())
        {
            break; // Faut pas depasser les bornes des limites !
        }

        // On recherche la premiere source suivante a partir de la position courante
        while ((iterRegime != currentMap.end()) && (!((*iterRegime).first->isA("TYUserSourcePonctuelle")))) { iterRegime++; }

        // On lui applique le bon regime
        QTableWidgetItem* pCheck = (QTableWidgetItem*)_tableSource->item(row, 1);
        QComboBox* pComboItem = (QComboBox*)_tableSource->cellWidget(row, 2);

        // On convertit l'element en source ponctuelle
        TYUserSourcePonctuelle* pSource = TYUserSourcePonctuelle::safeDownCast((*iterRegime).first);

        if (pSource)
        {
            pSource->setCurrentRegime(pComboItem->currentIndex());
            if (pCheck->checkState() == Qt::Checked)
            {
                pSource->setIsRayonnant(true);
            }
            else
            {
                pSource->setIsRayonnant(false);
            }
        }

        // Enregistrement du regime dans le map
        (*iterRegime).second = pComboItem->currentIndex();

        // Enregistrement de l'etat dans le map
        if (pCheck->checkState() == Qt::Checked)
        {
            currentEtat[pSource] = true;
        }
        else
        {
            currentEtat[pSource] = false;
        }

        // On avance a l'element suivant
        iterRegime++;
    }

    double hauteur = _dimensionsHauteurLineEdit->text().toDouble();
    getElement()->setHauteur(hauteur);

    emit modified();
}

void TYEtageWidget::editSol()
{
    getElement()->getSol()->setIsRayonnant(_checkBoxSol->isChecked());
    int ret = getElement()->getSol()->edit(this);

    if (ret == QDialog::Accepted)
    {
        _lineEditNomSol->setText(getElement()->getSol()->getName());
    }
    _checkBoxSol->setChecked(getElement()->getSol()->getIsRayonnant());
}

void TYEtageWidget::editPlafond()
{
    getElement()->getPlafond()->setIsRayonnant(_checkBoxPlafond->isChecked());
    int ret = getElement()->getPlafond()->edit(this);

    if (ret == QDialog::Accepted)
    {
        _lineEditNomPlafond->setText(getElement()->getPlafond()->getName());
    }
    _checkBoxPlafond->setChecked(getElement()->getPlafond()->getIsRayonnant());
}

void TYEtageWidget::editMur(const int& item)
{
    LPTYMur pMur = TYMur::safeDownCast(getElement()->getTabMur()[item]->getElement());
    QTableWidgetItem* pCheck = (QTableWidgetItem*) _tableFaces->item(item, 1);
    if (pCheck->checkState() == Qt::Checked)
    {
        pMur->setIsRayonnant(true);
    }
    else
    {
        pMur->setIsRayonnant(false);
    }

    QTableWidgetItem* pText  = (QTableWidgetItem*)_tableFaces->item(item, 0);
    pMur->setName(pText->text());

    int ret = getElement()->getTabMur()[item]->getElement()->edit(this);

    if (ret == QDialog::Accepted)
    {
        if (pMur->getIsRayonnant())
        {
            pCheck->setCheckState(Qt::Checked);
        }
        else
        {
            pCheck->setCheckState(Qt::Unchecked);
        }
        _tableFaces->item(item, 0)->setText(pMur->getName());
    }
}

void TYEtageWidget::editMachine(const int& item)
{
    //  getElement()->getTabMachine()[item->text(0).toInt()]->getElement()->edit(this);
    TYMachine* pMachine = TYMachine ::safeDownCast(getElement()->getTabMachine()[item]->getElement());
    QTableWidgetItem* pCheck = (QTableWidgetItem*) _tableMachine->item(item, 1);

    // On commence par mettre a jour la machine par rapport a l'etat du tableau
    if (pCheck->checkState() == Qt::Checked)
    {
        pMachine->setIsRayonnant(true);
    }
    else
    {
        pMachine->setIsRayonnant(false);
    }

    int ret = getElement()->getTabMachine()[item]->getElement()->edit(this);

    if (ret == QDialog::Accepted)
    {
        propagateRegime(getElement()->getCurRegime());
    }
}

void TYEtageWidget::editSource(const int& item)
{
    //  getElement()->getTabSources()[item->text(0).toInt()]->getElement()->edit(this);
    TYUserSourcePonctuelle* pSource = TYUserSourcePonctuelle::safeDownCast(getElement()->getTabSources()[item]->getElement());
    QTableWidgetItem* pCheck = (QTableWidgetItem*) _tableSource->item(item, 1);

    // On commence par mettre a jour la machine par rapport a l'etat du tableau
    if (pCheck->checkState() == Qt::Checked)
    {
        pSource->setIsRayonnant(true);
    }
    else
    {
        pSource->setIsRayonnant(false);
    }

    int ret = getElement()->getTabSources()[item]->getElement()->edit(this);

    if (ret == QDialog::Accepted)
    {
        propagateRegime(getElement()->getCurRegime());
    }
}

void TYEtageWidget::contextMenuEvent(QContextMenuEvent* e)
{
    if (_tabWidget->currentIndex() == 0)
    {
        QPoint point = _tableFaces->mapFrom(this, e->pos());

        if ((point.x() >= 0) && (point.y() >= 0) && (point.x() <= _tableFaces->width()) && (point.y() <= _tableFaces->height()))
        {
            QPoint resPoint = QPoint(point.x(), point.y() - _tableFaces->horizontalHeader()->height());
            QTableWidgetItem* item = _tableFaces->itemAt(resPoint);
            if (item)
            {
                QMenu* pMenu = new QMenu(this);
                QAction* prop = pMenu->addAction(TR("id_property"));

                QAction* ret = pMenu->exec(_tableFaces->mapToGlobal(point));
                if ((ret) && (ret == prop)) { editMur(_tableFaces->indexAt(resPoint).row()); }
            }
        }
    }
    else if (_tabWidget->currentIndex() == 1)
    {
        if (_tabWidgetSources->currentIndex() == 0) // C'est les machines
        {
            QPoint point = _tableMachine->mapFrom(this, e->pos());

            if ((point.x() >= 0) && (point.y() >= 0) && (point.x() <= _tableMachine->width()) && (point.y() <= _tableMachine->height()))
            {
                QPoint resPoint = QPoint(point.x(), point.y() - _tableMachine->horizontalHeader()->height());
                QTableWidgetItem* item = _tableMachine->itemAt(resPoint);
                if (item)
                {
                    QMenu* pMenu = new QMenu(this);
                    QAction* prop = pMenu->addAction(TR("id_property"));

                    QAction* ret = pMenu->exec(_tableMachine->mapToGlobal(point));
                    if ((ret) && (ret == prop)) { editMachine(_tableMachine->indexAt(resPoint).row()); }
                }
            }
        }
        else // C'est les sources
        {
            QPoint point = _tableSource->mapFrom(this, e->pos());

            if ((point.x() >= 0) && (point.y() >= 0) && (point.x() <= _tableSource->width()) && (point.y() <= _tableSource->height()))
            {
                QPoint resPoint = QPoint(point.x(), point.y() - _tableSource->horizontalHeader()->height());
                QTableWidgetItem* item = _tableSource->itemAt(resPoint);
                if (item)
                {
                    QMenu* pMenu = new QMenu(this);
                    QAction* prop = pMenu->addAction(TR("id_property"));

                    QAction* ret = pMenu->exec(_tableSource->mapToGlobal(point));
                    if ((ret) && (ret == prop)) { editSource(_tableSource->indexAt(resPoint).row()); }
                }
            }
        }
    }
    else
    {
        return;
    }
}



void TYEtageWidget::propagateRegime(int regime)
{
    clearTables();

    // On met a jour l'etat du widget des murs
    updateFromMurs();

    // Gestion des regimes des machines et des sources

    TYMapPtrElementInt& currentMap = getElement()->getMapRegimes(regime);
    TYMapPtrElementInt::iterator iterRegime;

    TYMapPtrElementBool& currentEtat = getElement()->getMapEtats(regime);


    // Update des Machines
    int row = 0;
    for (iterRegime = currentMap.begin(); iterRegime != currentMap.end(); iterRegime++)
    {
        TYElement* pEmetteur = (*iterRegime).first;
        if (strcmp(pEmetteur->getClassName(), "TYMachine") == 0)
        {
            _tableMachine->setRowCount(_tableMachine->rowCount() + 1);
            _tableMachine->setItem(row, 0, new QTableWidgetItem((*iterRegime).first->getName()));

            QTableWidgetItem* pCheckItemEmit = new QTableWidgetItem("");
            if (currentEtat[pEmetteur])
            {
                pCheckItemEmit->setCheckState(Qt::Checked);
            }
            else
            {
                pCheckItemEmit->setCheckState(Qt::Unchecked);
            }

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

            QComboBox* pComboItemRegime = new QComboBox(_tableMachine);
            pComboItemRegime->addItems(listRegimes);
            _tableMachine->setCellWidget(row, 2, pComboItemRegime);
            pComboItemRegime->setCurrentIndex((*iterRegime).second); // Selection du regime

            row++;
        }
    }

    // Update des Sources ponctuelles
    row = 0;
    for (iterRegime = currentMap.begin(); iterRegime != currentMap.end(); iterRegime++)
    {
        TYElement* pEmetteur = (*iterRegime).first;
        if (strcmp(pEmetteur->getClassName(), "TYUserSourcePonctuelle") == 0)
        {
            _tableSource->setRowCount(_tableSource->rowCount() + 1);
            _tableSource->setItem(row, 0, new QTableWidgetItem((*iterRegime).first->getName()));

            QTableWidgetItem* pCheckItemSrc = new QTableWidgetItem("");
            if (currentEtat[pEmetteur])
            {
                pCheckItemSrc->setCheckState(Qt::Checked);
            }
            else
            {
                pCheckItemSrc->setCheckState(Qt::Unchecked);
            }

            _tableSource->setItem(row, 1, pCheckItemSrc);

            // Ajout des comboBox
            QStringList listRegimes;  // Liste des regimes pour "peupler" le combo
            TYUserSourcePonctuelle* pSource = TYUserSourcePonctuelle::safeDownCast(pEmetteur);
            QString nom;
            for (short i = 0 ; i < pSource->getNbRegimes(); i++)
            {
                nom = pSource->getRegimeNb(i).getName();
                listRegimes.append(nom);
            }

            QComboBox* pComboItemRegime = new QComboBox(_tableSource);
            pComboItemRegime->addItems(listRegimes);
            _tableSource->setCellWidget(row, 2, pComboItemRegime);
            pComboItemRegime->setCurrentIndex((*iterRegime).second); // Selection du regime

            row++;
        }
    }

}

void TYEtageWidget::clearTables()
{
    for (int i = 0; i < _tableMachine->rowCount(); i++)
    {
        _tableMachine->takeItem(i, 0);
        _tableMachine->takeItem(i, 1);
        _tableMachine->takeItem(i, 2);
    }
    _tableMachine->setRowCount(0);
    for (int i = 0; i < _tableSource->rowCount(); i++)
    {
        _tableSource->takeItem(i, 0);
        _tableSource->takeItem(i, 1);
        _tableSource->takeItem(i, 2);
    }
    _tableSource->setRowCount(0);
}

void TYEtageWidget::updateFromMurs()
{
    _lineEditNomSol->setText(getElement()->getSol()->getName());
    _lineEditNomPlafond->setText(getElement()->getPlafond()->getName());
    _checkBoxSol->setChecked(getElement()->getSol()->getIsRayonnant());
    _checkBoxPlafond->setChecked(getElement()->getPlafond()->getIsRayonnant());

    for (unsigned int i = 0, row = 0; i < getElement()->getTabMur().size() ; i++, row++)
    {
        LPTYMur pMur = TYMur::safeDownCast(getElement()->getTabMur()[i]->getElement());
        _tableFaces->setRowCount(i + 1);
        _tableFaces->setItem(row, 0, new QTableWidgetItem(pMur->getName()));
        QTableWidgetItem* pCheckItemEmit = new QTableWidgetItem("");
        if (pMur->getIsRayonnant())
        {
            pCheckItemEmit->setCheckState(Qt::Checked);
        }
        else
        {
            pCheckItemEmit->setCheckState(Qt::Unchecked);
        }
        _tableFaces->setItem(row, 1, pCheckItemEmit);
    }
}

void TYEtageWidget::applyToMurs()
{
    getElement()->getSol()->setIsRayonnant(_checkBoxSol->isChecked());
    getElement()->getPlafond()->setIsRayonnant(_checkBoxPlafond->isChecked());

    for (int i = 0; i < _tableFaces->rowCount(); i++)
    {
        LPTYMur pMur = TYMur::safeDownCast(getElement()->getTabMur()[i]->getElement());
        QTableWidgetItem* pCheck = _tableFaces->item(i, 1);
        if (pCheck->checkState() == Qt::Checked)
        {
            pMur->setIsRayonnant(true);
        }
        else
        {
            pMur->setIsRayonnant(false);
        }
        QTableWidgetItem* pText  = (QTableWidgetItem*)_tableFaces->item(i, 0);
        pMur->setName(pText->text());
    }
}

void TYEtageWidget::showReverb()
{
	static_cast<TYSpectre>(getElement()->getReverb().toDB()).edit(this);
}

void TYEtageWidget::showTR()
{
    TYSpectre::setXMLPrecision(5);
    getElement()->getTR().edit(this);
    TYSpectre::setXMLPrecision();
}

