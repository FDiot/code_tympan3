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
 * \file TYOpenElementDialog.h
 * \brief Boite de dialogue pour le chargement d'un element metier (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_OPEN_ELEMENT_DIALOG__
#define __TY_OPEN_ELEMENT_DIALOG__

#include "TYAppDefines.h"
#include "Tympan/MetierSolver/DataManagerCore/TYDefines.h"

#include <qdialog.h>
#include <qlist.h>

#include "Tympan/MetierSolver/DataManagerCore/TYElementCollection.h"

class QListWidget;
class QPushButton;

/**
 * \class TYOpenElementDialog
 * \brief Boite de dialogue pour le chargement d'un element metier.
 * L'element peut etre ouvert dans un modeleur (Projet, Site, Bâtiment et Machine)
 * ou directement sauvegarde en BDD.
 */
class TYOpenElementDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * Constructeur par defaut.
     */
    TYOpenElementDialog(QWidget* parent = 0, const char* name = 0, Qt::WFlags f = 0);
    /**
     * Destructeur.
     */
    virtual ~TYOpenElementDialog();

    /**
     * Etat du fichier
     */
    bool isFileReadOnly() { return _isReadOnly; }

    /**
     * Nom du fichier lu
     */
    QString getFileName() { return _fileName; }
    const QString getFileName() const { return _fileName; }

    /// Get du nom du repertoire de travail
    QString getDirName() { return _dirName; }
    const QString getDirName() const { return _dirName; }

    /// Set du nom du repertoire de travail
    void setDirName(const QString& value) { _dirName = value; }

    /**
     * Capacite de selection de la listBox (true=multiselection/false=selection unique)
     */
    void setMultiSelect(const bool& b);

    /**
     * Lecture du tableau des elements lus
     */
    std::vector<LPTYElement> getTabElem() { return _tabElem; }

    /**
     * Retourne les elements de la liste de choix du dialogue
     */
    LPTYElementArray getAvailableElements() { return _elements; }


public slots:
    /**
     * Affiche la boite de dialogue.
     */
    void open();

    /**
     * Open DXF-BDTOPO files.
     */
    void openBDTopo();

    /**
     * Gere l'ouverture d'un element au sein de l'application.
     *
     * @param pElt L'element a ouvrir.
     */
    void openElement(LPTYElement pElt);
    /**
     * Slot appele lorsque l'utilisateur accepte la base de de donnee.
     */
    virtual void accept();
    /**
     * Slot appele lorsque l'utilisateur rejete la base de de donnee.
     */
    virtual void reject();

protected slots:
    void updateFrame();

    /// Lit la selection
    void openSelected();

    /// Lit le premier element selectionne
    void openSelectedElement();

    /// Lit l'ensemble des elements selectionnes
    void openSelectedElements();

    void save(LPTYElement pElt);


protected:
    ///La collection d'elements dans la liste.
    LPTYElementArray _elements;

    ///La liste des indices des elements selectionnes.
    QList<int> _selectedElts;

    // Nom du fichier lu
    QString _fileName;

    // Nom du repertoire de travail
    QString _dirName;

    QListWidget* _pElementChoiceListBox;
    QPushButton* _pOpenBtn;

    // Tableau des elements lus
    std::vector<LPTYElement> _tabElem;

    bool _isReadOnly;
    bool _multiSelect;
};

#endif //__TY_OPEN_ELEMENT_DIALOG__
