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

#ifndef __TY_PANEL__
#define __TY_PANEL__

#include "Tympan/MetierSolver/DataManagerCore/TYElement.h"

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerGraphic/TYPanelGraphic.h"
#endif

/**
 * \file TYPanel.h
 * \class TYPanel
 * \brief Classe de definition d'un panel d'information.
 * \version v1.0
 * \author Projet_Tympan
 * \date
 */
class TYPanel : public TYElement
{
    OPROTOSUPERDECL(TYPanel, TYElement)
    TY_EXT_GRAPHIC_DECL(TYPanel)

    // Methodes
public:
    /**
     * \fn TYPanel()
     * \brief Constructeur.
     *  Le constructeur de la calsse TYPanel
     */
    TYPanel();

    /**
     * \fn TYPanel(const TYPanel& other)
     * \brief Constructeur par copie.
     *  Le constructeur par copie de la calsse TYPanel
     */
    TYPanel(const TYPanel& other);

    /**
     * \fn virtual ~TYPanel()
     * \brief Destructeur.
     * Le destructeur de la calsse TYPanel
     */
    virtual ~TYPanel();

    ///Operateur =.
    TYPanel& operator=(const TYPanel& other);

    ///Operateur ==.
    bool operator==(const TYPanel& other) const;

    ///Operateur !=.
    bool operator!=(const TYPanel& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    /**
     * \fn  std::string getFirstPos()
     * \brief Get/set de la premiere position
     * \return _firstPos
     */
    std::string getFirstPos() const { return _firstPos; }

    /**
     * \fn void setFirstPos(const std::string& firstPos)
     * \brief Set de la premiere position
     */
    void setFirstPos(const std::string& firstPos) { _firstPos = firstPos; }

    /**
     * \fn  std::string getSecondPos()
     * \brief Get de la seconde position
     * \return _secondPos
     */
    std::string getSecondPos() const { return _secondPos; }

    /**
     *  \fn    void setSecondPos(const std::string& secondPos)
     * \brief Set de la seconde position
     */
    void setSecondPos(const std::string& secondPos) { _secondPos = secondPos; }

    /**
     * \fn std::string getValue()
     * \brief Get de la valeur
     * \return _value
     */
    std::string getValue() const { return _value; }

    /**
     *  \fn   void setValue(const std::string& value)
     * \brief Set de la valeur
     */
    void setValue(const std::string& value) { _value = value; }

protected:
    std::string _firstPos;
    std::string _secondPos;
    std::string _value;
};

#endif // __TY_PANEL__
