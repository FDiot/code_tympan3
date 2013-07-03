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


#ifndef __OGL_ELEMENT
#define __OGL_ELEMENT

/**
 * Classe de liste d'objets OGL a afficher
 *
 * @author Projet_Tympan.
 */
class OGLElement
{
public:
    /**
     * Constructeur.
     */
    OGLElement() {m_DisplayPositionX = m_DisplayPositionY = 0; m_bVisible = false;};

    /**
     * Destructeur.
     */
    virtual ~OGLElement() {};

    void setDisplayPosition(double displayPositionX, double displayPositionY) {m_DisplayPositionX = displayPositionX; m_DisplayPositionY = displayPositionY; };
    void setVisibility(bool bVisible) {m_bVisible = bVisible;};
    bool getVisibility() {return m_bVisible;};
    void setIs3D(bool bIs3D) {m_bIs3D = bIs3D;};
    bool getIs3D() {return m_bIs3D;};

    virtual int render() {return 0;};

protected:
    double m_DisplayPositionX;
    double m_DisplayPositionY;
    bool m_bVisible;
    bool m_bIs3D;
};

#endif //__OGL_ELEMENT
