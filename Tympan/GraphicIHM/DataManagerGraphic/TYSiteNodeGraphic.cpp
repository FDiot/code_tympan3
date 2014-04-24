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
 * \file TYSiteNodeGraphic.cpp
 * \brief Repri¿½sentation graphique d'un ensemble de sites
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHGraphic.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/Site/TYSiteNode.h"

TYSiteNodeGraphic::TYSiteNodeGraphic(TYSiteNode* pElement) : TYElementGraphic(pElement)
{
    _texture = 0;
    _imgDataPtr = NULL;
    _imgXSize = 0;
    _imgYSize = 0;
    _imgBytesPerPixel = 0;
    _visible = true;
}

TYSiteNodeGraphic::~TYSiteNodeGraphic()
{
    if (_imgDataPtr != NULL)
    {
        delete [] _imgDataPtr;
        _imgDataPtr = NULL;
    }
}


void TYSiteNodeGraphic::update(bool force /*=false*/)
{
    TYElementGraphic::update(force);
}

void TYSiteNodeGraphic::getChilds(TYListPtrTYElementGraphic& childs, bool recursif /*=true*/)
{
    TYElementGraphic* pTYElementGraphic ;

    pTYElementGraphic = getElement()->getTopographie()->getGraphicObject();
    childs.push_back(pTYElementGraphic);
    if (recursif)
    {
        pTYElementGraphic->getChilds(childs, recursif);
    }

    pTYElementGraphic = getElement()->getInfrastructure()->getGraphicObject();
    childs.push_back(pTYElementGraphic);
    if (recursif)
    {
        pTYElementGraphic->getChilds(childs, recursif);
    }

    // Sites childs
    TYSiteNode* pTYSiteNode = getElement();
    TYTabSiteNodeGeoNode& TabSite = pTYSiteNode->getListSiteNode();
    for (unsigned int i = 0; i < TabSite.size(); i++)
    {
        pTYElementGraphic = TabSite.at(i)->getGraphicObject();
        childs.push_back(pTYElementGraphic);
        if (recursif)
        {
            pTYElementGraphic->getChilds(childs, recursif);
        }
    }
}

void TYSiteNodeGraphic::display(GLenum mode /*=GL_RENDER*/)
{
    if (!getElement()->isInCurrentCalcul())
    {
        return;
    }

    TYElementGraphic::display(mode);

    if ((getElement()->getUseTopoFile()) && (_visible))
    {
        TYSiteNode* pSite = getElement();
        std::string TopoFile = pSite->getTopoFileName();
        const char* sTopoFile = TopoFile.data();
        if ((_sOldTopoFile != QString(sTopoFile)) || (_imgDataPtr == NULL))
        {
            _sOldTopoFile = QString(sTopoFile);
            if (_imgDataPtr != NULL)
            {
                delete [] _imgDataPtr;
                _imgDataPtr = NULL;
            }
            QImage img(sTopoFile);
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

        if (_imgDataPtr != NULL)
        {
            glColor3f(1.0, 1.0, 1.0);
            if (0 == _texture) { glGenTextures(1, &_texture); } //az++
            glBindTexture(GL_TEXTURE_2D, _texture);

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

            TYPoint pt = getElement()->getPosition();

            // Tentative d'ajustement de la position de l'image de fond
            TYElement* pParent = getElement()->getParent();
            if (pParent && pParent->inherits("TYSiteNode")) // Si il a un parent
            {
                TYSiteNode* pSite = static_cast<TYSiteNode*>(pParent);
                int posParent = pSite->getPosition()._z;
                pt._z = posParent + 1;
            }
            else
            {
                pt._z = 0;
            }

            getElement()->setPosition(pt);
            // FIn de tentative de gestion des superpositions d'image

            double angle = 0.0;
            OVector3D vecOrientation = getElement()->getOrientation().toVector3D();

            // On verifie que le vecteur orientation n'est pas nul
            if (vecOrientation.norme() != 0)
            {
                // Calcul de l'angle
                angle = SIGNE(vecOrientation._y) * acos(vecOrientation._x / vecOrientation.norme());
                angle = (M_PI / 2.0) - angle;
            }

            float x = _imgXSize / 2 * getElement()->getEchelle();
            float y = _imgYSize / 2 * getElement()->getEchelle();

            glEnable(GL_TEXTURE_2D);

            glBindTexture(GL_TEXTURE_2D, _texture);
            glPushMatrix();
            glRotatef(angle * 180 / M_PI, 0.0, 0.0, 1.0);
            glBegin(GL_QUADS);

            glTexCoord2f(0.0, 0.0);
            glVertex3f(pt._x - x, pt._y - y, pt._z);

            glTexCoord2f(0.0, 1.0);
            glVertex3f(pt._x - x, pt._y + y, pt._z);

            glTexCoord2f(1.0, 1.0);
            glVertex3f(pt._x + x, pt._y + y, pt._z);

            glTexCoord2f(1.0, 0.0);
            glVertex3f(pt._x + x, pt._y - y, pt._z);

            glEnd();
            glPopMatrix();

            glDisable(GL_TEXTURE_2D);

            ((TYTopographieGraphic*)((TYElementGraphic*)getElement()->getTopographie()->getGraphicObject()))->setBackgroundImage(_sOldTopoFile, x, y, pt, vecOrientation);
        }
    }

    if (!getElement()->getUseTopoFile())
    {
        ((TYTopographieGraphic*)((TYElementGraphic*)getElement()->getTopographie()->getGraphicObject()))->unsetBackgroundImage();
    }

    getElement()->getTopographie()->getGraphicObject()->display(mode);
    getElement()->getInfrastructure()->getGraphicObject()->display(mode);
    // Sites childs
    TYSiteNode* pTYSiteNode = getElement();//az++

    glTranslatef(0.0f, 0.0f, 1.0f);
    TYTabSiteNodeGeoNode& TabSite = pTYSiteNode->getListSiteNode();//az++
    for (unsigned int i = 0; i < TabSite.size(); i++)
    {
        //az++
        // Le site n'est affichi¿½ que s'il est dans le calcul courant
        if (TabSite.at(i)->getElement()->isInCurrentCalcul())
        {
            TabSite.at(i)->getGraphicObject()->display(mode);   //az++
        }
    }//az++

    glTranslatef(0.0f, 0.0f, -1.0f);
}

#if TY_USE_IHM
void TYSiteNodeGraphic::connectUpdateSignal(QObject* pReceiver, const char* member)
{
    //  _pUpdateSignal->connect(pReceiver, member);
}
#endif // TY_USE_IHM

#if TY_USE_IHM
void TYSiteNodeGraphic::disconnectUpdateSignal(QObject* pReceiver, const char* member)
{
    //  _pUpdateSignal->disconnect(pReceiver, member);
}
#endif // TY_USE_IHM

