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
 * \file TYOutputFrame.h
 * \brief Frame pour les messages de retour (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_OUTPUT_FRAME__
#define __TY_OUTPUT_FRAME__


#include "TYAppDefines.h"
#include <qwidget.h>

class QTextEdit;


/**
 * \class TYOutputFrame
 * \brief Frame pour les messages de retour
 */
class TYOutputFrame : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructeur par defaut.
     */
    TYOutputFrame(QWidget* parent = 0, const char* name = 0, Qt::WFlags f = 0);
    /**
     * Destructeur.
     */
    virtual ~TYOutputFrame();

    /**
     * Ajoute un nouveau message dans le fenetre de sortie.
     */
    void appendMsg(QString msg);

protected:
    QTextEdit* _pTextEdit;
};

#endif //__TY_OUTPUT_FRAME__
