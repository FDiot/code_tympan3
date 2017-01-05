#ifndef TY_MODELS_COMMON_MATRIX
#define TY_MODELS_COMMON_MATRIX

#include<vector>

#include "Tympan/models/common/spectre.h"


namespace tympan
{
typedef OSpectre Spectrum;

/**
 * \brief Spectrum matrix N*M used to store results.
 * N is the number of receptors.
 * M is the number of sources.
 */
class SpectrumMatrix
{
public:

    typedef std::vector<std::vector<Spectrum> > impl_matrix_t;

    /// Constructors
    SpectrumMatrix();
    SpectrumMatrix(size_t nb_receptors, size_t nb_sources);
    SpectrumMatrix(const SpectrumMatrix& matrix);
    virtual ~SpectrumMatrix() {};

    /// Number of columns (sources) of the matrix
    size_t nb_sources()   const { return _nb_sources; };
    /// Number of rows (receptors) of the matrix
    size_t nb_receptors() const { return data.size(); };

    const Spectrum& operator()(size_t receptor_idx, size_t sources_idx) const;
    Spectrum& operator()(size_t receptor_idx, size_t sources_idx);
    /// Set a Spectrum into the matrix
    void setSpectre(size_t receptor_idx, size_t sources_idx, Spectrum spectrum);

    const std::vector<Spectrum>& by_receptor(size_t receptor_idx) const;

    void clearReceptor(size_t receptor_idx);

    /// Clear the matrix
    void clear() {data.clear(); };

    /// Resize the matrix (data is cleared)
    void resize(size_t nb_receptors, size_t nb_sources);

protected:
    impl_matrix_t data;

private:
    size_t _nb_sources;

}; // class SpectrumMatrix

}

#endif
