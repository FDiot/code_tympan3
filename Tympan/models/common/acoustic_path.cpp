/*
 * Copyright (C) <2012-2014> <EDF-R&D> <FRANCE>
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

#include <vector>
#include <cassert>
#include "Tympan/models/common/3d.h"
#include "Tympan/models/common/mathlib.h"
#include "Tympan/geometric_methods/AnalyticRayTracer/geometry_modifier.h"
#include "acoustic_path.h"

acoustic_event::acoustic_event():   distNextEvent(0.0),
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

acoustic_event::acoustic_event(const OPoint3D& pt) :    pos(pt),
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

acoustic_event::acoustic_event(const acoustic_event& ev)
{
    *this = ev;
}

acoustic_event::~acoustic_event()
{
    if(previous) delete previous;
    if(next) delete next;
    if(endEvent) delete endEvent;

}

acoustic_event& acoustic_event::operator=(const acoustic_event& other)
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

double acoustic_path:: sampler_step = 20.0;

acoustic_path::acoustic_path()
{
}

acoustic_path::acoustic_path(const acoustic_path& ray)
{
    *this = ray;
}

acoustic_path::~acoustic_path()
{
    cleanEventsTab();
}

void acoustic_path::cleanEventsTab()
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

acoustic_path& acoustic_path::operator=(const acoustic_path& other)
{
    _identifiant = other.getIdentifiant();
    source_idx = other.source_idx;
    receptor_idx = other.receptor_idx;
    _posSourceGlobal = other._posSourceGlobal;
    _posReceptGlobal = other._posReceptGlobal;

    for (unsigned int i = 0; i < other._events.size(); i++)
    {
        _events.push_back( new acoustic_event( *(other._events.at(i)) ) );
    }

    return *this;
}

bool acoustic_path::deepCopy(acoustic_path* other)
{
    assert(other);

    cleanEventsTab();

    _identifiant = other->_identifiant;
    source_idx = other->source_idx;
    receptor_idx = other->receptor_idx;
    _posSourceGlobal = other->_posSourceGlobal;
    _posReceptGlobal = other->_posReceptGlobal;

    for (size_t i = 0; i < other->getEvents().size(); i++)
    {
        addEvent(new acoustic_event(*(other->getEvents().at(i))));
    }

    return true;
}

void acoustic_path::setSource(unsigned int source_idx_, OPoint3D& globalPosition)
{
    source_idx = source_idx_;
    _posSourceGlobal = globalPosition;
}

void acoustic_path::setRecepteur(unsigned  int receptor_idx_, OPoint3D& globalPosition)
{
    receptor_idx = receptor_idx_;
    _posReceptGlobal = globalPosition;
}

double acoustic_path::getLength()
{
    double length = 0.0;
    for (unsigned int i = 0; i < _events.size(); i++)
    {
        length += _events.at(i)->distNextEvent;
    }

    return length;
}

std::vector<int> acoustic_path::getIndexOfEvents(const int& eventType) const
{
    std::vector<int> eventsIndexList;
    for (size_t i = 0; i < _events.size(); i++)
    {
        if (_events[i]->type & eventType)
        {
            eventsIndexList.push_back(i);
        }
    }

    return eventsIndexList;
}

void acoustic_path::copyEvents(const acoustic_path* tyRay, ACOUSTIC_EVENT_TYPES eventType)
{
    assert(tyRay);

    cleanEventsTab(); // Clean old tab of events

    const tab_acoustic_events& tabEvents = tyRay->getEvents();

    std::vector<int> tabIndexEvents = tyRay->getIndexOfEvents(eventType);

    for (unsigned int i = 0; i < tabIndexEvents.size(); i++)
    {
        addEvent(tabEvents.at(tabIndexEvents.at(i)));
    }
}

void acoustic_path::setNextDistance(ACOUSTIC_EVENT_TYPES eventType)
{
    std::vector<int> idxList = getIndexOfEvents(eventType);

    unsigned int j = 0;
    double length = 0.;
    for (size_t i = 0; i < idxList.size() - 1; i++)
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

void acoustic_path::setAngles(ACOUSTIC_EVENT_TYPES eventType)
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

void acoustic_path::overSample(IGeometryModifier* transformer, const double& dMin)
{
    if (dMin == 0.) { return; }

    TabPoint3D tabPoints;
    acoustic_event* rayEvent = NULL;

    // Gestion des iterateurs
    tab_acoustic_events::iterator iter = _events.begin();
    tab_acoustic_events::iterator endIter = _events.end() - 1;
    // On va traiter les evenements deux a deux
    while (iter != endIter)
    {
        // Recuperation des deux points initiaux, dans le repere de la geometrie deformee
        tabPoints.push_back( vec3toOPoint3D( transformer->fonction_h( OPoint3Dtovec3( (*iter)->pos ) ) ) );
        tabPoints.push_back( vec3toOPoint3D( transformer->fonction_h( OPoint3Dtovec3( (*(iter + 1))->pos ) ) ) );

        // Surechantillonnage du tableau de points
        tabPoints = OPoint3D::checkPointsMaxDistance(tabPoints, dMin);

        // Insertion des points sous forme d'evenement au tableau des evenements
        tab_acoustic_events::iterator iter2 = iter + 1;
        for (unsigned int i = 1; i < tabPoints.size() - 1; i++)
        {
            rayEvent = new acoustic_event();
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

void acoustic_path::build_links_between_events()
{
    std::vector<int> tabIndex = getIndexOfEvents(TYREFLEXION | TYREFLEXIONSOL | TYRECEPTEUR);
    unsigned int k = 0;

    for (unsigned int j = 0; j < _events.size() - 1; j++)
    {
        _events.at(j)->next = _events.at(j + 1);

        if (j > 0) { _events.at(j)->previous = _events.at(j - 1); }
        if (j == _events.size() - 2) { _events.at(j + 1)->previous = _events.at(j); }

        if (j == tabIndex[k]) { k++; }

        _events.at(j)->endEvent = _events.at(tabIndex[k]);
    }
}

void acoustic_path::compute_shot_angle()
{
    vec3 P0 = OPoint3Dtovec3( _events[0]->pos );
    vec3 P1 = OPoint3Dtovec3( _events[1]->pos );
    vec3 v0(P0, P1);
    vec3 v1(v0);
    vec3 v2(v0);
    v1.z = 0;
    v2.y = 0;
    v0.normalize();
    v1.normalize();
    v2.normalize();

    // Angle phi
    double result = v0 * v1;
    int sign = v0.z > 0 ?  1 : -1;
    double angle = ::acos(result) * sign;

    _events[0]->angle = angle;

    // Angle theta
    result = v0 * v2;
    sign = v0.y > 0 ?  1 : -1;
    angle = ::acos(result) * sign;
    angle = v0.x < 0 ? M_PI - angle : angle;

    _events[0]->angletheta = angle;
}

void acoustic_path::sampleAndCorrection(IGeometryModifier* transformer)
{
        // Récupération des longueurs simples (éléments suivants)
        nextLenghtCompute(transformer);

        // Récupération des distances aux évènements pertinents
        endLenghtCompute(transformer);

        // Distance entre évènement précédent et suivant
        prevNextLengthCompute(transformer);

        // Récupération des angles
        angleCompute(transformer);

        // Correction de la position des évènements
        eventPosCompute(transformer);
}

void acoustic_path::nextLenghtCompute(IGeometryModifier* transformer)
{
    for (unsigned j = 0; j < _events.size() - 1; j++)
    {
        // Path length correction for distance from one event to next event needed for calculus
        _events.at(j)->distNextEvent = lengthCorrection(_events.at(j), _events.at(j)->next, transformer);
    }
}

double acoustic_path::lengthCorrection(acoustic_event* ev1, const acoustic_event* ev2, IGeometryModifier* transformer)
{
    TabPoint3D tabPoint = OPoint3D::checkPointsMaxDistance(ev1->pos, ev2->pos, sampler_step);
    double length = 0.;

    // Calculation with h for each event
    // Useful for lengths & angles
    for (size_t i = 0; i < tabPoint.size(); i++)
    {
        OPoint3D& point = tabPoint[i];
        point = vec3toOPoint3D( transformer->fonction_h_inverse( OPoint3Dtovec3(point) ) );
    }

    for (size_t i = 0; i < tabPoint.size() - 1; i++)
    {
        length += tabPoint[i].distFrom(tabPoint[i + 1]);
    }

    return length;
}

void acoustic_path::endLenghtCompute(IGeometryModifier* transformer)
{
    for (unsigned j = 0; j < _events.size() - 1; j++)
    {
        // Path length correction for distance from one event to next event needed for calculus
        _events.at(j)->distEndEvent = lengthCorrection(_events.at(j), _events.at(j)->endEvent, transformer);
    }
}

void acoustic_path::prevNextLengthCompute(IGeometryModifier* transformer)
{
    for (unsigned j = 1; j < _events.size() - 1; j++)
    {
        // Path length correction gives back the corrected distance between event-1 and event+1
        _events.at(j)->distPrevNext = lengthCorrection(_events.at(j)->previous, _events.at(j)->next, transformer);
    }
}

void acoustic_path::angleCompute(IGeometryModifier* transformer)
{
    for (unsigned j = 1; j < _events.size() - 1; j++)
    {
        // Path length correction for distance from one event to next event needed for calculus
        _events.at(j)->angle = angleCorrection(_events.at(j)->previous, _events.at(j), _events.at(j)->next, transformer);
    }
}

void acoustic_path::eventPosCompute(IGeometryModifier* transformer)
{
    for (unsigned i = 0; i < _events.size(); i++)
    {
        OPoint3D& point = _events[i]->pos;
        point = vec3toOPoint3D( transformer->fonction_h_inverse( OPoint3Dtovec3(point) ) );
    }
}

double acoustic_path::angleCorrection(const acoustic_event* ev1,
                                    acoustic_event* ev2,
                              const acoustic_event* ev3,
                              IGeometryModifier* transformer)
{
    TabPoint3D tabPoint1 = OPoint3D::checkPointsMaxDistance(ev1->pos, ev2->pos, sampler_step);
    TabPoint3D tabPoint2 = OPoint3D::checkPointsMaxDistance(ev2->pos, ev3->pos, sampler_step);

    // Corrects position for the only three events of interest
    OPoint3D points[3] = { tabPoint1.at(tabPoint1.size() - 2), tabPoint1.at(tabPoint1.size() - 1), tabPoint2.at(1) };
    for (int i = 0; i < 3; i++)
    {
        OPoint3D& point = points[i];
        point = vec3toOPoint3D( transformer->fonction_h_inverse( OPoint3Dtovec3(point) ) );
    }

    OVector3D vec1(points[1], points[0]);
    OVector3D vec2(points[1], points[2]);

    return (M_PI - vec1.angle(vec2)) / 2.;
}

void acoustic_path::tyRayCorrection(IGeometryModifier* transformer)
{
    // Repositionnement des elements du rayon
    overSample(transformer, sampler_step);

    std::vector<int> listIndex = getIndexOfEvents(TYREFRACTION);

    for (unsigned int i = 0 ; i < listIndex.size() ; i++)
    {
        OPoint3D& point = _events[ listIndex[i] ]->pos;
        point = vec3toOPoint3D( transformer->fonction_h_inverse( OPoint3Dtovec3(point) ) );
    }
}
