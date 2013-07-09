/*! \file relations.cpp
  \brief Instanciates and initializze the relations between each entity
  of the solver data model.


  \date 22 nov. 2012
  \author Anthony Truchet <anthony.truchet@logilab.fr>
*/

#include "relations.hpp"

template class RelationDefinition < tympan::has_node_0_rtype,
         tympan::AcousticTriangle,
         tympan::Node >;

template class RelationDefinition < tympan::has_node_1_rtype,
         tympan::AcousticTriangle,
         tympan::Node >;

template class RelationDefinition < tympan::has_node_2_rtype,
         tympan::AcousticTriangle,
         tympan::Node >;


