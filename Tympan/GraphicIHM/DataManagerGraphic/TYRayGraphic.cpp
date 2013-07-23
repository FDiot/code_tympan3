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
#include "TYPHGraphic.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYRay.h"

bool TYRayGraphic::_gVisible = false;

TYRayGraphic::TYRayGraphic(TYRay* pElement) :
    TYElementGraphic(pElement)
{
}

TYRayGraphic::~TYRayGraphic()
{
}

void TYRayGraphic::update(bool force /*=false*/)
{
    TYElementGraphic::update(force);
}

void TYRayGraphic::computeBoundingBox()
{
    OBox reset;
    _boundingBox = reset;
}

void TYRayGraphic::getRayEventColor(TYRayEvent& e, float& r, float& g, float& b)
{
    switch (e.type)
    {
        case TYSOURCE:
            r = 182; g = 102; b = 210; // violet; r=255; g=255; b=0; // jaune;
            break;
        case TYDIFFRACTION:
            r = 49; g = 140; b = 231; // bleu; r=0; g=0; b=255; // bleu;
            /*if (_lastRayEventType == TYDIFFRACTION)
            {
                if (_repeatRayEventCount==0)
                {
                    r=1; g=215; b=88; // vert ;r=0; g=255; b=0; // vert
                }
                _repeatRayEventCount = (_repeatRayEventCount+1)%2;
            }
            else
            {
                _repeatRayEventCount = 0;
            }*/
            break;
        case TYREFLEXION:
            r = 255; g = 0; b = 0; // rouge;
            /*if (_lastRayEventType == TYREFLEXION)
            {
                if (_repeatRayEventCount==0)
                {
                    r=255; g=128; b=0; // orange
                }
                _repeatRayEventCount = (_repeatRayEventCount+1)%2;
            }
            else
            {
                _repeatRayEventCount = 0;
            }*/
            break;
        case TYREFRACTION :
            r = 0, g = 0, b = 255; // bleu
            break;
        case TYRECEPTEUR:
            r = 255; g = 110; b = 180; //rose r=255; g=255; b=0; // jaune;
            break;
    }

}

void TYRayGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    LPTYRay pRay = getElement();
    TYPoint pt1, pt2;
    TYPoint diff;

    /*if (_highlight)
    {
        if(_bFirstDisp)
        {
            computeBoundingBox();
            _bFirstDisp = false;
        }
        drawLineBoundingBox();
        if(mode == GL_COMPILE) drawName();
    }*/

    if (_visible)
    {
        // Dessine les evenements du rayon
        glLineWidth(2.0);
        glBegin(GL_LINE_STRIP);

        _lastRayEventType = 0;
        _repeatRayEventCount = 0;
        for (unsigned int j = 0; j < pRay->getEvents().size(); j++)
        {
            float r, g, b;
            TYRayEvent e = pRay->getEvents().at(j);

            if (j != 0)
            {
                // 2nd point du segment
                TYPoint pt(e.pos);
                glVertex3f(pt._x, pt._y, pt._z);
            }

            getRayEventColor(e, r, g, b);
            glColor3f(r, g, b);
            _lastRayEventType = e.type;
            _lastRayEventPos = e.pos;

            // 1er point du segment
            TYPoint pt(e.pos);
            glVertex3f(pt._x, pt._y, pt._z);
        }

        glEnd();

        //Calcul du volume englobant pour le fit:
        //_globalBoundingBox.Enlarge(_boundingBox);
    }
}

