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
#include "oops/util/FieldSetHelpers.h"

#include "src/Geometry.h"
#include "src/State.h"
#include "src/Constants.h"
#include "src/VariableChange.h"

namespace genint {

// -------------------------------------------------------------------------------------------------

VariableChange::VariableChange(const Parameters_ & params, const Geometry & geometry)
    : mapVariables_(geometry.mapVariables()), vader_() {
//  : mapVariables_(geometry.mapVariables()), inputParam_(), vader_() {


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


  //inputParam_ = params.inputParam;

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
  oops::Log::trace() << x << std::endl;

  oops::Variables varsCha = vars_out;
  oops::Log::trace() <<  "varsCha" << std::endl;
  oops::Log::trace() <<  varsCha << std::endl;


  atlas::FieldSet xfsIn;
  x.toFieldSet(xfsIn);
  // FIXME: this is a hack to avoid mpi issues.
  // State will create a fields set to 0 if the variable is not in file
  // assuming this will be result of the variable change.
  // If an additional variable is added to the state without being
  // in the constructor in the first place this causes memory issues.
  // TODO: State::updateFields should has the capability to deal with the memory
  // allocation
  oops::Variables varsMove;
  for (auto &var : varsCha.variables()){
    oops::Log::trace() << var << " norm: " << x.normVar(var) << std::endl;
    if (x.normVar(var) == 0) {
      varsMove.push_back(var);
    }
  }
  util::removeFieldsFromFieldSet(xfsIn, varsMove.variables());

  oops::Log::trace() <<  "varsMove" << std::endl;
  oops::Log::trace() <<  varsMove << std::endl;

  const std::vector<std::string>& varsVec = xfsIn.field_names();
  oops::Log::trace() <<  "varsVec" << std::endl;
  oops::Log::trace() <<  varsVec << std::endl;

  std::map<std::string,std::string> mapVars = mapVariables_;
  oops::Log::trace() <<  "mapVars: " << std::endl;
  oops::Log::trace() <<  mapVars << std::endl;


  // replace var names by the long names from the map in config
  // and create the fieldset with the required vars only
  atlas::FieldSet xfsVader;
  for (auto &var : varsVec) {
    xfsIn.field(var).rename(mapVars[var]);
    xfsVader.add(xfsIn.field(var));
  }
  // call to vader
  oops::Variables varsState =  x.variables();
  //oops::Variables varsMove = x.variables();
  oops::Variables varsAdd = x.variables();


  varsAdd += vader_->changeVar(xfsVader, varsCha);
  varsAdd -= varsState;
  oops::Log::trace() << "vader_->changeVar done" << std::endl;

  x.updateFields(varsAdd);
  //oops::Log::trace() << x << std::endl;

  atlas::FieldSet xfsOut;
  x.toFieldSet(xfsOut);
  util::removeFieldsFromFieldSet(xfsOut, varsMove.variables());
  for (auto &var : varsMove.variables()) {
    xfsOut.add(xfsVader.field(var));
  }

  oops::Log::trace() <<  "xfsOut" << std::endl;
  oops::Log::trace() <<  xfsOut.field_names() << std::endl;

  // x.fields() = xfsOut;
  // oops::Log::trace() << "x.fields() = xfsOut" << std::endl;
  // oops::Log::trace() << x << std::endl;
  //
  // atlas::Field prsi = xfsVader.field("air_pressure_levels");
  // const int gridSize = prsi.shape(0);
  // const int nLevels = prsi.shape(1);
  // auto prsi_view = atlas::array::make_view<double, 2>(prsi);
  // for (int level = 0; level < nLevels + 1; ++level) {
  //     for ( size_t jNode = 0; jNode < gridSize ; ++jNode ) {
  //         oops::Log::trace() <<  prsi_view(jNode, level) << std::endl;
  //     }
  // }

  // State xout(x);
  // oops::Log::trace() << xout.variables() << std::endl;

  // atlas::Field prsi = xfsOut.field("air_pressure_levels");
  // const int gridSize = prsi.shape(0);
  // const int nLevels = prsi.shape(1);
  // auto prsi_view = atlas::array::make_view<double, 2>(prsi);
  // for (int level = 0; level < nLevels + 1; ++level) {
  //     for ( size_t jNode = 0; jNode < gridSize ; ++jNode ) {
  //       if (prsi_view(jNode, level)!=0) {
  //         oops::Log::trace() <<  prsi_view(jNode, level) << std::endl;
  //       }
  //     }
  // }


  x.fromFieldSet(xfsOut);
  for (auto &var : varsMove.variables()) {
    oops::Log::trace() << "check var! varcha:" << std::endl;
    oops::Log::trace() << xfsOut.field(var) << std::endl;
  }


  // x = xout;






  // oops::Log::trace() <<  "vader_->changeVar done" << std::endl;
  // oops::Log::trace() <<  xfsIn.field_names() << std::endl;
  // oops::Log::trace() <<  xfsVader.field_names() << std::endl;
  //
  //
  // std::vector<std::string> varsIn = xfsIn.field_names();
  // std::vector<std::string> varsVader = xfsVader.field_names();
  // std::vector<std::string> varsAdd;
  //
  //
  // std::set_difference(varsVader.begin(), varsVader.end(),
  //                     varsIn.begin(), varsIn.end(),
  //                     std::inserter(varsAdd, varsAdd.begin()));

  // update the fieldset with the new variables
  // atlas::FieldSet xfsOut;
  // x.toFieldSet(xfsOut);
  // for (auto &var : varsAdd) {
  //   xfsOut.add(xfsVader.field(var));
  // }
  // oops::Log::trace() << xfsOut.field_names() << std::endl;
  // x.fromFieldSet(xfsOut);


  oops::Log::trace() << "VariableChange::changeVar done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void VariableChange::changeVarInverse(State & x, const oops::Variables & vars_out) const {
  // Trace
  oops::Log::trace() << "VariableChange::changeVarInverse starting" << std::endl;

  // get the input variable change from config
  oops::Variables varsVader = vars_out;

  // replace var names by the long names from the map in config
  // and create the fieldset with the required vars only
  atlas::FieldSet xfs;
  x.toFieldSet(xfs);
  const std::vector<std::string>& varsVec = xfs.field_names();
  std::map<std::string,std::string> mapVars = mapVariables_;
  for (auto &var : varsVec) {
    xfs.field(var).rename(mapVars[var]);
  }

  // call to vader
  vader_->changeVar(xfs, varsVader);

  // update the fieldset with the new variables
  oops::Log::trace() << varsVec << std::endl;
  for (auto &var : varsVec) {
    xfs.field(var).rename(var);
  }
  x.fromFieldSet(xfs);

  oops::Log::trace() << "VariableChange::changeVarInverse done" << std::endl;
}

// -------------------------------------------------------------------------------------------------

void VariableChange::print(std::ostream & os) const {
  os << *vader_;
}

// -------------------------------------------------------------------------------------------------


}  // namespace genint
