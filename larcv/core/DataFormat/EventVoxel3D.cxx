#ifndef __LARCV_EVENTVOXEL3D_CXX__
#define __LARCV_EVENTVOXEL3D_CXX__

#include "EventVoxel3D.h"

namespace larcv {

  /// Global larcv::EventVoxel3D to register factory
  static EventVoxel3DFactory __global_EventVoxel3DFactory__;

  void EventVoxel3D::clear()
  { EventBase::clear(); VoxelSet::Clear(); _meta.Clear(); }

}

#endif
