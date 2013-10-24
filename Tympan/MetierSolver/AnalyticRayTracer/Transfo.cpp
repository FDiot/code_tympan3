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
#include "Transfo.h"
#include "interpolation.h"
#include "Tympan/MetierSolver/ToolsMetier/ODelaunayMaker.h"

#include <map>
#include <cstdlib>
#include <vector>

using namespace std;


Transfo::Transfo() : source(0), recepteurs(NULL), Meteo(meteo()), h(0.001), TMax(3.0), temps(NULL), dmax(1000), nbRay(1), planDepart(NULL), planTransfo(NULL)
{
}


Transfo::Transfo(Lancer& L) : source(0), recepteurs(L.recepteurs), planDepart(L.plan), Meteo(L.Meteo), h(L.h), TMax(L.TMax), temps(L.temps), dmax(L.dmax), nbRay(L.nbRay), shot(L)
{
}


Transfo::Transfo(Transfo& r) :  source(r.source), recepteurs(r.recepteurs), Meteo(r.Meteo), nbRay(r.nbRay), dmax(r.dmax), h(r.h), TMax(r.TMax), temps(r.temps), shot(r.shot), planDepart(r.planDepart), planTransfo(r.planTransfo), MatRes(r.MatRes), MatTransfo(r.MatTransfo), methode(r.methode)
{
}


Transfo::~Transfo()
{
    purge();
}

void Transfo::purge()
{
    for (unsigned int i = 0; i < planDepart.size(); i++)
    {
        if (planDepart[i]) { delete [] planDepart[i]; }
        planDepart[i] = NULL;
    }

    for (unsigned int i = 0; i < planTransfo.size(); i++)
    {
        R3* plan = planTransfo[i];
        if (planTransfo[i]) { delete [] planTransfo[i]; }
        planTransfo[i] = NULL;
    }

    for (unsigned int i = 0; i < MatRes.size(); i++)
    {
        if (MatRes[i]) { delete [] MatRes[i]; }
        MatRes[i] = NULL;
    }

    for (unsigned int i = 0; i < MatTransfo.size(); i++)
    {
        if (MatTransfo[i]) { delete [] MatTransfo[i]; }
        MatTransfo[i] = NULL;
    }

    recepteurs.clear();
    temps.clear();
    planDepart.clear();
    planTransfo.clear();
    MatRes.clear();
    MatTransfo.clear();
    Liste_triangles.clear();
}

void Transfo::Init()
{
    shot.Init();
    purge();
}


R Transfo::distance_max()
{
    R result = 0;

    R3 src, rcpt;

    for (unsigned int nr = 0; nr < recepteurs.size(); ++nr)
    {
        src = shot.sources[source];
        rcpt = recepteurs[nr];
        result = max(result, sqrt((rcpt.x - src.x) * (rcpt.x - src.x) + (rcpt.y - src.y) * (rcpt.y - src.y) + (rcpt.z - src.z) * (rcpt.z - src.z)));
    }

    return result;
}


void Transfo::createTemps()
{
    R T = 0.0;
    do
    {
        temps.push_back(T);
        T += h;
    }
    while (T <= TMax);
}


void Transfo::createShot()
{
    shot.Meteo.setC0(Meteo.c0);
    shot.Meteo.setGradC(Meteo.gradC);
    shot.Meteo.setGradV(Meteo.gradV);
	shot.Meteo.setWindDirection(Meteo.windDirection);
    setSrcForMapping(source);
    shot.recepteurs = recepteurs;
    shot.setNbRay(nbRay);
    shot.setTMax(TMax);
    shot.setDMax(dmax);
    shot.setTimeStep(h);

    shot.run();
}

void Transfo::RemplirSurf(vector<R3>& vectSurf_Interp)
{
    vectSurf_Interp.reserve(nbRay * shot.temps.size() + 10);

    for (unsigned int i = 0; i < nbRay; ++i)                                                      // boucle sur les rayons lances
    {
        for (unsigned int j = 0; j < shot.MatRes[0][i].coord.size(); ++j)                         // boucle sur les points du rayon
        {
			R3 point(shot.MatRes[0][i].coord[j].x, shot.MatRes[0][i].coord[j].y, shot.MatRes[0][i].coord[j].z);
			vectSurf_Interp.push_back(point);
		}
	}
}

R3 Transfo::fonction_h(const R3& P)
{
    // c'est notre fonction h(x,y) qui transforme notre geometrie

    R3 R(P);

    // On cherche a quel triangle de Surf_Interp le point P appartient.
    // Un point P appartient au triangle ABC si l'aire signee des triangles ABP, BCP et CAP est positive.

    R3* TabTriangle = new R3[3];

    for (int i = 0; i < Liste_triangles.size(); ++i)
    {
        //On boucle sur les triangles du plan de transformation (Liste_triangles),
        //Si le point appartient a un triangle, on le transforme.
        //Sinon, le point n'est pas modifie (il est en dehors de la zone de transformation).

		OPoint3D v1 = list_vertex[Liste_triangles[i]._p1];
        OPoint3D v2 = list_vertex[Liste_triangles[i]._p2];
        OPoint3D v3 = list_vertex[Liste_triangles[i]._p3];

		TabTriangle[0] = R3(v1._x, v1._y, v1._z);
        TabTriangle[1] = R3(v2._x, v2._y, v2._z);
        TabTriangle[2] = R3(v3._x, v3._y, v3._z);

        if (IsInTriangle(P, TabTriangle))
        {
            R.z = R.z - interpo(TabTriangle, R) + shot.sources[source].z ;
            break;
        }
    }

    delete [] TabTriangle; // Gestion memoire

    return R;
}


R3 Transfo::fonction_h_inverse(const R3& P)
{
    // Fonction inverse de la fonction h.

    R3 R(P);

    // On cherche a quel triangle de Surf_Interp le point P appartient.
    // Un point P appartient au triangle ABC si l'aire signee des triangles ABP, BCP et CAP est positive.
    
    R3* TabTriangle = new R3[3];

    for (int i = 0; i < Liste_triangles.size(); ++i)
    {
        //On boucle sur les triangles du plan de transformation (Liste_triangles),
        //On cherche dans quel triangle est le point et on lui applique la transformation inverse.

        OPoint3D v1 = list_vertex[Liste_triangles[i]._p1];
        OPoint3D v2 = list_vertex[Liste_triangles[i]._p2];
        OPoint3D v3 = list_vertex[Liste_triangles[i]._p3];

		TabTriangle[0] = R3(v1._x, v1._y, v1._z);
        TabTriangle[1] = R3(v2._x, v2._y, v2._z);
        TabTriangle[2] = R3(v3._x, v3._y, v3._z);

        if (IsInTriangle(P, TabTriangle))
        {
            R.z = R.z + interpo(TabTriangle, R) - shot.sources[source].z;
            break;
        }

    }

	delete [] TabTriangle; // Gestion memoire
    
	return R;
}


void Transfo::trianguleNappe()
{
     vector<R3> vectSurf_Interp;  // vector qui contient les coordonnees des points dans l'ordre (coord de P1, coord de P2, ...)
	 RemplirSurf(vectSurf_Interp);

	// 1- On creer nos triangles de Delaunay
    ODelaunayMaker oDelaunayMaker(1e-5);
    //OPoint3D oVertex;

	for (unsigned int i=0; i<vectSurf_Interp.size(); i++)
	{
        R3 point(vectSurf_Interp[i].x, vectSurf_Interp[i].y, vectSurf_Interp[i].z);
		oDelaunayMaker.addVertex(OPoint3D(point.x, point.y, point.z));
	}

    oDelaunayMaker.compute();


    // 2- On a notre liste de triangles
    Liste_triangles = oDelaunayMaker.getFaces();
	list_vertex = oDelaunayMaker.getVertex();
}

void Transfo::Transfo_Geom1()
{
    // transformation de la topographie par la methode 1
    trianguleNappe();

    assert(Liste_triangles.isEmpty() == 0);  // on verifie que notre liste n'est pas vide


    // 3- On creer un fichier .txt contenant nos triangles et un autre contenant nos points

    ofstream ff("DelaunaySRS.txt");

    for (int i = 0; i < Liste_triangles.size(); i++)
    {
        ff << Liste_triangles[i]._p1 << " " << Liste_triangles[i]._p2 << " " << Liste_triangles[i]._p3 << endl;
    }

    // 4- Transformation

    // On deforme chaque point de la topographie (planDepart)

    for (int it = 0; static_cast<unsigned int>(it) < planDepart.size(); ++it)
    {
        // On boucle sur les triangles du plan de depart
        // On construit le nouveau triangle (triangleT) que l'on va mettre dans planTransfo

        R3* triangleT = new R3[3];
        triangleT[0] = planDepart[it][0];
        triangleT[1] = planDepart[it][1];
        triangleT[2] = planDepart[it][2];

        for (int i = 0; i < 3; ++i)
        {
            //On boucle sur les sommets du triangle,
            //On transforme chaque sommet.
            triangleT[i] = fonction_h(triangleT[i]);
        }

        planTransfo.push_back(triangleT);
    }

    ofstream fft("planTranfoSRS.txt");

    for (int i = 0; static_cast<unsigned int>(i) < planTransfo.size(); i++)
    {
        fft << planTransfo[i][0].x << "\t" << planTransfo[i][0].y << "\t" << planTransfo[i][0].z << endl;
        fft << planTransfo[i][1].x << "\t" << planTransfo[i][1].y << "\t" << planTransfo[i][1].z << endl;
        fft << planTransfo[i][2].x << "\t" << planTransfo[i][2].y << "\t" << planTransfo[i][2].z << endl << endl;
    }
}


void Transfo::Transfo_Geom2()
{
    methode = 2;
}


void Transfo::TransfoR1()
{
    methode = 1;

    // on commence par transformer notre espace
    vector<R3*>::iterator it;

}


void Transfo::TransfoR2()
{
    methode = 2;
}


void Transfo::Transformer()
{
    createShot();

    switch (methode)
    {
        case 1:
            Transfo_Geom1();
            break;
        case 2:
            Transfo_Geom2();
            // lance les rayons droits
            TransfoR2();
            break;
        default:
            methode = 0;
    }
}


void Transfo::setSrcForMapping(unsigned int& sourceIdx)
{
    R3 src = shot.sources[sourceIdx];
    shot.sources.clear();
    shot.sources.push_back(src);
}

bool IsInTriangle(const R3& P, const R3* triangle)  // R2:: ?
{
    R2 A(triangle[0].x, triangle[0].y);
    R2 B(triangle[1].x, triangle[1].y);
    R2 C(triangle[2].x, triangle[2].y);
    R2 Q(P.x, P.y);

    R resultat1 = area(A, B, Q);
    if (resultat1 < 0) { return false; }

    R resultat2 = area(B, C, Q);
    if (resultat2 < 0) { return false; }

    R resultat3 = area(C, A, Q);
    if (resultat3 < 0) { return false; }

    return true;
}
