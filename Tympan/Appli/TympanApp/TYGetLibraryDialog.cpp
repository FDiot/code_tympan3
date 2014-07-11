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
 * \file TYGetLibraryDialog.cpp
 * \brief Boite de dialogue pour la lecture d'un element dans la bibliotheque
 *
 *
 */



#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/models/business/TYXMLManager.h"

#include <qcursor.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QBoxLayout>
#include <QHBoxLayout>

#include "Tympan/Tools/OLocalizator.h"

#define TR(id) OLocalizator::getString("TYGetLibraryDialog", (id))

TYGetLibraryDialog::TYGetLibraryDialog(int filter, QWidget* parent, const char* name, Qt::WFlags f)
    : QDialog(parent, f)
{
    setObjectName(name);
    setWindowTitle(TR("id_caption"));

    // Layout
    QGridLayout* pLayout = new QGridLayout();
    setLayout(pLayout);

    // On affiche la bibliotheque
    _pLibrary = new TYLibraryWidget(filter, this);
    pLayout->addWidget(_pLibrary, 0, 0);

    // Push buttons
    QBoxLayout* pBtnLayout = new QHBoxLayout();
    pLayout->addLayout(pBtnLayout, 1, 0);
    pBtnLayout->addStretch(1);

    _pOpenBtn = new QPushButton(TR("id_ok_btn"), this);
    _pOpenBtn->setEnabled(false);
    QObject::connect(_pOpenBtn, SIGNAL(clicked()), this, SLOT(openSelected()));
    pBtnLayout->addWidget(_pOpenBtn);

    QPushButton* pCloseBtn = new QPushButton(TR("id_cancel_btn"), this);
    pCloseBtn->setShortcut(Qt::Key_Escape);
    QObject::connect(pCloseBtn, SIGNAL(clicked()), this, SLOT(reject()));
    pBtnLayout->addWidget(pCloseBtn);

    QObject::connect(_pLibrary, SIGNAL(highlighted(const QString&)), this, SLOT(setEnableButton(const QString&)));
}

TYGetLibraryDialog::~TYGetLibraryDialog()
{
    QObject::disconnect(_pOpenBtn, SIGNAL(clicked()), this, SLOT(openSelected()));
    QObject::disconnect(_pLibrary, SIGNAL(highlighted(const QString&)), this, SLOT(setEnableButton(const QString&)));
}

LPTYElementArray TYGetLibraryDialog::getElements()
{
    return _elements;
}

void TYGetLibraryDialog::openSelected()
{
    QList<QString> selectedElts = _pLibrary->getSelectedEltsList();

    if (selectedElts.size() == 0)
    {
        return;
    }

    if (QMessageBox::question(this, TR("id_box_caption"), TR("id_box_text"), TR("id_box_yes"), TR("id_box_no"), QString::null, 0, 1))
    {
        return;
    }

    accept();

    _elements.clear();

    TYApplication::setOverrideCursor(Qt::WaitCursor);

    TYElement::setRegenerateID(true); // Force le renouvellement de l'Id des fichiers importes

    for (unsigned int i = 0; i < selectedElts.size(); ++i)
    {
        //QString filename = QDir::convertSeparators(getTYApp()->tympanUserDir() + "/library/" + selectedElts[i] + ".xml");
        QString filename = QDir::convertSeparators(_pLibrary->getCurrentDir() + "/" + selectedElts[i] + ".xml");

        // Chargement effectif du fichier
        TYXMLManager xmlManager;
        int res = xmlManager.load(filename.toLatin1(), _elements);

        // Backup du fichier (si le fichier est valide)
        if (res == 1)
        {
            bool bBackupOk = getTYMainWnd()->backupFile(filename);

            if (!bBackupOk)
            {
                OMessageManager::get()->info("***********************************************");
                OMessageManager::get()->info("*                                             *");
                OMessageManager::get()->info("* La copie de sauvegarde n'a pas fonctionne ! *");
                OMessageManager::get()->info("*                                             *");
                OMessageManager::get()->info("***********************************************");
            }
        }
        else
        {
            OMessageManager::get()->info("************************************************");
            OMessageManager::get()->info("*                                              *");
            OMessageManager::get()->info("*        Le document n est pas valide.         *");
            OMessageManager::get()->info("* La copie de sauvegarde du dernier document   *");
            OMessageManager::get()->info("* valide a ete preservee avec la terminaison   *");
            OMessageManager::get()->info("*                   .bak                       *");
            OMessageManager::get()->info("*                                              *");
            OMessageManager::get()->info("************************************************");
        }
    }

    TYElement::setRegenerateID(false); // Revient a l'etat initial

    TYApplication::restoreOverrideCursor();
}

void TYGetLibraryDialog::setEnableButton(const QString& str)
{
    if (str.isNull())
    {
        _pOpenBtn->setEnabled(false);
    }
    else
    {
        _pOpenBtn->setEnabled(true);
    }
}
