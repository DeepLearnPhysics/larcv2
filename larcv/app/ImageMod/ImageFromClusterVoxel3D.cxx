#ifndef __IMAGEFROMCLUSTERVOXEL3D_CXX__
#define __IMAGEFROMCLUSTERVOXEL3D_CXX__

#include "ImageFromClusterVoxel3D.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"

namespace larcv {

  static ImageFromClusterVoxel3DProcessFactory __global_ImageFromClusterVoxel3DProcessFactory__;

  ImageFromClusterVoxel3D::ImageFromClusterVoxel3D(const std::string name)
    : ProcessBase(name)
  {}
    
  void ImageFromClusterVoxel3D::configure(const PSet& cfg)
  {
    _voxel3d_producer = cfg.get<std::string>("ClusterVoxel3DProducer");
    _plane_type = (ProjectionPlane_t)(cfg.get<size_t>("ProjectionPlaneType"));
  }

  void ImageFromClusterVoxel3D::initialize()
  {}

  bool ImageFromClusterVoxel3D::process(IOManager& mgr)
  {
    auto const& ev_voxel3d = mgr.get_data<larcv::EventClusterVoxel3D>(_voxel3d_producer);
    
  }

  void ImageFromClusterVoxel3D::finalize()
  {}

}
#endif
