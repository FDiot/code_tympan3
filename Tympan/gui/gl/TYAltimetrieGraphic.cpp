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
 * \file TYAltimetrieGraphic.cpp
 * \brief Representation graphique de l'altimetrie
 *
 *
 */



#ifdef _MSC_VER
#pragma warning( disable : 4284 )
#endif


#include "Tympan/core/color.h"
#include "Tympan/models/business/OLocalizator.h"

#include "Tympan/models/business/topography/TYAltimetrie.h"

#include "Tympan/gui/gl/TYPickingTable.h"
#include "Tympan/models/business/TYPreferenceManager.h"

#include "TYAltimetrieGraphic.h"

#define IMG(id) OLocalizator::getPicture("TYAltimetrieGraphic", (id))

TYAltimetrieGraphic::TYAltimetrieGraphic(TYAltimetrie* pElement) :
    TYElementGraphic(pElement)
{
    float hueRange[2] = {0.4f, 0.15f};
    float saturationRange[2] = {1.0f, 0.8f};
    float valueRange[2] = {0.5f, 0.25f};
    TYColorManager::getLinearColorTable(256, hueRange, saturationRange, valueRange, _oColorMap);
    _pTex = new OGLTexture2D();
    _isBgImage = false;
    _theTexture = 0;
    _angle = 0.0;
    _sTopoFileName = _sOldTopoFileName = "";
    _imgDataPtr = NULL;
    _imgXSize = 0;
    _imgYSize = 0;
    _imgBytesPerPixel = 0;
    _visible = false;
}

TYAltimetrieGraphic::~TYAltimetrieGraphic()
{
    if (_imgDataPtr != NULL)
    {
        delete [] _imgDataPtr;
        _imgDataPtr = NULL;
    }
}

void TYAltimetrieGraphic::update(bool force /*=false*/)
{
    if (getModified() || force)
    {
        float hsvMinValue[3] = { 80.0f, 255.0f, 255.0f };
        float hsvMaxValue[3] = { 120.0f, 255.0f, 112.0f };

#if TY_USE_IHM
        // Couleurs
        if ((TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "AltiGraphicColorMinR")) &&
            (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "AltiGraphicColorMaxR")))
        {
            TYPreferenceManager::getColor(TYDIRPREFERENCEMANAGER, "AltiGraphicColorMin", hsvMinValue[0], hsvMinValue[1], hsvMinValue[2]);
            TYPreferenceManager::getColor(TYDIRPREFERENCEMANAGER, "AltiGraphicColorMax", hsvMaxValue[0], hsvMaxValue[1], hsvMaxValue[2]);
        }
        else
        {
            TYPreferenceManager::setColor(TYDIRPREFERENCEMANAGER, "AltiGraphicColorMin", hsvMinValue[0], hsvMinValue[1], hsvMinValue[2]);
            TYPreferenceManager::setColor(TYDIRPREFERENCEMANAGER, "AltiGraphicColorMax", hsvMaxValue[0], hsvMaxValue[1], hsvMaxValue[2]);
        }
#endif

        computeBoundingBox();
    }

    TYElementGraphic::update(force);
}

void TYAltimetrieGraphic::getChilds(TYListPtrTYElementGraphic& childs, bool recursif /*=true*/)
{
    for (int i = 0; i < getElement()->getListFaces().size(); i++)
    {
        TYElementGraphic* pTYElementGraphic = getElement()->getFace(i)->getGraphicObject();
        childs.push_back(pTYElementGraphic);
        if (recursif)
        {
            pTYElementGraphic->getChilds(childs, recursif);
        }
    }
}

void TYAltimetrieGraphic::computeBoundingBox()
{
    OBox reset;
    _boundingBox = reset;

    for (int i = 0; i < getElement()->getListFaces().size(); i++)
    {
        _boundingBox.Enlarge((float)(getElement()->getFace(i)->getPoints()[0]._x), (float)(getElement()->getFace(i)->getPoints()[0]._y), (float)(getElement()->getFace(i)->getPoints()[0]._z));
        _boundingBox.Enlarge((float)(getElement()->getFace(i)->getPoints()[1]._x), (float)(getElement()->getFace(i)->getPoints()[1]._y), (float)(getElement()->getFace(i)->getPoints()[1]._z));
        _boundingBox.Enlarge((float)(getElement()->getFace(i)->getPoints()[2]._x), (float)(getElement()->getFace(i)->getPoints()[2]._y), (float)(getElement()->getFace(i)->getPoints()[2]._z));
    }
}

void TYAltimetrieGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    if (getElement() == NULL) { return; }

    if (_visible)
    {
        update(true);

        if (mode == GL_SELECT)
        {
            TYPickingTable::addElement(getElement());
            glPushName((GLuint)(TYPickingTable::getIndex()));
        }

        double scalar;//az++
        double zmin, zmax, zrange;
        zmin = _boundingBox._min._z;
        zmax = _boundingBox._max._z;
        zrange = zmax - zmin;
        double color0[3], color1[3], color2[3];

        bool toTex = false;
        //if(getElement()->getSol()->getVegetActive())
        //{
        //  toTex = true;
        //}
        bindTexture();

        if (toTex)
        {
            glEnable(GL_TEXTURE_2D);
        }

        for (int i = 0; i < getElement()->getListFaces().size(); i++)
        {
            if (zrange > 0)
            {
                scalar = (getElement()->getFace(i)->getPoints()[0]._z - zmin) / zrange;
                OColor oColor0 = _oColorMap[scalar * 255];
                color0[0] = oColor0.r;
                color0[1] = oColor0.g;
                color0[2] = oColor0.b;

                scalar = (getElement()->getFace(i)->getPoints()[1]._z - zmin) / zrange;
                OColor oColor1 = _oColorMap[scalar * 255];
                color1[0] = oColor1.r;
                color1[1] = oColor1.g;
                color1[2] = oColor1.b;

                scalar = (getElement()->getFace(i)->getPoints()[2]._z - zmin) / zrange;
                OColor oColor2 = _oColorMap[scalar * 255];
                color2[0] = oColor2.r;
                color2[1] = oColor2.g;
                color2[2] = oColor2.b;
            }
            else
            {
                scalar = (getElement()->getFace(i)->getPoints()[0]._z - zmin);
                OColor oColor0 = _oColorMap[scalar * 255];
                color0[0] = oColor0.r;
                color0[1] = oColor0.g;
                color0[2] = oColor0.b;

                scalar = (getElement()->getFace(i)->getPoints()[1]._z - zmin);
                OColor oColor1 = _oColorMap[scalar * 255];
                color1[0] = oColor1.r;
                color1[1] = oColor1.g;
                color1[2] = oColor1.b;

                scalar = (getElement()->getFace(i)->getPoints()[2]._z - zmin);
                OColor oColor2 = _oColorMap[scalar * 255];
                color2[0] = oColor2.r;
                color2[1] = oColor2.g;
                color2[2] = oColor2.b;
            }

            TYElementGraphic* pTYElementGraphic = getElement()->getFace(i)->getGraphicObject();
            LPTYPolygonGraphic pPolygonGraphic = (TYPolygonGraphic*)pTYElementGraphic;
            if (pPolygonGraphic != NULL)
            {
                pPolygonGraphic->unsetTextureBg();
                pPolygonGraphic->setTexture(toTex);
                if (_isBgImage)
                {
                    glColor4fv(OColor::WHITE);
                    pPolygonGraphic->setTextureBg(_semiX, _semiY, _imagePosition, _bgImageOrientation);
                }
                pPolygonGraphic->setAltimetrieColor(color0, color1, color2);
                pPolygonGraphic->display(mode);
            }
        }

        if (toTex || _isBgImage)
        {
            glDisable(GL_TEXTURE_2D);
        }

        if (mode == GL_SELECT)
        {
            glPopName();
        }
    }
}

void TYAltimetrieGraphic::bindTexture()
{
    // Si le sol est de type vegetation...
    //if(getElement()->getSol()->getVegetActive())
    //{
    //  double r = getElement()->getSol()->getResistivite();

    //  // On charge la texture qui va bien.
    //  if(r>=5.0 && r<20.0)
    //      _pTex->load(IMG("id_terrain_0").toAscii().data());
    //  else if(r>=20.0 && r<50.0)
    //      _pTex->load(IMG("id_terrain_1").toAscii().data());
    //  else if(r>=50.0 && r<100.0)
    //      _pTex->load(IMG("id_terrain_2").toAscii().data());
    //  else if(r>=100.0 && r<200.0)
    //      _pTex->load(IMG("id_terrain_3").toAscii().data());
    //  else if(r>=200.0 && r<500.0)
    //      _pTex->load(IMG("id_terrain_4").toAscii().data());
    //  else if(r>=500.0 && r<1000.0)
    //      _pTex->load(IMG("id_terrain_5").toAscii().data());
    //  else if(r>=1000.0 && r<5000.0)
    //      _pTex->load(IMG("id_terrain_6").toAscii().data());
    //  else if(r>=5000.0 && r<10000.0)
    //      _pTex->load(IMG("id_terrain_7").toAscii().data());
    //  else if(r>=10000.0 && r<20000.0)
    //      _pTex->load(IMG("id_terrain_8").toAscii().data());
    //  else if(r>=20000.0)
    //      _pTex->load(IMG("id_terrain_9").toAscii().data());

    //  // On met la couleur en blanc pour etre sur qu'il n'y a pas de melange de couleur.
    //  glColor4fv(OColor::WHITE);

    //  // On applique la texture.
    //  _pTex->bind();
    //}
    //else
    if (_isBgImage)
    {
        if ((_sOldTopoFileName != _sTopoFileName) || (_imgDataPtr == NULL))
        {
            _sOldTopoFileName = _sTopoFileName;
            if (_imgDataPtr != NULL)
            {
                delete [] _imgDataPtr;
                _imgDataPtr = NULL;
            }
            QImage img(_sOldTopoFileName);
            if (!img.isNull())
            {
                QImage glImg = QGLWidget::convertToGLFormat(img);

                _imgXSize = glImg.width();
                _imgYSize = glImg.height();

                int tabSize = glImg.bytesPerLine() * glImg.height();
                _imgDataPtr = new unsigned char[tabSize];
                memcpy(_imgDataPtr, glImg.bits(), tabSize);
                _imgBytesPerPixel = glImg.bytesPerLine() / glImg.width();
            }
        }

        glColor3f(1.0, 1.0, 1.0);

        if (0 == _theTexture) { glGenTextures(1, &_theTexture); } //az++
        glBindTexture(GL_TEXTURE_2D, _theTexture);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);

        GLenum format = GL_LUMINANCE;
        int internalFormat = _imgBytesPerPixel;
        switch (_imgBytesPerPixel)
        {
            case 1: format = GL_LUMINANCE; break;
            case 2: format = GL_LUMINANCE_ALPHA; break;
            case 3: format = GL_RGB; break;
            case 4: format = GL_RGBA; break;
        }

        glTexImage2D(GL_TEXTURE_2D, 0 , internalFormat,
                     _imgXSize, _imgYSize, 0, format,
                     GL_UNSIGNED_BYTE, (const GLvoid*)_imgDataPtr);

        // don't accept fragments if they have zero opacity. this will stop the
        // zbuffer from be blocked by totally transparent texture fragments.
        glAlphaFunc(GL_GREATER, (GLclampf) 0);
        glEnable(GL_ALPHA_TEST);

        _angle = 0.0;

        // On verifie que le vecteur orientation n'est pas nul
        if (_bgImageOrientation.norme() != 0)
        {
            // Calcul de l'angle
            _angle = SIGNE(_bgImageOrientation._y) * acos(_bgImageOrientation._x / _bgImageOrientation.norme());
            _angle = (M_PI / 2.0) - _angle;
        }

        glEnable(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, _theTexture);
    }
}

void TYAltimetrieGraphic::setBackgroundImage(QString sTopoFileName, int semiX, int semiY, TYPoint ptPosition, OVector3D bgImageOrientation)
{
    _isBgImage = true;
    _semiX = semiX;
    _semiY = semiY;
    _imagePosition = ptPosition;
    _bgImageOrientation = bgImageOrientation;
    _sTopoFileName = sTopoFileName;
}

void TYAltimetrieGraphic::unsetBackgroundImage()
{
    _isBgImage = false;
    _semiX = 0;
    _semiY = 0;
    _imagePosition = TYPoint();
    _bgImageOrientation = OVector3D();
    _sTopoFileName = QString();
    if (_imgDataPtr != NULL)
    {
        delete [] _imgDataPtr;
        _imgDataPtr = NULL;
    }
}
