#ifndef __EMPTYVOXELFILTER_CXX__
#define __EMPTYVOXELFILTER_CXX__

#include "EmptyVoxelFilter.h"
#include "DataFormat/EventVoxel3D.h"
namespace larcv {

  static EmptyVoxelFilterProcessFactory __global_EmptyVoxelFilterProcessFactory__;

  EmptyVoxelFilter::EmptyVoxelFilter(const std::string name)
    : ProcessBase(name)
  {}
    
  void EmptyVoxelFilter::configure(const PSet& cfg)
  {
    _min_voxel_count = cfg.get<size_t>("MinVoxelCount");
    _voxel_producer  = cfg.get<std::string>("VoxelProducer");
    _min_voxel_value = cfg.get<float>("MinVoxelCharge",0.);
  }

  void EmptyVoxelFilter::initialize()
  {}

  bool EmptyVoxelFilter::process(IOManager& mgr)
  {
    static ProducerID_t producer_id = kINVALID_SIZE;
    if(producer_id == kINVALID_SIZE) {
      ProducerName_t name_id("voxel3d",_voxel_producer);
      producer_id = mgr.producer_id(name_id);
    }
    auto event_voxel = (VoxelSet*)(mgr.get_data(producer_id));
    if(!event_voxel) {
      LARCV_CRITICAL() << "VoxelSet with name " << _voxel_producer << " not found..." << std::endl;
      throw larbys();
    }

    size_t ctr=0;
    for(auto const& vox : event_voxel->VoxelArray()) {
      if(vox.Value()<_min_voxel_value) continue;
      ctr++;
    }
    return (ctr >= _min_voxel_count);
  }

  void EmptyVoxelFilter::finalize()
  {}

}
#endif
