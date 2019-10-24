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

#ifndef COPLANARITY_SELECTOR
#define COPLANARITY_SELECTOR

#include "Selector.h"
#include <map>
#include <vector>



/*!
 * \brief : To keep only one from two or more rays which have the same history (events on the same primitive)
 */
template<typename T>
class CoPlanaritySelector : public Selector<T>
{
public:
	/// Constructor
	CoPlanaritySelector() : Selector<T>(){}
    /// Destructor
    virtual ~CoPlanaritySelector() {}

    virtual Selector<T>* Copy()
    {
        CoPlanaritySelector* newSelector = new CoPlanaritySelector();
        newSelector->setIsDeletable(this->deletable);
        return newSelector;
    }
    virtual void reset() { selectedRays.clear(); return; }




	bool haveCoPlanarEvents(T* r1,T* r2){

		if(r1->getEvents()->size()==r2->getEvents()->size()){

			for(unsigned int i=0;i<r1->getEvents()->size();i++){
				Event* ev1 = r1->getEvents()->at(i).get();
				Event* ev2 = r2->getEvents()->at(i).get();

				if(coPlanarityTest(ev1,ev2))
					return true;
			}
		}
		return false;

	}

	bool areBothReflections(Event* ev1,Event* ev2){
		return ev1->getType()==SPECULARREFLEXION && ev2->getType()==SPECULARREFLEXION;
	}

	bool coPlanarityTest(Event* ev1,Event* ev2){

		// If any of the two events is not a reflection, events are not co-planar
		if(areBothReflections(ev1,ev2)){

			vec3 n1=ev1->getShape()->getNormal();
			vec3 n2=ev2->getShape()->getNormal();

			// Check if the normals are equal
			if( n1.compare(n2)){
				vec3 v  = ev2->getPosition() - ev1->getPosition();
				v.normalize();

				// Check if the faces are in the same plane
				if(v*n1 < EPSILON_4 && v*n2 < EPSILON_4)
					return true;
			}
		}

		return false;
	}

    virtual SELECTOR_RESPOND canBeInserted(T* r, unsigned long long& replace)
    {
		
        typename std::map<std::vector<unsigned int>, vector<T*>, CompareToKey>::iterator it;

        std::vector<unsigned int> event_signature = r->getEventSignature();

		it = selectedRays.find(event_signature); 

        // if there already is a ray with the same event signatures
		if (it != selectedRays.end())
        {
            r->computeLongueur();
            //double currentDistance = r->getLongueur();

			vector<T*> rays=it->second;
			cerr<<"rays : "<<rays.size()<<endl;
			/*for(unsigned int i=0;i<rays.size();i++){

				
				Ray* r2 = rays.at(i);
				if (currentDistance < r2->getLongueur())
				{
					//replace the older ray by the new one
					replace = r2->getConstructId();
					return SELECTOR_REPLACE;
				}
				else
				{     
					return SELECTOR_REJECT;
				}

			} */
			return SELECTOR_REJECT;
        }
        return SELECTOR_ACCEPT;
    }

    virtual void insert(T* r)
    {
		typename std::map<std::vector<unsigned int>, vector<T*>, CompareToKey>::iterator it;

        std::vector<unsigned int> event_signature = r->getEventSignature();

		it = selectedRays.find(event_signature); 

        // if there already is a ray with the same event signatures
		if (it != selectedRays.end())
        {
            r->computeLongueur();
            //double currentDistance = r->getLongueur();

			vector<T*> rays=it->second;
			cerr<<"rays : "<<rays.size()<<endl;

            return;
        }
        else
        {
            // if none of the already selected rays has the same history than r, then add the ray and its hsitory to selectedPath
			vector<T*> vec;
			vec.push_back(r);
            selectedRays.insert(std::pair<std::vector<unsigned int>, vector<T*>>(event_signature,vec));
        }
		
		return ;
    }

    virtual bool insertWithTest(T* r)
    {
		typename std::map<std::vector<unsigned int>, vector<T*>, CompareToKey>::iterator it;

        std::vector<unsigned int> event_signature = r->getEventSignature();

		it = selectedRays.find(event_signature); 

        // if there already is a ray with the same event signatures
		if (it != selectedRays.end())
        {
            r->computeLongueur();
            //double currentDistance = r->getLongueur();

			vector<T*> rays=it->second;
			cerr<<"rays : "<<rays.size()<<endl;

            return false;
        }
        else
        {
            // if none of the already selected rays has the same history than r, then add the ray and its hsitory to selectedPath
			vector<T*> vec;
			vec.push_back(r);
            selectedRays.insert(std::pair<std::vector<unsigned int>, vector<T*>>(event_signature,vec));
			return true ;
        }
		
		return false;
    }

	/**
	* \brief Return the class type of the selector
	*/
	virtual const char* getSelectorName(){
		return typeid(this).name();
	}

protected:
     std::map<std::vector<unsigned int>, vector<T*>, CompareToKey> selectedRays; //!< map of all event signatures with their corresponding rays
};

#endif
