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

#include <map>
#include "Tympan/MetierSolver/AcousticRaytracer/Ray/Ray.h"

#ifndef POST_FILTER_H
#define POST_FILTER_H

typedef std::map< signature, std::vector<Ray*> > families;
typedef std::list<Event*> sequence;
typedef std::map< sequence, std::vector<Ray*> > sequenceMap;

class postFilter : public Base
{
public:
	postFilter(std::vector<Ray*> *tabRay) : _tabRay(tabRay){}
	~postFilter() {}
	
	/*!
	 * \fn unsigned int Traite();
	 * \brief apply a filter to the group of valid rays found by ray tracing
	 * \return number of rays suppressed
	 */
	virtual unsigned int Process() { return 0; }

	/*!
	 * \fn unsigned int buildFamilies(families& mapFamilies)
	 * \brief Group rays with the same signature (same source and receptor events of same kind in the same order
	 * \return Number of families created
	 */
	virtual unsigned int buildFamilies(families& mapFamilies, typeevent typeEv) 
	{ 
		for (unsigned int i=0; i<_tabRay->size(); i++)
		{
			mapFamilies[_tabRay->at(i)->getSignature(typeEv)].push_back(_tabRay->at(i));
		}

		return mapFamilies.size();
	}

protected:
	/*!
	 * \fn sequence buildSequence(const signature &sig, Ray* ray, const typeevent& evType)
	 * \brief build the sequence of physical shape encountred by the ray
	 */
	virtual sequence buildSequence(const signature &sig, Ray* ray, const typeevent& evType)
	{
		sequence res;
		unsigned int posEv = sig.first;

		for (unsigned int i=0; i<ray->getNbEvents(); i++)
		{
			if (posEv  & 1) { res.push_back( ray->getEvents()->at(i).data() ); }
			posEv = posEv >> 1;
		}

		return res;
	}

	/*!
	 * \fn sequenceCount buildSequenceCount(const signature &sig, std::vector<Ray*>, const typeevent& evType)
	 * \brief Sort rays by events of type evType encountered along is path
	 */
	sequenceMap buildSequenceMap(const signature &sig, std::vector<Ray*> tabRay, const typeevent& evType)
	{
		sequenceMap seqMap;

		for (unsigned int i=0; i<tabRay.size(); i++)
		{
			sequence seq = buildSequence(sig, tabRay.at(i), evType);
			seqMap[seq].push_back( tabRay.at(i) );
		}

		return seqMap;
	}

	///*!
	// * \fn sequence buildReflexionBitSet(const signature &sig, Ray* ray)
	// * \brief build the bit sequence repesenting sequence of reflections in a ray
	// */
	//virtual unsigned int buildReflexionBitSet(const signature &sig, Ray* ray)
	//{
	//	unsigned int dif = sig.second;
	//	unsigned int nbEvents = ray->getNbEvents();

	//	// Construction de la séquence filtre
	//	//core_mathlib::buildComplementaryBitSet;
	//	unsigned int filter = ::powl(2, nbEvents) - 1;

	//	return  (dif ^ filter);
	//}

public : 
	std::vector<Ray*> *_tabRay;
};

#endif //POST_FILTER_H