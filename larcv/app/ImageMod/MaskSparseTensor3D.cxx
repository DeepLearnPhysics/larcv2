#ifndef __MASKSPARSETENSOR3D_CXX__
#define __MASKSPARSETENSOR3D_CXX__

#include "MaskSparseTensor3D.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
namespace larcv {

  static MaskSparseTensor3DProcessFactory __global_MaskSparseTensor3DProcessFactory__;

  MaskSparseTensor3D::MaskSparseTensor3D(const std::string name)
    : ProcessBase(name)
  {}
    
  void MaskSparseTensor3D::configure(const PSet& cfg)
  {
    _value_min = cfg.get<float>("MinVoxelValue",0.);
    _output_producer = cfg.get<std::string>("OutputProducer");
    _reference_producer = cfg.get<std::string>("ReferenceProducer");
    _target_producer = cfg.get<std::string>("TargetProducer");
  }

  void MaskSparseTensor3D::initialize()
  {}

  bool MaskSparseTensor3D::process(IOManager& mgr)
  {
    auto const& ev_ref    = mgr.get_data<larcv::EventSparseTensor3D>(_reference_producer);
    auto const& ev_target = mgr.get_data<larcv::EventSparseTensor3D>(_target_producer);
    auto ev_output = (larcv::EventSparseTensor3D*)(mgr.get_data("sparse3d",_output_producer));
    ev_output->meta(ev_target.meta());

    for(auto const& vox : ev_target.as_vector()) {
      auto const& ref_vox = ev_ref.find(vox.id());
      if(ref_vox.id() == kINVALID_VOXELID) continue;
      if(ref_vox.value() < _value_min) continue;
      ((VoxelSet*)ev_output)->emplace(vox.id(), vox.value(), false);
    }
    return true;
  }

  void MaskSparseTensor3D::finalize()
  {}

}
#endif
