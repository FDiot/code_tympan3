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

static int FindPowerOfTwo(int i)
{
    int size;

    for (i--, size = 1; i > 0; size *= 2)
    {
        i /= 2;
    }

    // [these lines added by Tim Hutton (implementing Joris Vanden Wyngaerd's suggestions)]
    // limit the size of the texture to the maximum allowed by OpenGL
    // (slightly more graceful than texture failing but not ideal)
    GLint maxDimGL;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxDimGL);
    if (size > maxDimGL)
    {
        size = maxDimGL ;
    }
    // end of Tim's additions

    return size;
}

// Creates resampled unsigned char texture map that is a power of two in bith x and y.
static unsigned char* ResampleToPowerOfTwo(int& xs, int& ys, unsigned char* dptr, int bpp)
{
    unsigned char* tptr, *p, *p1, *p2, *p3, *p4;
    int xsize, ysize, i, j, k, jOffset, iIdx, jIdx;
    float pcoords[3], hx, hy, rm, sm, w0, w1, w2, w3;

    xsize = FindPowerOfTwo(xs);
    ysize = FindPowerOfTwo(ys);

    hx = (float)(xs - 1.0) / (xsize - 1.0);
    hy = (float)(ys - 1.0) / (ysize - 1.0);

    tptr = p = new unsigned char[xsize * ysize * bpp];

    //Resample from the previous image. Compute parametric coordinates and interpolate
    for (j = 0; j < ysize; j++)
    {
        pcoords[1] = j * hy;

        jIdx = (int)pcoords[1];
        if (jIdx >= (ys - 1)) //make sure to interpolate correctly at edge
        {
            jIdx = ys - 2;
            pcoords[1] = 1.0;
        }
        else
        {
            pcoords[1] = pcoords[1] - jIdx;
        }
        jOffset = jIdx * xs;
        sm = 1.0 - pcoords[1];

        for (i = 0; i < xsize; i++)
        {
            pcoords[0] = i * hx;
            iIdx = (int)pcoords[0];
            if (iIdx >= (xs - 1))
            {
                iIdx = xs - 2;
                pcoords[0] = 1.0;
            }
            else
            {
                pcoords[0] = pcoords[0] - iIdx;
            }
            rm = 1.0 - pcoords[0];

            // Get pointers to 4 surrounding pixels
            p1 = dptr + bpp * (iIdx + jOffset);
            p2 = p1 + bpp;
            p3 = p1 + bpp * xs;
            p4 = p3 + bpp;

            // Compute interpolation weights interpolate components
            w0 = rm * sm;
            w1 = pcoords[0] * sm;
            w2 = rm * pcoords[1];
            w3 = pcoords[0] * pcoords[1];
            for (k = 0; k < bpp; k++)
            {
                *p++ = (unsigned char)(p1[k] * w0 + p2[k] * w1 + p3[k] * w2 + p4[k] * w3);
            }
        }
    }

    xs = xsize;
    ys = ysize;

    return tptr;
}


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

    // vieaw of altimetry for the current site (i.e. main site on the display)
	TYSiteNode* pSiteNode = getElement();
	if (pSiteNode->getRoot())
	{
		static_cast<TYTopographieGraphic*>(pSiteNode->getTopographie()->getGraphicObject().getRealPointer())->showAlti(true);
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
			TYSiteNode* pSubSite = TYSiteNode::safeDownCast(TabSite.at(i)->getElement());
			static_cast<TYTopographieGraphic*>(pSubSite->getTopographie()->getGraphicObject().getRealPointer())->showAlti(false);
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

