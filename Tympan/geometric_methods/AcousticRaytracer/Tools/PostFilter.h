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
#include <deque>
#include "Ray/Ray.h"

#ifndef POST_FILTER_H
#define POST_FILTER_H

typedef std::map< signature, std::deque<Ray*> > families;
typedef std::list<Event*> sequence;
typedef std::map< sequence, std::deque<Ray*> > sequenceMap;

/**
 * \brief Class to apply a filter to the group of valid rays found by ray tracing
 */
class PostFilter : public Base
{
public:
	/// Constructor
	PostFilter(std::deque<Ray*> *tabRay) : _tabRays(tabRay){}
	/// Destructor
    ~PostFilter() { _tabRays = NULL; }

    /*!
     * \brief Apply a filter to the group of valid rays found by ray tracing
     * \return number of rays suppressed
     */
    virtual unsigned int Process() { return 0; }

    /*!
     * \brief Group rays with the same signature (same source and receptor events of same kind in the same order)
     * \return Number of families created
     */
    inline virtual unsigned int buildFamilies(families& mapFamilies, typeevent typeEv)
    {
        for (unsigned int i = 0; i < _tabRays->size(); i++)
        {
            mapFamilies[_tabRays->at(i)->getSignature(typeEv)].push_back(_tabRays->at(i));
        }

        return mapFamilies.size();
    }

    /*!
     * \brief Rebuild rays tab after treatment
     */
    inline virtual void rebuildTabRays(families& mapFamilies)
    {
        _tabRays->clear();

        // Relecture des familles
        std::map< signature, std::deque<Ray*> >::iterator it_families;
        for (it_families = mapFamilies.begin(); it_families != mapFamilies.end(); it_families++)
        {
            std::deque<Ray*>& aTabRay= (*it_families).second; // Get list of Rays
            for (unsigned int i=0; i<aTabRay.size(); i++)
            {
                _tabRays->push_back( aTabRay.at(i) );
            }
        }
    }

    /*!
     * \brief Sort rays by events of type evType encountered along is path
     */
    inline virtual sequenceMap buildSequenceMap(const signature& sig, std::vector<Ray*> tabRay, const typeevent& evType)
    {
        sequenceMap seqMap;

        for (unsigned int i = 0; i < tabRay.size(); i++)
        {
            sequence seq = buildSequence(sig, tabRay.at(i), evType);
            seqMap[seq].push_back(tabRay.at(i));
        }

        return seqMap;
    }

    /*!
     * \brief compute minimum acceptable distance between two rays
     *        thickness is the thickness of a ray after a given distance
     *        d_R is the difference length between two rays
     */
    inline decimal minimum_distance(const decimal& thickness, const decimal& d_R) const
    {
        return sqrt(thickness * thickness + d_R * d_R);
    }

protected:
    /*!
     * \brief Build the sequence of physical shape encountered by the ray
     */
    inline virtual sequence buildSequence(const signature& sig, Ray* ray, const typeevent& evType)
    {
        sequence res;

        for (unsigned int i = 0; i < ray->getNbEvents(); i++)
        {
            if (ray->getEvents()->at(i).get()->getType() == evType) { res.push_back(ray->getEvents()->at(i).get()); }
        }

        return res;
    }
    /// Delete an element in the rays array
    inline virtual std::deque<Ray*>::iterator erase_element(std::deque<Ray*>& tabRays, std::deque<Ray*>::iterator& iter)
    {
        delete(*iter);
        (*iter) = NULL;

        return tabRays.erase(iter);
    }



protected:

	std::deque<Ray*> *_tabRays;	//!< Rays tab: pointers list of rays
};

#endif //POST_FILTER_H
