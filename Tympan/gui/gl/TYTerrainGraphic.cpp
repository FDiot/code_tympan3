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
 * \file TYTerrainGraphic.cpp
 * \brief Representation graphique d'un terrain
 */


#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/topography/TYTerrain.h"
#include "Tympan/gui/gl/TYPickingTable.h"
#include "TYTerrainGraphic.h"

#define IMG(id) OLocalizator::getPicture("TYTerrainGraphic", (id))


TYTerrainGraphic::TYTerrainGraphic(TYTerrain* pElement) :
    TYElementGraphic(pElement)
{
    _pPolygon = new TYPolygon();

    _pPolyLineGraphic = new TYPolyLineGraphic(true);
    _pPolyLineGraphic->setElement(pElement);

    _pTex = new OGLTexture2D();
}

void TYTerrainGraphic::update(bool force /*=false*/)
{
    TYElementGraphic::update(force);
}

void TYTerrainGraphic::getChilds(TYListPtrTYElementGraphic& childs, bool recursif /*=true*/)
{
    TYElementGraphic* pTYElementGraphic = _pPolygon->getGraphicObject();
    childs.push_back(pTYElementGraphic);
    if (recursif)
    {
        pTYElementGraphic->getChilds(childs, recursif);
    }

    pTYElementGraphic = _pPolyLineGraphic;
    childs.push_back(pTYElementGraphic);
    if (recursif)
    {
        pTYElementGraphic->getChilds(childs, recursif);
    }
}

void TYTerrainGraphic::computeBoundingBox()
{
    OBox reset;
    _boundingBox = reset;
    _pPolygon->getGraphicObject()->computeBoundingBox();
    _pPolyLineGraphic->computeBoundingBox();
    _boundingBox.Enlarge(_pPolygon->getGraphicObject()->GetBox());
    _boundingBox.Enlarge(_pPolyLineGraphic->GetBox());
}

void TYTerrainGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    if (!_visible) { return; }

    TYElementGraphic::display(mode);

    TYTabPoint tabpoints = getElement()->getListPoints();

    // Ajout d'un offset pour eviter le flickering avec l'altimetrie
    for (register unsigned int i = 0; i < tabpoints.size(); i++)
    {
        tabpoints[i]._z += 0.05;
    }

    _pPolygon->setPoints(tabpoints);

    _pPolyLineGraphic->setModified(true);
    _pPolyLineGraphic->setTabPoint(tabpoints);

    if (mode == GL_SELECT)
    {
        TYPickingTable::addElement(getElement());
        glPushName((GLuint)(TYPickingTable::getIndex()));
    }

    _pPolyLineGraphic->highlight(_highlight);
    _pPolygon->getGraphicObject()->highlight(_highlight);

    glColor4fv(getElement()->getColor());
    _pPolyLineGraphic->display(mode);

    glColor4fv(getElement()->getColor());
    bindTexture();
    _pPolygon->getGraphicObject()->display(mode);

    if (mode == GL_SELECT)
    {
        glPopName();
    }
}

void TYTerrainGraphic::bindTexture()
{
    // Desactive le texturing.
    _pPolygon->getGraphicObject()->setTexture(false);

    // Si le sol est de type vegetation...
    if (getElement()->getSol()->getVegetActive())
    {
        // On active le texturing.
        _pPolygon->getGraphicObject()->setTexture();

        double r = getElement()->getSol()->getResistivite();

        // On charge la texture qui va bien.
        if (r >= 5.0 && r < 20.0)
        {
            _pTex->load(IMG("id_terrain_0").toAscii().data());
        }
        else if (r >= 20.0 && r < 50.0)
        {
            _pTex->load(IMG("id_terrain_1").toAscii().data());
        }
        else if (r >= 50.0 && r < 100.0)
        {
            _pTex->load(IMG("id_terrain_2").toAscii().data());
        }
        else if (r >= 100.0 && r < 200.0)
        {
            _pTex->load(IMG("id_terrain_3").toAscii().data());
        }
        else if (r >= 200.0 && r < 500.0)
        {
            _pTex->load(IMG("id_terrain_4").toAscii().data());
        }
        else if (r >= 500.0 && r < 1000.0)
        {
            _pTex->load(IMG("id_terrain_5").toAscii().data());
        }
        else if (r >= 1000.0 && r < 5000.0)
        {
            _pTex->load(IMG("id_terrain_6").toAscii().data());
        }
        else if (r >= 5000.0 && r < 10000.0)
        {
            _pTex->load(IMG("id_terrain_7").toAscii().data());
        }
        else if (r >= 10000.0 && r < 20000.0)
        {
            _pTex->load(IMG("id_terrain_8").toAscii().data());
        }
        else if (r >= 20000.0)
        {
            _pTex->load(IMG("id_terrain_9").toAscii().data());
        }

        // On met la couleur en blanc pour etre sur qu'il n'y a pas de melange de couleur.
        glColor4fv(OColor::WHITE);

        // On applique la texture.
        _pTex->bind();
    }
}
