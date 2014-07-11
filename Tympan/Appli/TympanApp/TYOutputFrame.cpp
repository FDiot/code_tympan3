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
 * \file TYOutputFrame.cpp
 * \brief Frame pour les messages de retour
 *
 *
 */



#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include <qtextedit.h>
#include <qlayout.h>
//Added by qt3to4:
#include <QBoxLayout>
#include <QVBoxLayout>

#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/models/business/TYPreferenceManager.h"


#define TR(id) OLocalizator::getString("TYOutputFrame", (id))


TYOutputFrame::TYOutputFrame(QWidget* parent, const char* name, Qt::WFlags f):
    QWidget(parent, f)
{
    setObjectName(name);
    QBoxLayout* pLayout = new QVBoxLayout(this);

    _pTextEdit = new QTextEdit(this);
    _pTextEdit->setReadOnly(true);
    //  _pTextEdit->setFontFamily("Courier");
    //  _pTextEdit->setFontPointSize(10);

    pLayout->addWidget(_pTextEdit);
}

TYOutputFrame::~TYOutputFrame()
{
}

void TYOutputFrame::appendMsg(QString msg)
{
    _pTextEdit->append(msg);
}
