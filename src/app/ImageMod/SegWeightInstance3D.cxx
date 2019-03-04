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
    _weight_vertex = cfg.get<float>("WeightVertex");
    _weight_surrounding = cfg.get<bool>("WeightSurrounding", true);
  }

  void SegWeightInstance3D::initialize()
  {}

  void SegWeightInstance3D::find_neighbors(const Voxel3DMeta& meta,
      const Voxel& vox,
      std::vector<VoxelID_t>& result) const
  {
    result.resize(26);
    size_t ctr=0;
    for(int xshift=-1; xshift<=1; ++xshift) {
      for(int yshift=-1; yshift<=1; ++yshift) {
        for(int zshift=-1; zshift<=1; ++zshift) {
          if(xshift != 0 || yshift !=0 || zshift !=0 ) {
            result[ctr] = meta.shift(vox.id(),xshift,yshift,zshift);
            ++ctr;
          }
        }
      }
    }
    /*
    result[0] = meta.shift(vox.id(), -1, 0, 0);
    result[1] = meta.shift(vox.id(), 1, 0, 0);
    result[2] = meta.shift(vox.id(), 0, -1, 0);
    result[3] = meta.shift(vox.id(), 0, 1, 0);
    result[4] = meta.shift(vox.id(), 0, 0, -1);
    result[5] = meta.shift(vox.id(), 0, 0, 1);
    */
  }

  bool SegWeightInstance3D::process(IOManager& mgr)
  {
    auto const& event_instance = mgr.get_data<larcv::EventSparseTensor3D>(_instance_producer);
    auto const& event_keypoint = mgr.get_data<larcv::EventClusterVoxel3D>(_keypoint_producer);
    auto event_weight = (EventSparseTensor3D*)(mgr.get_data("sparse3d", _weight_producer));

    if (event_keypoint.meta() != event_instance.meta()) {
      LARCV_CRITICAL() << "Voxel3DMeta from " << _instance_producer << " (EventSparseTensor3D) "
                       << " differs from " << _keypoint_producer << " (EventClusterVoxel3D)" << std::endl;
      throw larbys();
    }

    auto const meta = event_instance.meta();
    event_weight->meta(meta);

    double surrounding_weight = 1.;
    larcv::VoxelSet surrounding_vs;
    larcv::Point3D pt;
    std::vector<VoxelID_t> neighbor_voxel_id;

    std::vector<size_t> instance_count_v;
    std::vector<double> instance_weight_v;
    for (auto const& vox : event_instance.as_vector()) {
      // count per-instance voxels
      //if(vox.value() < 1.) continue;
      size_t instance_id = (size_t)(vox.value());
      if (instance_id >= instance_count_v.size()) instance_count_v.resize(instance_id + 1, 0.);
      instance_count_v[instance_id] += 1.;
      // check surrounding voxels
      if (_weight_surrounding) {
        this->find_neighbors(meta, vox, neighbor_voxel_id);
        for (auto const& neighbor_id : neighbor_voxel_id) {
          if (neighbor_id == kINVALID_VOXELID) continue;
          auto const& instance_vox = event_instance.find(neighbor_id);
          if (instance_vox.id() != kINVALID_VOXELID) continue;
          larcv::Voxel neighbor_vox(neighbor_id, 1.);
          surrounding_vs.emplace(std::move(neighbor_vox), false);
        }
      }
    }
    // compute weights
    double weight_max = 0;
    if (surrounding_vs.as_vector().size()) {
      surrounding_weight = ((double)(meta.size()) / (double)(surrounding_vs.as_vector().size()));
      weight_max = surrounding_weight;
    }
    instance_weight_v.resize(instance_count_v.size());
    for (size_t instance_id = 0; instance_id < instance_count_v.size(); ++instance_id) {
      double weight_factor = 0.;
      if (instance_count_v[instance_id]) weight_factor = (double)(meta.size()) / (double)(instance_count_v[instance_id]);
      if (weight_factor > weight_max) weight_max = weight_factor;
      instance_weight_v[instance_id] = weight_factor;
    }

    // Normalize via weight_max if weight_max > _weight_max
    double weight_scale = std::min((double)1., _weight_max / weight_max);
    if (surrounding_vs.as_vector().size()) {
      double original_weight = surrounding_weight;
      surrounding_vs *= ((float)(surrounding_weight * weight_scale));
      LARCV_INFO() << "Charge-surrounding pixel: " << surrounding_vs.as_vector().size()
                   << " weighting factor: " << original_weight
                   << " ... after scaling: " << surrounding_vs.as_vector().front().value()
                   << std::endl;
    }

    for (size_t instance_id = 0; instance_id < instance_count_v.size(); ++instance_id) {
      double original_weight = instance_weight_v[instance_id];
      instance_weight_v[instance_id] *= weight_scale;
      LARCV_INFO() << "Cluster id " << instance_id
                   << " pixel: " << instance_count_v[instance_id]
                   << " weighting factor: " << original_weight
                   << " ... after scaling: " << instance_weight_v[instance_id]
                   << std::endl;
    }

    //
    // Combine weights
    //
    for (auto const& instance_vox : event_instance.as_vector()) {
      size_t instance_id = (size_t)(instance_vox.value());
      larcv::Voxel vox(instance_vox.id(), (float)(instance_weight_v[instance_id]));
      ((VoxelSet*)event_weight)->emplace(std::move(vox), true);
    }
    for (auto const& surrounding_vox : surrounding_vs.as_vector())
      event_weight->add(surrounding_vox);
    //
    // figure out keypoint weights
    //
    size_t num_vertex = 0;
    for (auto const& keypoints : event_keypoint.as_vector()) {
      for (auto const& keyvox : keypoints.as_vector()) {
        auto vox = event_instance.find(keyvox.id());
        if (vox.id() == kINVALID_VOXELID) continue;
        ((VoxelSet*)event_weight)->emplace(vox.id(), _weight_vertex, false);
        num_vertex += 1;
      }
    }
    LARCV_INFO() << "Vertex pixel: " << num_vertex << " weighting factor: " << _weight_vertex << std::endl;
    return true;
  }

  void SegWeightInstance3D::finalize()
  {}

}
#endif
