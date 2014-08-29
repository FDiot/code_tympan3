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
 * \file TYTopographieGraphic.cpp
 * \brief Représentation graphique d'une topographie
 */


#include "Tympan/models/business/infrastructure/TYTopographie.h"
#include "Tympan/models/business/infrastructure/TYSiteNode.h"
#include "Tympan/gui/gl/TYAltimetrieGraphic.h"
#include "TYTopographieGraphic.h"


TYTopographieGraphic::TYTopographieGraphic(TYTopographie* pElement) :
    TYElementGraphic(pElement)
{
    // Pour materialiser l'emprise
    _pEmpriseGraphic = new TYPolyLineGraphic(true);
    _pEmpriseGraphic->setElement(pElement);
}

void TYTopographieGraphic::update(bool force /*=false*/)
{
    // Cet element est toujours actif quelque soit le calcul courant
    getElement()->setInCurrentCalcul(true);

    TYElementGraphic::update(force);
}

void TYTopographieGraphic::getChilds(TYListPtrTYElementGraphic& childs, bool recursif /*=true*/)
{
    TYListPtrTYElementGraphic Localchilds;

    Localchilds.push_back(_pEmpriseGraphic);

    unsigned int i;

    // Courbes de niveau
    TYTabCourbeNiveauGeoNode* pTabCrbNiv = &getElement()->getListCrbNiv();
    for (i = 0; i < pTabCrbNiv->size(); i++)
    {
        TYCourbeNiveauGeoNode* pCrbNivGeoNode = pTabCrbNiv->operator[](i);
        Localchilds.push_back(pCrbNivGeoNode->getGraphicObject());
    }

    // Plans d'eau
    TYTabPlanEauGeoNode* pTabPlanEau = &getElement()->getListPlanEau();
    for (i = 0; i < pTabPlanEau->size(); i++)
    {
        TYPlanEauGeoNode* pPlanEauGeoNode = pTabPlanEau->operator[](i);
        Localchilds.push_back(pPlanEauGeoNode->getGraphicObject());
    }

    // Altimetrie
    // 1 altimetry for all the site hierarchy. From the topography, retrieve the parent
    // site, and from this site, the root site and its altimetry.
    TYTopographie* pTYTopographie = getElement();
    LPTYSiteNode pParentSite = dynamic_cast<TYSiteNode*>(pTYTopographie->getParent());
    assert(pParentSite != nullptr && "The parent of a TYTopographie element must be a TYSiteNode");
    LPTYAltimetrie altimetrie =  pParentSite->getAltimetry();
    Localchilds.push_back(altimetrie->getGraphicObject());


    // Terrains
    TYTabTerrainGeoNode* pTabTerrain = &getElement()->getListTerrain();
    for (i = 0; i < pTabTerrain->size(); i++)
    {
        TYTerrainGeoNode* pTerrainGeoNode = pTabTerrain->operator[](i);
        Localchilds.push_back(pTerrainGeoNode->getGraphicObject());
    }

    // Cours d'Eau
    TYTabCoursEauGeoNode* pTabCrsEau = &getElement()->getListCrsEau();
    for (i = 0; i < pTabCrsEau->size(); i++)
    {
        TYCoursEauGeoNode* pCrsEauGeoNode = pTabCrsEau->operator[](i);
        Localchilds.push_back(pCrsEauGeoNode->getGraphicObject());
    }

    TYListPtrTYElementGraphic::iterator ite = Localchilds.begin();
    for (i = 0; i < Localchilds.size(); i++)
    {
        TYElementGraphic* pTYElementGraphic = (TYElementGraphic*)(*ite);
        childs.push_back(pTYElementGraphic);
        if (recursif)
        {
            pTYElementGraphic->getChilds(childs, recursif);
        }
        ite++;
    }
}

void TYTopographieGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    unsigned int i = 0;

    // Emprise
    _pEmpriseGraphic->setModified(true);
    _pEmpriseGraphic->setTabPoint(getElement()->getEmprise());

    float r = 0.0f, g = 0.0f, b = 0.0f;
#if TY_USE_IHM  //L'emprise est de la couleur par defaut des polylines
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "TYPolylineGraphicColorR"))
    {
        TYPreferenceManager::getColor(TYDIRPREFERENCEMANAGER, "TYPolylineGraphicColorR", r, g, b);
    }
    else
    {
        TYPreferenceManager::setColor(TYDIRPREFERENCEMANAGER, "TYPolylineGraphicColorR", r, g, b);
    }
#endif

    getElement()->setEmpriseColor(OColor(r / 255, g / 255, b / 255));

    glColor4fv(getElement()->getEmpriseColor());
    _pEmpriseGraphic->display(mode);

    // Altimetrie
    TYTopographie* pTYTopographie = getElement();
    LPTYSiteNode pParentSite = dynamic_cast<TYSiteNode*>(pTYTopographie->getParent());
    assert(pParentSite != nullptr && "The parent of a TYTopographie element must be a TYSiteNode");
    LPTYAltimetrie altimetrie =  pParentSite->getAltimetry();
    LPTYElementGraphic ElementGraphic = altimetrie->getGraphicObject();
    ElementGraphic->display(mode);

    // Plans d'eau
    TYTabPlanEauGeoNode* pTabPlanEau = &getElement()->getListPlanEau();
    TYPlanEauGeoNode* pPlanEauGeoNode = NULL;
    for (i = 0; i < pTabPlanEau->size(); i++)
    {
        pPlanEauGeoNode = pTabPlanEau->operator[](i);
        pPlanEauGeoNode->getGraphicObject()->display(mode);
    }

    // Terrains
    TYTabTerrainGeoNode* pTabTerrain = &getElement()->getListTerrain();
    TYTerrainGeoNode* pTerrainGeoNode = NULL;
    for (i = 0; i < pTabTerrain->size(); i++)
    {
        pTerrainGeoNode = pTabTerrain->operator[](i);
        pTerrainGeoNode->getGraphicObject()->display(mode);
    }

    // Cours d'Eau
    TYTabCoursEauGeoNode* pTabCrsEau = &getElement()->getListCrsEau();
    TYCoursEauGeoNode* pCrsEauGeoNode = NULL;
    for (i = 0; i < pTabCrsEau->size(); i++)
    {
        pCrsEauGeoNode = pTabCrsEau->operator[](i);
        pCrsEauGeoNode->getGraphicObject()->display(mode);
    }

    // Courbes de niveau
    // xbh: les courbes de niveau en dernier et separement pour que l'anti-aliasing soit correct
    TYTabCourbeNiveauGeoNode* pTabCrbNiv = &getElement()->getListCrbNiv();
    TYCourbeNiveauGeoNode* pCrbNivGeoNode = NULL;
    for (i = 0; i < pTabCrbNiv->size(); i++)
    {
        pCrbNivGeoNode = pTabCrbNiv->operator[](i);
        pCrbNivGeoNode->getGraphicObject()->display(mode);
    }
}

void TYTopographieGraphic::setBackgroundImage(QString sTopoFileName, int semiX, int semiY, TYPoint ptPosition, OVector3D bgOrientation)
{
    // Altimetrie
    TYTopographie* pTYTopographie = getElement();
    LPTYSiteNode pParentSite = dynamic_cast<TYSiteNode*>(pTYTopographie->getParent());
    assert(pParentSite != nullptr && "The parent of a TYTopographie element must be a TYSiteNode");
    LPTYAltimetrie altimetrie =  pParentSite->getAltimetry();
    LPTYElementGraphic ElementGraphic = altimetrie->getGraphicObject();
    ((TYAltimetrieGraphic*)((TYElementGraphic*)ElementGraphic))->setBackgroundImage(sTopoFileName, semiX, semiY, ptPosition, bgOrientation);
}

void TYTopographieGraphic::unsetBackgroundImage()
{
    // Altimetrie
    TYTopographie* pTYTopographie = getElement();
    LPTYSiteNode pParentSite = dynamic_cast<TYSiteNode*>(pTYTopographie->getParent());
    assert(pParentSite != nullptr && "The parent of a TYTopographie element must be a TYSiteNode");
    LPTYAltimetrie altimetrie =  pParentSite->getAltimetry();
    LPTYElementGraphic ElementGraphic = altimetrie->getGraphicObject();
    ((TYAltimetrieGraphic*)((TYElementGraphic*)ElementGraphic))->unsetBackgroundImage();
}

