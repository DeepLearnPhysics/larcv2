#ifndef __LARCV_VOXEL3D_CXX__
#define __LARCV_VOXEL3D_CXX__

#include "Voxel3D.h"
#include <iostream>

namespace larcv {

  void SparseTensor3D::add(const double x, const double y, const double z, 
    const float val)
  {
    auto id = _meta.id(x,y,z);
    if(id != kINVALID_VOXELID) emplace(id,val);
  }


}

#endif
