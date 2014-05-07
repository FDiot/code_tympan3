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

AcousticMaterialBase::AcousticMaterialBase(
    const string& name_
) : name(name_) {}

AcousticBuildingMaterial::AcousticBuildingMaterial(
    const string& name_,
    const ComplexSpectrum& spectrum_
) : AcousticMaterialBase(name_), spectrum(spectrum_) {}

AcousticGroundMaterial::AcousticGroundMaterial(
    const string& name_, double resistivity_
) : AcousticMaterialBase(name_), resistivity(resistivity_) {}

AcousticSource::AcousticSource(
    const Point& position_,
    const binary_uuid& id_,
    const Spectrum& spectrum_ )
    : position(position_)
    , id(id_)
    , spectrum(spectrum_)
    , directivity(new SphericalSourceDirectivity())
{}

AcousticReceptor::AcousticReceptor(
    const Point& position_
) : position(position_) {}

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
