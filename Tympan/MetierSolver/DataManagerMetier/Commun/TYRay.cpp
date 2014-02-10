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

TYRayEvent::TYRayEvent(const TYPoint& pt, const double& ang, const double& angtheta, const TY_RAY_EVENT& ev, const int& idF1, const int& idF2) : pos(pt),
    angle(ang),
    angletheta(angtheta),
    type(ev),
    idFace1(idF1),
    idFace2(idF2)
{
}

TYRay::TYRay()
{
    _source = NULL;
    _recepteur = NULL;
}

TYRay::TYRay(const TYRay& ray)
{
    _identifiant = ray.getIdentifiant();
    _source = ray._source;
    _recepteur = ray._recepteur;
    for (unsigned int i = 0; i < ray._events.size(); i++)
    {
        _events.push_back(ray._events.at(i));
    }
}

//TYRay::TYRay(TYSourcePonctuelle *sourceP, TYPointControl *recepP, std::vector<TYRayEvent> &events)
TYRay::TYRay(TYSourcePonctuelle* sourceP, TYPointCalcul* recepP, std::vector<TYRayEvent> &events)
{
    if (events.front().type != TYSOURCE)
    {
        std::cout << "Erreur : Le premier event du rayon n'est pas une source." << std::endl;
        _source = NULL;
        _recepteur = NULL;
        return;
    }
    if (events.back().type != TYRECEPTEUR)
    {
        std::cout << "Erreur : Le dernier event du rayon n'est pas un recepteur." << std::endl;
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

/*void TYRay::setTabPoint(TYTabPoint &tabPoint){
    polyligne.clear();
    for(unsigned int i =0;i < tabPoint.size(); i++)
        polyligne.push_back(tabPoint.at(i));
}*/

void TYRay::setSource(TYSourcePonctuelle* source, TYPoint& globalPosition)
{
    _source = source;
    _posSourceGlobal = globalPosition;
    //if(_events.front().type == TYSOURCE)
    //{
    //  _source = source;
    //  double coord[3];
    //  globalPosition.getCoords(coord);
    //  (*_events.begin()).pos.setCoords(coord);
    //}
    //else
    //  std::cout << "Incoherence : le premier event n'est pas une source." << std::endl;
}

//void TYRay::setRecepteur(TYPointControl *recep, TYPoint &globalPosition)
void TYRay::setRecepteur(TYPointCalcul* recep, TYPoint& globalPosition)
{
    _recepteur = recep;
    _posReceptGlobal = globalPosition;
    //if(_events.back().type == TYRECEPTEUR)
    //{
    //  _recepteur = recep;
    //  double coord[3];
    //  globalPosition.getCoords();
    //  (*_events.end()).pos.setCoords(coord);
    //}
    //else
    //  std::cout << "Incoherence : le dernier event n'est pas un recepteur." << std::endl;
}

int TYRay::getNbrReflexions()
{
    int nbrRefl = 0;

    for (int i = 0; i < _events.size(); i++)
        if (_events[i].type == TYREFLEXION) { nbrRefl++; }

    return nbrRefl;
}

int TYRay::getNbrDiffractions()
{
    int nbrDiff = 0;

    for (int i = 0; i < _events.size(); i++)
        if (_events[i].type == TYDIFFRACTION) { nbrDiff++; }

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

double TYRay::getSize()
{
    const OPoint3D S = *_source->getPos();

    double coord[3];
    _recepteur->getCoords(coord);
    const OPoint3D R = OPoint3D(coord);

    int i;
    double raySize = 0.0; // initialisation

    // distance entre S et P0 : 1er pt oi¿½ a lieu un evenement
    //raySize = S.distFrom(_events[0].pos);

    for (i = 0; i < _events.size() - 1; i++)
    {
        raySize += _events[i].pos.distFrom(_events[i + 1].pos);    // distance entre Pi et Pi+1
    }

    // distance entre dernier P et R
    //raySize += R.distFrom(_events[i+1].pos);

    return raySize;
}
