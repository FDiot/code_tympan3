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
 * \file TYProjetWidget.cpp
 * \brief outil IHM pour un projet
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/TYProjet.h"
#include <qmenu.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QDateEdit>
#include <QTextEdit>
#include <QLabel>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QContextMenuEvent>
#include <QHeaderView>



#define TR(id) OLocalizator::getString("TYProjetWidget", (id))


TYProjetWidget::TYProjetWidget(TYProjet* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{

    _elmW = new TYElementWidget(pElement, this);

    resize(300, 460);
    setWindowTitle(TR("id_caption"));
    _projetLayout = new QGridLayout();
    setLayout(_projetLayout);
    _projetLayout->addWidget(_elmW, 0, 0);

    // Groupe identification (Auteur, Date creation/modification, commentaire)
    _groupBox = new QGroupBox(this);
    _groupBox->setTitle(TR(""));
    QGridLayout* _groupBoxLayout = new QGridLayout();
    _groupBox->setLayout(_groupBoxLayout);

    _lineEditAuteur = new QLineEdit(_groupBox);
    _groupBoxLayout->addWidget(_lineEditAuteur, 0, 1);

    _labelAuteur = new QLabel(_groupBox);
    _labelAuteur->setText(TR("id_auteur_label"));
    _groupBoxLayout->addWidget(_labelAuteur, 0, 0);

    QDate date;

    _editDateCreation = new QDateEdit(_groupBox);
    if (getElement()->getDateCreation() == "2001-10-01")
    {
        getElement()->setDateCreation(date.currentDate().toString(Qt::ISODate));
    }
    _editDateCreation->setDate(date.fromString(getElement()->getDateCreation(), Qt::ISODate));
    _groupBoxLayout->addWidget(_editDateCreation, 2, 1);

    _labelDateCreation = new QLabel(_groupBox);
    _labelDateCreation->setText(TR("id_datecreation_label"));
    _groupBoxLayout->addWidget(_labelDateCreation, 2, 0);

    _labelDateModif = new QLabel(_groupBox);
    _labelDateModif->setText(TR("id_datemodif_label"));
    _groupBoxLayout->addWidget(_labelDateModif, 3, 0);

    _editDateModif = new QDateEdit(_groupBox);
    if (getElement()->getDateModif() == "2001-10-01")
    {
        getElement()->setDateModif(date.currentDate().toString(Qt::ISODate));
    }
    _editDateModif->setDate(date.fromString(getElement()->getDateModif(), Qt::ISODate));
    _groupBoxLayout->addWidget(_editDateModif, 3, 1);

    _lineEditComment = new QTextEdit(_groupBox);
    _groupBoxLayout->addWidget(_lineEditComment, 5, 0, 1, 2);

    _labelComment = new QLabel(_groupBox);
    _labelComment->setText(TR("id_comment_label"));
    _groupBoxLayout->addWidget(_labelComment, 4, 0);

    _projetLayout->addWidget(_groupBox, 1, 0);

    // Groupe Site
    _groupBoxSite = new QGroupBox(this);
    _groupBoxSite->setTitle(TR("id_site"));
    QGridLayout* groupBoxSiteLayout = new QGridLayout();
    _groupBoxSite->setLayout(groupBoxSiteLayout);

    QPushButton* _pushButtonSite = new QPushButton(_groupBoxSite);
    _pushButtonSite->setText(TR("id_proprietes_button"));
    groupBoxSiteLayout->addWidget(_pushButtonSite, 0, 1);

    _lineEditNomSite = new QLineEdit(_groupBoxSite);
    _lineEditNomSite->setEnabled(FALSE);
    groupBoxSiteLayout->addWidget(_lineEditNomSite, 0, 0);

    _labelDistMax = new QLabel(_groupBoxSite);
    _labelDistMax->setText(TR("id_distMax_label"));
    groupBoxSiteLayout->addWidget(_labelDistMax, 1, 0);

    _lineEditDistMax = new QLineEdit(_groupBoxSite);
    groupBoxSiteLayout->addWidget(_lineEditDistMax, 1, 1);

    _labelDelaunayTolerence = new QLabel(_groupBoxSite) ;
    _labelDelaunayTolerence->setText(TR("id_delaunay_label"));
    groupBoxSiteLayout->addWidget(_labelDelaunayTolerence, 2, 0);

    _lineEditDelaunayTolerence = new QLineEdit(_groupBoxSite);
    groupBoxSiteLayout->addWidget(_lineEditDelaunayTolerence, 2, 1);

    _labelUseDefault = new QLabel(_groupBoxSite);
    _labelUseDefault->setText(TR("id_useDefault_label"));
    groupBoxSiteLayout->addWidget(_labelUseDefault, 3, 0);

    QPushButton* _pushButtonUseDefault = new QPushButton(_groupBoxSite);
    _pushButtonUseDefault->setText(TR("id_default_button"));
    groupBoxSiteLayout->addWidget(_pushButtonUseDefault, 3, 1) ;

    _projetLayout->addWidget(_groupBoxSite, 2, 0);

    // Groupe calcul
    _groupBoxCurCalcul = new QGroupBox(this);
    _groupBoxCurCalcul->setTitle(TR("id_curcalcul"));
    QGridLayout* groupBoxCurCalculLayout = new QGridLayout();
    _groupBoxCurCalcul->setLayout(groupBoxCurCalculLayout);

    QPushButton* _pushButtonCurCalcul = new QPushButton(_groupBoxCurCalcul);
    _pushButtonCurCalcul->setText(TR("id_proprietes_button"));
    groupBoxCurCalculLayout->addWidget(_pushButtonCurCalcul, 0, 1);

    _lineEditNomCurCalcul = new QLineEdit(_groupBoxCurCalcul);
    _lineEditNomCurCalcul->setEnabled(FALSE);

    groupBoxCurCalculLayout->addWidget(_lineEditNomCurCalcul, 0, 0);

    _projetLayout->addWidget(_groupBoxCurCalcul, 3, 0);

    // Groupe tableau des calculs
    _groupBoxTab = new QGroupBox(this);
    _groupBoxTab->setTitle(TR("id_tabCalcul_box"));
    QGridLayout* groupBoxTabLayout = new QGridLayout();
    _groupBoxTab->setLayout(groupBoxTabLayout);

    _listViewTabCalcul = new QTreeWidget(_groupBoxTab);
    _listViewTabCalcul->setColumnCount(2);
    QStringList stringList;
    stringList.append(TR(""));
    stringList.append(TR("id_Calcul"));
    _listViewTabCalcul->setHeaderLabels(stringList);
    _listViewTabCalcul->setRootIsDecorated(TRUE);
    //_listViewTabCalcul->setHeaderHidden(true);

    groupBoxTabLayout->addWidget(_listViewTabCalcul, 0, 0);

    _projetLayout->addWidget(_groupBoxTab, 4, 0);

    updateContent();

    connect(_pushButtonCurCalcul, SIGNAL(clicked()), this, SLOT(editCurCalcul()));
    connect(_pushButtonSite, SIGNAL(clicked()), this, SLOT(editSite()));
    connect(_pushButtonUseDefault, SIGNAL(clicked()), this, SLOT(updateDefault()));
}

TYProjetWidget::~TYProjetWidget()
{
}

void TYProjetWidget::updateContent()
{
    QDate date;

    _elmW->updateContent();

    _lineEditAuteur->setText(getElement()->getAuteur());
    _lineEditNomSite->setText(QString(getElement()->getSite()->getName()));
    if (getElement()->getCurrentCalcul() != NULL)
    {
        _groupBoxCurCalcul->setEnabled(TRUE);
        _lineEditNomCurCalcul->setText(QString(getElement()->getCurrentCalcul()->getName()));
    }
    else
    {
        _groupBoxCurCalcul->setEnabled(FALSE);
    }

    _lineEditComment->setPlainText(getElement()->getComment());
    _editDateModif->setDate(date.currentDate());
    _editDateCreation->setDate(date.fromString(getElement()->getDateCreation(), Qt::ISODate));

    _lineEditDistMax->setText(QString().setNum(getElement()->getDMax()));
    _lineEditDelaunayTolerence->setText(QString().setNum(getElement()->getDelaunayTolerence()));

    _listViewTabCalcul->clear();
    for (unsigned int i = 0; i < getElement()->getListCalcul().size(); i++)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(_listViewTabCalcul, 0);
        item->setText(0, QString().setNum(i));
        item->setText(1, getElement()->getListCalcul().at(i)->getName());
    }
}

void TYProjetWidget::apply()
{
    _elmW->apply();

    getElement()->setComment(_lineEditComment->toPlainText());
    getElement()->setAuteur(_lineEditAuteur->text());
    getElement()->setDateModif(_editDateModif->date().currentDate().toString(Qt::ISODate));
    getElement()->setDateCreation(_editDateCreation->date().toString(Qt::ISODate));

    getElement()->setDMax(_lineEditDistMax->text().toDouble());
    getElement()->setDelaunayTolerence(_lineEditDelaunayTolerence->text().toDouble());

    getElement()->setIsGeometryModified(true);
    getElement()->getSite()->setIsGeometryModified(true);

    emit modified();
}

void TYProjetWidget::editCurCalcul()
{
    int ret = getElement()->getCurrentCalcul()->edit(this);

    if (ret == QDialog::Accepted)
    {
        _lineEditNomCurCalcul->setText(getElement()->getCurrentCalcul()->getName());
    }
}

void TYProjetWidget::editSite()
{
    int ret = getElement()->getSite()->edit(this);

    if (ret == QDialog::Accepted)
    {
        _lineEditNomSite->setText(getElement()->getSite()->getName());
    }
}

void TYProjetWidget::editCalcul(QTreeWidgetItem* item)
{
    int ret = getElement()->getListCalcul()[item->text(0).toInt()]->edit(this);

    if (ret == QDialog::Accepted)
    {
        _listViewTabCalcul->clear();
        for (unsigned int i = 0; i < getElement()->getListCalcul().size(); i++)
        {
            QTreeWidgetItem* item = new QTreeWidgetItem(_listViewTabCalcul, 0);
            item->setText(0, QString().setNum(i));
            item->setText(1, getElement()->getListCalcul().at(i)->getName());
        }
    }
}

void TYProjetWidget::updateDefault()
{
    _lineEditDistMax->setText(QString().setNum(getElement()->getDefaultDMax()));
    _lineEditDelaunayTolerence->setText(QString().setNum(getElement()->getDefaultDelaunayTolerence()));
}


void TYProjetWidget::contextMenuEvent(QContextMenuEvent* e)
{
    //si on trouve un item associe
    QPoint point = _listViewTabCalcul->mapFrom(this, e->pos());
    QTreeWidgetItem* item = _listViewTabCalcul->itemAt(point.x(), point.y() - _listViewTabCalcul->header()->height());

    if (item != NULL)
    {
        QMenu* pMenu = new QMenu(this);

        QAction* prop = pMenu->addAction(TR("id_property"));
        QAction* ret = pMenu->exec(_listViewTabCalcul->mapToGlobal(point));

        if (ret == prop)
        {
            editCalcul(item);
        }
    }
}
