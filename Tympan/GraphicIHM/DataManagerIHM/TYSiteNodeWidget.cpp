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
 * \file TYSiteNodeWidget.cpp
 * \brief outil IHM pour un ensemble de sites
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerMetier/Site/TYSiteNode.h"

#include <qmenu.h>
#include <qcombobox.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <qfiledialog.h>


#define TR(id) OLocalizator::getString("TYSiteNodeWidget", (id))


TYSiteNodeWidget::TYSiteNodeWidget(TYSiteNode* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{
    unsigned short wPos = 0; // Position du widget


    resize(300, 600);
    setWindowTitle(TR("id_caption"));

    _sitenodeLayout = new QGridLayout();
    setLayout(_sitenodeLayout);

    _elmW = new TYElementWidget(pElement, this);

    _sitenodeLayout->addWidget(_elmW, wPos, 0);


    // Code de TYSiteWidget
    _groupBox_0 = new QGroupBox(this);
    _groupBox_0->setTitle(TR(""));
    _groupBox_0Layout = new QGridLayout();
    _groupBox_0->setLayout(_groupBox_0Layout);

    _labelEchelle = new QLabel(_groupBox_0);
    _labelEchelle->setText(TR("id_echelle_label"));
    _groupBox_0Layout->addWidget(_labelEchelle, 0, 0);

    _lineEditEchelle = new QLineEdit(_groupBox_0);
    _groupBox_0Layout->addWidget(_lineEditEchelle, 0, 1);
    QLabel* pUnitEchelle = new QLabel(_groupBox_0);
    pUnitEchelle->setText(TR("id_unite_echelle"));
    _groupBox_0Layout->addWidget(pUnitEchelle, 0, 2);

    _checkBoxUseTopoFile = new QCheckBox(_groupBox_0);
    _checkBoxUseTopoFile->setText(TR("id_use_topo_file"));
    _groupBox_0Layout->addWidget(_checkBoxUseTopoFile, 1, 0);

    _lineEditTopoFile = new QLineEdit(_groupBox_0);
    _lineEditTopoFile->setEnabled(true);
    _groupBox_0Layout->addWidget(_lineEditTopoFile, 1, 1);

    _pushButtonTopoFile = new QPushButton(TR("id_select_topo_file"), _groupBox_0);
    _groupBox_0Layout->addWidget(_pushButtonTopoFile, 1, 2);

    _sitenodeLayout->addWidget(_groupBox_0, ++wPos, 0);

    _groupBoxInfra = new QGroupBox(this);
    _groupBoxInfra->setTitle(TR("id_infra"));
    _groupBoxInfraLayout = new QGridLayout();
    _groupBoxInfra->setLayout(_groupBoxInfraLayout);

    _pushButtonInfra = new QPushButton(_groupBoxInfra);
    _pushButtonInfra->setText(TR("id_proprietes_button"));
    _groupBoxInfraLayout->addWidget(_pushButtonInfra, 0, 1);

    _lineEditNomInfra = new QLineEdit(_groupBoxInfra);
    _lineEditNomInfra->setText(getElement()->getInfrastructure()->getName());
    _lineEditNomInfra->setEnabled(false);
    _groupBoxInfraLayout->addWidget(_lineEditNomInfra, 0, 0);

    _sitenodeLayout->addWidget(_groupBoxInfra, ++wPos, 0);

    _groupBoxTopo = new QGroupBox(this);
    _groupBoxTopo->setTitle(TR("id_topo"));
    _groupBoxTopoLayout = new QGridLayout();
    _groupBoxTopo->setLayout(_groupBoxTopoLayout);

    _lineEditNomTopo = new QLineEdit(_groupBoxTopo);
    _lineEditNomTopo->setText(getElement()->getTopographie()->getName());
    _lineEditNomTopo->setEnabled(false);
    _groupBoxTopoLayout->addWidget(_lineEditNomTopo, 0, 0);

    _pushButtonTopo = new QPushButton(_groupBoxTopo);
    _pushButtonTopo->setText(TR("id_proprietes_button"));
    _groupBoxTopoLayout->addWidget(_pushButtonTopo, 0, 1);

    _sitenodeLayout->addWidget(_groupBoxTopo, ++wPos, 0);


    // Code de TYSiteNodeWidget

    // =============================================================================================

    // Choix du systeme de SIG
    QGroupBox* pGroupBoxData = new QGroupBox(this);
    pGroupBoxData->setTitle(TR("id_SIG"));
    QGridLayout* pGroupBoxDataLayout = new QGridLayout();
    pGroupBoxData->setLayout(pGroupBoxDataLayout);

    pGroupBoxDataLayout->setColumnStretch(0, 1);
    pGroupBoxDataLayout->setColumnStretch(1, 0);

    // Choix du systeme de coordonnees
    _labelModeCoord = new QLabel(pGroupBoxData);
    _labelModeCoord->setText(TR("id_system_coord"));
    pGroupBoxDataLayout->addWidget(_labelModeCoord, 0, 0);
    _comboSIG = new QComboBox(pGroupBoxData);
    pGroupBoxDataLayout->addWidget(_comboSIG, 0, 1);
    updateSIGList();

    // Saisie des coordonnees
    _labelSIG_X = new QLabel(pGroupBoxData);
    _labelSIG_X->setText(TR("id_SIG_X"));
    pGroupBoxDataLayout->addWidget(_labelSIG_X, 0, 2);
    _lineEditSIG_X = new QLineEdit(pGroupBoxData);
    pGroupBoxDataLayout->addWidget(_lineEditSIG_X, 0, 3);

    _labelSIG_Y = new QLabel(pGroupBoxData);
    _labelSIG_Y->setText(TR("id_SIG_Y"));
    pGroupBoxDataLayout->addWidget(_labelSIG_Y, 0, 4);
    _lineEditSIG_Y = new QLineEdit(pGroupBoxData);
    pGroupBoxDataLayout->addWidget(_lineEditSIG_Y, 0, 5);

    connect(_comboSIG, SIGNAL(activated(int)), this, SLOT(setSIGCoord(int)));

    _sitenodeLayout->addWidget(pGroupBoxData, ++wPos, 0);

    // ==============================================================================================

    _groupBox_3 = new QGroupBox(this);
    _groupBox_3->setTitle(TR(""));
    _groupBox_3Layout = new QGridLayout();
    _groupBox_3->setLayout(_groupBox_3Layout);

    _labelRoot = new QLabel(_groupBox_3);
    _labelRoot->setText(TR("id_root_label"));
    _groupBox_3Layout->addWidget(_labelRoot, 0, 0);

    _checkBoxRoot = new QCheckBox(_groupBox_3);
    _checkBoxRoot->setText(TR(""));
    _checkBoxRoot->setEnabled(false);
    _groupBox_3Layout->addWidget(_checkBoxRoot, 0, 1);

    _labelUseEmprise = new QLabel(_groupBox_3);
    _labelUseEmprise->setText(TR("id_emprise_label"));
    _groupBox_3Layout->addWidget(_labelUseEmprise, 1, 0);

    _checkBoxUseEmprise = new QCheckBox(_groupBox_3);
    _checkBoxUseEmprise->setText(TR(""));
    _checkBoxUseEmprise->setEnabled(true);
    _groupBox_3Layout->addWidget(_checkBoxUseEmprise, 1, 1);

    _labelAltiEmprise = new QLabel(_groupBox_3) ;
    _labelAltiEmprise->setText(TR("id_alti_emprise"));
    _groupBox_3Layout->addWidget(_labelAltiEmprise, 1, 2);

    _lineEditAltiEmprise = new QLineEdit(_groupBox_3) ;
    _lineEditAltiEmprise->setText(TR(""));
    _lineEditAltiEmprise->setEnabled(false);
    _groupBox_3Layout->addWidget(_lineEditAltiEmprise, 1, 3);

    _sitenodeLayout->addWidget(_groupBox_3, ++wPos, 0);

    _groupBox = new QGroupBox(this);
    _groupBox->setTitle(TR("id_tabsiterepere_box"));
    _groupBoxLayout = new QGridLayout();
    _groupBox->setLayout(_groupBoxLayout);

    _listViewListSite = new QTreeWidget(_groupBox);
    _listViewListSite->setColumnCount(2);
    QStringList stringList;
    stringList.append(TR(""));
    stringList.append(TR("id_site"));
    _listViewListSite->setHeaderLabels(stringList);
    _listViewListSite->setRootIsDecorated(TRUE);

    _groupBoxLayout->addWidget(_listViewListSite, 0, 0);

    _sitenodeLayout->addWidget(_groupBox, ++wPos, 0);


    connect(_pushButtonInfra, SIGNAL(clicked()), this, SLOT(editInfra()));
    connect(_pushButtonTopoFile, SIGNAL(clicked()), this, SLOT(chooseTopoFile()));
    connect(_pushButtonTopo, SIGNAL(clicked()), this, SLOT(editTopo()));
    connect(_checkBoxUseTopoFile, SIGNAL(clicked()), this, SLOT(changeStateTopoFile()));    connect(_checkBoxUseEmprise, SIGNAL(clicked()), this, SLOT(updateEditAlti()));

    updateContent();
}

TYSiteNodeWidget::~TYSiteNodeWidget()
{
}

void TYSiteNodeWidget::updateContent()
{
    // Code de TYSiteWidget
    QString num;

    _elmW->updateContent();

    _checkBoxUseTopoFile->setChecked(getElement()->getUseTopoFile());
    _lineEditTopoFile->setText(getElement()->getTopoFileName().c_str());
    if (!_checkBoxUseTopoFile->isChecked()) { _pushButtonTopoFile->setEnabled(false); }

    _lineEditEchelle->setText(num.setNum(getElement()->getEchelle(), 'f', 2));
    _lineEditNomTopo->setText(getElement()->getTopographie()->getName());
    _lineEditNomInfra->setText(getElement()->getInfrastructure()->getName());

    // Code de TYSiteNodeWidget
    _checkBoxRoot->setChecked(getElement()->getRoot());
    _checkBoxUseEmprise->setChecked(getElement()->getUseEmpriseAsCrbNiv());

    _lineEditAltiEmprise->setText(QString().setNum(getElement()->getAltiEmprise()));
    if (_checkBoxUseEmprise->isChecked()) { _lineEditAltiEmprise->setEnabled(true); }

    _listViewListSite->clear();
    for (unsigned int i = 0; i < getElement()->getListSiteNode().size(); i++)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(_listViewListSite);
        item->setText(0,  QString().setNum(i));
        item->setText(1, getElement()->getListSiteNode()[i]->getElement()->getName());
    }

    updateSIGData();
}

void TYSiteNodeWidget::apply()
{
    // Code de TYSiteWidget
    _elmW->apply();

    getElement()->setEchelle(_lineEditEchelle->text().toDouble());
    getElement()->updateGraphic();
    getElement()->setUseTopoFile(_checkBoxUseTopoFile->isChecked());

    if ((_checkBoxUseTopoFile->isChecked()) && (!_lineEditTopoFile->text().isEmpty()))
    {
        getElement()->setTopoFileName(_lineEditTopoFile->text().toStdString());
        getElement()->loadTopoFile(_lineEditTopoFile->text().toStdString());
    }

    // Code de TYSiteNodeWidget
    getElement()->setRoot(_checkBoxRoot->isChecked());
    getElement()->setUseEmpriseAsCrbNiv(_checkBoxUseEmprise->isChecked());
    if (_checkBoxUseEmprise->isChecked())
    {
        getElement()->setAltiEmprise(_lineEditAltiEmprise->text().toDouble());
    }
    else
    {
        getElement()->setAltiEmprise(0.0);
    }

    getElement()->setSIGType(_comboSIG->currentIndex());
    getElement()->setSIG_X(_lineEditSIG_X->text().toFloat());
    getElement()->setSIG_Y(_lineEditSIG_Y->text().toFloat());

    getElement()->setIsGeometryModified(true);

    emit modified();
}

void TYSiteNodeWidget::editSite(QTreeWidgetItem* item)
{
    int ret = getElement()->getListSiteNode()[item->text(0).toInt()]->getElement()->edit(this);

    if (ret == QDialog::Accepted)
    {
        _listViewListSite->clear();
        for (unsigned int i = 0; i < getElement()->getListSiteNode().size(); i++)
        {
            QTreeWidgetItem* item = new QTreeWidgetItem(_listViewListSite);
            item->setText(0,  QString().setNum(i));
            item->setText(1, getElement()->getListSiteNode()[i]->getElement()->getName());
        }
    }
}

void TYSiteNodeWidget::contextMenuEvent(QContextMenuEvent* e)
{
    //si on trouve un item associe
    QPoint point = _listViewListSite->mapFrom(this, e->pos());
    QTreeWidgetItem* item = _listViewListSite->itemAt(point.x(), point.y() - _listViewListSite->header()->height());

    if (item != NULL)
    {
        QMenu* pMenu = new QMenu(this);

        QAction* prop = pMenu->addAction(TR("id_property"));
        QAction* ret = pMenu->exec(_listViewListSite->mapToGlobal(point));

        if ((ret) && (ret == prop)) { editSite(item); }
    }
}

void TYSiteNodeWidget::updateEditAlti()
{
    if (_checkBoxUseEmprise->isChecked())
    {
        _lineEditAltiEmprise->setEnabled(true);
    }
    else
    {
        _lineEditAltiEmprise->setEnabled(false);
    }

    _pElement->setIsGeometryModified(true);
}

void TYSiteNodeWidget::updateSIGList()
{
    QString str = "TYMPAN";
    _comboSIG->insertItem(0, str);

    str = "LAMBERT IIe";
    _comboSIG->insertItem(1, str);

    str = "GPS (UTM)";
    _comboSIG->insertItem(2, str);

}

void TYSiteNodeWidget::updateSIGData()
{
    // SIG - Actif si le site est root
    if (getElement()->getRoot() && (getElement()->getSIGType() != 0))
    {
        _comboSIG->setEnabled(true);
        _lineEditSIG_X->setEnabled(true);
        _lineEditSIG_Y->setEnabled(true);
        _comboSIG->setCurrentIndex(getElement()->getSIGType());
        _lineEditSIG_X->setText(QString().setNum(getElement()->getSIG_X()));
        _lineEditSIG_Y->setText(QString().setNum(getElement()->getSIG_Y()));
    }
    else if (!getElement()->getRoot())
    {
        _comboSIG->setEnabled(false);
        _lineEditSIG_X->setText(QString().setNum(0.000));
        _lineEditSIG_X->setEnabled(false);
        _lineEditSIG_Y->setText(QString().setNum(0.000));
        _lineEditSIG_Y->setEnabled(false);
    }
    else
    {
        _comboSIG->setEnabled(true);
        _lineEditSIG_X->setText(QString().setNum(0.000));
        _lineEditSIG_X->setEnabled(false);
        _lineEditSIG_Y->setText(QString().setNum(0.000));
        _lineEditSIG_Y->setEnabled(false);
    }
}


void TYSiteNodeWidget::setSIGCoord(int typeCoord)
{
    getElement()->setSIGType(typeCoord);

    updateSIGData();
}


void TYSiteNodeWidget::changeStateTopoFile()
{
    if (!_checkBoxUseTopoFile->isChecked())
    {
        _pushButtonTopoFile->setEnabled(false);
        getElement()->setUseTopoFile(false);
    }
    else
    {
        _pushButtonTopoFile->setEnabled(true);
        getElement()->setUseTopoFile(true);
    }
}

void TYSiteNodeWidget::chooseTopoFile()
{
    QString file = QFileDialog::getOpenFileName(this, "Choose a file",
                                                "Fichier image", "Images (*.bmp *.jpg *.png *.tif)");

    if (!file.isNull())
    {
        _lineEditTopoFile->setText(file);
    }
}

void TYSiteNodeWidget::editInfra()
{
    int ret = getElement()->getInfrastructure()->edit(this);

    if (ret == QDialog::Accepted)
    {
        _lineEditNomInfra->setText(getElement()->getInfrastructure()->getName());
    }
}

void TYSiteNodeWidget::editTopo()
{
    int ret = getElement()->getTopographie()->edit(this);

    if (ret == QDialog::Accepted)
    {
        _lineEditNomTopo->setText(getElement()->getTopographie()->getName());
    }
}
