#ifndef __SEGWEIGHTINSTANCE3D_CXX__
#define __SEGWEIGHTINSTANCE3D_CXX__

#include "SegWeightInstance3D.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"

namespace larcv {

  static SegWeightInstance3DProcessFactory __global_SegWeightInstance3DProcessFactory__;

  SegWeightInstance3D::SegWeightInstance3D(const std::string name)
    : ProcessBase(name)
  {}
    
  void SegWeightInstance3D::configure(const PSet& cfg)
  {
    _instance_producer = cfg.get<std::string>("InstanceProducer");
    _keypoint_producer = cfg.get<std::string>("KeyPointProducer");
    _weight_producer   = cfg.get<std::string>("WeightProducer");
    _weight_max = cfg.get<float>("WeightMax");
    _weight_surrounding = cfg.get<bool>("WeightSurrounding", true);
  }

  void SegWeightInstance3D::initialize()
  {}

  void SegWeightInstance3D::find_neighbors(const Voxel3DMeta& meta, 
                                           const Voxel& vox,
                                           std::vector<VoxelID_t>& result) const
  {
    result.resize(6);
    result[0] = meta.shift(vox.id(),-1, 0, 0);
    result[1] = meta.shift(vox.id(), 1, 0, 0);
    result[2] = meta.shift(vox.id(), 0,-1, 0);
    result[3] = meta.shift(vox.id(), 0, 1, 0);
    result[4] = meta.shift(vox.id(), 0, 0,-1);
    result[5] = meta.shift(vox.id(), 0, 0, 1);
  }

  bool SegWeightInstance3D::process(IOManager& mgr)
  {
    auto const& event_instance = mgr.get_data<larcv::EventSparseTensor3D>(_instance_producer);
    auto const& event_keypoint = mgr.get_data<larcv::EventClusterVoxel3D>(_keypoint_producer);
    auto event_weight = (EventSparseTensor3D*)(mgr.get_data("sparse3d",_weight_producer));

    if(event_keypoint.meta() != event_instance.meta()) {
      LARCV_CRITICAL() << "Voxel3DMeta from " << _instance_producer << " (EventSparseTensor3D) "
      << " differs from " << _keypoint_producer << " (EventClusterVoxel3D)" << std::endl;
      throw larbys();
    }

    auto const meta = event_instance.meta();
    event_weight->meta(meta);

    larcv::VoxelSet surrounding_weight;
    larcv::Point3D pt;
    std::vector<VoxelID_t> neighbor_voxel_id;

    std::vector<float> instance_count_v;
    for(auto const& vox : event_instance.as_vector()) {
      // count per-instance voxels
      //if(vox.value() < 1.) continue;
      size_t instance_id = (size_t)(vox.value());
      if(instance_id >= instance_count_v.size()) instance_count_v.resize(instance_id+1,0.);
      instance_count_v[instance_id] += 1.;
      // check surrounding voxels
      if(_weight_surrounding) {
        this->find_neighbors(meta,vox,neighbor_voxel_id);
        for(auto const& neighbor_id : neighbor_voxel_id) {
          if(neighbor_id == kINVALID_VOXELID) continue;
          auto const& instance_vox = event_instance.find(neighbor_id);
          if(instance_vox.id() != kINVALID_VOXELID) continue;
          larcv::Voxel neighbor_vox(neighbor_id,1.);
          surrounding_weight.emplace(std::move(neighbor_vox),false);
        }
      }
    }
    // normalize surrounding voxel weight
    if(surrounding_weight.as_vector().size()) {
      float weight_factor = ((float)(meta.size()) / (float)(surrounding_weight.as_vector().size()));
      LARCV_INFO() << "Charge-surrounding pixel: " << surrounding_weight.as_vector().size()
      << " weighting factor: " << weight_factor << std::endl;
      surrounding_weight *= weight_factor;
    }
    for(size_t instance_id = 0; instance_id < instance_count_v.size(); ++instance_id) {
      float weight_factor = 0.;
      if(instance_count_v[instance_id]) weight_factor = (float)(meta.size()) / (float)(instance_count_v[instance_id]);
      LARCV_INFO() << "Cluster id " << instance_id
      << " pixel: " << instance_count_v[instance_id]
      << " weighting factor: " << weight_factor
      << std::endl;
      instance_count_v[instance_id] = weight_factor;
    }

    //
    // Combine weights
    //
    for(auto const& instance_vox : event_instance.as_vector()) {
      size_t instance_id = (size_t)(instance_vox.value());
      larcv::Voxel vox(instance_vox.id(), instance_count_v[instance_id]);
      ((VoxelSet*)event_weight)->emplace(std::move(vox),true);
    }
    for(auto const& surrounding_vox : surrounding_weight.as_vector())
      event_weight->add(surrounding_vox);
    //
    // figure out keypoint weights
    //
    for(auto const& keypoints : event_keypoint.as_vector()) {
      for(auto const& keyvox : keypoints.as_vector()) {
        auto vox = event_instance.find(keyvox.id());
        if(vox.id() == kINVALID_VOXELID) continue;
        ((VoxelSet*)event_weight)->emplace(vox.id(),_weight_max,true);
      }
    }

    return true;
  }

  void SegWeightInstance3D::finalize()
  {}

}
#endif
