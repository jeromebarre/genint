/*
 * (C) Copyright 2021 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include "oops/base/ModelSpaceCovarianceParametersBase.h"

#include "src/LinearVariableChange.h"
#include "src/TraitsFwd.h"

namespace genint {
// -----------------------------------------------------------------------------
/// \brief Parameters passed to the Covariance class.

class CovarianceParameters : public oops::ModelSpaceCovarianceParametersBase<genint::Traits>{
  OOPS_CONCRETE_PARAMETERS(CovarianceParameters,
                           oops::ModelSpaceCovarianceParametersBase<genint::Traits>)

 public:
};
// -----------------------------------------------------------------------------

}  // namespace genint
