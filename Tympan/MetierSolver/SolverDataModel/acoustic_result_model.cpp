/**
* \file acoustic_result_model.cpp
* \brief Implementation for top-level of the acoustic result model
*
* \date april 2nd 2014
* \author Laura Médioni <laura.medioni@logilab.fr>
*/

#include "acoustic_result_model.hpp"


namespace tympan
{

    SpectrumMatrix::SpectrumMatrix(size_t nb_sources, size_t nb_receptors)
        : _nb_sources(nb_sources)
    {
        data.clear();
        data.reserve(nb_receptors);

        Spectrum nullSpectrum(0);
        nullSpectrum.setType(SPECTRE_TYPE_LP);
        nullSpectrum.setEtat(SPECTRE_ETAT_LIN);

        for (size_t i = 0; i < nb_receptors; i++)
        {
            data.push_back(std::vector<Spectrum>(nb_sources, nullSpectrum));
        }
        assert(data.size()==nb_receptors);
    }

    SpectrumMatrix::SpectrumMatrix() : _nb_sources(0) {}

    const Spectrum& SpectrumMatrix::operator()(size_t receptor_idx, size_t sources_idx) const
    {
        assert(receptor_idx < nb_receptors());
        assert(sources_idx < nb_sources());
        return data[receptor_idx][sources_idx];
    }

    Spectrum& SpectrumMatrix::operator()(size_t receptor_idx, size_t sources_idx)
    {
        assert(receptor_idx < nb_receptors());
        assert(sources_idx < nb_sources());
        return data[receptor_idx][sources_idx];
    }

    const std::vector<Spectrum>& SpectrumMatrix::by_receptor(size_t receptor_idx) const
    {
        assert(receptor_idx < nb_receptors());
        return data[receptor_idx];
    }

    void SpectrumMatrix::clearReceptor(size_t receptor_idx)
    {
        assert(receptor_idx < nb_receptors());
        data[receptor_idx].clear();
    }

    AcousticResultModel::AcousticResultModel() 
    {
    };

    AcousticResultModel::~AcousticResultModel() 
    {
    };

} // namespace tympan
