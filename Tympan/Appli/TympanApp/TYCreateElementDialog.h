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
 * \file TYCreateElementDialog.h
 * \brief Boite de dialogue pour la creation d'un nouvel element metier. L'element cree peut etre sauvegarde en BDD ou exporter en XML. (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_CREATE_ELEMENT_DIALOG__
#define __TY_CREATE_ELEMENT_DIALOG__


#include "TYAppDefines.h"
#include <qdialog.h>

#include "Tympan/MetierSolver/DataManagerCore/TYElement.h"

class QListWidget;


/**
 * \class TYCreateElementDialog
 * \brief Boite de dialogue pour la creation d'un nouvel element metier. L'element cree peut etre sauvegarde en BDD ou exporter en XML.
 */
class TYCreateElementDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * \fn TYCreateElementDialog(QWidget * parent = 0, const char * name = 0, Qt::WFlags f = 0)
     * \brief Constructeur par defaut.
     */
    TYCreateElementDialog(QWidget* parent = 0, const char* name = 0, Qt::WFlags f = 0);
    /**
     * \fn virtual ~TYCreateElementDialog()
     * \brief Destructeur.
     */
    virtual ~TYCreateElementDialog();


public slots:
    /**
     * \fn void createElement(QString eltType)
     * \brief Gere la creation d'un nouvel element au sein de l'application.
     * \param eltType Le type d'element a creer.
     */
    void createElement(QString eltType);


protected slots:
    /**
     * \fn virtual void done(int r)
     * \brief Ce slot est appele lorsqu'on valide un choix de la boite de dialogue.
     */
    virtual void done(int r);


protected:
    QListWidget* _pElementChoiceListBox;
};

#endif //__TY_CREATE_ELEMENT_DIALOG__
