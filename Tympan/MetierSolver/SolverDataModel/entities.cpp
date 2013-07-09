/**
 * \file entities.cpp
 *
 *  \date  nov. 6 2012
 *  \author Anthony Truchet <anthony.truchet@logilab.fr>
 *  \author Damien Garaud <damien.garaud@logilab.fr>
 */

#include "entities.hpp"

#include "relations.hpp"

namespace tympan
{

Node::Node(
    const Point& p_
) : p(p_) {}

DiffractionEdge::DiffractionEdge(
    const Point& p1_, const Point& p2_, double angle_
) : p1(p1_), p2(p2_), angle(angle_) {}

AcousticBuildingMaterial::AcousticBuildingMaterial(
) : name("") {}

AcousticBuildingMaterial::AcousticBuildingMaterial(
    const string& name_
) : name(name_) {}

AcousticGroundMaterial::AcousticGroundMaterial(
    const string& name_, double resistivity_
) : name(name_), resistivity(resistivity_) {}

AcousticReceptor::AcousticReceptor(
    const Point& point_
) : point(point_) {}

AcousticEvent::AcousticEvent(
    const string& event_type_, const Point& point_
) : event_type(event_type_), point(point_) {}

AcousticProblem::AcousticProblem(
    const string& name_
) : name(name_) {}


SiteElement::SiteElement(
    const binary_uuid& uid_
) : uid(uid_) {}

SiteAcousticReceptor::SiteAcousticReceptor(
    unsigned int id_
) : id(id_) {}

Site::Site(
    unsigned int id_
) : id(id_) {}


SpectrumSample::SpectrumSample(
    double modulus_, const string& type_, double phase_
) : modulus(modulus_), type(type_), phase(phase_) {}

DirectivityCoefficient::DirectivityCoefficient(
    double value_, double theta_, double phi_, bool solid_angle_
) : value(value_), theta(theta_), phi(phi_), solid_angle(solid_angle_) {}

Frequency::Frequency(
    double value_
) : value(value_) {}


/**
 * @brief provide arbitrary order over Nodes
 * @param lhs pointer to the left hand side node
 * @param rhs pointer to the right hand side node
 * @return the strict (lexical) order of \c lhs relative to \c rhs
 */
bool operator<(Node::pointer lhs, Node::pointer rhs)
{
	for(unsigned int k=0; k<3; k++)
		if (lhs->p._value[k] < rhs->p._value[k])
			return true;
	return false;
}

AcousticTriangle::pointer
AcousticTriangle::make_triangle(
		const Node::pointer& p1, const Node::pointer& p2, const Node::pointer& p3)
{
	Node::pointer tab[3] = {p1, p2, p3};
	return make_triangle(&tab);
}

AcousticTriangle::pointer
AcousticTriangle::make_triangle(Node::pointer (* const points)[3])
{
	AcousticTriangle::pointer surf = make<AcousticTriangle>();
	Node::pointer tab[3];

	// We determine the index k0 of the "smallest" of the three vertices
	Node::pointer * const begin = &(*points)[0];
	Node::pointer * const end = &(*points)[3];
	const unsigned k0 = std::min_element(begin, end) - begin;
	// We fill the tab array starting from k0
	for(unsigned int k=k0; k<k0+3; k++)
		tab[k%3] = (*points)[k];
	surf->add<has_node_0>(tab[0]);
	surf->add<has_node_1>(tab[1]);
	surf->add<has_node_2>(tab[2]);
	return surf;
}


// XXX Copy/paste these methods to the \c AcousticGroundMaterial See
// Tympan/MetierSolver/DataManagerMetier/EltMateriaux/TYSol.h|TYSol.cpp These
// commented lines are here in order to list the methods of TYSol to use in the
// future AcousticGroundMaterial.

// /**
//  * Set/Get de l'impedance.
//  */
// const OSpectreComplex getImpedance() const { return *_pImpedance; }

// /**
// * Set/Get de l'impedance.
// */
// void setImpedance(const OSpectreComplex imp) { _pImpedance = new OSpectreComplex(imp); /*_pImpedance->setParent(this);*/ }

// /**
// * Calule le spectre d'absorption du sol
// * en fonction de l'angle d'incidence de l'onde acoustique.
// *
// * @param angle Angle d'incidence de l'onde acoustique.
// * @param Rr distance du trajet reflechi sur le sole entre le point de reflexion et le point suivant
// * @param pAtmo atmosphere courante du calcul
// */
// OSpectreComplex abso(const double& angle, const double& rR, const TYAtmosphere& Atmo) const;

// /**
// * Calcul de Zc, impedance caracteristique du sol a partir de
// * la resistance a l'ecoulement du sol exprimee en kRayl.
// * d'apres la formulation de DELANY-BAZLEY
// *
// * @return Le spectre de l'impedence caracteristique du sol
// */
// void calculZc() const;

// /**
// * Calcul de l'impedance specifique du sol Zc
// */
// void calculZs(const double phi, const TYAtmosphere& atmos) const;

// /**
// * Calcul de w d'apres : "H. JONASSON : The propagation and screening of wheel/rail noise"
// *
// * @param rR distance du trajet reflechi sur le sol entre la source et le point recepteur
// * @param phi angle d'incidence de l'onde acoustique
// *
// * @return w un OSpectreComplex
//      */
// OSpectreComplex calculW(const double rR, const double phi, const TYAtmosphere& atmos) const;

// public:  //Temporairement
// /**
// * Calcul de la fonction Fw facteur de perte pour l'effet de sol
// *
// * @param w LPTYSpectre
// * @return fw LPTYSpectre representant le facteur de perte
// */
// OSpectreComplex calculFw(const OSpectreComplex& w) const;

// /**
//  * Calcul de K, nombre d'onde dans la surface du sol
//  * d'apres la formulation de DELANY-BAZLEY
//  *
//  * @return Le spectre du nombre d'onde dans la surface du sol
//  */
// void calculNombreDOnde(const TYAtmosphere& atmos) const;
// /**
//  * Calcul du coefficient de reflexion du sol en onde plane
//  *
//  * @param phi Angle d'incidence de l'onde acoustique
//  *
//  * @return rP OSpectreComplex
//  */
// OSpectreComplex calculRp(const double& phi) const;

// /**
//  * Calcul du coefficient de reflexion du sol en ondes spheriques
//  *
//  * @param phi Angle d'incidence de l'onde acoustique
//  * @param rp coefficient de reflexion du sol en ondes planes
//  *
//  * @return Q OSpectreComplex
//  */
// OSpectreComplex calculQ(const double& phi, const OSpectreComplex& rp, const OSpectreComplex& fw) const ;

// /**
//  * \fn calculQ(const double& phi, const OSpectreComplex & fw) const
//  * \brief Calcul du coefficient de reflexion du sol en ondes spheriques
//  * \param phi Angle d'incidence de l'onde acoustique
//  * \param fw spectre
//  * \return Q OSpectreComplex
//  */
// OSpectreComplex calculQ(const double& phi, const OSpectreComplex& fw) const;

// /**
//  * Fonctions utilisees dans le calcul de la fonction Fw
//  *
//  * @param un complexe appartenant au vecteur [w]
//  *
//  * @return un complexe repondant au cas specifie
//  */
// TYComplex erfcCas1(const TYComplex& wValue) const;
// inline TYComplex erfcCas2(const TYComplex& wValue) const;
// inline TYComplex erfcCas3(const TYComplex& wValue) const;

// /**
//  * Fonction utilisee dans le traitement du signe de G
//  *
//  * @param TYComplex W Valeur locale de la fonction W
//  * @param TYComplex G Valeur locale de la fonction G
//  *
//  * @return TYComplex
//  */
// inline TYComplex sgnReIm(const TYComplex& W, const TYComplex& G) const ;



} /* namespace tympan */
