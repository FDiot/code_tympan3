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
 
#include <vector>
#include <map>

#include "Geometry/mathlib.h"
#include "Geometry/Cylindre.h"
#include "Acoustic/Event.h"
#include "Ray/Ray.h"
#include "PostFilter.h"
#include "duplicateEventPostFilter.h"

using namespace std;

unsigned int duplicateEventPostFilter::Process()
{
	unsigned int nbSuppress = 0;

	// first filter for diffraction)
	families famD;
	unsigned int nbFamD = buildFamilies(famD, DIFFRACTION);

	// Second level sorting for reflection
	families::iterator iterD;

	for (iterD = famD.begin(); iterD != famD.end(); iterD++)
	{
		// Si un seul ray dans la famille,  on passe au suivant
		if ( (*iterD).second.size() <= 1 ) { continue; }

		//Seconde passe pour traiter les reflexions en debut de rayon qui ne sont pas detectable (suite de 0)
		postFilter *pF = new postFilter( &((*iterD).second) );
		families famR;
		pF->buildFamilies(famR, SPECULARREFLEXION);

		// Boucle sur la nouvelle serie
		families::iterator iterR;
		for (iterR = famR.begin(); iterR != famR.end(); iterR++)
		{
			if ( (*iterR).second.size() <= 1 ) { continue; }

			// third filter to separate different diffractions (sharing same shapes)
			sequenceMap sm = buildSequenceMap( (*iterR).first, (*iterR).second, DIFFRACTION );

			// On boucle sur cette nouvelle série
			sequenceMap::iterator iterSM;
			for (iterSM = sm.begin(); iterSM != sm.end(); iterSM++)
			{
				if ( (*iterSM).second.size() <= 1) { continue; }

			}
		}



		//families famR;
		//unsigned int nbFamR = buildFamilies(famD, SPECULARREFLEXION);

		//families::iterator iterR;
		//for (iterR = famR.begin(); iterR != famR.end(); iterR++)
		//{
		//	// Si un seul ray dans la famille,  on passe au suivant
		//	if ( (*iterR).second.size() <= 1 ) { continue; }

		//	// Réflechir sur la comparaison des évènement entre rayons
		//	// ATTENTION AU CAS OU IL Y A PLUS DE DEUX RAYONS

		//}


		//// Travail sur les évènements détectés
		//sequenceMap sm = buildSequenceMap( (*iter).first, (*iter).second, SPECULARREFLEXION);
		//sequenceMap::iterator itm;
		//for (itm = sm.begin(); itm != sm.end(); itm++)
		//{
		//	// Si un seul ray dans la séquence, on passe au suivant
		//	if ( (*iter).second.size() <= 1 ) { continue; }
		//}
	}

	return nbSuppress;
}