/**
 * @file SolverDataModelBuilder.cpp
 *
 * @brief Implementation of how to build a solver data model from a site.
 */

#include <boost/foreach.hpp>


#include "Tympan/MetierSolver/SolverDataModel/SolverDataModelBuilder.hpp"

namespace tympan
{

SolverDataModelBuilder::SolverDataModelBuilder()
    : is_frequency(false)
{
    // TODO Auto-generated constructor stub

}

SolverDataModelBuilder::~SolverDataModelBuilder()
{
    // TODO Auto-generated destructor stub
}


////////////////////
// Access methods //
////////////////////




/////////////////////
// Setting methods //
/////////////////////

void SolverDataModelBuilder::walkTroughtSite(LPTYSiteNode site_ptr)
{
    // TODO Implement it.

    // Acoustic geometries.
    setGeometricEntities(site_ptr);
}


void SolverDataModelBuilder::setGeometricEntities(LPTYSiteNode site_ptr)
{

    // Get \c AcousticSurface.
    // Have to link to \c AcousticSurface with relation \c surface_has_node
    unsigned int nb_building_faces = 0;
    // XXX Maybe swith to 'true'.
    bool with_ecran = false;
    TYTabAcousticSurfaceGeoNode face_list;
    std::vector<bool> estUnIndexDeFaceEcran;
    site_ptr->getListFaces(with_ecran, face_list, nb_building_faces,
                           estUnIndexDeFaceEcran);

    // Faces d'infrastructure
    TYAcousticPolygon* pPoly = NULL;
    TYAcousticRectangle* pRect = NULL;
    TYAcousticCircle* pCircle = NULL;
    TYAcousticSemiCircle* pSemiCircle = NULL;

    std::cout << " # Nb of building faces: " << nb_building_faces << std::endl;

    // 'face_list' can contain topography elements. Not relevant here.
    for (unsigned int i = 0 ; i < nb_building_faces ; i++)
    {
        // The uid for SiteElement (parent of an acoustic surface, i.e. the
        // geometric volume).
        string uid;
        if (pPoly = TYAcousticPolygon::safeDownCast(face_list[i]->getElement()))
        {
            // List of points of the surface.
            TYTabPoint tabPoint = pPoly->getContour();
            // Get the volume (and its ID) which contains the surface.
            TYElement* element = pPoly->getParent();
            uid = element->getStringID().toStdString();
            setAcousticSurface(uid, tabPoint);
            // Get building material.
            LPTYMateriauConstruction material_ptr = pPoly->getMateriau();
            setAcousticBuildMaterial(material_ptr);
        }
        else if (pRect = TYAcousticRectangle::safeDownCast(face_list[i]->getElement()))
        {
            TYTabPoint tabPoint = pPoly->getContour();
            TYElement* element = pPoly->getParent();
            uid = element->getStringID().toStdString();
            setAcousticSurface(uid, tabPoint);
            // Get building material.
            LPTYMateriauConstruction material_ptr = pPoly->getMateriau();
            setAcousticBuildMaterial(material_ptr);
        }
        else if (pCircle = TYAcousticCircle::safeDownCast(face_list[i]->getElement()))
        {
            TYTabPoint tabPoint = pPoly->getContour();
            TYElement* element = pPoly->getParent();
            uid = element->getStringID().toStdString();
            setAcousticSurface(uid, tabPoint);
            // Get building material.
            LPTYMateriauConstruction material_ptr = pPoly->getMateriau();
            setAcousticBuildMaterial(material_ptr);
        }
        else if (pSemiCircle = TYAcousticSemiCircle::safeDownCast(face_list[i]->getElement()))
        {
            TYTabPoint tabPoint = pPoly->getContour();
            TYElement* element = pPoly->getParent();
            uid = element->getStringID().toStdString();
            setAcousticSurface(uid, tabPoint);
            // Get building material.
            LPTYMateriauConstruction material_ptr = pPoly->getMateriau();
            setAcousticBuildMaterial(material_ptr);
        }
        else
        {
            continue; // type de face non identifiee
        }
    }
}


void SolverDataModelBuilder::setAcousticSurface(const string& element_uid,
                                                const TYTabPoint& point_list)
{
    boost::shared_ptr<AcousticSurface> face_ptr(new AcousticSurface());
    acoustic_surfaces.push_back(face_ptr);
    // Link surface with SiteElement.
    boost::shared_ptr<SiteElement> element_ptr(new SiteElement(element_uid));
    face_ptr->add<surface_of_rdef>(element_ptr);

    // Create all nodes related to the surface.
    TYTabPoint::const_iterator point_it = point_list.begin();
    for (; point_it != point_list.end(); ++point_it)
    {
        boost::shared_ptr<Node> node_ptr(new Node(*point_it));
        nodes.push_back(node_ptr);
        face_ptr->add<surface_has_node>(node_ptr);
    }
}


void SolverDataModelBuilder::setFrequencyTab(const OTabFreq& freq_tab)
{
    OTabFreq::const_iterator freq_it;
    for (; freq_it != freq_tab.end(); ++freq_it)
    {
        double value = *freq_it;
        boost::shared_ptr<Frequency> freq_ptr(new Frequency(value));
        // Fill the dedicated container.
        frequencies.push_back(freq_ptr);
    }
}


void SolverDataModelBuilder::setAcousticBuildMaterial(LPTYMateriauConstruction material_ptr)
{
    typedef deque<boost::shared_ptr<Frequency> > frequency_container;

    std::cout << " # setAcousticBuildMaterial #" << std::endl;
    // Building material.
    boost::shared_ptr<AcousticBuildingMaterial>
    build_mat_ptr(new AcousticBuildingMaterial());

    // Spectrum from the \c TYMateriauConstruction.
    const TYSpectre& ty_spectrum = material_ptr->getSpectreAbso();

    // Create the entity \c tympan::AcousticSpectrum
    boost::shared_ptr<AcousticSpectrum> acoustic_spectrum_ptr(new AcousticSpectrum());

    // \todo XXX Need to link \c AcousticSpectrum with \c AcousticSource
    // (emission_spectrum_rdef)

    // Link AcousticBuildingMaterial with a \c AcousticSpectrum.
    build_mat_ptr->add<reflection_spectrum_rdef>(acoustic_spectrum_ptr);

    // Freq. range (static table in Tympan). \note frequencies table is unique
    // for a single computation (and static in a \c OSpectre instance). That is
    // why there is the 'is_frequency' attr.
    OTabFreq freq_tab = ty_spectrum.getTabFreqExact();
    if (!is_frequency)
    {
        setFrequencyTab(freq_tab);
        is_frequency = true;
    }

    // Loop on 'tab_modulus'. Get 'modules' and type of spectrum sample.
    const double* tab_modulus = ty_spectrum.getTabValReel();
    string type = "third-octave";
    for (int i = 0; i < ty_spectrum.getNbValues(); ++i)
    {
        double modulus = tab_modulus[i];
        boost::shared_ptr<SpectrumSample> sample_ptr(new SpectrumSample(modulus,
                                                                        type));

        // Link the \c tympan::SpectrumSample with all frequencies via
        // \c tympan::frequency_rdef.
        frequency_container::const_iterator freq_it;
        for (; freq_it != frequencies.end(); ++freq_it)
        {
            sample_ptr->add<frequency_rdef>(*freq_it);
        }

        // Link the spectrum sample with the single \c tympan::AcousticSpectrum via
        // \c tympan::sample_of_rdef.
        sample_ptr->add<sample_of_rdef>(acoustic_spectrum_ptr);
    }
}


} /* namespace tympan */
