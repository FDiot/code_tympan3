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




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHCore.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER


#if TY_USE_IHM

#include <qwidget.h>
#include <qrect.h>
#include <qstring.h>
#include <qfile.h>

#include "Tympan/models/business/acoustic/TYSpectre.h"

#include "TYPreferenceManager.h"


///Nombre de parametres par defaut.
static const int nbDefaultParams = 5;
///Parametres par defaut.
static const char* defaultParams[nbDefaultParams][2] =
{
    { "MainWindowMaximized", "true" } ,
    { "MainWindowPosX", "0" } ,
    { "MainWindowPosY", "0" } ,
    { "MainWindowSizeX", "800" } ,
    { "MainWindowSizeY", "600" }
};


OPreferenceManager* TYPreferenceManager::_prefMngr = NULL;
QString* TYPreferenceManager::_fileName = NULL;

OPreferenceManager& TYPreferenceManager::getInstance()
{
    if (_prefMngr == NULL)
    {
        _prefMngr = new OPreferenceManager("Tympan");
    }
    return *_prefMngr;
}

QString& TYPreferenceManager::getFileName()
{
    if (_fileName == NULL)
    {
        _fileName = new QString("");
    }
    return *_fileName;
}

void TYPreferenceManager::init(const QString& fileName /*=QString::null*/)
{
    if (!fileName.isEmpty() && !fileName.isNull())
    {
        setFileName(fileName);
    }
    else
    {
        getInstance().loadDefault(defaultParams, nbDefaultParams);
    }
}

void TYPreferenceManager::reset()
{
    getInstance().reset();
}

void TYPreferenceManager::setFileName(const QString& fileName)
{
    getFileName() = fileName;
}

bool TYPreferenceManager::read()
{
    if (QFile::exists(getFileName()))
    {
        return getInstance().readXML(getFileName());
    }
    else
    {
        return false;
    }
}

bool TYPreferenceManager::write()
{
    return getInstance().writeXML(getFileName());
}

bool TYPreferenceManager::exists(const QString& pref)
{
    return getInstance().exists(pref);
}

void TYPreferenceManager::saveGeometryToPreferences(const QString& pref, const QWidget* pWidget)
{
    Q_ASSERT(pWidget);

    getInstance().setBool(pref + "Maximized", pWidget->isMaximized());

    QPoint pos = pWidget->pos();
    QSize size = pWidget->size();

    if (pos.x() < 0) { pos.setX(0); }
    if (pos.y() < 0) { pos.setY(0); }
    if (size.width() <= 0) { size.setWidth(100); }
    if (size.height() <= 0) { size.setHeight(100); }

    getInstance().setFrame(pref, pos.x(), pos.y(), size.width(), size.height());
}

void TYPreferenceManager::loadGeometryFromPreferences(const QString& pref, QWidget* pWidget)
{
    Q_ASSERT(pWidget);

    if (getInstance().getBool(pref + "Maximized"))
    {
        pWidget->showMaximized();
    }
    else
    {
        int posX, posY, sizeX, sizeY;

        getInstance().getFrame(pref, posX, posY, sizeX, sizeY);

        if (posX < 0) { posX = 0; }
        if (posY < 0) { posY = 0; }
        pWidget->move(posX, posY);
        if (sizeX <= 0) { sizeX = 100; }
        if (sizeY <= 0) { sizeY = 100; }
        pWidget->resize(sizeX, sizeY);
    }
}

void TYPreferenceManager::setSpectre(const QString& pref, const TYSpectre* pSpectre)
{
    setSpectre(getInstance().getCurrentDirectory(), pref, pSpectre);
}

void TYPreferenceManager::setSpectre(const QString& dir, const QString& pref, const TYSpectre* pSpectre)
{
    for (unsigned int i = 0; i < pSpectre->getNbValues(); i++)
    {
        setFloat(pref + OPreferenceManager::intToString(i), pSpectre->getTabValReel()[i]);
    }
}

TYSpectre* TYPreferenceManager::getSpectre(const QString& pref)
{
    return getSpectre(getInstance().getCurrentDirectory(), pref);
}

TYSpectre* TYPreferenceManager::getSpectre(const QString& dir, const QString& pref)
{
    int nbFreq = 0;

    // Recherche de la taille du tableau
    while (exists(dir, pref + OPreferenceManager::intToString(nbFreq)))
    {
        ++nbFreq;
    }

    if (nbFreq <= 0) { return NULL; }

    TYSpectre* pSpectre = new TYSpectre();
    for (int i = 0; i < nbFreq; i++)
    {
        double value = getFloat(dir, pref + OPreferenceManager::intToString(i));
        pSpectre->getTabValReel()[i] = value;
    }

    return pSpectre;
}


#endif // TY_USE_IHM
