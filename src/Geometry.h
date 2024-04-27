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
#include <unordered_map>
#include <vector>

#include "atlas/field.h"
#include "atlas/functionspace.h"
#include "atlas/grid.h"

#include "eckit/config/Configuration.h"
#include "eckit/mpi/Comm.h"

#include "oops/mpi/mpi.h"
#include "oops/util/Logger.h"

#include "oops/base/Variables.h"
#include "oops/util/ObjectCounter.h"
#include "oops/util/parameters/OptionalParameter.h"
#include "oops/util/parameters/Parameter.h"
#include "oops/util/parameters/Parameters.h"
#include "oops/util/parameters/RequiredParameter.h"
#include "oops/util/Printable.h"

namespace genint {

// -----------------------------------------------------------------------------
/// Group parameters

class GroupParameters : public oops::Parameters {
  OOPS_CONCRETE_PARAMETERS(GroupParameters, Parameters)

 public:

  /// JEDI variables
  oops::RequiredParameter<std::map<std::string,std::string>> mapVariables{"map jedi names", this};

  /// Number of levels
  oops::Parameter<size_t> levels{"levels", 1, this};

  /// Corresponding level for 2D variables (first or last)
  oops::Parameter<std::string> lev2d{"lev2d", "first", this};

  // Vertical coordinate type
  oops::Parameter<std::string> verticalCoordinate{"vertical coordinate", "deltap", this};

  // Top pressure
  oops::Parameter<double> pTop{"top pressure", 0.0, this};

  /// Vertical unit
  oops::OptionalParameter<std::vector<double>> vunit{"vunit", this};

  /// Sigma pressure coefs
  oops::OptionalParameter<std::vector<double>> ak{"ak", this};
  oops::OptionalParameter<std::vector<double>> bk{"bk", this};

  /// Mask type
  oops::Parameter<std::string> maskType{"mask type", "none", this};

  /// Mask path
  oops::Parameter<std::string> maskPath{"mask path", "../genint/data/landsea.nc", this};

  // Base Potential Temperature
  oops::OptionalParameter<double> baseTheta{"base theta", this};

  // Levels Top-down
  oops::Parameter<bool> levTopDown{"level top down", "true", this};

};

// -----------------------------------------------------------------------------
/// Geometry parameters

class GeometryParameters : public oops::Parameters {
  OOPS_CONCRETE_PARAMETERS(GeometryParameters, Parameters)

 public:
  /// Function space
  oops::RequiredParameter<std::string> functionSpace{"function space", this};

  /// Grid
  oops::RequiredParameter<eckit::LocalConfiguration> grid{"grid", this};

  /// Partitioner
  oops::Parameter<std::string> partitioner{"partitioner", "equal_regions", this};

  /// Units
  oops::Parameter<std::string> units{"units", "degrees", this};

  /// Variables groups
  oops::RequiredParameter<std::vector<GroupParameters>> groups{"groups", this};

  /// Halo size
  oops::OptionalParameter<size_t> halo{"halo", this};
};

// -----------------------------------------------------------------------------
/// Geometry handles geometry for  model.

class Geometry : public util::Printable,
                 private util::ObjectCounter<Geometry> {
 public:

  static const std::string classname() {return "genint::Geometry";}

  Geometry(const eckit::Configuration &,
           const eckit::mpi::Comm & comm = oops::mpi::world());
  Geometry(const Geometry &);

  const eckit::mpi::Comm & getComm() const {return comm_;}
  const size_t halo() const {return halo_;}
  const atlas::Grid grid() const {return grid_;}
  const std::string gridType() const {return gridType_;}
  const atlas::grid::Partitioner partitioner() const {return partitioner_;}
  const atlas::Mesh mesh() const {return mesh_;}
  const atlas::FunctionSpace & functionSpace() const {return functionSpace_;}
  atlas::FunctionSpace & functionSpace() {return functionSpace_;}
  const atlas::FieldSet & fields() const {return groups_[0].fields_;}
  atlas::FieldSet & fields() {return groups_[0].fields_;}
  const atlas::FieldSet & fields(const size_t & groupIndex) const
    {return groups_[groupIndex].fields_;}
  size_t levels(const size_t & groupIndex) const {return groups_[groupIndex].levels_;}
  size_t levels(const std::string & var) const;
  size_t groups() const {return groups_.size();}
  size_t groupIndex(const std::string & var) const;

  size_t variableSize(const std::string &) const;
  size_t maskLevel(const std::string &, const size_t &) const;
  std::vector<size_t> variableSizes(const oops::Variables & vars) const;
  void latlon(std::vector<double> &, std::vector<double> &, const bool) const;
  bool levelsAreTopDown() const {return groups_[0].levTopDown_;}

  // Functions to retrieve geometry features
  const std::vector<double> & ak() const {return groups_[0].ak_;}
  const std::vector<double> & bk() const {return groups_[0].bk_;}
  const double & pTop() const {return groups_[0].pTop_;}
  const size_t & levels() const {return groups_[0].levels_;}

  // Mapping io var to jedi names
  std::map<std::string,std::string> mapVariables() const;

 private:
  void print(std::ostream &) const;
  void readSeaMask(const std::string &, const size_t &, const std::string &, atlas::Field &) const;
  const eckit::mpi::Comm & comm_;
  size_t halo_;
  atlas::Grid grid_;
  std::string gridType_;
  bool unstructuredGrid_;
  atlas::grid::Partitioner partitioner_;
  atlas::grid::Distribution distribution_;
  atlas::Mesh mesh_;
  atlas::FunctionSpace functionSpace_;
  std::unordered_map<std::string, size_t> groupIndex_;
  struct groupData {
    size_t levels_;
    std::string lev2d_;
    std::string verticalCoordinate_;
    std::vector<double> vunit_;
    std::vector<double> ak_;
    std::vector<double> bk_;
    double pTop_;
    std::map<std::string,std::string> mapVariables_;
    atlas::FieldSet fields_;
    double gmaskSize_;
    bool levTopDown_;
  };
  std::vector<groupData> groups_;
};
// -----------------------------------------------------------------------------

}  // namespace genint
