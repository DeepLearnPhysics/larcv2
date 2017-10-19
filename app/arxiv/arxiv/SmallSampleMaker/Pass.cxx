#ifndef __PASS_CXX__
#define __PASS_CXX__

#include "Pass.h"

namespace larcv {

  static PassProcessFactory __global_PassProcessFactory__;

  Pass::Pass(const std::string name)
    : ProcessBase(name)
  {}
    
  void Pass::configure(const PSet& cfg)
  {}

  void Pass::initialize()
  {}

  bool Pass::process(IOManager& mgr)
  {
    return true;
  }

  void Pass::finalize()
  {}

}
#endif
