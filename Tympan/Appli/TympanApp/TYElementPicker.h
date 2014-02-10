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
 * \file TYElementPicker.h
 * \brief gestion des elements selectionnes par picking (fichier header)
 *
 *
 *
 *
 *
 */


#ifndef __TY_ELEMENT_PICKER__
#define __TY_ELEMENT_PICKER__


#include <qobject.h>

#include "TYAppDefines.h"
#include "Tympan/MetierSolver/DataManagerCore/TYElementCollection.h"


class TYMaillage;
class TYPointCalcul;

/**
 * \class TYElementPicker
 * \brief gestion des elements selectionnes par picking
 */
class TYElementPicker : public QObject
{
    Q_OBJECT

public:
    TYElementPicker(TYModelerFrame* pModeler);
    ~TYElementPicker();

    /**
     * \fn TYElement * getLastPickedElement()
     * \brief Retourne le dernier element picke.
     */
    TYElement* getLastPickedElement() { return _pLastPickedElt; }

public slots:
    /**
     * \fn bool pick(int x, int y)
     * \brief Effectue un picking en (x, y).
     */
    bool pick(int x, int y);

    /**
     * \fn void updateParents(TYElement * pElt)
     * \brief Parcours les parents depuis le dernier element picke
     * et emets les signaux correcpondants.
     */
    void updateParents(TYElement* pElt);

    /**
     * \fn void highlightElement(TYElement * pElt)
     * \brief Dessine la boite englobante de l'element passe.
     */
    void highlightElement(TYElement* pElt);

    /**
     * \fn void reset()
     * \brief Retourne dans son etat initial.
     */
    void reset();


signals:
    /**
     * \fn elementPicked(TYElement * pElt)
     * \brief Indique qu'un element a ete picke, sans connaitre son type.
     * Celui-ci sera de type "bas niveau".
     * \param pElt L'element picke.
     */
    void elementPicked(TYElement* pElt);

    /**
     * \fn void highestParentPicked(TYElement * pElt)
     * \brief Indique qu'un element parent a ete picke, ce sera le dernier
     * parent de l'element de base picke.
     * Celui-ci sera de type "haut niveau".
     * \param pElt L'element picke.
     */
    void highestParentPicked(TYElement* pElt);

    /**
     * \fn void elementCollectionPicked(LPTYElementCollection pElts)
     * \brief Indique qu'un element a ete picke, sans connaitre son type.
     * La collection d'elements pickes est composee d'au minimum
     * l'elemenet picke puis de sa famille (parent, parent du parent...).
     * \param pElts L'element picke et ses parents.
     */
    void elementCollectionPicked(LPTYElementCollection pElts);

    // Ces elements sont pickes directement ("bas niveau")
    void pointPicked(TYElement* pElt);
    void pointCalculPicked(TYElement* pElt);
    void pointControlPicked(TYElement* pElt);
    void terrainPicked(TYElement* pElt);
    void planEauPicked(TYElement* pElt);
    void courbeNiveauPicked(TYElement* pElt);
    void sourcePonctuellePicked(TYElement* pElt);
    void acousticLinePicked(TYElement* pElt);
    void reseauTransportPicked(TYElement* pElt);
    void routePicked(TYElement* pElt);
    void coursEauPicked(TYElement* pElt);
    void acousticRectanglePicked(TYElement* pElt);
    void murElementPicked(TYElement* pElt);
    void dallePicked(TYElement* pElt);
    void acousticCirclePicked(TYElement* pElt);
    void acousticSemiCirclePicked(TYElement* pElt);
    void acousticCylinderPicked(TYElement* pElt);
    void acousticSemiCylinderPicked(TYElement* pElt);
    void maillagePicked(TYElement* pElt);
    void rectangularMaillagePicked(TYElement* pElt);
    void linearMaillagePicked(TYElement* pElt);

    // Ces elements sont pickes indirectement ("haut niveau")
    // Ce sont les parents des elements bas niveau.
    void acousticRectangleNodePicked(TYElement* pElt);
    void murPicked(TYElement* pElt);
    void acousticBoxPicked(TYElement* pElt);
    void acousticVolumeNodePicked(TYElement* pElt);
    void etagePicked(TYElement* pElt);
    void machinePicked(TYElement* pElt);
    void batimentPicked(TYElement* pElt);
    void topographiePicked(TYElement* pElt);
    void sitePicked(TYElement* pElt);
    void siteNodePicked(TYElement* pElt);

    void acousticFaceSetPicked(TYElement* pElt);
    void ecranPicked(TYElement* pElt);


protected:
    /**
     * \fn bool checkType(TYElement * pElt)
     * \brief Teste le type de l'element passe et emet le signal correspondant
     * au type, avec en parametre l'element et sa position dans la collection
     * du picking courant.
     * \param pElt Un element a tester.
     * \return <code>true</code> si le type de l'element a ete trouve;
     *         <code>false</code> sinon.
     */
    bool checkType(TYElement* pElt);

    /**
     * \fn TYPointCalcul * findPointCalculOnMaillage(TYMaillage * pMaillage)
     * \brief Effectue un picking sur un maillage.
     * \param pMaillage Un maillage a tester.
     * \return Le point de calcul picke ou NULL.
     */
    TYPointCalcul* findPointCalculOnMaillage(TYMaillage* pMaillage);


protected:
    ///Le modeler dans lequel on effectue le picking.
    TYModelerFrame* _pModeler;

    ///Le dernier element picke.
    TYElement* _pLastPickedElt;

    ///Pointeur sur le dernier objet graphique highlighte.
    LPTYElementGraphic _pHighlightedGraphicObject;

    ///Position courante en x
    int _x;
    ///Position courante en y
    int _y;
};

#endif // __TY_ELEMENT_PICKER__
