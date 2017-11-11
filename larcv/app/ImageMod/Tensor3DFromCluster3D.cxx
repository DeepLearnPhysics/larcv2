#ifndef __Tensor3DFromCluster3D_CXX__
#define __Tensor3DFromCluster3D_CXX__

#include "Tensor3DFromCluster3D.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
namespace larcv {

  static Tensor3DFromCluster3DProcessFactory
  __global_Tensor3DFromCluster3DProcessFactory__;

  Tensor3DFromCluster3D::Tensor3DFromCluster3D(const std::string name)
    : ProcessBase(name) {}

  void Tensor3DFromCluster3D::configure(const PSet& cfg) {
    _cluster3d_producer = cfg.get<std::string>("ClusterVoxel3DProducer");
    _output_producer = cfg.get<std::string>("OutputProducer", "");
    _fixed_pi = cfg.get<float>("FixedPI", 100.);
    _pi_type  = (PIType_t)(cfg.get<unsigned short>("PIType", (unsigned short)(PIType_t::kPITypeInputVoxel)));
  }

  void Tensor3DFromCluster3D::initialize() {}

  bool Tensor3DFromCluster3D::process(IOManager& mgr) {
    std::vector<std::string> _producer_list;
    if (_cluster3d_producer == "")
      _producer_list = mgr.producer_list("cluster3d");
    else
      _producer_list.push_back(_cluster3d_producer);

    for (auto producer : _producer_list) {
      auto const& ev_cluster3d =
        mgr.get_data<larcv::EventClusterVoxel3D>(producer);

      larcv::EventSparseTensor3D* ev_voxel3d = nullptr;
      std::string output_producer = producer;
      if (!_output_producer.empty()) {
        if (_producer_list.size() == 1) output_producer = _output_producer;
        else output_producer = producer + "_" + _output_producer;
      }
      ev_voxel3d = (larcv::EventSparseTensor3D*)(mgr.get_data("sparse3d", output_producer));
      auto const& meta = ev_cluster3d.meta();
      ev_voxel3d->meta(meta);

      auto const& clusters = ev_cluster3d.as_vector();
      for (size_t cluster_index = 0; cluster_index < clusters.size(); ++cluster_index) {

        auto const& cluster = clusters[cluster_index];

        switch (_pi_type) {
        case PIType_t::kPITypeFixedPI:
          for (auto const& vox : cluster.as_vector()) {((VoxelSet*)ev_voxel3d)->emplace(vox.id(), _fixed_pi, false);}
          break;
        case PIType_t::kPITypeInputVoxel:
          for (auto const& vox : cluster.as_vector()) {ev_voxel3d->add(vox);}
          break;
        case PIType_t::kPITypeClusterIndex:
          for (auto const& vox : cluster.as_vector()) {
            // check if this voxel already exists
            auto const& prev_vox = ((VoxelSet*)ev_voxel3d)->find(vox.id());
            if (prev_vox.id() == kINVALID_VOXELID) {
              ((larcv::VoxelSet*)ev_voxel3d)->emplace(vox.id(), (float)(cluster_index + 1), false);
              LARCV_DEBUG() << "Inserted voxel id " << vox.id() << " for cluster_index " << cluster_index << std::endl;
              continue;
            }
            LARCV_DEBUG() << "Found previously registered voxel @ " << prev_vox.id() << " cluster " << prev_vox.value() - 1.
                          << " ... for voxel " << vox.id() << " cluster " << cluster_index << std::endl;
            auto const& orig_vox = clusters[(size_t)(prev_vox.value()) - 1].find(prev_vox.id());
            if (orig_vox.id() == kINVALID_VOXELID) {
              LARCV_CRITICAL() << "Logic error: could not locate the original voxel reference..." << std::endl;
              throw larbys();
            }
            if (orig_vox.value() > vox.value()) continue;
            ((larcv::VoxelSet*)ev_voxel3d)->emplace(vox.id(), (float)(cluster_index + 1), false);
          }
          break;
        case PIType_t::kPITypeUndefined:
          throw larbys("PITypeUndefined and kPITypeClusterIndex not supported!");
          break;
        }
      }
      if (this->logger().level() <= msg::Level_t::kINFO) {
        // report number of voxels per class
        std::vector<size_t> vox_count;
        for (auto const& vox : ev_voxel3d->as_vector()) {
          size_t class_index = vox.value();
          if (vox_count.size() <= class_index) vox_count.resize(class_index + 1, 0);
          vox_count[class_index] += 1;
        }
        for (size_t class_index = 0; class_index < vox_count.size(); ++class_index)
          LARCV_INFO() << "Class " << class_index << " ... " << vox_count[class_index] << " voxels" << std::endl;
      }
    }

    return true;
  }

  void Tensor3DFromCluster3D::finalize() {}
}
#endif
