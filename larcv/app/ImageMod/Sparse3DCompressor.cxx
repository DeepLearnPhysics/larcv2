#ifndef __Sparse3DCompressor_CXX__
#define __Sparse3DCompressor_CXX__

#include "Sparse3DCompressor.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"

namespace larcv {

  static Sparse3DCompressorProcessFactory __global_Sparse3DCompressorProcessFactory__;

  Sparse3DCompressor::Sparse3DCompressor(const std::string name)
    : ProcessBase(name)
  {}

  void Sparse3DCompressor::configure(const PSet& cfg)
  {
    _voxel_producer  = cfg.get<std::string>("TargetProducer");
    _output_producer = cfg.get<std::string>("OutputProducer");
    _comp_factor     = cfg.get<size_t>("CompressionFactor");
    _scale_factor    = cfg.get<float>("ScaleFactor", 1.);
    _pool_type       = (PoolType_t)(cfg.get<unsigned short>("PoolType",(unsigned short)kSumPool));
  }

  void Sparse3DCompressor::initialize()
  {}

  bool Sparse3DCompressor::process(IOManager& mgr)
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

    larcv::VoxelSet vs;

    for (auto const& in_vox : event_voxel.as_vector()) {
      auto id = meta.id(orig_meta.position(in_vox.id()));

      switch(_pool_type) {

        case kSumPool: {
          vs.emplace(id,in_vox.value() * _scale_factor,true);
          break;
        }
        case kMaxPool: {
          auto const& out_vox = vs.find(id);
          if(out_vox.id() == kINVALID_VOXELID) {
            vs.emplace(id,in_vox.value() * _scale_factor,true);
            break;
          }else if(out_vox.value() < (in_vox.value() * _scale_factor)) {
            vs.emplace(id,in_vox.value() * _scale_factor,false);
            break;
          }
          break;
        }
        default: {
          LARCV_CRITICAL() << "PoolType_t " << (unsigned short)(_pool_type) << " not supported!" << std::endl;
          throw larbys();
        }
      }
    }
    auto& output_voxel = mgr.get_data<larcv::EventSparseTensor3D>(_output_producer);
    output_voxel.emplace(std::move(vs),meta);
    return true;
  }

  void Sparse3DCompressor::finalize()
  {}

}
#endif
