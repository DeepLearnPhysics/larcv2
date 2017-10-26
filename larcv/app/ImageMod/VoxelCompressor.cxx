#ifndef __VOXELCOMPRESSOR_CXX__
#define __VOXELCOMPRESSOR_CXX__

#include "VoxelCompressor.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"

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
    _scale_factor    = cfg.get<float>("ScaleFactor", 1.);
  }

  void VoxelCompressor::initialize()
  {}

  bool VoxelCompressor::process(IOManager& mgr)
  {
    auto const& event_voxel = mgr.get_data<larcv::EventSparseTensor3D>(_voxel_producer);

    auto const& orig_meta = event_voxel.meta();
    Voxel3DMeta meta;
    meta.set(orig_meta.min_x(), orig_meta.min_y(), orig_meta.min_z(),
             orig_meta.max_x(), orig_meta.max_y(), orig_meta.max_z(),
             orig_meta.num_voxel_x() / _comp_factor,
             orig_meta.num_voxel_y() / _comp_factor,
             orig_meta.num_voxel_z() / _comp_factor,
             orig_meta.unit());

    auto& output_voxel = mgr.get_data<larcv::EventSparseTensor3D>(_output_producer);
    output_voxel.meta(meta);
    for(auto const& in_vox : event_voxel.as_vector()) {
      Voxel out_vox(meta.id(orig_meta.position(in_vox.id())), in_vox.value());
      output_voxel.emplace(std::move(out_vox));
    }

    return true;
  }

  void VoxelCompressor::finalize()
  {}

}
#endif
