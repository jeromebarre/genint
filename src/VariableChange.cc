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
  : mapVariables_(geometry.mapVariables()), vader_() {

  // Create vader cookbook
  vader::cookbookConfigType vaderCustomCookbook =
                                        params.variableChangeParameters.value().vaderCustomCookbook;


  // Create the VaderConstructConfig object
  vader::VaderConstructConfig vaderConstructConfig(vaderCustomCookbook);

  // Add all constants to vader constructor config
  //std::vector<std::string> allConstantsNames = getAllConstantsNames();
  //for (std::string allConstantsName : allConstantsNames) {
  //  vaderConstructConfig.addToConfig<double>(allConstantsName, getConstant(allConstantsName));
  //}

  // Add geometry data to vader constructor config
  vaderConstructConfig.addToConfig<double>("air_pressure_at_top_of_atmosphere_model",
                                           geometry.pTop());
  vaderConstructConfig.addToConfig<std::vector<double>>
                                  ("sigma_pressure_hybrid_coordinate_a_coefficient", geometry.ak());
  vaderConstructConfig.addToConfig<std::vector<double>>
                                  ("sigma_pressure_hybrid_coordinate_b_coefficient", geometry.bk());
  vaderConstructConfig.addToConfig<int>("nLevels", geometry.levels());

  // Create vader with genint custom cookbook
  vader_.reset(new vader::Vader(params.variableChangeParameters.value().vader,
                                vaderConstructConfig));
  // Create the variable change
  variableChange_.reset(VariableChangeFactory::create(geometry,
                                                      params.variableChangeParameters.value()));
}

// -------------------------------------------------------------------------------------------------

VariableChange::~VariableChange() {}

// -------------------------------------------------------------------------------------------------

void VariableChange::changeVar(State & x, const oops::Variables & vars_out) const {
  // Trace
  oops::Log::trace() << "VariableChange::changeVar starting" << std::endl;

  // Use map to get long names
  // ---------------------------
  const std::vector<std::string>& varsVec = vars_out.variables();
  std::map<std::string,std::string> mapVars = mapVariables_;
  std::vector<std::string> longNameVec;
  for (auto &var : varsVec) {
    longNameVec.push_back(mapVars[var]);
  }
  const oops::Variables vars = oops::Variables(longNameVec);

  // Return if required vars in input
  // --------------------------------
  //if (vars <= x.variables()) {
  //  x.updateFields(vars);
  //  oops::Log::info() << "VariableChange::changeVar done (identity)" << std::endl;
  //  return;
  //}

  // Call Vader to perform first set of variable transforms
  // ------------------------------------------------------

  // Record start variables
  oops::Variables varsFilled = x.variables();

  oops::Variables varsVader = vars;
  //varsVader -= varsFilled;  // Pass only the needed variables

  // Call Vader. On entry, varsVader holds the vars requested from Vader; on exit,
  // it holds the vars NOT fulfilled by Vader, i.e., the vars still to be requested elsewhere.
  // vader_->changeVar also returns the variables fulfilled by Vader. These variables are allocated
  // and populated and added to the FieldSet (xfs).
  atlas::FieldSet xfs;
  x.toFieldSet(xfs);
  varsFilled += vader_->changeVar(xfs, varsVader);
  //x.updateFields(varsFilled);
  x.fromFieldSet(xfs);

  // Create output state
  //State xout(x.geometry(), vars, x.time());

  // Remove fields not in output
  //x.updateFields(vars);

  // Copy data from temporary state
  //x = xout;

  // Trace
  oops::Log::trace() << "VariableChange::changeVar done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void VariableChange::changeVarInverse(State & x, const oops::Variables & vars_out) const {
  // Trace
  oops::Log::trace() << "VariableChange::changeVarInverse starting" << std::endl;

  // Use map to get long names
  // ---------------------------
  const std::vector<std::string>& varsVec = vars_out.variables();
  std::map<std::string,std::string> mapVars = mapVariables_;
  std::vector<std::string> longNameVec;
  for (auto &var : varsVec) {
    longNameVec.push_back(mapVars[var]);
  }
  const oops::Variables vars = oops::Variables(longNameVec);

  // Return if required vars in input
  // --------------------------------
  //if (vars <= x.variables()) {
  //  x.updateFields(vars);
  //  oops::Log::info() << "VariableChange::changeVar done (identity)" << std::endl;
  //  return;
  //}

  // Call Vader to perform first set of variable transforms
  // ------------------------------------------------------

  // Record start variables
  oops::Variables varsFilled = x.variables();

  oops::Variables varsVader = vars;
  //varsVader -= varsFilled;  // Pass only the needed variables

  // Call Vader. On entry, varsVader holds the vars requested from Vader; on exit,
  // it holds the vars NOT fulfilled by Vader, i.e., the vars still to be requested elsewhere.
  // vader_->changeVar also returns the variables fulfilled by Vader. These variables are allocated
  // and populated and added to the FieldSet (xfs).
  atlas::FieldSet xfs;
  x.toFieldSet(xfs);
  varsFilled += vader_->changeVar(xfs, varsVader);
  //x.updateFields(varsFilled);
  x.fromFieldSet(xfs);

  // Create output state
  //State xout(x.geometry(), vars, x.time());

  // Remove fields not in output
  //x.updateFields(vars);

  // Copy data from temporary state
  //x = xout;

  // Trace
  oops::Log::trace() << "VariableChange::changeVarInverse done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void VariableChange::print(std::ostream & os) const {
  os << *variableChange_;
}

// -------------------------------------------------------------------------------------------------


}  // namespace genint
