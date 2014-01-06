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
 */



#include "TYProgressManager.h"

#if TY_USE_IHM

#include "OLocalizator.h"

#include <qapplication.h>
#include <qcursor.h>
#include <iostream>

#if defined(WIN32)
#include <crtdbg.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif


#define TR(id) OLocalizator::getString("TYProgressManager", (id))


// Initialisation.
TYProgressDialog* TYProgressManager::_pProgressDialog = NULL;
int TYProgressManager::_stepSize = 1;
bool TYProgressManager::_isConsole = false;


/*static*/ void TYProgressManager::create(QWidget* pParent, const char* name /*=0*/)
{
    if (!_isConsole)
    {
        QString strBtnCancel = TR("id_cancel_btn");
        if (strBtnCancel.length() == 0)
        {
            strBtnCancel = "Cancel";
        }

        _pProgressDialog = new TYProgressDialog(name, strBtnCancel, 100, pParent, name, true);

        _pProgressDialog->setAutoClose(true);
        _pProgressDialog->setAutoReset(true);
        _pProgressDialog->setMinimumDuration(0);

        // "Decoration"
        _pProgressDialog->setCursor(Qt::ArrowCursor);

        QString strCaption = TR("id_caption");
        if (strCaption.length() == 0)
        {
            strCaption = name;
        }
        _pProgressDialog->setWindowTitle(strCaption);
    }
}

/*static*/ void TYProgressManager::set(int totalSteps, int stepSize /*=1*/)
{
    if (!_pProgressDialog) { return; }

    _pProgressDialog->reset();

    if (totalSteps <= 0) { return; }
    if (stepSize <= 0) { return; }

    _stepSize = stepSize;
    _pProgressDialog->setMaximum(totalSteps);
    _pProgressDialog->show();
}

/*static*/ void TYProgressManager::step()
{
    bool dummy;
    step(dummy);
}

/*static*/ void TYProgressManager::step(bool& wasCancelled)
{
    if (_isConsole)
    {
        std::cout << '.';
        return;
    }

    if (!_pProgressDialog) { return; }

    // Si le bouton "Cancel" a ete enfonce
    wasCancelled = _pProgressDialog->wasCanceled();

    // On progresse de la taille du pas
    _pProgressDialog->setValue(_pProgressDialog->value() + _stepSize);

    // On donne la main a l'application si necessaire (redraw...)
    if (qApp->hasPendingEvents())
    {
        qApp->processEvents();
    }
}
/*static*/ int TYProgressManager::getProgress()
{
    if (!_pProgressDialog) { return 0; }

    // on retourne la valeur de progression
    return _pProgressDialog->value();
}

/*static*/ void TYProgressManager::setProgress(int progress)
{
    if (!_pProgressDialog) { return; }

    // on positionne la progression a la valeur donnee
    _pProgressDialog->setValue(progress);

    // On donne la main a l'application si necessaire (redraw...)
    if (qApp->hasPendingEvents())
    {
        qApp->processEvents();
    }
}

/*static*/ void TYProgressManager::stepToEnd()
{
    if (_isConsole)
    {
        std::cout << std::endl;
        return;
    }

    if (!_pProgressDialog) { return; }

    // On progresse jusqu'a la fin
    _pProgressDialog->setValue(_pProgressDialog->maximum());

    // On donne la main a l'application si necessaire (redraw...)
    if (qApp->hasPendingEvents())
    {
        qApp->processEvents();
    }
}

/* static */ void TYProgressManager::setWindowTitle(const char* caption)
{
    if (!_pProgressDialog) { return; }

    // On progresse jusqu'a la fin
    _pProgressDialog->setWindowTitle(caption);

    // On donne la main a l'application si necessaire (redraw...)
    if (qApp->hasPendingEvents())
    {
        qApp->processEvents();
    }
}
/* static */ void TYProgressManager::setMessage(const char* message)
{
    if (!_pProgressDialog) { return; }

    // On progresse jusqu'a la fin
    _pProgressDialog->setLabelText(message);

    // On donne la main a l'application si necessaire (redraw...)
    if (qApp->hasPendingEvents())
    {
        qApp->processEvents();
    }
}

#endif // TY_USE_IHM
