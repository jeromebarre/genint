/*
 * (C) Copyright 2021-2023 UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <map>
#include <ostream>
#include <string>
#include <vector>

#include "oops/mpi/mpi.h"
#include "oops/util/Logger.h"

#include "src/Geometry.h"
#include "src/State.h"
#include "src/Constants.h"
#include "src/VariableChange.h"

namespace genint {

// -------------------------------------------------------------------------------------------------

VariableChange::VariableChange(const Parameters_ & params, const Geometry & geometry)
  : fieldsMetadata_(geometry.fieldsMetaData())) {

  // Create vader cookbook
  vader::cookbookConfigType vaderCustomCookbook =
                                        params.variableChangeParameters.value().vaderCustomCookbook;


  // Create the VaderConstructConfig object
  vader::VaderConstructConfig vaderConstructConfig(vaderCustomCookbook);

  // Add all constants to vader constructor config
  std::vector<std::string> allConstantsNames = getAllConstantsNames();
  for (std::string allConstantsName : allConstantsNames) {
    vaderConstructConfig.addToConfig<double>(allConstantsName, getConstant(allConstantsName));
  }

  // Add geometry data to vader constructor config
  vaderConstructConfig.addToConfig<double>("air_pressure_at_top_of_atmosphere_model",
                                           geometry.pTop());
  vaderConstructConfig.addToConfig<std::vector<double>>
                                  ("sigma_pressure_hybrid_coordinate_a_coefficient", geometry.ak());
  vaderConstructConfig.addToConfig<std::vector<double>>
                                  ("sigma_pressure_hybrid_coordinate_b_coefficient", geometry.bk());
  vaderConstructConfig.addToConfig<int>("nLevels", geometry.levels());

  // Create vader with fv3-jedi custom cookbook
  vader_.reset(new vader::Vader(params.variableChangeParameters.value().vader,
                                vaderConstructConfig));
  // Create the variable change
  variableChange_.reset(VariableChangeFactory::create(geometry,
                                                      params.variableChangeParameters.value()));
}

// -------------------------------------------------------------------------------------------------

VariableChange::~VariableChange() {}

}  // namespace genint
