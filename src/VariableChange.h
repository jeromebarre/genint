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

#include <boost/ptr_container/ptr_vector.hpp>

#include "oops/base/VariableChangeParametersBase.h"
#include "oops/util/parameters/OptionalParameter.h"
#include "oops/util/parameters/Parameter.h"
#include "oops/util/parameters/Parameters.h"
#include "oops/util/parameters/RequiredParameter.h"
#include "oops/util/Printable.h"

#include "vader/vader.h"

#include "src/Geometry.h"
#include "src/State.h"
#include "src/VariableChangeBase.h"

namespace genint {
  class Geometry;
  class State;

// -----------------------------------------------------------------------------
/// genint change of variable

class VariableChange : public util::Printable {
 public:
  static const std::string classname() {return "genint::VariableChange";}

  typedef VariableChangeParametersWrapper Parameters_;

  explicit VariableChange(const Parameters_ &, const Geometry &);
  ~VariableChange();

/// Perform transforms
  void changeVar(State &, const oops::Variables &) const;
  void changeVarInverse(State &, const oops::Variables &) const;

 private:
  void print(std::ostream &) const override;
  std::unique_ptr<VariableChangeBase> variableChange_;
  std::map<std::string,std::string> mapVariables_;
  std::unique_ptr<vader::Vader> vader_;
};
// -----------------------------------------------------------------------------

}  // namespace genint
