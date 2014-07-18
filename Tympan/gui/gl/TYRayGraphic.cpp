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




#include "Tympan/models/business/TYRay.h"

#include "TYRayGraphic.h"

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

void TYRayGraphic::getRayEventColor(acoustic_event& e)
{
    switch (e.type)
    {
        case TYSOURCE:
            _r = 1.0f; _g = 1.0f; _b = 0.0f; // jaune;
            break;

        case TYDIFFRACTION:
            _r = 0.2f; _g = 0.55f; _b = 0.91f; // bleu
            break;

        case TYREFLEXION:
            _r = 1.0f; _g = 0.0f; _b = 0.0f; // rouge
            break;

        case TYREFLEXIONSOL:
            _r = 1.0f; _g = 0.5f; _b = 0.0f; // orange

        case TYREFRACTION :
            // Do nothing
            //            _r=0.0f; _g=0.84f; _b=0.35f; // vert
            break;

        case TYRECEPTEUR:
            _r = 1.0f; _g = 0.43f; _b = 0.71f; //rose
            break;
    }
}

void TYRayGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    TYRay* pRay = getElement();

    if (_visible)
    {
        // Dessine les evenements du rayon
        glLineWidth(2.0);
        glBegin(GL_LINE_STRIP);

        _lastRayEventType = 0;
        _repeatRayEventCount = 0;
        for (unsigned int j = 0; j < pRay->getEvents().size(); j++)
        {
            acoustic_event* e = pRay->getEvents().at(j);

            if (j != 0)
            {
                // 2nd point du segment
                OPoint3D pt(e->pos);
                glVertex3f(pt._x, pt._y, pt._z);
            }

            getRayEventColor(*e);
            glColor3f(_r, _g, _b);
            _lastRayEventType = e->type;
            _lastRayEventPos = e->pos;

            // 1er point du segment
            OPoint3D pt(e->pos);
            glVertex3f(pt._x, pt._y, pt._z);
        }

        glEnd();
    }
}

