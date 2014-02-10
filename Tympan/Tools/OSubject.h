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

/*
 *
 *
 *
 *
 */

#ifndef __O_SUBJECT__
#define __O_SUBJECT__

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "OObserver.h"
#include <vector>

using namespace std;


///Collection de reference de OObserver.
typedef vector<OObserver*> OTabPtrObserver;


/**
 * Classe abstraite Subject du pattern Observer.
 *
 * @author Projet_Tympan
 * @see     OObserver
 */
class OSubject
{
    // Methodes
public:
    /**
     * Ajoute un nouvel observer.
     *
     * @param pObserver Un observer a attacher.
     */
    void attach(OObserver* pObserver);

    /**
     * Enleve un observer.
     *
     * @param pObserver Un observer a enlever.
     *
     * @return <code>true</code> si l'observer a correctement ete supprime;
     *         <code>false</code> sinon.
     */
    bool detach(OObserver* pObserver);

    /**
     * Supprime tous les observers.
     */
    void reset();

    /**
     * Notifie (methode update()) tous les observers attaches au subject.
     */
    void notify();

    // Membres
private:
    OTabPtrObserver _observers;
};


#endif // __O_SUBJECT__
