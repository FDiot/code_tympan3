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
 * \file TYAddLibraryDialog.cpp
 * \brief Boite de dialogue pour l'ajout d'un element dans la bibliotheque
 *
 *
 */



#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/TYVersion.h"
#include "Tympan/models/business/TYXMLManager.h"

#include <qcursor.h>
#include <qdir.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QList>

#include "Tympan/models/business/OLocalizator.h"

#define TR(id) OLocalizator::getString("TYAddLibraryDialog", (id))

TYAddLibraryDialog::TYAddLibraryDialog(QWidget* parent, const char* name, Qt::WFlags f)
    : QDialog(parent, f), _element(0)
{
    setObjectName(name);
    setWindowTitle(TR("id_caption"));

    // Layout
    QGridLayout* pLayout = new QGridLayout();
    setLayout(pLayout);

    // On affiche la bibliotheque
    _pLibrary = new TYLibraryWidget(eNoneFilter, this);
    pLayout->addWidget(_pLibrary, 0, 0);

    // Nom de sauvegarde
    QBoxLayout* pNameLayout = new QHBoxLayout(this);
    pNameLayout->setMargin(5);
    pNameLayout->setSpacing(5);
    pLayout->addLayout(pNameLayout, 1, 0);
    pNameLayout->setAlignment(Qt::AlignLeft);

    _labelName = new QLabel(this);
    _labelName->setText(TR("id_name_label"));
    pNameLayout->addWidget(_labelName, 0, 0);

    _lineEditName = new QLineEdit(this);
    pNameLayout->addWidget(_lineEditName);
    QObject::connect(_pLibrary, SIGNAL(highlighted(const QString&)), _lineEditName, SLOT(setText(const QString&)));

    // Push buttons
    QBoxLayout* pBtnLayout = new QHBoxLayout();
    pLayout->addLayout(pBtnLayout, 2, 0);
    pBtnLayout->addStretch(1);

    _pSaveBtn = new QPushButton(TR("id_ok_btn"), this);
    _pSaveBtn->setEnabled(false);
    QObject::connect(_pSaveBtn, SIGNAL(clicked()), this, SLOT(save()));
    pBtnLayout->addWidget(_pSaveBtn);

    QPushButton* pCloseBtn = new QPushButton(TR("id_cancel_btn"), this);
    pCloseBtn->setShortcut(Qt::Key_Escape);
    QObject::connect(pCloseBtn, SIGNAL(clicked()), this, SLOT(reject()));
    pBtnLayout->addWidget(pCloseBtn);

    QObject::connect(_lineEditName, SIGNAL(textChanged(const QString&)), this, SLOT(setEnableButton(const QString&)));

    _lineEditName->clear();
}

TYAddLibraryDialog::~TYAddLibraryDialog()
{
    QObject::disconnect(_pLibrary, SIGNAL(highlighted(const QString&)), _lineEditName, SLOT(setText(const QString&)));
    QObject::disconnect(_lineEditName, SIGNAL(textChanged(const QString&)), this, SLOT(setEnableButton(const QString&)));
}

void TYAddLibraryDialog::setElement(LPTYElement element)
{
    _element = element;
}

void TYAddLibraryDialog::save()
{
    // Securite
    if (!_element)
    {
        return;
    }

    // Si il n'ya pas de texte, on retourne
    if (_lineEditName->text().isEmpty())
    {
        return;
    }

    // Si le nom se termine par l'extension ".xml", on la retire.
    QString name = _lineEditName->text();
    if (name.section(".", -1, -1) == "xml")
    {
        name = name.section(".", -2, -2);
    }

    // Si l'element existe deja dans la liste, on demande si on doit le remplacer
    QList<QString> eltsList = _pLibrary->getEltsList();
    int i = eltsList.indexOf(name);
    if (i != -1)
    {
        if (QMessageBox::question(this, TR("id_box_caption"), TR("id_box_text").arg(name), TR("id_box_yes"), TR("id_box_no"), QString::null, 0, 1))
        {
            return;
        }
    }

    TYApplication::setOverrideCursor(Qt::WaitCursor);

    QString filename = QDir::convertSeparators(_pLibrary->getCurrentDir() + "/" + name + ".xml");

    TYXMLManager xmlManager;

    QString version(TY_PRODUCT_VERSION_);
    QString tiret(" - ");
    QString licencie(TY_CUSTOMER_);
    QString licenceNumber(TY_LICENCE_NUMBER_);

    QString messageVersion = version + tiret + licencie + tiret + licenceNumber;

    xmlManager.createDoc(TY_PRODUCT_XMLTAG_, messageVersion);
    xmlManager.addElement(_element);

    if (xmlManager.save(filename) == 0)
    {
        QMessageBox::information(this, "Tympan", TR("id_export_ok").arg(name));
    }
    else
    {
        QMessageBox::warning(this, "Tympan", TR("id_export_failed").arg(name));
    }

    TYApplication::restoreOverrideCursor();

    accept();
}

void TYAddLibraryDialog::setEnableButton(const QString& str)
{
    if (str.isEmpty())
    {
        _pSaveBtn->setEnabled(false);
    }
    else
    {
        _pSaveBtn->setEnabled(true);
    }
}
