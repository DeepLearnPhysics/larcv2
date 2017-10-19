#ifndef __VOXELCOMPRESSOR_CXX__
#define __VOXELCOMPRESSOR_CXX__

#include "VoxelCompressor.h"
#include "DataFormat/EventVoxel3D.h"

namespace larcv {

  static VoxelCompressorProcessFactory __global_VoxelCompressorProcessFactory__;

  VoxelCompressor::VoxelCompressor(const std::string name)
    : ProcessBase(name)
  {}
    
  void VoxelCompressor::configure(const PSet& cfg)
  {
    _voxel_producer  = cfg.get<std::string>("VoxelProducer");
    _output_producer = cfg.get<std::string>("OutputProducer");
    _comp_factor     = cfg.get<size_t>("CompressionFactor");
    _scale_factor    = cfg.get<float>("ScaleFactor",1.);
  }

  void VoxelCompressor::initialize()
  {}

  bool VoxelCompressor::process(IOManager& mgr)
  {
    auto event_voxel = (EventVoxel3D*)(mgr.get_data(kProductVoxel3D,_voxel_producer));
    if(!event_voxel) {
      LARCV_CRITICAL() << "EventVoxel3D by " << _voxel_producer << " not found..." << std::endl;
      throw larbys();
    }
    auto const& orig_meta = event_voxel->GetVoxelMeta();
    Voxel3DMeta meta;
    meta.Set(orig_meta.MinX(),orig_meta.MaxX(),
	     orig_meta.MinY(),orig_meta.MaxY(),
	     orig_meta.MinZ(),orig_meta.MaxZ(),
	     orig_meta.NumVoxelX() / _comp_factor,
	     orig_meta.NumVoxelY() / _comp_factor,
	     orig_meta.NumVoxelZ() / _comp_factor);

    Voxel3DSet vox_set;
    vox_set.Reset(meta);

    for(auto const& vox : event_voxel->GetVoxelSet()) {
      Voxel3D avox (meta.ID(orig_meta.X(vox.ID()),
			    orig_meta.Y(vox.ID()),
			    orig_meta.Z(vox.ID())), vox.Value() * _scale_factor);
      vox_set.Emplace(std::move(avox));
    }

    auto output_voxel = (EventVoxel3D*)(mgr.get_data(kProductVoxel3D,_output_producer));
    if(!output_voxel) {
      LARCV_CRITICAL() << "EventVoxel3D by " << _output_producer << " could not be created..." << std::endl;
      throw larbys();
    }
    output_voxel->Move(std::move(vox_set));
    
    return true;
  }

  void VoxelCompressor::finalize()
  {}

}
#endif
