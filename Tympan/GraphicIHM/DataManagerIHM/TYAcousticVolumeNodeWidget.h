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
 * \file TYAcousticVolumeNodeWidget.h
 * \brief outil IHM pour un ensemble de volumes acoustiques (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_ACOUSTICVOLUMENODE_WIDGET__
#define __TY_ACOUSTICVOLUMENODE_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYAcousticVolumeNode;
class QLineEdit;
class QGridLayout;
class QLabel;
class QGroupBox;
class TYAcousticInterfaceWidget;
class TYVolumeInterfaceWidget;
class TYElementWidget;
class TYColorInterfaceWidget;
class QWidget;
class QTableWidget;

/**
 * \class TYAcousticVolumeNodeWidget
 * \brief Classe de l'objet IHM pour un ensemble de volumes acoustiques
 */
class TYAcousticVolumeNodeWidget : public TYWidget
{
    Q_OBJECT


    TY_DECL_METIER_WIDGET(TYAcousticVolumeNode)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYAcousticVolumeNodeWidget(TYAcousticVolumeNode* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYAcousticVolumeNodeWidget();

private:
    /** Applique les modifs aux sous volumes */
    void applyToSubVol();

    /** Met a jour la boite de dialogue des volumes*/
    void updateFromSubVol();


public slots:
    virtual void updateContent();
    virtual void apply();

    /**
     * Edite la widget de l'acoustic volume selectionne.
     *
     * @param item Element de la liste correspondant a l'acoustic volume selectionne.
     */
    void editAccVol(const int& item);

    /**
     * Supprime le volume node selectionne dans le tableau
     */
    void remAccVol(const int& item);

    /// Action a engager lorsque l'on ferme/cacahe la boite de dialogue des acousticVol
    void hideAccVolWidget();

    /**
     * Affiche un menu contextuel.
     */
    virtual void contextMenuRequested(const QPoint& point);

    void propagateRegime(int regime);

    /**
     * Indique que la couleur a ete modifiee.
     */
    void modifyColor();

signals:
    void regimeChanged(int);

    // Membres
protected:

    QGroupBox* _groupBox;
    QTableWidget* _tableFaces;

    QGridLayout* _acousticVolumeNodeLayout;
    QGridLayout* _groupBoxLayout;

    TYAcousticInterfaceWidget* _interW;
    TYVolumeInterfaceWidget* _volumeW;
    TYColorInterfaceWidget* _colorW;
    TYElementWidget* _elmW;

    QWidget* _pAccVolsWidget;

    bool _isColorModified;
};


#endif // __TY_ACOUSTICVOLUMENODE_WIDGET__
