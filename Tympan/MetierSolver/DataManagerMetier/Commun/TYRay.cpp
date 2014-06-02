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

#include "Tympan/MetierSolver/AcousticRaytracer/Acoustic/Event.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Acoustic/Recepteur.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/Cylindre.h"

#include "Tympan/MetierSolver/AnalyticRayTracer/geometry_modifier.h"

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

/*static*/ TYRayEvent TYRayEvent::build_from_RayEvent(const Event *rev)
{
    TYRayEvent tyrev;

    Cylindre* cyl = NULL;
    switch (rev->getType())
    {
        case SPECULARREFLEXION :
            if (rev->getShape()->isSol())
            {
                tyrev.type = TYREFLEXIONSOL;
            }
            else
            {
                tyrev.type = TYREFLEXION;
            }

            tyrev.idFace1 = rev->getShape()->getFaceId();

            break;

        case DIFFRACTION:
            tyrev.type = TYDIFFRACTION;
            cyl = dynamic_cast<Cylindre*>(const_cast<Event *>(rev)->getShape());
            if (cyl)
            {
                tyrev.idFace1 = cyl->getFirstShape()->getFaceId();
                tyrev.idFace2 = cyl->getSecondShape()->getFaceId();
            }
            break;

        default:
            tyrev.type = TY_NO_TYPE;
            break;
    }

    tyrev.pos = OPoint3D(rev->getPosition().x, rev->getPosition().y, rev->getPosition().z);
    tyrev.angle = rev->getAngle();

    return tyrev;
}

// ===================================================================================================================================================
double TYRay:: sampler_step = 1.0;

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

TYRay TYRay::build_from_Ray(int sens, Ray* ray)
{
    TYRay tyRay;
    
    tyRay.build_event_list_from_Ray(sens, ray);

    return tyRay;
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
        _events.push_back( new TYRayEvent( *(other._events.at(i)) ) );
    }

    build_links_between_events();
    compute_shot_angle();

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

void TYRay::build_event_list_from_Ray(int sens, Ray* ray)
{
    //Definition des Evenements.
    TYRayEvent* e = NULL;

    //Add source as an event
    e = new TYRayEvent();
    e->type = TYSOURCE;
    e->pos = OPoint3D(ray->source->getPosition().x, ray->source->getPosition().y, ray->source->getPosition().z);
    e->angle = 0.0;
    _events.push_back(e);

    if (sens == 1) // Rays traveled from receptor to source
    {
        //Creation des evenements de diffractions et reflexions
        std::vector<QSharedPointer<Event> >::reverse_iterator rit;

        for (rit = ray->getEvents()->rbegin(); rit != ray->getEvents()->rend(); rit++)
        {
            e = new TYRayEvent( TYRayEvent::build_from_RayEvent( (*rit).data() ) );
            _events.push_back(e);
        }
    }
    else // Rays traveled from source to receptor (normal
    {

        //Creation des evenements de diffractions et reflexions
        std::vector<QSharedPointer<Event> >::iterator rit;

        for (rit = ray->getEvents()->begin(); rit != ray->getEvents()->end(); rit++)
        {
            e = new TYRayEvent( TYRayEvent::build_from_RayEvent( (*rit).data() ) );
            _events.push_back(e);
        }
    }

    // Add receptor as an event
    e = new TYRayEvent();
    e->type = TYRECEPTEUR;
    Recepteur* recep = (static_cast<Recepteur*>(ray->getRecepteur()));
    e->pos = OPoint3D(recep->getPosition().x, recep->getPosition().y, recep->getPosition().z);
    e->angle = 0.0;
    _events.push_back(e);
}

void TYRay::build_links_between_events()
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

void TYRay::compute_shot_angle()
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

void TYRay::sampleAndCorrection(geometry_modifier& transformer)
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

void TYRay::nextLenghtCompute(geometry_modifier& transformer)
{
    for (unsigned j = 0; j < _events.size() - 1; j++)
    {
        // Path length correction for distance from one event to next event needed for calculus
        _events.at(j)->distNextEvent = lengthCorrection(_events.at(j), _events.at(j)->next, transformer);
    }
}

double TYRay::lengthCorrection(TYRayEvent* ev1, const TYRayEvent* ev2, geometry_modifier& transformer)
{
    TabPoint3D tabPoint = OPoint3D::checkPointsMaxDistance(ev1->pos, ev2->pos, sampler_step);

    // Calculation with h for each event
    // Useful for lengths & angles
    for (int i = 0; i < tabPoint.size(); i++)
    {
        vec3 point = OPoint3Dtovec3(tabPoint[i]);
        point = transformer.fonction_h_inverse(point);
        tabPoint[i] = vec3toOPoint3D(point);
    }

    double length = 0.;
    for (int i = 0; i < tabPoint.size() - 1; i++)
    {
        length += tabPoint[i].distFrom(tabPoint[i + 1]);
    }

    return length;
}

void TYRay::endLenghtCompute(geometry_modifier& transformer)
{
    for (unsigned j = 0; j < _events.size() - 1; j++)
    {
        // Path length correction for distance from one event to next event needed for calculus
        _events.at(j)->distEndEvent = lengthCorrection(_events.at(j), _events.at(j)->endEvent, transformer);
    }
}

void TYRay::prevNextLengthCompute(geometry_modifier& transformer)
{
    for (unsigned j = 1; j < _events.size() - 1; j++)
    {
        // Path length correction gives back the corrected distance between event-1 and event+1
        _events.at(j)->distPrevNext = lengthCorrection(_events.at(j)->previous, _events.at(j)->next, transformer);
    }
}

void TYRay::angleCompute(geometry_modifier& transformer)
{
    for (unsigned j = 1; j < _events.size() - 1; j++)
    {
        // Path length correction for distance from one event to next event needed for calculus
        _events.at(j)->angle = angleCorrection(_events.at(j)->previous, _events.at(j), _events.at(j)->next, transformer);
    }
}

void TYRay::eventPosCompute(geometry_modifier& transformer)
{
    for (unsigned i = 0; i < _events.size(); i++)
    {
        vec3 point = transformer.fonction_h_inverse( OPoint3Dtovec3(_events[i]->pos) );
        _events[i]->pos = vec3toOPoint3D(point);
    }
}

double TYRay::angleCorrection(const TYRayEvent* ev1, 
                                    TYRayEvent* ev2, 
                              const TYRayEvent* ev3, 
                              geometry_modifier& transformer)
{
    TabPoint3D tabPoint1 = OPoint3D::checkPointsMaxDistance(ev1->pos, ev2->pos, sampler_step);
    TabPoint3D tabPoint2 = OPoint3D::checkPointsMaxDistance(ev2->pos, ev3->pos, sampler_step);

    // Corrects position for the only three events of interest
    OPoint3D points[3] = { tabPoint1.at(tabPoint1.size() - 2), tabPoint1.at(tabPoint1.size() - 1), tabPoint2.at(1) };
    for (int i = 0; i < 3; i++)
    {
        vec3 point = transformer.fonction_h_inverse( OPoint3Dtovec3(points[i]) );
        points[i] = vec3toOPoint3D(point);
    }

    OVector3D vec1(points[1], points[0]);
    OVector3D vec2(points[1], points[2]);

    return (M_PI - vec1.angle(vec2)) / 2.;
}

void TYRay::tyRayCorrection(geometry_modifier& transformer)
{
    // Repositionnement des elements du rayon
    for (unsigned i = 0; i < _events.size(); i++)
    {
        vec3 point = transformer.fonction_h( OPoint3Dtovec3(_events[i]->pos) );
        _events[i]->pos = vec3toOPoint3D(point);
    }

    overSample(sampler_step);

    for (unsigned int i = 0 ; i < _events.size() ; i++)
    {
        vec3 point = transformer.fonction_h_inverse( OPoint3Dtovec3(_events[i]->pos) );
        _events[i]->pos = vec3toOPoint3D(point);
    }
}