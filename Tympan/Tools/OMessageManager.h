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


#ifndef __O_MESSAGE_MANAGER__
#define __O_MESSAGE_MANAGER__


#include "OSmartPtr.h"

#include <iostream>
#include <time.h>
#include <qstring.h>


#define MSG_DEBUG   0x0001
#define MSG_BENCH   0x0002
#define MSG_INFO    0x0004
#define MSG_WARNING 0x0008
#define MSG_ERROR   0x0010
#define MSG_FATAL   0x0012


class OMessageManager;
///Smart pointer sur OMessageManager.
typedef SmartPtr<OMessageManager> LPOMessageManager;


/**
 * Classe utilitaire pour la gestion des messages.
 * Les messages sont formates et affiches selon leur type (info,
 * toDo...) ou niveau d'erreur (warning, error, fatal).
 * Cette classe peut etre derivee afin de surcharger notamment
 * les methodes format() et output(), pour respectivement
 * modifier le formatage des messages et les rediriger.
 * Reprise du code C de Pascal Mobuchon.
 *
 * @author Projet_Tympan
 * @author Projet_Tympan
 */
class OMessageManager : public IRefCount
{
    // Methods
public:
    /**
     * Constructeur.
     */
    OMessageManager();
    /**
     * Destructeur.
     */
    virtual  ~OMessageManager();


    /**
     * Definit cette instance comme singleton.
     * Attention : cette instance doit absolument avoir ete creee
     * sur la heap (operateur new).
     *
     * @return Indique si un singleton etait deja defini ou pas.
     */
    bool setAsSingleton();
    /**
     * Retourne l'instance singleton.
     */
    static OMessageManager* get();


    /**
     * Message de type warning.
     */
    virtual void warning(const char* message, ...);

    /**
     * Message de type erreur.
     */
    virtual void error(const char* message, ...);

    /**
     * Message de type erreur.
     */
    virtual void fatal(const char* message, ...);

    /**
     * Message de type information.
     */
    virtual void info(const char* message, ...);

    /**
     * Message de type debug.
     */
    virtual void debug(const char* message, ...);

    /**
     * Message d'erreur general pour un fichier absent.
     */
    virtual void missingFile(const char* nomFic);

    /**
     * Message informant que la fonctionnalite n'est pas encore implementee.
     */
    virtual void toDo(const char* message);

    /**
     * Trace dans un fichier.
     */
    virtual void trace(const char* message, ...);

    /**
    * Message de type warning.
    */
    virtual void warning(const QString& message, ...);

    /**
     * Message de type erreur.
     */
    virtual void error(const QString& message, ...);

    /**
     * Message de type erreur.
     */
    virtual void fatal(const QString& message, ...);

    /**
     * Message de type information.
     */
    virtual void info(const QString& message, ...);

    /**
     * Message de type debug.
     */
    virtual void debug(const QString& message, ...);

    /**
     * Message d'erreur general pour un fichier absent.
     */
    virtual void missingFile(const QString& nomFic);

    /**
     * Message informant que la fonctionnalite n'est pas encore implementee.
     */
    virtual void toDo(const QString& message);

    /**
     * Trace dans un fichier.
     */
    virtual void trace(const QString& message, ...);

    /**
     * Formate les messages.
     * Cette methode est utilisee par les methodes specifiques
     * a chaque type de message (warning(), toDo(), etc.).
     * Elle peut etre surchargee pour formater les messages d'une
     * maniere differente.
     *
     * @param level Niveau du message (MSG_DEBUG, MSG_INFO, MSG_WARNING, ...).
     * @param message Contenu du message (format printf).
     */
    virtual void format(int level, const char* message, ...);

    /**
     * Affiche/ecrit le message final, le niveau du message est passe dans
     * le cas ou la redirection des messages depend de celui-ci.
     * Cette methode est appelee une fois que le message est ete formate.
     * Elle peut etre surchargee pour rediriger les messages, par
     * defaut ils sont envoyes vers stdout ou stderr selon le type.
     *
     * @param message Message final a afficher/ecrire.
     * @param level Niveau du message (MSG_DEBUG, MSG_INFO, MSG_WARNING, ...).
     *
     * @see format()
     */
    virtual void output(const char* message, int level);


    /**
     * Convertit et formate la date en une chaine de caractere.
     */
    static char* getStrDate();

    /**
     * Test la validite dans le temps d'un fichier de trace.
     */
#ifdef _WIN32
    static void checkFile(struct _finddata_t* c_file, time_t theTime);
#else
    static void checkFile(const char* c_file, time_t theTime);
#endif

protected:
    /**
     * Initialise le fichier de trace.
     */
    virtual int initTrace();


    // Members
protected:
    ///Le fichier de trace.
    FILE*   _ficTrace;


private:
    ///Instance unique du singleton.
    static LPOMessageManager _pInstance;
};

#endif // __O_MESSAGE_MANAGER__
