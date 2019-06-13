
#include "Tympan/models/common/spectrum_matrix.h"

namespace tympan
{
    SpectrumMatrix::SpectrumMatrix(size_t nb_receptors,size_t nb_sources)
        : _nb_sources(nb_sources)
    {
        resize(nb_receptors, nb_sources);
    }

    SpectrumMatrix::SpectrumMatrix() : _nb_sources(0) {}

    SpectrumMatrix::SpectrumMatrix(const SpectrumMatrix& matrix)
    {
        // Build matrix
        resize(matrix.data.size(), matrix._nb_sources);
        // number of sources
        _nb_sources = matrix._nb_sources;
        // Copy spectra
        for (size_t i = 0; i < data.size(); i++)
        {
            for(size_t j = 0; j < _nb_sources; j++)
            {
                data[i][j] = matrix.data[i][j];
            }
        }
    }

     void SpectrumMatrix::resize(size_t nb_receptors, size_t nb_sources)
     {
        _nb_sources = nb_sources;

        if(data.size() > 0){
            data.clear();
        }

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

    void SpectrumMatrix::setSpectre(size_t receptor_idx, size_t sources_idx, Spectrum spectrum)
    {
        assert(receptor_idx < nb_receptors());
        assert(sources_idx < nb_sources());
        data[receptor_idx][sources_idx] = spectrum;
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
}

