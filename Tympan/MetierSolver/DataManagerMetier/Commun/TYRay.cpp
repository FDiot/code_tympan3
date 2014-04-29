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

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerGraphic/TYRayGraphic.h"
#endif

TY_EXT_GRAPHIC_INST(TYRay);

#include "TYRay.h"

TYRayEvent::TYRayEvent():   distNextEvent(0.0),
    distEndEvent(0.0),
    distPrevNext(0.0),
    angle(0.0),
    angletheta(0.0),
    type(TY_NO_TYPE),
    idFace1(-9999),
    idFace2(-9999),
    previous(NULL),
    next(NULL),
    endEvent(NULL)
{
}

TYRayEvent::TYRayEvent(const OPoint3D& pt) :    pos(pt),
    distNextEvent(0.0),
    distEndEvent(0.0),
    distPrevNext(0.0),
    angle(0.0),
    angletheta(0.0),
    type(TY_NO_TYPE),
    idFace1(-9999),
    idFace2(-9999),
    previous(NULL),
    next(NULL),
    endEvent(NULL)
{
}

TYRayEvent::TYRayEvent(const TYRayEvent& ev)
{
    *this = ev;
}

TYRayEvent::~TYRayEvent()
{
}

TYRayEvent& TYRayEvent::operator=(const TYRayEvent& other)
{
    pos = other.pos;
    distNextEvent = other.distNextEvent;
    distEndEvent = other.distEndEvent;
    distPrevNext = other.distPrevNext;
    angle = other.angle;
    angletheta = other.angletheta;
    type = other.type;
    idFace1 = other.idFace1;
    idFace2 = other.idFace2;
    previous = other.previous;
    next = other.next;
    endEvent = other.endEvent;

    return *this;
}

// ===================================================================================================================================================

TYRay::TYRay()
{
    _source = NULL;
    _recepteur = NULL;
}

TYRay::TYRay(const TYRay& ray)
{
    *this = ray;
}

TYRay::TYRay(TYSourcePonctuelle* sourceP, TYPointCalcul* recepP, std::vector<TYRayEvent*> &events)
{
    if (events.front()->type != TYSOURCE)
    {
        //std::cout << "Erreur : Le premier event du rayon n'est pas une source." << std::endl;
        _source = NULL;
        _recepteur = NULL;
        return;
    }
    if (events.back()->type != TYRECEPTEUR)
    {
        //std::cout << "Erreur : Le dernier event du rayon n'est pas un recepteur." << std::endl;
        _source = NULL;
        _recepteur = NULL;
        return;
    }
    _source = sourceP;
    _recepteur = recepP;

    for (unsigned int i = 0; i < events.size(); i++)
    {
        _events.push_back(events.at(i));
    }
}

TYRay::~TYRay()
{
    cleanEventsTab();
}

void TYRay::cleanEventsTab()
{
    for (unsigned int i = 0; i < _events.size(); i++)
    {
        if (_events[i] != NULL)
        {
            delete _events[i];
            _events[i] = NULL;
        }
    }

    _events.clear();
}

TYRay& TYRay::operator=(const TYRay& other)
{
    _identifiant = other.getIdentifiant();
    _source = other._source;
    _recepteur = other._recepteur;
    _posSourceGlobal = other._posSourceGlobal;
    _posReceptGlobal = other._posReceptGlobal;

    for (unsigned int i = 0; i < other._events.size(); i++)
    {
        _events.push_back(other._events.at(i));
    }

    return *this;
}

bool TYRay::deepCopy(TYRay* other)
{
    assert(other);

    cleanEventsTab();

    _identifiant = other->_identifiant;
    _source = other->_source;
    _recepteur = other->_recepteur;
    _posSourceGlobal = other->_posSourceGlobal;
    _posReceptGlobal = other->_posReceptGlobal;

    for (size_t i = 0; i < other->getEvents().size(); i++)
    {
        addEvent(new TYRayEvent(*(other->getEvents().at(i))));
    }

    return true;
}

void TYRay::setSource(TYSourcePonctuelle* source, OPoint3D& globalPosition)
{
    _source = source;
    _posSourceGlobal = globalPosition;
}

void TYRay::setRecepteur(TYPointCalcul* recep, OPoint3D& globalPosition)
{
    _recepteur = recep;
    _posReceptGlobal = globalPosition;
}

int TYRay::getNbrReflexions()
{
    int nbrRefl = 0;

    for (int i = 0; i < _events.size(); i++)
        if (_events[i]->type == TYREFLEXION) { nbrRefl++; }

    return nbrRefl;
}

int TYRay::getNbrDiffractions()
{
    int nbrDiff = 0;

    for (int i = 0; i < _events.size(); i++)
        if (_events[i]->type == TYDIFFRACTION) { nbrDiff++; }

    return nbrDiff;
}

double TYRay::getDistanceSR() const
{
    const OPoint3D S = *_source->getPos(); // coordonnees de la source

    double coord[3];
    _recepteur->getCoords(coord);
    const OPoint3D R = OPoint3D(coord); // coordonnees du recepteur

    return S.distFrom(R);
}

double TYRay::getLength()
{
    double length = 0.0;
    for (unsigned int i = 0; i < _events.size(); i++)
    {
        length += _events.at(i)->distNextEvent;
    }

    return length;
}

std::vector<int> TYRay::getIndexOfEvents(const int& eventType) const
{
    vector<int> eventsIndexList;
    for (size_t i = 0; i < _events.size(); i++)
    {
        if (_events[i]->type & eventType)
        {
            eventsIndexList.push_back(i);
        }
    }

    return eventsIndexList;
}

void TYRay::copyEvents(const TYRay* tyRay, TY_RAY_EVENT eventType)
{
    assert(tyRay);

    cleanEventsTab(); // Clean old tab of events

    const TYTabRayEvent& tabEvents = tyRay->getEvents();

    vector<int> tabIndexEvents = tyRay->getIndexOfEvents(eventType);

    for (unsigned int i = 0; i < tabIndexEvents.size(); i++)
    {
        addEvent(tabEvents.at(tabIndexEvents.at(i)));
    }
}

void TYRay::setNextDistance(TY_RAY_EVENT eventType)
{
    vector<int> idxList = getIndexOfEvents(eventType);

    unsigned int j = 0;
    double length = 0.;
    for (int i = 0; i < idxList.size() - 1; i++)
    {
        length = 0.;
        j = idxList[i];
        do
        {
            length += _events.at(j)->pos.distFrom(_events.at(j + 1)->pos);
            j++;
        }
        while (j != idxList[i + 1]);

        _events.at(idxList.at(i))->distNextEvent = length;
    }
}

void TYRay::setAngles(TY_RAY_EVENT eventType)
{
    for (unsigned int i = 1; i < _events.size() - 1; i++)
    {
        if (_events.at(i)->type & eventType)
        {
            OVector3D vec1(_events.at(i)->pos, _events.at(i - 1)->pos);
            OVector3D vec2(_events.at(i)->pos, _events.at(i + 1)->pos);

            _events.at(i)->angle = (M_PI - vec1.angle(vec2)) / 2.;
        }
    }
}

void TYRay::overSample(const double& dMin)
{
    if (dMin == 0.) { return; }

    TabPoint3D tabPoints;
    TYRayEvent* rayEvent = NULL;

    // Gestion des iterateurs
    TYTabRayEvent::iterator iter = _events.begin();
    TYTabRayEvent::iterator endIter = _events.end() - 1;
    // On va traiter les evenements deux a deux
    while (iter != endIter)
    {
        // Recuperation des deux points initiaux
        tabPoints.push_back((*iter)->pos);
        tabPoints.push_back((*(iter + 1))->pos);

        // Surechantillonnage du tableau de points
        tabPoints = OPoint3D::checkPointsMaxDistance(tabPoints, dMin);

        // Insertion des points sous forme d'evenement au tableau des evenements
        TYTabRayEvent::iterator iter2 = iter + 1;
        for (unsigned int i = 1; i < tabPoints.size() - 1; i++)
        {
            rayEvent = new TYRayEvent();
            rayEvent->type = TYREFRACTION;
            rayEvent->angle = 0.0;
            rayEvent->pos = tabPoints[i];
            iter2 = _events.insert(iter2, rayEvent);
            iter2++;
        }

        tabPoints.clear();
        endIter = _events.end() - 1;  // Actualisation car le tableau a ete modifie
        iter = iter2;               // Atualisation car le tableau a ete modifie
    };
}
