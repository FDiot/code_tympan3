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

#ifndef __TY_PROGRESS_MANAGER__
#define __TY_PROGRESS_MANAGER__


#include "TYConfigure.h"

#if TY_USE_IHM

#include <qprogressdialog.h>
#include <QLabel>

/**
 * Boite de dialogue de progression.
 *
 * @author Projet_Tympan
 * @version $Revision: 1.1 $
 */
class TYProgressDialog : public QProgressDialog
{
public:
    TYProgressDialog(QWidget* creator = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags f = 0) :
        //QProgressDialog(creator, name, modal, f)
        QProgressDialog(creator, f)
    {
        QLabel* label = new QLabel(name);
        setLabel(label);
        if (modal)
        {
            setWindowModality(Qt::WindowModal);
        }
        else
        {
            setWindowModality(Qt::NonModal);
        }
    }
    TYProgressDialog(const QString& labelText, const QString& cancelButtonText, int totalSteps, QWidget* creator = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags f = 0) :
        QProgressDialog(labelText, cancelButtonText, 0, totalSteps, creator, f)
    {
        QLabel* label = new QLabel(name);
        setLabel(label);
        if (modal)
        {
            setWindowModality(Qt::WindowModal);
        }
        else
        {
            setWindowModality(Qt::NonModal);
        }
    }

public:
    /**
     * Surcharge pour que cette dialogue ne soit jamais disabled.
     */
    virtual void setEnabled(bool)
    {
        // Do nothing !
    }
};


/**
 * Gestionnaire de progression.
 * Affiche une boite de dialogue avec une barre de progression
 * pour indiquer l'avancement d'operations longues ou lentes.
 * Il est aussi possible d'annuler l'operation en cours.
 *
 * @author Projet_Tympan
 * @version $Revision: 1.1 $
 */
class TYProgressManager
{
public:
    /**
     * Fait office de constructeur.
     *
     * @param pParent Le widget parent (ne peut pas etre NULL).
     * @param name Le nom pour ce widget.
     */
    static void create(QWidget* pParent, const char* name = 0);


    /**
     * Initialise le progress manager pour une nouvelle operation.
     *
     * @param totalSteps Le nombre d'etape de la barre de progression.
     * @param stepSize La taille d'une etage
     */
    static void set(int totalSteps, int stepSize = 1);

    /**
     * Renvoie la valeur de la progression
     *
     * @return int La progression
     */
    static int getProgress();

    /**
     * Positionne la progression a une valeur donnee..
     *
     * @param progress la valeur de progression.
     */
    static void setProgress(int progress);

    /**
     * Incremente la barre de progression.
     */
    static void step();
    /**
     * Incremente la barre de progression.
     *
     * @param wasCancelled En passant un bool il est possible de savoir
     *                     si l'utilisateur a voulu annuler l'operation.
     */
    static void step(bool& wasCancelled);

    /**
     * Incremente la barre de progression jusqu'au max.
     */
    static void stepToEnd();
    /**
     * Change le titre de la boite de dialogue.
     */
    static void setWindowTitle(const char* caption);
    /**
     * Change le texte de la boite de dialogue.
     */
    static void setMessage(const char* message);
    /**
     * Retourne la ProgressDialog.
     */
    static TYProgressDialog* getProgressDialog() { return _pProgressDialog; }

    /**
     * Fixe le mode console.
     */
    static void setConsoleMode(bool isConsole) { _isConsole = isConsole; }
    /**
     * Indique si l'on est en mode console.
     */
    static bool getConsoleMode() { return _isConsole; }

protected:
    /**
     * Constructeur par defaut.
     */
    TYProgressManager() { }


private:
    ///La boite de dialogue de progression.
    static TYProgressDialog* _pProgressDialog;

    ///La taille d'un step.
    static int _stepSize;

    ///Mode console
    static bool _isConsole;
};

#else // TY_USE_IHM

class QWidget;

/**
 * Cette classe ne fait rien dans le cas ou Tympan n'utilise
 * pas l'API Qt. Seule la declaration des methodes est faite
 * afin de satisfaire les appels correspondant depuis le DataManager.
 *
 * @author Projet_Tympan
 * @version $Revision: 1.1 $
 */
class TYProgressManager
{
public:
    static void create(QWidget* pParent, const char* name = 0) { }
    static void set(int totalSteps, int stepSize = 1) { }
    static void step() { }
    static void step(bool& wasCancelled) { }
    static void stepToEnd() { }

protected:
    TYProgressManager() { }
};

#endif // TY_USE_IHM

#endif //__TY_PROGRESS_MANAGER__
