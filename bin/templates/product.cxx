#ifndef EMPTY_CLASS_NAME_CXX
#define EMPTY_CLASS_NAME_CXX

#include "Empty_Class_Name.h"

namespace larcv{
  /// Global larcv::SBClusterFactory to register ClusterAlgoFactory
  static Empty_Class_NameFactory __global_Empty_Class_NameFactory__;

  void Empty_Class_Name::clear()
  {
    EventBase::clear();
  }
}

#endif
