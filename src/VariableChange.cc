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
#include "src/VariableChange.h"

namespace genint {

// -------------------------------------------------------------------------------------------------

VariableChange::VariableChange(const Parameters_ & params, const Geometry & geometry)
  : vader_() {
    // Create vader cookbook
    vader::cookbookConfigType vaderCustomCookbook =
                                          params.variableChangeParameters.value().vaderCustomCookbook;

    // Create the VaderConstructConfig object
    vader::VaderConstructConfig vaderConstructConfig(vaderCustomCookbook);

  }
} // genint namespace
