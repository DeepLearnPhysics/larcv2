#ifndef __SUPERA_CXX__
#define __SUPERA_CXX__

#include "Supera.h"

namespace larcv {

  static SuperaProcessFactory __global_SuperaProcessFactory__;

  Supera::Supera(const std::string name)
    : ProcessBase(name)
  {}
    
  void Supera::configure(const PSet& cfg)
  {}

  void Supera::initialize()
  {}

  bool Supera::process(IOManager& mgr)
  {}

  void Supera::finalize()
  {}

}
#endif
