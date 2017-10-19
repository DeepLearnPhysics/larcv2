#ifndef __EMPTY_CLASS_NAME_CXX__
#define __EMPTY_CLASS_NAME_CXX__

#include "Empty_Class_Name.h"

namespace larcv {

  static Empty_Class_NameProcessFactory __global_Empty_Class_NameProcessFactory__;

  Empty_Class_Name::Empty_Class_Name(const std::string name)
    : ProcessBase(name)
  {}
    
  void Empty_Class_Name::configure(const PSet& cfg)
  {}

  void Empty_Class_Name::initialize()
  {}

  bool Empty_Class_Name::process(IOManager& mgr)
  {}

  void Empty_Class_Name::finalize()
  {}

}
#endif
