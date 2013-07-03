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
 * \file TYActionManager.cpp
 * \brief Gere les undos
 *
 *
 */



#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
//Added by qt3to4:
#include <QList>

#if defined(WIN32)
#include <crtdbg.h>
#endif

#if defined(WIN32)
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif


TYActionManager::TYActionManager(int size)
    : _current(-1), _size(size),  _savedAt(-1), _modified(false)
{
}

TYActionManager::~TYActionManager()
{
    while (!_history.isEmpty())
    {
        delete _history.takeFirst();
    }
}

void TYActionManager::addAction(TYAction* pAction)
{
    if (_current < (int)_history.count() - 1)
    {
        if (_current < _savedAt)
        {
            _savedAt = -2;
        }

        QList<TYAction*> actions;

        for (int i = 0; i <= _current; ++i)
        {
            actions.insert(i, _history.at(0));
            _history.takeAt(0);
        }

        actions.append(pAction);
        while (!_history.isEmpty())
        {
            delete _history.takeFirst();
        }
        _history = actions;
    }
    else
    {
        _history.append(pAction);
    }

    if ((int)_history.count() > _size)
    {
        _savedAt--;
        _history.removeFirst();
    }
    else
    {
        ++_current;
    }

    emitUndoRedo();
    _modified = true;
    modificationChanged(_modified);
}

void TYActionManager::emitUndoRedo()
{
    TYAction* undoAction = 0;
    TYAction* redoAction = 0;

    if (_current >= 0 && _current < (int)_history.count())
    {
        undoAction = _history.at(_current);
    }
    if (_current + 1 >= 0 && _current + 1 < (int)_history.count())
    {
        redoAction = _history.at(_current + 1);
    }

    bool ua = (undoAction != 0);
    QString uc;
    if (ua)
    {
        uc = undoAction->getName();
    }

    bool ra = (redoAction != 0);
    QString rc;
    if (ra)
    {
        rc = redoAction->getName();
    }

    emit undoRedoChanged(ua, ra, uc, rc);

    TYMainWindow* pMW = getTYMainWnd();
    if (pMW != NULL)
    {
        pMW->updateModelers();
    }
}

void TYActionManager::setModified(bool modified)
{
    _modified = modified;

    if (!_modified)
    {
        _savedAt = _current;
    }

    modificationChanged(_modified);
}

void TYActionManager::undo()
{
    if (_current > -1)
    {
        _history.at(_current)->undo();
        --_current;
    }

    emitUndoRedo();
    _modified = _savedAt != _current;
    modificationChanged(_modified);
}

void TYActionManager::redo()
{
    if (_current > -1)
    {
        if (_current < (int)_history.count() - 1)
        {
            ++_current;
            _history.at(_current)->redo();
        }
    }
    else
    {
        if (_history.count() > 0)
        {
            ++_current;
            _history.at(_current)->redo();
        }
    }

    emitUndoRedo();
    _modified = _savedAt != _current;
    modificationChanged(_modified);
}

void TYActionManager::setHistorySize(int size)
{
    _size = size;

    // On supprime les entrees en trop si la nouvelle taille est
    // plus grande que la precedente
    while ((int) _history.count() > _size)
    {
        _savedAt--;
        _history.removeLast();
        _modified = true;
    }

    // Trim au max si current depasse
    _current = (_current > _size - 1) ? _size - 1 : _current;

    emitUndoRedo();
    modificationChanged(_modified);
}

void TYActionManager::reset()
{
    while (!_history.isEmpty())
    {
        delete _history.takeFirst();
    }

    _current = -1;
    _savedAt = -1;
    _modified = false;

    emitUndoRedo();
    _modified = true;
    modificationChanged(_modified);
}
