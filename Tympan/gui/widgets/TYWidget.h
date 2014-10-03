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
 * \file TYWidget.h
 * \brief outil IHM pour un objet metier de type TYElement (fichier header)
 * \author Projet_Tympan
 *
 */

#ifndef __TY_WIDGET__
#define __TY_WIDGET__


#include <QWidget>

class QString;
class TYElement;


#define TY_DECL_METIER_WIDGET(classname) \
    public: \
    inline classname* getElement() const { return (classname*)_pElement; }


/**
 * \class TYWidget
 * \brief classe de l'objet IHM pour un objet metier de type TYElement
 */
class TYWidget : public QWidget
{
    Q_OBJECT

    // Methodes
public:
    /**
     * Constructeur.
     * Il est identique a celui de la classe QWidget.
     */
    TYWidget(TYElement* pElement, QWidget* parent = 0, const char* name = 0, Qt::WFlags f = 0);
    /**
     * Destructeur.
     */
    virtual ~TYWidget();


    /**
     * Methode statique qui affiche le widget associe a l'element passe
     * pour l'edition de ses parametres dans une boite de dialogue avec
     * les boutons Appliquer, Cancel et OK.
     *
     * @param pElement L'element pour lequel on souhaite afficher une boite
     *                 de dialogue.
     * @param pParent Un pointeur sur le widget parent.
     *
     * @return Retourne le 'DialogCode', QDialog::Accepted si l'element a
     *         ete modifie (Bouton OK), QDialog::Rejected sinon.
     */
    int static edit(TYElement* pElement, QWidget* pParent = NULL);

    /**
     * Retourne le nom d'affichage de la donnee metier associee a cette classe.
     * Le nom retourne tiens compte de la localisation.
     *
     * @param pElt Le pointeur sur un element metier.
     *
     * @return Le nom d'affichage de la donnee metier.
     */
    static QString getDisplayName(TYElement* pElt);

    void setElement(TYElement *pElement) { _pElement = pElement; }

public slots:
    /**
     * Remplis les champs avec les valeurs de l'element associe.
     */
    virtual void updateContent() = 0;

    /**
     * Effectue la tâche inverse de updateContent().
     * Mets a jour les parametres de l'element associe avec les
     * valeurs des champs.
     */
    virtual void apply() = 0;

    /**
     * Sortie sans modif de l'utilisateur ("Cancel")
     */
    virtual void reject() {}

signals:
    /**
     * Ce signal est emis lorsque les modification (slot apply())
     * ont ete effectuees.
     */
    void modified();

protected :
    //sm++ donnee membre pour la correction du bug lie a la macro TY_DECL_METIER_WIDGET
    TYElement* _pElement;

    // booleen permettant de bloquer certains champs (en fonction du type d'objet)
    bool _locked;
};


#endif // __TY_WIDGET__
