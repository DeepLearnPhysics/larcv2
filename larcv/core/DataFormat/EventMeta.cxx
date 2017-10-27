#ifndef LARCV_EVENTMETA_CXX
#define LARCV_EVENTMETA_CXX

#include "EventMeta.h"

namespace larcv{
  /// Global larcv::SBClusterFactory to register ClusterAlgoFactory
  static EventMetaFactory __global_EventMetaFactory__;

  void EventMeta::clear()
  {
    EventBase::clear();
    Meta::clear_data();
  }
}

#endif
