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

#include <memory>

#include "Tympan/models/common/spectrum_matrix.h"
#include "Tympan/models/common/acoustic_path.h"
#include "data_model_common.hpp"
#include "entities.hpp"

namespace tympan
{
/**
 * @brief Contains the results of the model solved
 */
class AcousticResultModel
{
public:
    AcousticResultModel() {};

    virtual ~AcousticResultModel();

    SpectrumMatrix& get_data() { return acoustic_data; } //!< Return the results matrix
    tab_acoustic_path& get_path_data() { return path_data; } //!< Return the array of the acoustic paths

protected: // data members

    SpectrumMatrix acoustic_data;  //!< Matrix of the spectrum results
    tab_acoustic_path path_data;   //!< Array of the acoustic paths

};  // class AcousticResultModel

std::unique_ptr<AcousticResultModel> make_AcousticResultModel();

} // namespace tympan



#endif // TYMPAN__ACOUSTIC_RESULT_MODEL_H__INCLUDED
