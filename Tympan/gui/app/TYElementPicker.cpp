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
 * \file TYElementPicker.cpp
 * \brief gestion des elements selectionnes par picking
 */


#include "Tympan/models/business/TYElement.h"
#include "Tympan/models/business/TYRectangularMaillage.h"
#include "Tympan/models/business/TYLinearMaillage.h"
#include "Tympan/gui/gl/TYPickHandler.h"
#include "Tympan/gui/widgets/TYWidget.h"
#include "Tympan/gui/gl/TYPickingTable.h"
#include "Tympan/gui/app/TYApplication.h"
#include "Tympan/gui/app/TYModelerFrame.h"
#include "TYElementPicker.h"

#if defined(WIN32)
  #include <crtdbg.h>
  #ifdef _DEBUG
    #undef THIS_FILE
    static char THIS_FILE[] = __FILE__;
    #define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
  #endif
#endif

///La tolerance pour le picking.
static const float ElementPickerTolerance = 0.01f;

///La tolerance pour la recherche de point de calcul dans un maillage.
static const float MaillagePickingTolerance = 1.0f;

///Taille du buffer pour le picking
#define BUFSIZE 512


TYElementPicker::TYElementPicker(TYModelerFrame* pModeler)
{
    Q_CHECK_PTR(pModeler);
    _pModeler = pModeler;

    _pLastPickedElt = NULL;
    _pHighlightedGraphicObject = NULL;
    //_pOutlineActor = NULL;//az--: plus utilise

    _x = 0;
    _y = 0;
}

TYElementPicker::~TYElementPicker()
{
}

bool TYElementPicker::pick(int x, int y)
{
    bool res = true;

    _x = x;
    _y = y;

    GLuint selectBuf[BUFSIZE], names, *ptr;
    GLint hits;

    glSelectBuffer(BUFSIZE, selectBuf);
    (void) glRenderMode(GL_SELECT);

    glInitNames();

    _pModeler->getView()->getRenderer()->OpenGLRender(GL_SELECT, x, y);

    hits = glRenderMode(GL_RENDER);

    _pLastPickedElt = NULL;
    int i, j;
    float z1, z2;

    ptr = (GLuint*) selectBuf;

    TYElement* pElementAutreQueAltimetrie = NULL;//az++
    float zmin = (float)0xFFFFFFFF; //az++
    float zElementAutreQueAltimetrie = -1;

    for (i = 0; i < hits; i++) //Pour chaque primitive OpenGL
    {
        // nombre de noms
        names = *ptr; ptr++;//Recuperer la liste de nom correspondant a ce hit
        // z1 : distance minimale, toujours positive
        z1 = *ptr; ptr++;
        // z2 : distance maximale, toujours positive
        z2 = *ptr; ptr++;

        TYElement* pElementCourant = NULL;//az++
        QString sCurrentClassName;
        for (j = 0; j < (int)names; j++)
        {
            pElementCourant = TYPickingTable::getElement(*ptr);
            sCurrentClassName = QString(pElementCourant->getClassName());
            if (sCurrentClassName != "TYAltimetrie")
            {
                if (NULL == pElementAutreQueAltimetrie)
                {
                    pElementAutreQueAltimetrie = pElementCourant;
                    zElementAutreQueAltimetrie = z1;
                }
                else
                {
                    if (z1 < zElementAutreQueAltimetrie)
                    {
                        pElementAutreQueAltimetrie = pElementCourant;
                        zElementAutreQueAltimetrie = z1;
                    }
                    else if (z1 == zElementAutreQueAltimetrie)
                    {
                        //Cas des cheminees et autres bouches sur Face, voir des fenetres sur les murs
                        if (pElementAutreQueAltimetrie == pElementCourant->getParent())
                        {
                            pElementAutreQueAltimetrie = pElementCourant;
                            zElementAutreQueAltimetrie = z1;
                        }
                    }
                }
            }
            else
            {
                if (z1 < zmin) //az++
                {
                    //on s'interresse aux z les plus proches
                    zmin = z1;
                    _pLastPickedElt = TYPickingTable::getElement(*ptr);
                }
            }
            ptr++;
        }
    }
    if (pElementAutreQueAltimetrie)
    {
        _pLastPickedElt = pElementAutreQueAltimetrie;
    }

    TYPickingTable::purgeElements();

    if (_pLastPickedElt)
    {
        writeDebugMsg(QString("Element picke de type : ") + QString(_pLastPickedElt->getClassName()));

        // Qq chose de picker
        emit(elementPicked(_pLastPickedElt));

        // Recherche des parents
        updateParents(_pLastPickedElt);
    }
    else
    {
        res = false;
    }

    return res;
}

void TYElementPicker::highlightElement(TYElement* pElt)
{
    bool change = false;

    if (_pHighlightedGraphicObject)
    {
        _pHighlightedGraphicObject->highlight(false);
        _pHighlightedGraphicObject = NULL;
    }

    if (!pElt)
    {
        return;
    }

    if (_pHighlightedGraphicObject != pElt->getGraphicObject())
    {
        change = true;
    }

    if (!pElt->isA("TYPoint"))
    {
        // On conserve un pointeur sur l'objet graphique
        _pHighlightedGraphicObject = pElt->getGraphicObject();
        _pHighlightedGraphicObject->highlight();//az++
    }

    if (change) { _pModeler->getView()->getRenderer()->updateDisplayList(); }//az++ test

    _pModeler->updateView(false, false);
}

void TYElementPicker::reset()
{
    if (_pHighlightedGraphicObject)
    {
        _pHighlightedGraphicObject->highlight(false);
        _pHighlightedGraphicObject = NULL;
    }

    _pLastPickedElt = NULL;
    _pModeler->getView()->getRenderer()->updateDisplayList();//az++ test

    _pModeler->getView()->updateGL();
}

void TYElementPicker::updateParents(TYElement* pElt)
{
    if (!pElt)
    {
        return;
    }

    std::shared_ptr<LPTYElementArray> pElts (new LPTYElementArray());
    TYElement* pTmpElt = pElt;

    while (pTmpElt != NULL)
    {
        // Cas particulier du maillage
        TYMaillage* pMail = dynamic_cast<TYMaillage*>(pTmpElt);
        if (pMail != nullptr)
        {
            // Recherche du point de calcul clicke
            TYElement* pPtCalcul = findPointCalculOnMaillage(pMail);

            // Detecte le type pour emettre le signal
            if (checkType(pPtCalcul))
            {
                // Ajout
                pElts->push_back(pPtCalcul);
            }
        }

        // Detecte le type pour emettre le signal
        checkType(pTmpElt); // DT-- le 31/08/04

        // Ajoute a la collection
        pElts->push_back(pTmpElt);

        // Recupere le parent
        pTmpElt = pTmpElt->getParent();
    }

    // Au moins un parent a ete trouve
    if (pElts->size() > 1)
    {
        // On signale le dernier parent trouve.
        emit(highestParentPicked(pElts->back()));
    }

    emit(elementCollectionPicked(pElts));
}

bool TYElementPicker::checkType(TYElement* pElt)
{
    if (!pElt)
    {
        return false;
    }

    bool ret = true;

    if (pElt->isA("TYPoint"))
    {
        emit(pointPicked(pElt));
    }
    else if (pElt->isA("TYPointCalcul"))
    {
        emit(pointCalculPicked(pElt));
    }
    else if (pElt->isA("TYPointControl"))
    {
        emit(pointControlPicked(pElt));
    }
    else if (pElt->isA("TYTerrain"))
    {
        emit(terrainPicked(pElt));
    }
    else if (pElt->isA("TYPlanEau"))
    {
        emit(planEauPicked(pElt));
    }
    else if (pElt->isA("TYCourbeNiveau"))
    {
        emit(courbeNiveauPicked(pElt));
    }
    else if (pElt->isA("TYUserSourcePonctuelle"))
    {
        emit(sourcePonctuellePicked(pElt));
    }
    else if (pElt->isA("TYAcousticLine"))
    {
        emit(acousticLinePicked(pElt));
    }
    else if (pElt->isA("TYReseauTransport"))
    {
        emit(reseauTransportPicked(pElt));
    }
    else if (pElt->isA("TYRoute"))
    {
        emit(routePicked(pElt));
    }
    else if (pElt->isA("TYCoursEau"))
    {
        emit(coursEauPicked(pElt));
    }
    else if (pElt->isA("TYAcousticRectangle"))
    {
        emit(acousticRectanglePicked(pElt));
    }
    else if (pElt->isA("TYMurElement"))
    {
        emit(murElementPicked(pElt));
    }
    else if (pElt->isA("TYDalle"))
    {
        emit(dallePicked(pElt));
    }
    else if (pElt->isA("TYAcousticCircle"))
    {
        emit(acousticCirclePicked(pElt));
    }
    else if (pElt->isA("TYAcousticSemiCircle"))
    {
        emit(acousticSemiCirclePicked(pElt));
    }
    else if (pElt->isA("TYAcousticCylinder"))
    {
        emit(acousticCylinderPicked(pElt));
    }
    else if (pElt->isA("TYAcousticSemiCylinder"))
    {
        emit(acousticSemiCylinderPicked(pElt));
    }
    else if (pElt->isA("TYMaillage"))
    {
        emit(maillagePicked(pElt));
    }
    else if (pElt->isA("TYRectangularMaillage"))
    {
        emit(rectangularMaillagePicked(pElt));
    }
    else if (pElt->isA("TYLinearMaillage"))
    {
        emit(linearMaillagePicked(pElt));
    }
    else if (pElt->isA("TYAcousticRectangleNode"))
    {
        emit(acousticRectangleNodePicked(pElt));
    }
    else if (pElt->isA("TYMur"))
    {
        emit(murPicked(pElt));
    }
    else if (pElt->isA("TYAcousticBox"))
    {
        emit(acousticBoxPicked(pElt));
    }
    else if (pElt->isA("TYAcousticVolumeNode"))
    {
        emit(acousticVolumeNodePicked(pElt));
    }
    else if (pElt->isA("TYEtage"))
    {
        emit(etagePicked(pElt));
    }
    else if (pElt->isA("TYMachine"))
    {
        emit(machinePicked(pElt));
    }
    else if (pElt->isA("TYBatiment"))
    {
        emit(batimentPicked(pElt));
    }
    else if (pElt->isA("TYTopographie"))
    {
        emit(topographiePicked(pElt));
    }
    else if (pElt->isA("TYSiteNode"))
    {
        emit(sitePicked(pElt));
    }
    else if (pElt->isA("TYSiteNode")) //???
    {
        emit(siteNodePicked(pElt));
    }
    else if (pElt->isA("TYEcran"))
    {
        emit(ecranPicked(pElt));
    }
    else if (pElt->isA("TYAcousticFaceSet"))
    {
        emit(acousticFaceSetPicked(pElt));
    }
    else
    {
        ret = false;
    }

    return ret;
}

TYPointCalcul* TYElementPicker::findPointCalculOnMaillage(TYMaillage* pMaillage)
{
    Q_ASSERT(pMaillage);
    TYPointCalcul* pPtCalcul = NULL;

    float mapperPos[2];
    mapperPos[0] = _x; mapperPos[1] = _y;

    for (unsigned int i = 0; i < pMaillage->getPtsCalcul().size(); i++)
    {
        pPtCalcul = pMaillage->getPtsCalcul()[i];
        Q_ASSERT(pPtCalcul);

        if ((mapperPos[0] <= pPtCalcul->_x + MaillagePickingTolerance) && (mapperPos[0] >= pPtCalcul->_x - MaillagePickingTolerance) &&
            (mapperPos[1] >= pPtCalcul->_y - MaillagePickingTolerance) && (mapperPos[1] <= pPtCalcul->_y + MaillagePickingTolerance))
        {
            // Point trouve
            return pPtCalcul;
        }
    }

    return NULL;
}
