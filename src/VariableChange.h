/*
 * (C) Copyright 2017-2021 UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include <boost/noncopyable.hpp>

#include <boost/ptr_container/ptr_vector.hpp>

#include "eckit/config/Configuration.h"
#include "oops/base/VariableChangeParametersBase.h"
#include "oops/util/parameters/OptionalParameter.h"
#include "oops/util/parameters/Parameter.h"
#include "oops/util/parameters/Parameters.h"
#include "oops/util/parameters/RequiredParameter.h"
#include "oops/util/Printable.h"

#include "oops/base/VariableChangeParametersBase.h"
#include "oops/base/Variables.h"
#include "oops/util/AssociativeContainers.h"
#include "oops/util/parameters/ConfigurationParameter.h"
#include "oops/util/parameters/HasParameters_.h"
#include "oops/util/parameters/ParametersOrConfiguration.h"
#include "oops/util/parameters/PolymorphicParameter.h"
#include "oops/util/ObjectCounter.h"

#include "vader/vader.h"

#include "src/Geometry.h"
#include "src/State.h"
#include "src/VaderCookbook.h"

namespace genint {
  class Geometry;
  class State;

// -----------------------------------------------------------------------------

class VariableChangeParameters : public oops::VariableChangeParametersBase {
  OOPS_CONCRETE_PARAMETERS(VariableChangeParameters, oops::VariableChangeParametersBase)
 public:
   oops::Parameter<std::map<std::string, std::vector<std::string>>> vaderCustomCookbook{
     "vader custom cookbook", vaderGenintCustomCookbook(), this};
   oops::Parameter<vader::VaderParameters> vaderParam{"vader", {}, this};
};

// -----------------------------------------------------------------------------
/// genint change of variable

class VariableChange : public util::Printable,
                       private util::ObjectCounter<VariableChange> {
 public:
  static const std::string classname() {return "genint::VariableChange";}

  //typedef VariableChangeParameters Parameters_;

  explicit VariableChange(const eckit::Configuration &, const Geometry &);
  ~VariableChange();

/// Perform transforms
  void changeVar(State &, const oops::Variables &) const;
  void changeVarInverse(State &, const oops::Variables &) const;

 private:
  void print(std::ostream &) const override;
  std::map<std::string,std::string> mapVariables_;
  std::unique_ptr<vader::Vader> vader_;
};
// -----------------------------------------------------------------------------

}  // namespace genint
