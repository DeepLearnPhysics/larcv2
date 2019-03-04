#ifndef __COMBINETENSOR3D_CXX__
#define __COMBINETENSOR3D_CXX__

#include "CombineTensor3D.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
namespace larcv {

  static CombineTensor3DProcessFactory __global_CombineTensor3DProcessFactory__;

  CombineTensor3D::CombineTensor3D(const std::string name)
    : ProcessBase(name)
  {}
    
  void CombineTensor3D::configure(const PSet& cfg)
  {
    _output_producer = cfg.get<std::string>("OutputProducer");
    _tensor3d_producer_v = cfg.get<std::vector<std::string> >("Tensor3DProducers");
    _pool_type = (PoolType_t)(cfg.get<unsigned short>("PoolType",(unsigned short)kSumPool));
    if(_tensor3d_producer_v.empty()) {
      LARCV_CRITICAL() << "Tensor3DProducers parameter cannot be empty" << std::endl;
      throw larbys();
    }
  }

  void CombineTensor3D::initialize()
  {}

  bool CombineTensor3D::process(IOManager& mgr)
  {
    larcv::VoxelSet vs;
    Voxel3DMeta meta;

    for(auto const& producer : _tensor3d_producer_v) {
      auto const& ev_tensor3d = mgr.get_data<larcv::EventSparseTensor3D>(producer);
      if(!meta.valid()) meta = ev_tensor3d.meta();
      if(meta != ev_tensor3d.meta()) {
        LARCV_CRITICAL() << "Producer " << producer << " has incompatible Voxel3DMeta" << std::endl;
        throw larbys();
      }
      switch(_pool_type) {
        case kSumPool:
        for(auto const& vox : ev_tensor3d.as_vector())
          vs.add(vox);
        break;
        case kMaxPool:
        for(auto const& vox : ev_tensor3d.as_vector()) {
          auto const& exist_vox = vs.find(vox.id());
          if(exist_vox.id() == kINVALID_VOXELID) {
            vs.add(vox);
            continue;
          }
          if(vox.value() > exist_vox.value())
            vs.emplace(vox.id(),vox.value(),false);
        }
        break;
      }
    }

    auto& out_tensor3d = mgr.get_data<larcv::EventSparseTensor3D>(_output_producer);
    out_tensor3d.emplace(std::move(vs),meta);

    return true;
  }

  void CombineTensor3D::finalize()
  {}

}
#endif
