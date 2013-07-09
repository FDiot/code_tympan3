/*! \file relations.hpp
  \brief This file provides the declaration of the relations between each entity
  of the solver data model.

  For instance, there is one or more \c tympan::AcousticTriangle linked with a \c
  tympan::SiteElement with the relation \c tympan::surface_of_rdef.

  You have to declare smart pointers of each instance of a specific type as:
  \code
  AcousticTriangle::pointer surf_a(new AcousticTriangle(Vector));
  AcousticTriangle::pointer surf_b(new AcousticTriangle(Vector));
  SiteElement::pointer element(new SiteElement(uid));
  \endcode

  To link these instances:
  \code
  surface_of_rdef::add(surf_a, element);
  surface_of_rdef::add(surf_b, element);
  \endcode

  You can have a 'view' of a set of surfaces (i.e. the subject) or the set of
  elements (i.e. the object). You can give any name for each 'view'.
  \code
  surface_of_rdef::subject_view::type& surface_of = surface_of_rdef::by<subject>();
  surface_of_rdef::object_view::type& composed = surface_of_rdef::by<object>();
  \endcode

  \date 20 nov. 2012
  \author Damien Garaud <damien.garaud@logilab.fr>
*/

#ifndef TYMPAN__RELATIONS_H__INCLUDED
#define TYMPAN__RELATIONS_H__INCLUDED

#include <string>
using std::string;

#include "data_model_common.hpp"
#include "entities.hpp"

#include "yams/yams.hh"

namespace tympan
{

/// Type of 'has_node_0' relation : first vertex of a face
struct has_node_0_rtype {};
typedef RelationDefinition < has_node_0_rtype,
        AcousticTriangle,
        Node > has_node_0;
/// Type of 'has_node_1' relation : second vertex of a face
struct has_node_1_rtype {};
typedef RelationDefinition < has_node_1_rtype,
        AcousticTriangle,
        Node > has_node_1;
/// Type of 'has_node_2' relation : third vertex of a face
struct has_node_2_rtype {};
typedef RelationDefinition < has_node_2_rtype,
        AcousticTriangle,
        Node > has_node_2;

/// Type of 'edge_of' relation.
struct edge_of_rtype {};
/// Relation between 0..n diffraction edges and an \c AcousticTriangle.
typedef RelationDefinition < edge_of_rtype,
        DiffractionEdge,
        AcousticTriangle > edge_of_rdef;

/// Type of 'building_made_of' relation.
struct building_made_of_rtype {};
/// Relation between 0..n acoustic surfaces and an \c AcousticBuildingMaterial.
typedef RelationDefinition < building_made_of_rtype,
        AcousticTriangle,
        AcousticBuildingMaterial > building_made_of_rdef;

/// Type of 'ground_made_of' relation.
struct ground_made_of_rtype {};
/// Relation between 0..n acoustic surfaces and an \c AcousticGroundMaterial.
typedef RelationDefinition < ground_made_of_rtype,
        AcousticTriangle,
        AcousticGroundMaterial > ground_made_of_rdef;

/// Type of 'surface_of' relation.
struct surface_of_rtype {};
/// Relation between 0..n acoustic surfaces and an \c SiteElement.
typedef RelationDefinition < surface_of_rtype,
        AcousticTriangle,
        SiteElement > surface_of_rdef;

/// Type of 'from_user_receptor' relation.
struct from_user_receptor_rtype {};
/// Relation between a site receptor via a \c SiteElement and 0..n \c AcousticReceptor.
typedef RelationDefinition < from_user_receptor_rtype,
        AcousticReceptor,
        SiteElement
         > from_user_receptor_rdef;

/// Type of 'to_receptor' relation.
struct to_receptor_rtype {};
/// Relation between 0..n acoustic paths and a \c AcousticReceptor.
typedef RelationDefinition < to_receptor_rtype,
        AcousticPath,
        AcousticReceptor > to_receptor_rdef;

/// Type of 'receptor' relation.
struct receptor_rtype {};
/// Relation between 0..n global contributions and a \c AcousticReceptor.
typedef RelationDefinition < receptor_rtype,
        GlobalContribution,
        AcousticReceptor > receptor_rdef;

/// Type of 'from_source' relation.
struct from_source_rtype {};
/// Relation between 0..n acoustic path and an \c AcousticSource.
typedef RelationDefinition < from_source_rtype,
        AcousticPath,
        AcousticSource > from_source_rdef;

/// Type of 'source' relation.
struct source_rtype {};
/// Relation between 0..n global contributions and an \c AcousticSource.
typedef RelationDefinition < source_rtype,
        GlobalContribution,
        AcousticSource > source_rdef;

/// Type of 'event_in' relation.
struct event_in_rtype {};
/// Relation between 0..n acoustic events and an \c AcousticPath.
typedef RelationDefinition < event_in_rtype,
        AcousticEvent,
        AcousticPath > event_in_rdef;

/// Type of 'next_event' relation.
struct next_event_rtype {};
/// Relation between 0..1 acoustic event and an \c AcousticEvent.
typedef RelationDefinition < next_event_rtype,
        AcousticEvent,
        AcousticEvent > newt_event_rdef;

/// Type of 'from_element' relation.
struct from_element_rtype {};
/// Relation between 1..n acoustic sources and 0..1 \c SiteElement
typedef RelationDefinition < from_element_rtype,
        AcousticSource,
        SiteElement > from_element_rdef;

/// Type of 'surface' relation.
struct surface_rtype {};
/// Relation between 0..n acoustic events and 0..1 \c AcousticTriangle.
typedef RelationDefinition < surface_rtype,
        AcousticEvent,
        AcousticTriangle > surface_rdef;

/// Type of 'frequencies' relation.
struct frequencies_rtype {};
/// Relation between 0..n acoustic problems and 1..n \c Frequency.
typedef RelationDefinition < frequencies_rtype,
        AcousticProblem,
        Frequency > frequencies_rdef;

/// Type of 'frequency' relation.
struct frequency_rtype {};
/// Relation between 0..n spectrum samples and a \c Frequency.
typedef RelationDefinition < frequency_rtype,
        SpectrumSample,
        Frequency > frequency_rdef;

/// Type of 'of_frequency' relation.
struct of_frequency_rtype {};
/// Relation between 0..n directives coeff. and \c SpectrumSample.
typedef RelationDefinition < of_frequency_rtype,
        DirectivityCoefficient,
        SpectrumSample > of_frequency_rdef;

// Type of 'sample_of' relation.
struct sample_of_rtype {};
/// Relation between 1..n spectrum samples and \c AcousticSpectrum.
typedef RelationDefinition < sample_of_rtype,
        SpectrumSample,
        AcousticSpectrum > sample_of_rdef;

// Type of 'pressure_spectrum' relation.
struct pressure_spectrum_rtype {};
/// Relation between a global constribution and 0..n \c AcousticSpectrum.
typedef RelationDefinition < pressure_spectrum_rtype,
        GlobalContribution,
        AcousticSpectrum > pressure_spectrum_rdef;

// Type of 'emission_spectrum' relation.
struct emission_spectrum_rtype {};
/// Relation between an acoustic source and one \c AcousticSpectrum.
typedef RelationDefinition < emission_spectrum_rtype,
        AcousticSource,
        AcousticSpectrum > emission_spectrum_rdef;

// Type of 'reflection_spectrum' relation.
struct reflection_spectrum_rtype {};
/// Relation between 0..1 acoustic building material and a \c AcousticSpectrum.
typedef RelationDefinition < reflection_spectrum_rtype,
        AcousticBuildingMaterial,
        AcousticSpectrum > reflection_spectrum_rdef;

/// Type of 'site' relation.
struct site_rtype {};
/// Relation between 0..n acoustic problems and 0..n \c Site.
typedef RelationDefinition<site_rtype, AcousticProblem, Site> site_rdef;


} /* namespace tympan */

// This statement must appear in the same namespace as the original template definition
/*
extern template class RelationDefinition < tympan::has_node_rtype,
       tympan::AcousticTriangle,
       tympan::Node >;
*/ // Does not work well on MSVC
#endif /* TYMPAN__RELATIONS_H__INCLUDED */
