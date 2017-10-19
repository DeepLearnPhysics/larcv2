#ifndef __SUPERAPARAMSVOXEL3D_CXX__
#define __SUPERAPARAMSVOXEL3D_CXX__

#include "ParamsVoxel3D.h"

namespace supera {

  void ParamsVoxel3D::configure(const supera::Config_t& cfg)
  {
    _out_voxel3d_producer  = cfg.get<std::string>("OutVoxel3DLabel",  "");
  }  
}
#endif
