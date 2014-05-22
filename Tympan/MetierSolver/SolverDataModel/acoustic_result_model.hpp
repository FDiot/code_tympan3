/**
 * \file acoustic_result_model.hpp
 * \brief This file provides the top-level declaration for the acoustic
 * result model
 *
 * \date march 13 2014
 * \author Laura Médioni <laura.medioni@logilab.fr>
 */

#ifndef TYMPAN__ACOUSTIC_RESULT_MODEL_H__INCLUDED
#define TYMPAN__ACOUSTIC_RESULT_MODEL_H__INCLUDED

#include <cstddef>
#include <vector>

#include "data_model_common.hpp"
#include "entities.hpp"

namespace tympan
{

class SpectrumMatrix
{
public:

    typedef std::vector<std::vector<Spectrum> > impl_matrix_t;

    SpectrumMatrix();
    SpectrumMatrix(size_t nb_receptors, size_t nb_sources);
    virtual ~SpectrumMatrix() {};

    size_t nb_sources()   const { return _nb_sources; };
    size_t nb_receptors() const { return data.size(); };

    const Spectrum& operator()(size_t receptor_idx, size_t sources_idx) const;
    Spectrum& operator()(size_t receptor_idx, size_t sources_idx);

    const std::vector<Spectrum>& by_receptor(size_t receptor_idx) const;

    void clearReceptor(size_t receptor_idx);

    void clear() {data.clear(); };

protected:
    impl_matrix_t data;
private:
    size_t _nb_sources;

}; // class SpectrumMatrix


class AcousticResultModel
{
public:

    AcousticResultModel();
    virtual ~AcousticResultModel();
protected: // data members

};  // class AcousticResultModel


} // namespace tympan




#endif // TYMPAN__ACOUSTIC_RESULT_MODEL_H__INCLUDED
