/*
 * (C) Copyright 2022 UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include <string>

namespace genint {

class Covariance;
class Geometry;
class Increment;
class LinearVariableChange;
class State;
class VariableChange;

struct Traits {
  static std::string name() {return "genint";}
  static std::string nameCovar() {return "genintCovariance";}

  typedef genint::Covariance           Covariance;
  typedef genint::Geometry             Geometry;
  typedef genint::Increment            Increment;
  typedef genint::LinearVariableChange LinearVariableChange;
  typedef genint::State                State;
  typedef genint::VariableChange       VariableChange;
};

}  // namespace genint
