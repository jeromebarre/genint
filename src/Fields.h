/*
 * (C) Copyright 2022 UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "atlas/field.h"

#include "oops/base/Variables.h"
#include "oops/util/abor1_cpp.h"
#include "oops/util/DateTime.h"
#include "oops/util/ObjectCounter.h"
#include "oops/util/Printable.h"
#include "oops/util/Serializable.h"

namespace genint {
  class Geometry;

// -----------------------------------------------------------------------------
/// Class to represent a Fields for the  model
class Fields : public util::Printable,
               public util::Serializable,
               private util::ObjectCounter<Fields> {
 public:
  static const std::string classname() {return "genint::Fields";}

// Constructors
  Fields(const Geometry &, const oops::Variables &,
         const oops::Variables &, const util::DateTime &,
         const eckit::Configuration &);
  Fields(const Geometry &, const oops::Variables &,
         const util::DateTime &);
  Fields(const Fields &, const Geometry &);
  Fields(const Fields &, const bool);
  Fields(const Fields &);
  ~Fields() {}

// Basic operators
  void zero();
  void constantValue(const double &);
  Fields & operator=(const Fields &);
  Fields & operator+=(const Fields &);
  Fields & operator-=(const Fields &);
  Fields & operator*=(const double &);
  void axpy(const double &, const Fields &);
  double dot_product_with(const Fields &) const;
  void schur_product_with(const Fields &);
  void dirac(const eckit::Configuration &);
  void random();
  void diff(const Fields &, const Fields &);

// ATLAS FieldSet
  void removeFields(atlas::FieldSet &, const oops::Variables &);
  void updateFields(const oops::Variables &);
  void mergeFieldSets(atlas::FieldSet &);
  void toFieldSet(atlas::FieldSet &) const;
  void fromFieldSet(const atlas::FieldSet &);

// Utilities
  void read(const eckit::Configuration &);
  void write(const eckit::Configuration &) const;
  double norm() const;
  double normVar(const std::string &) const;
  std::shared_ptr<const Geometry> geometry() const {return geom_;}
  const oops::Variables & variables() const {return vars_;}
  const util::DateTime & time() const {return time_;}
  util::DateTime & time() {return time_;}
  void updateTime(const util::Duration & dt) {time_ += dt;}

/// Serialization
  size_t serialSize() const override;
  void serialize(std::vector<double> &) const override;
  void deserialize(const std::vector<double> &, size_t &) override;

  atlas::FieldSet & fields() {return fset_;}

 private:
  void print(std::ostream &) const override;
  std::shared_ptr<const Geometry> geom_;
  oops::Variables vars_;
  oops::Variables varsf_;
  util::DateTime time_;
  atlas::FieldSet fset_;
};
// -----------------------------------------------------------------------------

}  // namespace genint
