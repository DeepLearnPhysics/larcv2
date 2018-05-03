#ifndef __Tensor3DFromCluster3D_CXX__
#define __Tensor3DFromCluster3D_CXX__

#include "Tensor3DFromCluster3D.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
namespace larcv {

  static Tensor3DFromCluster3DProcessFactory
  __global_Tensor3DFromCluster3DProcessFactory__;

  Tensor3DFromCluster3D::Tensor3DFromCluster3D(const std::string name)
    : ProcessBase(name) {}

  void Tensor3DFromCluster3D::configure_labels(const PSet& cfg)
  {
    _cluster3d_producer_v.clear();
    _output_producer_v.clear();
    _cluster3d_producer_v = cfg.get<std::vector<std::string> >("Cluster3DProducerList", _cluster3d_producer_v);
    _output_producer_v    = cfg.get<std::vector<std::string> >("OutputProducerList", _output_producer_v);

    if (_cluster3d_producer_v.empty()) {
      auto cluster3d_producer = cfg.get<std::string>("Cluster3DProducer", "");
      auto output_producer    = cfg.get<std::string>("OutputProducer", "");
      if (!cluster3d_producer.empty()) {
        _cluster3d_producer_v.push_back(cluster3d_producer);
        _output_producer_v.push_back(output_producer);
      }
    }

    if (_output_producer_v.empty()) {
      _output_producer_v.resize(_cluster3d_producer_v.size(), "");
    }
    else if (_output_producer_v.size() != _cluster3d_producer_v.size()) {
      LARCV_CRITICAL() << "Cluster3DProducer and OutputProducer must have the same array length!" << std::endl;
      throw larbys();
    }
  }

  void Tensor3DFromCluster3D::configure(const PSet& cfg) {
    configure_labels(cfg);

    _fixed_pi_v = cfg.get<std::vector<float> >("FixedPIList", _fixed_pi_v);
    if (_fixed_pi_v.empty()) {
      auto fixed_pi = cfg.get<float>("FixedPI", 100);
      _fixed_pi_v.resize(_cluster3d_producer_v.size(), fixed_pi);
    } else if (_fixed_pi_v.size() != _cluster3d_producer_v.size()) {
      LARCV_CRITICAL() << "FixedPIList size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }

    _pi_type_v = cfg.get<std::vector<unsigned short> >("PITypeList", _pi_type_v);
    if (_pi_type_v.empty()) {
      auto pi_type = cfg.get<unsigned short>("PIType", 1);
      _pi_type_v.resize(_cluster3d_producer_v.size(), pi_type);
    } else if (_pi_type_v.size() != _cluster3d_producer_v.size()) {
      LARCV_CRITICAL() << "PITypeList size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }
  }

  void Tensor3DFromCluster3D::initialize() {}

  bool Tensor3DFromCluster3D::process(IOManager& mgr) {
    std::vector<std::string> producer_list;
    std::vector<std::string> output_list;
    std::vector<unsigned short> pi_type_v;
    std::vector<float> fixed_pi_v;
    if (_cluster3d_producer_v.empty()) {
      producer_list = mgr.producer_list("cluster3d");
      output_list.resize(producer_list.size(), "");
      fixed_pi_v.resize(producer_list.size(),100);
      pi_type_v.resize(producer_list.size(),1);
    }
    else {
      producer_list = _cluster3d_producer_v;
      output_list   = _output_producer_v;
      fixed_pi_v    = _fixed_pi_v;
      pi_type_v     = _pi_type_v;
    }

    for (size_t label_index = 0; label_index < producer_list.size(); ++label_index) {
      auto const& producer = producer_list[label_index];
      auto output_producer = output_list[label_index];
      if (output_producer.empty()) output_producer = producer + "_tensor3d";

      auto const& ev_cluster3d =
        mgr.get_data<larcv::EventClusterVoxel3D>(producer);

      auto& ev_output = mgr.get_data<larcv::EventSparseTensor3D>(output_producer);
      // treat the case if this "output producer" already holds some data
      if (ev_output.meta().valid()) {
        static bool one_time_warning = true;
        if (_cluster3d_producer_v.empty()) {
          LARCV_CRITICAL() << "Over-writing existing EventSparseTensor3D data for label "
                           << output_producer << std::endl;
          throw larbys();
        }
        else if (one_time_warning) {
          LARCV_WARNING() << "Output EventSparseTensor3D producer " << output_producer
                          << " already holding valid data will be over-written!" << std::endl;
          one_time_warning = false;
        }
      }
      auto const meta = ev_cluster3d.meta();

      larcv::VoxelSet vs;
      auto pi_type  = (PIType_t)(pi_type_v[label_index]);
      auto const& fixed_pi = fixed_pi_v[label_index];

      auto const& clusters = ev_cluster3d.as_vector();
      for (size_t cluster_index = 0; cluster_index < clusters.size(); ++cluster_index) {

        auto const& cluster = clusters[cluster_index];

        switch (pi_type) {
        case PIType_t::kPITypeFixedPI:
          for (auto const& vox : cluster.as_vector()) {
            if (vox.id() == meta.invalid_voxel_id()) continue;
            vs.emplace(vox.id(), fixed_pi, false);
          }
          break;
        case PIType_t::kPITypeInputVoxel:
          for (auto const& vox : cluster.as_vector()) {
            if (vox.id() == meta.invalid_voxel_id()) continue;
            vs.add(vox);
          }
          break;
        case PIType_t::kPITypeClusterIndex:
          for (auto const& vox : cluster.as_vector()) {
            // check if this voxel already exists
            auto const& prev_vox = vs.find(vox.id());
            if (prev_vox.id() == kINVALID_VOXELID) {
              vs.emplace(vox.id(), (float)(cluster_index + 1), false);
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
            vs.emplace(vox.id(), (float)(cluster_index + 1), false);
          }
          break;
        case PIType_t::kPITypeUndefined:
          throw larbys("PITypeUndefined and kPITypeClusterIndex not supported!");
          break;
        }
      }
      ev_output.emplace(std::move(vs), meta);

      LARCV_INFO() << "EventClusterVoxel3D " << producer << " converted to EventSparseTensor3D " << output_producer << std::endl
                   << "Original meta:" << std::endl << meta.dump() << std::endl
                   << "New meta:" << std::endl << ev_output.meta().dump() << std::endl;

      if (this->logger().level() <= msg::Level_t::kINFO) {
        // report number of voxels per class
        std::vector<size_t> vox_count;
        for (auto const& vox : vs.as_vector()) {
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
