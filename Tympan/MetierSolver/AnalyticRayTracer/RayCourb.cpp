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

#include "RayCourb.h"
#include "R3.h"

RayCourb::RayCourb() : nbReflex(0)
{
}

RayCourb::RayCourb(const R3& a) : nbReflex(0)
{
    this->coord.insert(coord.begin(), a);
}

RayCourb::RayCourb(const RayCourb& r) : coord(r.coord), normale(r.normale), nbReflex(r.nbReflex), position(r.position), rencontre(r.rencontre)
{
}


RayCourb& RayCourb::operator= (const RayCourb& P)
{
    coord = P.coord;
    normale = P.normale;
    nbReflex = P.nbReflex;
    position = P.position;
    rencontre = P.rencontre;

    return *this;
}

void RayCourb::purge()
{
    coord.clear();
    normale.clear();
    position.clear();
    rencontre.clear();
}

RayCourb RayCourb::operator + (const RayCourb& P)const
{
    /* Somme de deux rayons : on somme les coordonnees des points et celles des normales. On ne se preoccupe pas des reflexions (nombre et position). */

    assert(coord.size() == P.coord.size());

    RayCourb result;

    result.coord.clear();
    result.normale.clear();
    result.position.clear();

    for (unsigned int i = 0; i < coord.size(); ++i)
    {
        result.coord.push_back(this->coord[i] + P.coord[i]);
        result.normale.push_back(this->normale[i] + P.normale[i]);
    }

    return result;
}


RayCourb RayCourb::operator * (R c)const
{
    /* Multiplication d'un rayon par un reel : on multiplie les coordonnees des points et celles des normales. Les reflexions (nombre et position) ne sont pas modifiees. */

    RayCourb result;

    result.coord.clear();
    result.normale.clear();
    result.position.clear();

    for (unsigned int i = 0; i < coord.size(); ++i)
    {
        result.coord[i] = coord[i] * c;
        result.normale[i] = normale[i] * c;
    }

    result.position = position;

    return result;
}


RayCourb RayCourb::operator / (R c)const
{
    /* Division d'un rayon par un reel : on divise les coordonnees des points et celles des normales. Les reflexions (nombre et position) ne sont pas modifiees. */

    RayCourb result;

    result.coord.clear();
    result.normale.clear();
    result.position.clear();

    for (unsigned int i = 0; i < coord.size(); ++i)
    {
        result.coord[i] = coord[i] / c;
        result.normale[i] = normale[i] / c;
    }

    result.position = position;

    return result;
}


vector<R3> RayCourb::operator[](int i)
{
    /* l'operateur [int i] sur un rayon, rend un vecteur contenant les coordonnees du point et de sa normale a la position i */

    vector<R3> result;
    result.clear();                  // on s'assure qu'il est vide

    result.push_back(coord[i]);
    result.push_back(normale[i]);

    return result;
}


RayCourb::~RayCourb()
{
}

//
//R cLin(const R3& P, const meteo& Meteo, R3& grad)
//{
//
//    // calcul de la celerite
//    R c = Meteo.gradC * P.z + Meteo.c0;
//
//    // calcul du gradient
//    grad.z = Meteo.gradC;
//
//    return c;
//};
//
//
//R3 vent(const R3& P, const meteo& Meteo, map<pair<int, int>, R> &jacob)
//{
//
//    // calcul du vent : on a une fonction lineaire fonction de la coordonnee z du point
//    R3 v;
//
//  double angleVent = 90.0;
//  double PI =355.0 / 113.0;
//
//  double angle = -(PI/2.0) - (angleVent * PI /180.0);
//  double DVx = cos(angle) * Meteo.gradV;
//  double DVy = sin(angle) * Meteo.gradV;
//
//    v.x = DVx * P.z;
//    v.y = DVy * P.z;
//    v.z = 0;
//
//
//
//    //v.x = 0.5 * Meteo.gradV * P.z;
//    //v.y = 0.5 * Meteo.gradV * P.z;
//    //v.z = 0;
//
//    // calcul de la jacobienne
//    jacob[make_pair(1, 3)] = DVx;
//    jacob[make_pair(2, 3)] = DVy;
//
//
//    //jacob[make_pair(1, 3)] = 0.5 * Meteo.gradV;
//    //jacob[make_pair(2, 3)] = 0.5 * Meteo.gradV;
//
//    return v;
//}


/* L'operateur << et la fonction show sont extraits du cours d'Informatique Scientifique enseigne en Master 1 a l'Universite Pierre et Marie Curie par Frederic Hecht et Ionut Danaila*/

template<typename A, typename B>
ostream& operator<<(ostream& f, pair<A, B> ab)
{
    f << ab.first << " : " << ab.second ;
    return f ;
}

template<typename T>
void show(const char* s, const T& l, const char* separateur = "")
{
    cout << s << separateur ;
    for (typename T::const_iterator i = l.begin() ; i != l.end() ; ++i)
    {
        cout << '\t' << * i << separateur ;
    }
    cout << endl ;
}

RayCourb operator*(const R& c, const RayCourb& P)
{
    /* Multiplication d'un rayon par un reel : on multiplie les coordonnees des points et celles des normales. Les reflexions (nombre et position) ne sont pas modifiees. */

    RayCourb result;
    result.coord.clear();
    result.normale.clear();
    result.position.clear();

    for (unsigned int i = 0; i < P.coord.size(); ++i)
    {
        result.coord[i] = c * P.coord[i];
        result.normale[i] = c * P.normale[i];
    }

    result.position = P.position;

    return result;
}

//int main(){
//  int n = 3;
//  R3 recep(500,0,2), source(0,0,2);
//  meteo Meteo(0.5,0.5,340);
//
//  R3 grad;
//  cout<<"grad : "<<grad.x<<", "<<grad.y<<", "<<grad.z<<endl;
//
//  double cele = cLin(source, Meteo, grad);
//  cout<<"grad : "<<grad<<endl;
//
//  map<pair<int,int>,double> jacob;
//  for(int i=0; i<n; ++i)
//      for(int j=0; j<n; ++j)
//          jacob[make_pair(i,j)] = 0;
//
//  R3 mon_vent = vent(source, Meteo, jacob);
//  cout<<"vent : "<<mon_vent<<endl;
//  show("jacobienne",jacob);
//
//  return 0;
//}
