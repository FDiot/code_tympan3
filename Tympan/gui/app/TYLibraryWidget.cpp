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
 * \file TYLibraryWidget.cpp
 * \brief Widget pour lister les elements de la bibilotheque
 */


#include <qlabel.h>
#include <qcombobox.h>
#include <qlistwidget.h>
#include <qmessagebox.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QList>
#include <QKeyEvent>

#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/TYXMLManager.h"
#include "Tympan/gui/app/TYApplication.h"
#include "TYLibraryWidget.h"

#define TR(id) OLocalizator::getString("TYLibraryWidget", (id))

TYLibraryWidget::TYLibraryWidget(int filter, QWidget* parent, const char* name, Qt::WFlags f)
    : QWidget(parent, f)
{
    setObjectName(name);
    setWindowTitle(TR("id_caption"));
    QGridLayout* pLayout = new QGridLayout();
    setLayout(pLayout);

    _pCategoryCurrentLabel = new QLabel(this);
    pLayout->addWidget(_pCategoryCurrentLabel, 0, 0);

    _pCategoryChoiceListBox = new QComboBox(this);
    pLayout->addWidget(_pCategoryChoiceListBox, 1, 0);

    _pElementChoiceListBox = new QListWidget(this);
    _pElementChoiceListBox->setSelectionMode(QAbstractItemView::ExtendedSelection);
    pLayout->addWidget(_pElementChoiceListBox, 2, 0);

    // Recuperation du dossier contenant la bibliotheque
    QString dirPath;
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "BiblioPath"))
    {
        dirPath = TYPreferenceManager::getString(TYDIRPREFERENCEMANAGER, "BiblioPath");
    }
    else
    {
        dirPath = QDir::convertSeparators(getTYApp()->tympanUserDir() + "/library/");
        TYPreferenceManager::setString(TYDIRPREFERENCEMANAGER, "BiblioPath", dirPath);
    }

    QDir dir(dirPath);

    _rootDir = dir.path();
    _currentDir = _rootDir;
    _currentFilter = filter;

    // Si la bibliotheque n'existe pas, on la cree
    if (!dir.exists())
    {
        dir.mkdir(dirPath);
    }

    // On rempli la liste
    filtering(dir, filter);

    QObject::connect(_pCategoryChoiceListBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(updateCategory(const QString&)));

    QObject::connect(_pElementChoiceListBox, SIGNAL(itemSelectionChanged()), this, SLOT(updateFrame()));
    QObject::connect(_pElementChoiceListBox, SIGNAL(highlighted(const QString&)), this, SLOT(updateHighlight(const QString&)));

    updateFrame();

}

TYLibraryWidget::~TYLibraryWidget()
{
    QObject::disconnect(_pCategoryChoiceListBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(updateCategory(const QString&)));

    QObject::disconnect(_pElementChoiceListBox, SIGNAL(itemSelectionChanged()), this, SLOT(updateFrame()));
    QObject::disconnect(_pElementChoiceListBox, SIGNAL(highlighted(const QString&)), this, SLOT(updateHighlight(const QString&)));
}

const QList<QString>& TYLibraryWidget::getSelectedEltsList() const
{
    return _selectedEltsList;
}

const QList<QString>& TYLibraryWidget::getEltsList() const
{
    return _eltsList;
}

const QString TYLibraryWidget::getCurrentDir() const
{
    return _currentDir;
}

void TYLibraryWidget::updateFrame()
{
    _selectedEltsList.clear();

    for (unsigned int i = 0; i < _pElementChoiceListBox->count(); ++i)
    {
        if (_pElementChoiceListBox->isItemSelected(_pElementChoiceListBox->item(i)))
        {
            _selectedEltsList.append(_pElementChoiceListBox->item(i)->text());
        }
    }

    if (_selectedEltsList.isEmpty())
    {
        emit highlighted(QString::null);
    }
    else
    {
        emit highlighted("ok");
    }
}

void TYLibraryWidget::updateHighlight(const QString& str)
{
    if (_selectedEltsList.isEmpty())
    {
        emit highlighted(QString::null);
    }
    else
    {
        emit highlighted(str);
    }
}

void TYLibraryWidget::updateCategory(const QString& str)
{
    if (!_isFiltering)
    {
        QDir new_dir(_currentDir + "/" + str);
        filtering(new_dir, _currentFilter);
        emit highlighted(QString::null);
    }
}

void TYLibraryWidget::filtering(const QDir& dir, int filter)
{
    _isFiltering = true;

    // Affichage de la liste des sous-repertoires (categrorie d'objets)
    QStringList subdirs_filters;
    subdirs_filters.append("*");
    QStringList subdirsList = dir.entryList(subdirs_filters, QDir::Dirs, QDir::Name);
    _pCategoryChoiceListBox->clear();
    _currentDir = dir.path();
    for (unsigned int j = 0; j < subdirsList.count(); ++j)
    {
        QString subdirname = subdirsList[j];
        if (subdirname == ".." && _currentDir != _rootDir)
            //on ne peut pas remonter depuis le repertoire racine de library
        {
            _pCategoryChoiceListBox->addItem(subdirname);
        }
        else
        {
            _pCategoryChoiceListBox->addItem(subdirname);
        }
    }

    QString categName = dir.canonicalPath();
    QDir rootDir(_rootDir);
    categName.remove(0, rootDir.canonicalPath().length());
    _pCategoryCurrentLabel->setText("." + categName);

    // Affichage de la liste des objets filtree par type
    QStringList filters;
    filters.append("*.xml");
    QStringList fileList = dir.entryList(filters, QDir::Files, QDir::Name);

    _pElementChoiceListBox->clear();
    _eltsList.clear();

    int count = 0;
    for (unsigned int i = 0; i < fileList.count(); ++i)
    {
        TYXMLManager xmlManager;

        QString filename = QDir::convertSeparators(dir.path() + "/" + fileList[i]);
        QString eltName;
        xmlManager.getEltType(filename, eltName);

        switch (filter)
        {
            case eNoneFilter:
            {
                QString item = fileList[i].section(".", -2, -2);
                _pElementChoiceListBox->insertItem(count++, new QListWidgetItem(item));
                _eltsList.push_back(item);
            }
            break;
            case eSiteFilter:
            {
                if ((eltName == "TYSiteNode") ||
                    (eltName == "TYInfrastructure") ||
                    (eltName == "TYUserSourcePonctuelle") ||
                    (eltName == "TYBatiment") ||
                    (eltName == "TYMachine") ||
                    (eltName == "TYReseauTransport") ||
                    (eltName == "TYRoute") ||
                    (eltName == "TYTopographie") ||
                    (eltName == "TYPlanEau") ||
                    (eltName == "TYCoursEau") ||
                    (eltName == "TYCourbeNiveau") ||
                    (eltName == "TYTerrain"))
                {
                    QString item = fileList[i].section(".", -2, -2);
                    _pElementChoiceListBox->insertItem(count++, new QListWidgetItem(item));
                    _eltsList.push_back(item);
                }
            }
            break;
            case eInfraFilter:
            {
                if ((eltName == "TYInfrastructure") ||
                    (eltName == "TYUserSourcePonctuelle") ||
                    (eltName == "TYBatiment") ||
                    (eltName == "TYMachine") ||
                    (eltName == "TYReseauTransport") ||
                    (eltName == "TYRoute"))
                {
                    QString item = fileList[i].section(".", -2, -2);
                    _pElementChoiceListBox->insertItem(count++, new QListWidgetItem(item));
                    _eltsList.push_back(item);
                }
            }
            break;
            case eSourceFilter:
            {
                if ((eltName == "TYUserSourcePonctuelle"))
                {
                    QString item = fileList[i].section(".", -2, -2);
                    _pElementChoiceListBox->insertItem(count++, new QListWidgetItem(item));
                    _eltsList.push_back(item);
                }
            }
            break;
            case eConstructionFilter:
            {
                if ((eltName == "TYBatiment"))
                {
                    QString item = fileList[i].section(".", -2, -2);
                    _pElementChoiceListBox->insertItem(count++, new QListWidgetItem(item));
                    _eltsList.push_back(item);
                }
            }
            break;
            case eBatimentFilter:
            {
                if ((eltName == "TYEtage") ||
                    (eltName == "TYAcousticCylinder"))
                {
                    QString item = fileList[i].section(".", -2, -2);
                    _pElementChoiceListBox->insertItem(count++, new QListWidgetItem(item));
                    _eltsList.push_back(item);
                }
            }
            break;
            case eEtageFilter:
            {
                if ((eltName == "TYUserSourcePonctuelle") ||
                    (eltName == "TYMachine"))
                {
                    QString item = fileList[i].section(".", -2, -2);
                    _pElementChoiceListBox->insertItem(count++, new QListWidgetItem(item));
                    _eltsList.push_back(item);
                }
            }
            break;
            case eMachineFilter:
            {
                if ((eltName == "TYMachine"))
                {
                    QString item = fileList[i].section(".", -2, -2);
                    _pElementChoiceListBox->insertItem(count++, new QListWidgetItem(item));
                    _eltsList.push_back(item);
                }
            }
            break;
            case eReseauTransportFilter:
            {
                if ((eltName == "TYReseauTransport"))
                {
                    QString item = fileList[i].section(".", -2, -2);
                    _pElementChoiceListBox->insertItem(count++, new QListWidgetItem(item));
                    _eltsList.push_back(item);
                }
            }
            break;
            case eRouteFilter:
            {
                if ((eltName == "TYRoute"))
                {
                    QString item = fileList[i].section(".", -2, -2);
                    _pElementChoiceListBox->insertItem(count++, new QListWidgetItem(item));
                    _eltsList.push_back(item);
                }
            }
            break;
            case eTopoFilter:
            {
                if ((eltName == "TYTopographie") ||
                    (eltName == "TYPlanEau") ||
                    (eltName == "TYCoursEau") ||
                    (eltName == "TYCourbeNiveau") ||
                    (eltName == "TYTerrain"))
                {
                    QString item = fileList[i].section(".", -2, -2);
                    _pElementChoiceListBox->insertItem(count++, new QListWidgetItem(item));
                    _eltsList.push_back(item);
                }
            }
            break;
            case ePlanEauFilter:
            {
                if ((eltName == "TYPlanEau"))
                {
                    QString item = fileList[i].section(".", -2, -2);
                    _pElementChoiceListBox->insertItem(count++, new QListWidgetItem(item));
                    _eltsList.push_back(item);
                }
            }
            break;
            case eCoursEauFilter:
            {
                if ((eltName == "TYCoursEau"))
                {
                    QString item = fileList[i].section(".", -2, -2);
                    _pElementChoiceListBox->insertItem(count++, new QListWidgetItem(item));
                    _eltsList.push_back(item);
                }
            }
            break;
            case eCourbeNiveauFilter:
            {
                if ((eltName == "TYCourbeNiveau"))
                {
                    QString item = fileList[i].section(".", -2, -2);
                    _pElementChoiceListBox->insertItem(count++, new QListWidgetItem(item));
                    _eltsList.push_back(item);
                }
            }
            break;
            case eTerrainFilter:
            {
                if ((eltName == "TYTerrain"))
                {
                    QString item = fileList[i].section(".", -2, -2);
                    _pElementChoiceListBox->insertItem(count++, new QListWidgetItem(item));
                    _eltsList.push_back(item);
                }
            }
            break;
            case eSitesFilter:
            {
                if ((eltName == "TYSiteNode"))
                {
                    QString item = fileList[i].section(".", -2, -2);
                    _pElementChoiceListBox->insertItem(count++, new QListWidgetItem(item));
                    _eltsList.push_back(item);
                }
            }
            break;
            default:
                break;
        }
    }

    _isFiltering = false;
}

void TYLibraryWidget::keyPressEvent(QKeyEvent* e)
{
    if (!_selectedEltsList.isEmpty())
    {
        if (e->key() == Qt::Key_Delete)
        {
            // Recuperation du dossier contenant la bibliotheque
            QString dirPath = QDir::convertSeparators(getTYApp()->tympanUserDir() + "/library/");
            QDir dir(dirPath);

            for (unsigned int i = 0; i < _selectedEltsList.count(); ++i)
            {
                if (!QMessageBox::question(this, TR("id_delete_box_caption"), TR("id_delete_box_text").arg(_selectedEltsList[i]), TR("id_delete_box_yes"), TR("id_delete_box_no"), QString::null, 0, 1))
                {
                    if (dir.remove(QDir::convertSeparators(_selectedEltsList[i] + ".xml")))
                    {
                        QMessageBox::information(this, "Tympan", TR("id_delete_ok").arg(_selectedEltsList[i]));

                        // Remove item from choice list box
                        QObject::disconnect(_pElementChoiceListBox, SIGNAL(itemSelectionChanged()), this, SLOT(updateFrame()));
                        if (_pElementChoiceListBox->findItems(_selectedEltsList[i], Qt::MatchExactly).count() > 0)
                        {
                            _pElementChoiceListBox->takeItem(_pElementChoiceListBox->row(_pElementChoiceListBox->findItems(_selectedEltsList[i], Qt::MatchExactly)[0]));
                        }
                        QObject::connect(_pElementChoiceListBox, SIGNAL(itemSelectionChanged()), this, SLOT(updateFrame()));

                        // Remove item from elements list
                        _eltsList.removeAll(_selectedEltsList[i]);
                    }
                    else
                    {
                        QMessageBox::warning(this, "Tympan", TR("id_delete_failed").arg(_selectedEltsList[i]));
                    }
                }
            }

            updateFrame();
        }
    }
}
