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

#include <typeinfo>

#include "oops/mpi/mpi.h"
#include "oops/util/Logger.h"

#include "src/Geometry.h"
#include "src/State.h"
#include "src/Constants.h"
#include "src/VariableChange.h"

namespace genint {

// -------------------------------------------------------------------------------------------------

VariableChange::VariableChange(const Parameters_ & params, const Geometry & geometry)
  : mapVariables_(geometry.mapVariables()), inputParam_(), vader_() {

  // Create vader cookbook
  vader::cookbookConfigType vaderCustomCookbook =
                                        params.vaderCustomCookbook;


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


  inputParam_ = params.inputParam;
  oops::Log::trace() << "inputParam_" << std::endl;
  oops::Log::trace() << inputParam_ << std::endl;

  // Create vader with genint custom cookbook
  vader_.reset(new vader::Vader(params.vaderParam,
                                vaderConstructConfig));
  // Create the variable change
  //variableChange_.reset(VariableChangeFactory::create(geometry,
  //                                                    params.variableChangeParameters.value()));
}

// -------------------------------------------------------------------------------------------------

VariableChange::~VariableChange() {}

// -------------------------------------------------------------------------------------------------

void VariableChange::changeVar(State & x, const oops::Variables & vars_out) const {
  // Trace
  oops::Log::trace() << "VariableChange::changeVar starting" << std::endl;

  // get the input variable change from config
  const std::vector<std::string>& vars_in = inputParam_;
  oops::Variables varsVader = vars_out;

  // replace var names by the long names from the map in config
  // and create the fieldset with the required vars only
  atlas::FieldSet xfs;
  atlas::FieldSet xfsIn;
  x.toFieldSet(xfs);
  const std::vector<std::string>& varsVec = xfs.field_names();
  std::map<std::string,std::string> mapVars = mapVariables_;
  size_t index = 0;
  for (auto &var : varsVec) {
    xfs.field(var).rename(mapVars[var]);
    if (mapVars[var] == vars_in[index]) {
      xfsIn.add(xfs.field(var));
      ++index;
    }
  }

  // call to vader
  vader_->changeVar(xfsIn, varsVader);

  // update the fieldset with the new variables
  atlas::FieldSet xfsOut;
  x.toFieldSet(xfsOut);
  for (auto &var : vars_out.variables()) {
    xfsOut.add(xfsIn.field(var));
  }

  x.fromFieldSet(xfsOut);

  oops::Log::trace() << "VariableChange::changeVar done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void VariableChange::changeVarInverse(State & x, const oops::Variables & vars_out) const {
  // Trace
  oops::Log::trace() << "VariableChange::changeVarInverse starting" << std::endl;

  // get the input variable change from config
  const std::vector<std::string>& vars_in = inputParam_;
  oops::Variables varsVader = vars_out;

  // replace var names by the long names from the map in config
  // and create the fieldset with the required vars only
  atlas::FieldSet xfs;
  atlas::FieldSet xfsIn;
  x.toFieldSet(xfs);
  const std::vector<std::string>& varsVec = xfs.field_names();
  std::map<std::string,std::string> mapVars = mapVariables_;
  size_t index = 0;
  for (auto &var : varsVec) {
    xfs.field(var).rename(mapVars[var]);
    if (mapVars[var] == vars_in[index]) {
      xfsIn.add(xfs.field(var));
      ++index;
    }
  }

  // call to vader
  vader_->changeVar(xfsIn, varsVader);

  // update the fieldset with the new variables
  atlas::FieldSet xfsOut;
  x.toFieldSet(xfsOut);
  for (auto &var : vars_out.variables()) {
    xfsOut.add(xfsIn.field(var));
  }

  x.fromFieldSet(xfsOut);

  oops::Log::trace() << "VariableChange::changeVarInverse done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void VariableChange::print(std::ostream & os) const {
  os << *vader_;
}

// -------------------------------------------------------------------------------------------------


}  // namespace genint
