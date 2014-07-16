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
 * \file TYPluginDialog.cpp
 * \brief Boite de dialogue pour la selection du plugin de calcul
 *
 *
 */



#include "Tympan/models/business/TYPluginManager.h"

#include <qlayout.h>
#include <qpushbutton.h>

#include "Tympan/models/business/OLocalizator.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>

#define TR(id) OLocalizator::getString("TYPluginDialog", (id))

TYPluginDialog::TYPluginDialog(QWidget* parent, const char* name, Qt::WFlags f)
    : QDialog(parent, f)
{
    setObjectName(name);
    setWindowTitle(TR("id_caption"));

    // Layout
    QGridLayout* pLayout = new QGridLayout();
    setLayout(pLayout);

    // Plug-in list
    _pPluginListView = new QTreeWidget(this);
    _pPluginListView->setColumnCount(6);
    QStringList stringList;
    stringList.append(TR("id_name"));
    stringList.append(TR("id_author"));
    stringList.append(TR("id_version"));
    stringList.append(TR("id_description"));
    stringList.append(TR("id_uuid"));
    stringList.append(TR("id_filename"));
    _pPluginListView->setHeaderLabels(stringList);
    _pPluginListView->setRootIsDecorated(TRUE);
    _pPluginListView->setSelectionMode(QAbstractItemView::SingleSelection);
    //_pPluginListView->setAllColumnsShowFocus(true);
    pLayout->addWidget(_pPluginListView, 0, 0);

    TYPluginManager::TYPluginList& plugins = TYPluginManager::get()->getPluginList();
    TYPluginManager::TYPluginList::iterator it = plugins.begin();
    for (; it != plugins.end(); ++it)
    {
        QStringList labels;
        labels.append((*it)->getPlugin()->getName());
        labels.append((*it)->getPlugin()->getAuthor());
        labels.append((*it)->getPlugin()->getVersion());
        labels.append((*it)->getPlugin()->getDescription());
        labels.append((*it)->getPlugin()->getUUID().toString());
        labels.append((*it)->filename);
        QTreeWidgetItem* item = new QTreeWidgetItem(_pPluginListView, labels);

        if ((*it)->getPlugin()->getUUID() == TYPluginManager::get()->getCurrent())
        {
            _pPluginListView->setCurrentItem(item);
            _pPluginListView->setItemSelected(item, true);
        }
    }


    // Push buttons
    QBoxLayout* pBtnLayout = new QHBoxLayout();
    pLayout->addLayout(pBtnLayout, 1, 0);
    pBtnLayout->addStretch(1);

    _pOkBtn = new QPushButton(TR("id_ok_btn"), this);
    _pOkBtn->setEnabled(false);
    QObject::connect(_pOkBtn, SIGNAL(clicked()), this, SLOT(updateCurrentPlugin()));
    pBtnLayout->addWidget(_pOkBtn);

    QPushButton* pCloseBtn = new QPushButton(TR("id_cancel_btn"), this);
    pCloseBtn->setShortcut(Qt::Key_Escape);
    QObject::connect(pCloseBtn, SIGNAL(clicked()), this, SLOT(reject()));
    pBtnLayout->addWidget(pCloseBtn);

    QObject::connect(_pPluginListView, SIGNAL(itemSelectionChanged()), this, SLOT(setEnableButton()));
    setEnableButton();
}

TYPluginDialog::~TYPluginDialog()
{
    QObject::disconnect(_pPluginListView, SIGNAL(itemSelectionChanged()), this, SLOT(setEnableButton()));
    QObject::disconnect(_pOkBtn, SIGNAL(clicked()), this, SLOT(updateCurrentPlugin()));
}

void TYPluginDialog::updateCurrentPlugin()
{
    TYPluginManager::get()->setCurrent(OGenID(_pPluginListView->selectedItems()[0]->text(4)));
    accept();
}

void TYPluginDialog::setEnableButton()
{
    if (!_pPluginListView->selectedItems()[0])
    {
        _pOkBtn->setEnabled(false);
    }
    else
    {
        _pOkBtn->setEnabled(true);
    }
}
