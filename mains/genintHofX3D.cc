

#include "oops/runs/HofX3D.h"
#include "oops/runs/Run.h"
#include "ufo/instantiateObsErrorFactory.h"
#include "ufo/instantiateObsFilterFactory.h"
#include "ufo/ObsTraits.h"
#include "src/Traits.h"

int main(int argc,  char ** argv) {
  oops::Run run(argc, argv);
  ufo::instantiateObsErrorFactory();
  ufo::instantiateObsFilterFactory();
  oops::HofX3D<genint::Traits, ufo::ObsTraits> hofx;
  return run.execute(hofx);
}
