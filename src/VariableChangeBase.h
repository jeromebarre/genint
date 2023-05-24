/*
 * (C) Copyright 2022 UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <boost/noncopyable.hpp>

#include "oops/base/VariableChangeParametersBase.h"
#include "oops/base/Variables.h"
#include "oops/util/AssociativeContainers.h"
#include "oops/util/parameters/ConfigurationParameter.h"
#include "oops/util/parameters/HasParameters_.h"
#include "oops/util/parameters/OptionalParameter.h"
#include "oops/util/parameters/Parameters.h"
#include "oops/util/parameters/ParametersOrConfiguration.h"
#include "oops/util/parameters/PolymorphicParameter.h"
#include "oops/util/Printable.h"
#include "vader/VaderParameters.h"

#include "src/Geometry.h"
#include "src/State.h"

namespace genint {
  class Geometry;
  class State;

// -------------------------------------------------------------------------------------------------

class VariableChangeParametersBase : public oops::VariableChangeParametersBase {
  OOPS_ABSTRACT_PARAMETERS(VariableChangeParametersBase, oops::VariableChangeParametersBase)
 public:
   oops::OptionalParameter<std::string> name{"variable change name", this};
   oops::Parameter<std::map<std::string, std::vector<std::string>>> vaderCustomCookbook{
     "vader custom cookbook",
     // Default VADER cookbook for genint
     {
       // P: from delp, from ps (and ak/bk)
       {"air_pressure_levels",          {"AirPressureAtInterface_B", "AirPressureAtInterface_A"}}
     },
     this};
   oops::Parameter<vader::VaderParameters> vader{"vader", {}, this};
};

// -------------------------------------------------------------------------------------------------

class GenericVariableChangeParameters : public VariableChangeParametersBase {
  OOPS_CONCRETE_PARAMETERS(GenericVariableChangeParameters, VariableChangeParametersBase)
 public:
  oops::ConfigurationParameter config{this};
};

// -------------------------------------------------------------------------------------------------

class VariableChangeBase : public util::Printable, private boost::noncopyable {
 public:
  VariableChangeBase() {}
  virtual ~VariableChangeBase() {}

  virtual void changeVar(const State &, State &) const = 0;
  virtual void changeVarInverse(const State &, State &) const = 0;

 private:
  virtual void print(std::ostream &) const = 0;
};

// -------------------------------------------------------------------------------------------------

class VariableChangeFactory;

// -------------------------------------------------------------------------------------------------

class VariableChangeParametersWrapper : public oops::VariableChangeParametersBase {
  OOPS_CONCRETE_PARAMETERS(VariableChangeParametersWrapper, oops::VariableChangeParametersBase)
 public:
  oops::PolymorphicParameter<genint::VariableChangeParametersBase, VariableChangeFactory>
    variableChangeParameters{"variable change name", "default", this};
};

// -------------------------------------------------------------------------------------------------

class VariableChangeFactory {
 public:
  static VariableChangeBase * create(const Geometry & geom,
                                      const VariableChangeParametersBase & params);

  static std::unique_ptr<VariableChangeParametersBase> createParameters(const std::string &name);

  static std::vector<std::string> getMakerNames() {
    return oops::keys(getMakers());
  }

  virtual ~VariableChangeFactory() = default;

 protected:
  explicit VariableChangeFactory(const std::string &name);

 private:
  virtual VariableChangeBase * make(const Geometry &, const VariableChangeParametersBase &) = 0;

  virtual std::unique_ptr<VariableChangeParametersBase> makeParameters() const = 0;

  static std::map < std::string, VariableChangeFactory * > & getMakers() {
    static std::map < std::string, VariableChangeFactory * > makers_;
    return makers_;
  }
};

// -------------------------------------------------------------------------------------------------

template<class T>
class VariableChangeMaker : public VariableChangeFactory {
  typedef oops::TParameters_IfAvailableElseFallbackType_t<T, GenericVariableChangeParameters>
    Parameters_;

  VariableChangeBase * make(const Geometry & geom,
                             const VariableChangeParametersBase & params) override {
    const auto &stronglyTypedParams = dynamic_cast<const Parameters_&>(params);
    return new T(geom, oops::parametersOrConfiguration<oops::HasParameters_<T>::value>(
                   stronglyTypedParams));
  }

  std::unique_ptr<VariableChangeParametersBase> makeParameters() const override {
    return std::make_unique<Parameters_>();
  }

 public:
  explicit VariableChangeMaker(const std::string & name) : VariableChangeFactory(name) {}
};

}  // namespace genint
