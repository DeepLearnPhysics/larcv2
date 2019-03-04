#ifndef EVENTVOXEL3D_CXX
#define EVENTVOXEL3D_CXX

#include "EventVoxel3D.h"

namespace larcv {

  /// Global larcv::EventClusterPixel3DFactory to register EventClusterPixel3D
  static EventClusterVoxel3DFactory __global_EventClusterVoxel3DFactory__;

  /// Global larcv::EventSparseTensor3DFactory to register EventSparseTensor3D
  static EventSparseTensor3DFactory __global_EventSparseTensor3DFactory__;

}

#endif
