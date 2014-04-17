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
 * \file TYCalculManager.h
 * \brief Gestionnaire des calculs acoustiques. Il fait l'interface entre l'IHM et le gestionnaire de donnees
 * pour la partie calcul (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */


#ifndef __TY_CALCUL_MANAGER__
#define __TY_CALCUL_MANAGER__


#include <qobject.h>
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYProjet.h"


/**
 * \class TYCalculManager
 * \brief Gestionnaire des calculs acoustiques. Il fait l'interface entre l'IHM et le gestionnaire de donnees pour la partie calcul.
 */
class TYCalculManager : public QObject
{
    Q_OBJECT

public:
    /**
     * \fn TYCalculManager()
     * \brief Constructeur par defaut.
     */
    TYCalculManager();
    /**
     * \fn virtual ~TYCalculManager()
     * \brief Destructeur.
     */
    virtual ~TYCalculManager();

    /**
     * \fn LPTYCalcul getCurrent()
     * \brief Set/Get du Calcul courant.
     */
    LPTYCalcul getCurrent() { return _pCurrentCalcul; }

    /**
     * \fn bool askForResetResultat()
     * \brief Previent l'utilisateur que le resultat va etre efface, si
     * celui-ci est valide.
     * \return Le choix de l'utilisateur : effacer (true) ou annuler (false).
     */
    bool askForResetResultat();


public slots:
    /**
     * \fn bool launchCurrent()
     * \brief Execute le calcul courant.
     */
    bool launchCurrent();
    /**
     * \fn bool launch(LPTYCalcul pCalcul)
     * \brief Execute un calcul.
     */
    bool launch(LPTYCalcul pCalcul);

    /**
     * \fn void setCurrent(LPTYCalcul pCalcul)
     * \brief Set du Calcul et Projet courant.
     */
    void setCurrent(LPTYCalcul pCalcul);

    /**
     * \fn bool updateAcoustic(LPTYElement pElement)
     * \brief Appelle la methode de calcul acoustique du volume node passe.
     */
    bool updateAcoustic(TYElement* pElement);

protected:
    ///Le Calcul courant.
    LPTYCalcul _pCurrentCalcul;
};


#endif // __TY_CALCUL_MANAGER__
