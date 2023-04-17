/*
 * (C) Copyright 2022 UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "oops/runs/Dirac.h"
#include "oops/runs/Run.h"
#include "saber/oops/instantiateCovarFactory.h"
#include "src/Traits.h"

int main(int argc,  char ** argv) {
  oops::Run run(argc, argv);
  saber::instantiateCovarFactory<genint::Traits>();
  oops::Dirac<genint::Traits> dir;
  return run.execute(dir);
}
