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
 * \file TYOpenElementDialog.cpp
 * \brief Boite de dialogue pour le chargement d'un element metier
 *
 *
 */



#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/TYVersion.h"

#include <qlistwidget.h>

#include "Tympan/MetierSolver/DataManagerCore/TYPreferenceManager.h"
#include "Tympan/MetierSolver/DataManagerCore/TYXMLManager.h"
#include "Tympan/Tools/OLocalizator.h"

#include <qlayout.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qfileinfo.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qcursor.h>
//Added by qt3to4:
#include <QHBoxLayout>
#include <QBoxLayout>
#include <QGridLayout>

#ifdef TY_USE_DXF
#include "TYDXFBDTOPOImporter.h"
// #include "TYDXFGenericImporter.h"
#endif // TY_USE_DXF

#if defined(WIN32)
#include <crtdbg.h>
#endif

#if defined(WIN32)
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif


#define TR(id) OLocalizator::getString("TYOpenElementDialog", (id))


TYOpenElementDialog::TYOpenElementDialog(QWidget* parent, const char* name, Qt::WFlags f):
    QDialog(parent, f)
{
    setObjectName(name);
    setWindowTitle(TR("id_caption"));

    // Layout
    QGridLayout* pLayout = new QGridLayout();
    setLayout(pLayout);

    _pElementChoiceListBox = new QListWidget(this);
    _pElementChoiceListBox->setSelectionMode(QAbstractItemView::SingleSelection);
    pLayout->addWidget(_pElementChoiceListBox, 0, 0);

    QBoxLayout* pBtnLayout = new QHBoxLayout();
    pLayout->addLayout(pBtnLayout, 1, 0);

    pBtnLayout->addStretch(1);

    _pOpenBtn = new QPushButton(TR("id_open_btn"), this);
    QObject::connect(_pOpenBtn, SIGNAL(clicked()), this, SLOT(openSelected()));
    pBtnLayout->addWidget(_pOpenBtn);

    QPushButton* pCloseBtn = new QPushButton(TR("id_close_btn"), this);
    pCloseBtn->setShortcut(Qt::Key_Escape);
    QObject::connect(pCloseBtn, SIGNAL(clicked()), this, SLOT(reject()));
    pBtnLayout->addWidget(pCloseBtn);

    QObject::connect(_pElementChoiceListBox, SIGNAL(itemSelectionChanged()), this, SLOT(updateFrame()));

    TYPreferenceManager::loadGeometryFromPreferences(metaObject()->className(), this);

    _isReadOnly = false;
    _multiSelect = false;

    updateFrame();
}

TYOpenElementDialog::~TYOpenElementDialog()
{
    QObject::disconnect(_pOpenBtn, SIGNAL(clicked()), this, SLOT(openSelected()));
    QObject::disconnect(_pElementChoiceListBox, SIGNAL(itemSelectionChanged()), this, SLOT(updateFrame()));

    TYPreferenceManager::saveGeometryToPreferences(metaObject()->className(), this);
}

void TYOpenElementDialog::open()
{
    TYXMLManager xmlManager;

    // NOUVELLE VERSION POUR RECUPERER LE REPERTOIRE
    QFileDialog* pDialog = new QFileDialog(parentWidget(), "Choose a file" , _dirName,
                                           "XML (*.xml)");
    pDialog->setFileMode(QFileDialog::ExistingFile); // Oblige a choisir un fichier existant

    pDialog->exec();

    QStringList list = pDialog->selectedFiles(); // Il existe un selectedFile, mais il n'est pas documente
    if ((pDialog->result() == QDialog::Accepted) && (!list.isEmpty()))
    {
        _fileName = list.first();

        QDir dir = pDialog->directory();
        _dirName = dir.absolutePath();
        QDir::setCurrent(_dirName);

        pDialog = NULL;

        TYApplication::setOverrideCursor(Qt::WaitCursor);

        if (!_fileName.endsWith(".xml"))
        {
            _fileName += ".xml";
        }

        QString itemName;

        _pElementChoiceListBox->clear();
        _elements.purge();

        // Verification de l'etat du fichier
        QFileInfo fi(_fileName);
        if (!(fi.isWritable())) { _isReadOnly = true; }



        // Chargement effectif du fichier
        int resuChargement = xmlManager.load(_fileName.toLatin1(), _elements);


        // Backup du fichier (si le fichier est valide)
        if (resuChargement == 1)
        {
            bool bBackupOk = getTYMainWnd()->backupFile(_fileName);

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

        // On rempli la liste
        for (unsigned int i = 0; i < _elements.size(); i++)
        {
            itemName = TYWidget::getDisplayName(_elements[i]);

            if (!_elements[i]->getName().isEmpty())
            {
                itemName += QString(" : %1").arg(_elements[i]->getName());
            }

            _pElementChoiceListBox->insertItem(i, new QListWidgetItem(itemName));
        }

        updateFrame();
        TYApplication::restoreOverrideCursor();

        // TO DO
        // RECUPERER LA DIRECTORY DU FICHIER CHOISI, LA COMMUNIQUER A L'APPLICATION QUI LA STOCKE
        accept(); // Changement d'etat de la boite de dialogue
    }
    else
    {
        // L'action a ete annulee, on ferme la dialog
        TYApplication::restoreOverrideCursor();
        reject();
    }


}

// Copy/pasted from an old version of Code_TYMPAN, changeset 04727c86256e &
// updated old Qt code (deprecated Qt API).
void TYOpenElementDialog::openBDTopo()
{
#ifdef TY_USE_DXF
    static QString DirectoryName = QString::null;

    // Chemin pour l'acces aux donnees
    DirectoryName = TYApplication::tympanUserDir() + "/echange";
    DirectoryName = QDir::convertSeparators(DirectoryName);

    DirectoryName = QFileDialog::getExistingDirectory(
                        this, TR("Choose a directory"), DirectoryName);


    if (!DirectoryName.isEmpty())
    {
        TYApplication::setOverrideCursor(Qt::WaitCursor);

        QString itemName;

        _pElementChoiceListBox->clear();
        _elements.purge();

        TYDXFBDTopoImporter importer;
        LPTYSiteNode pSite;
        importer.import(DirectoryName.toLatin1(), &pSite);
        _elements.add(pSite);
        // On rempli la liste
        for (unsigned int i = 0; i < _elements.size(); i++)
        {
            itemName = TYWidget::getDisplayName(_elements[i]);

            if (!_elements[i]->getName().isEmpty())
            {
                itemName += QString(" : %1").arg(_elements[i]->getName());
            }

            _pElementChoiceListBox->insertItem(i, new QListWidgetItem(itemName));
        }

        updateFrame();
        TYApplication::restoreOverrideCursor();
    }
    else
    {
        // L'action a ete annulee, on ferme la dialog
        reject();
    }
#endif // TY_USE_DXF
}

void TYOpenElementDialog::openSelected()
{
    if (_multiSelect)
    {
        openSelectedElements(); // Plusieurs elements potentiels
    }
    else
    {
        openSelectedElement(); // Un seul element
    }
}

void TYOpenElementDialog::openSelectedElement()
{
    if (_selectedElts.size() == 1)
    {
        openElement(_elements[_selectedElts[0]]);
    }

    reject();
}

void TYOpenElementDialog::openSelectedElements()
{
    _tabElem.clear();
    if (_selectedElts.size() == 0) { return; }  // Pas d'element selectionne

    for (unsigned int i = 0; i < _selectedElts.size(); i++)
    {
        _tabElem.push_back(_elements[_selectedElts[i]]);
    }

    reject();
}


void TYOpenElementDialog::openElement(LPTYElement pElt)
{
    if (pElt)
    {
        writeDebugMsg(QString("Ouverture d'un element de type : %1").arg(pElt->getClassName()));

        QMessageBox* pMsgBox = new QMessageBox(TR("id_caption"), "", QMessageBox::NoIcon,
                                               QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel | QMessageBox::Escape, this);

        // L'element est un Projet
        if (pElt->inherits("TYProjet"))
        {
            // Mise a jour des elements du projet
            LPTYProjet pProjet = TYProjet::safeDownCast(pElt);
            if (pProjet)
            {
                // Directement projet courant, la "place" etant libre
                getTYApp()->setCurProjet(pProjet);

                //// Mise a jour des elements du projet
                //TYSiteNode* pSite = pProjet->getSite();
                //TYCalcul* pCalcul = pProjet->getCurrentCalcul();
                //if (pSite && pCalcul) pSite->update();
            }
        }
        // L'element est un SiteNode
        else if (pElt->inherits("TYSiteNode"))
        {
            // Directement site courant, la "place" etant libre
            LPTYSiteNode pSite = TYSiteNode::safeDownCast(pElt);
            if (pSite)
            {
                getTYApp()->setCurSiteNode(pSite);
                pSite->updateAltimetrie();
            }
        }
        else if (pElt->inherits("TYBatiment"))
        {
            // Modeler directement
        }
        else if (pElt->inherits("TYMachine"))
        {
            // Modeler directement
        }
        // Dans tous les autres cas on edite l'element cree
        else if (pElt->edit(this) == Accepted)
        {
            // Si les modifications sont acceptes on demande ce qu'on fait de l'element
            save(pElt);
        }

        // Si c'est un element modifiable graphiquement, on ouvre un modeleur
        getTYMainWnd()->makeModeler(pElt);
    }
}

void TYOpenElementDialog::updateFrame()
{
    _selectedElts.clear();

    for (unsigned int i = 0; i < _pElementChoiceListBox->count(); i++)
    {
        if (_pElementChoiceListBox->isItemSelected(_pElementChoiceListBox->item(i)))
        {
            _selectedElts.append(i);
        }
    }

    _pOpenBtn->setEnabled(false);

    if (_selectedElts.size() >= 1)
    {
        _pOpenBtn->setEnabled(true);
    }
}

void TYOpenElementDialog::save(LPTYElement pElt)
{
    Q_CHECK_PTR(pElt);

    QMessageBox* pMsgBox = new QMessageBox(TR("id_savebox_caption"), TR("id_savebox_text"),
                                           QMessageBox::NoIcon, QMessageBox::Yes, QMessageBox::NoButton, QMessageBox::Cancel | QMessageBox::Escape, this);

    pMsgBox->setButtonText(QMessageBox::Yes, TR("id_xml_btn"));
    pMsgBox->setButtonText(QMessageBox::Cancel, TR("id_cancel_btn"));

    int code = pMsgBox->exec();

    if (code == QMessageBox::No)
    {
        // XML
        QString _fileName = QFileDialog::getSaveFileName(this, "", "", "XML (*.xml)");

        if (!_fileName.isEmpty())
        {
            if (!_fileName.endsWith(".xml"))
            {
                _fileName += ".xml";
            }

            TYXMLManager xmlManager;
            xmlManager.createDoc(TY_PRODUCT_XMLTAG_, TY_PRODUCT_VERSION_);
            xmlManager.addElement(pElt);

            if (xmlManager.save(_fileName) == 0)
            {
                QMessageBox::information(this, TR("id_savebox_caption"), TR("id_export_ok").arg(_fileName));
            }
            else
            {
                QMessageBox::warning(this, TR("id_savebox_caption"), TR("id_export_failed").arg(_fileName));
            }
        }
    }
}

/*virtual*/void TYOpenElementDialog::accept()
{
    QDialog::accept();
}

/*virtual*/void TYOpenElementDialog::reject()
{
    QDialog::reject();
}

void TYOpenElementDialog::setMultiSelect(const bool& b)
{
    _multiSelect = b;

    if (_multiSelect)
    {
        _pElementChoiceListBox->setSelectionMode(QAbstractItemView::ExtendedSelection);
    }
    else
    {
        _pElementChoiceListBox->setSelectionMode(QAbstractItemView::SingleSelection);
    }
}
