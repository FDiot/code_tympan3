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
 * \file TYLibraryWidget.h
 * \brief Widget pour lister les elements de la bibilotheque (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY__LIBRARY_WIDGET__
#define __TY__LIBRARY_WIDGET__


#include "TYAppDefines.h"

#include <qwidget.h>
#include <qlist.h>
//Added by qt3to4:
#include <QKeyEvent>

class QDir;
class QListWidget;
class QListWidgetItem;
class QComboBox;
class QLabel;

class TYElementCollection;

enum TYLibraryFilter
{
    eNoneFilter,            // Ne filtre pas
    eSiteFilter,            // Filtre tous les elements pouvant etre importes dans un site
    eInfraFilter,           // Filtre tous les elements pouvant etre importes dans une infrastructure
    eSourceFilter,          // Filtre toutes les sources
    eConstructionFilter,    // Filtre tous les batiments
    eBatimentFilter,        // Filtre tous les etages d'un batiment
    eEtageFilter,           // Filtre toutes les sources et machines
    eMachineFilter,         // Filtre toutes les machines
    eReseauTransportFilter, // Filtre tous les reseaux de transport
    eRouteFilter,           // Filtre toutes les routes
    eTopoFilter,            // Filtre tous les elements pouvant etre importe dans une topographie
    ePlanEauFilter,         // Filtre tous les plans d'eau
    eCoursEauFilter,        // Filtre tous les cours d'eau
    eCourbeNiveauFilter,    // Filtre toutes les courbes de niveau
    eTerrainFilter,         // Filtre tous les terrains
    eSitesFilter            // Filtre tous les sites
};

/**
 * \class TYLibraryWidget
 * \brief Widget pour lister les elements de la bibilotheque
 */
class TYLibraryWidget : public QWidget
{
    Q_OBJECT

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYLibraryWidget(int filter, QWidget* parent = 0, const char* name = 0, Qt::WFlags f = 0);

    /**
     * Destructeur.
     */
    virtual ~TYLibraryWidget();

    /**
     * Retourne la liste des elements selectionne
     */
    const QList<QString>& getSelectedEltsList() const;

    /**
     * Retourn la liste des elements
     */
    const QList<QString>& getEltsList() const;

    const QString getCurrentDir() const;

protected slots:
    void updateFrame();
    void updateHighlight(const QString&);
    void updateCategory(const QString&);

signals:
    void highlighted(const QString&);

protected:
    /**
     * Filtre les fichiers a afficher
     */
    void filtering(const QDir& dir, int filter = eSiteFilter);

    /**
     * Gestion des touches
     *  - Delete : Suppression des elements selectionne
     */
    void keyPressEvent(QKeyEvent* e);

    // Path relatif correspondant a la categorie courante
    QLabel* _pCategoryCurrentLabel;

    // les categories permettant de trier les elements dans des sous-repertoires
    QComboBox* _pCategoryChoiceListBox;

    // Liste des elements disponibles (en fonction du filtre de type et de la categorie en cours)
    QListWidget* _pElementChoiceListBox;

    ///La liste des elements selectionnes.
    QList<QString> _selectedEltsList;

    ///La liste des elements de la liste.
    QList<QString> _eltsList;

    // le repertoire racine des elements de la bibliotheques
    QString _rootDir;

    // le repertoire courant (categorie)
    QString _currentDir;

    // filtre sur le type d'element
    int _currentFilter;

    // indique si l'IHM est en cours de rafraichissement de son contenu
    bool _isFiltering;

};

#endif // __TY__LIBRARY_WIDGET__
