/*! \file relations.hpp
  \brief This file provides the declaration of the relations between each entity
  of the solver data model.

  \date 20 nov. 2012
  \author Damien Garaud <damien.garaud@logilab.fr>
*/

#ifndef TYMPAN__RELATIONS_H__INCLUDED
#define TYMPAN__RELATIONS_H__INCLUDED

#include "data_model_common.hpp"

#include "entities.hpp"

namespace tympan
{

namespace bm = boost::bimaps;

/// @brief Helper to handle relation represented outside the related entities
template<class RelationTypeT, class SubjectT, class ObjectT>
class ManyManyRelation
{

public:
    typedef tympan::shared_ptr< SubjectT > subject_pointer;
    typedef tympan::shared_ptr< ObjectT >  object_pointer;

    // This is for '**' cardinality.
    // To implement '1*' use (unordered_)set_of
    typedef bm::bimap <
    bm::unordered_multiset_of<subject_pointer>,
       bm::unordered_multiset_of<object_pointer>,
       bm::unordered_set_of_relation<>
       > bimap_type;

    typedef typename bimap_type::value_type relation;
    typedef typename bimap_type::left_map subject_view;
    typedef typename bimap_type::right_map  object_view;


protected:
    static bimap_type& Instance()
    {
        // static std::auto_ptr<bimap_type> the_bimap(new bimap_type());
        static bimap_type the_bimap;
        return the_bimap;
    }

public:
    static bool add(subject_pointer subject, object_pointer object)
    {
        // insert returns a pair (insertion_pos : iterator, success : bool)
        return Instance().insert(relation(subject, object)).second;
    }

    static bool remove(subject_pointer subject, object_pointer object)
    {
        // erase return the number of elements
        return Instance().erase(relation(subject, object));
    }

    static bool test(subject_pointer subject, object_pointer object)
    {
        return Instance().find(relation(subject, object)) != Instance().end();
    }

    static typename boost::select_second_const_range<typename subject_view::range_type>
    by_subject(subject_pointer subject)
    {
        return Instance().left.equal_range(subject) | boost::adaptors::map_values;
    }

    static boost::select_second_const_range<typename object_view::range_type>
    by_object(object_pointer object)
    {
        return Instance().right.equal_range(object) | boost::adaptors::map_values;
    }

}; // template class ManyManyRelation

} //namespace tympan

namespace tympan
{



} /* namespace tympan */

#endif /* TYMPAN__RELATIONS_H__INCLUDED */
