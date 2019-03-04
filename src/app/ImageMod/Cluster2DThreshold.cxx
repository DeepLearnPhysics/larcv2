#ifndef __CLUSTER2DTHRESHOLD_CXX__
#define __CLUSTER2DTHRESHOLD_CXX__

#include "Cluster2DThreshold.h"
#include "larcv/core/DataFormat/EventVoxel2D.h"

namespace larcv {

static Cluster2DThresholdProcessFactory
    __global_Cluster2DThresholdProcessFactory__;

Cluster2DThreshold::Cluster2DThreshold(const std::string name)
    : ProcessBase(name) {}

void Cluster2DThreshold::configure_labels(const PSet& cfg) {
  _cluster2d_producer_v.clear();
  _output_producer_v.clear();
  _cluster2d_producer_v
    = cfg.get<std::vector<std::string> >("Cluster2DProducerList",
                                         _cluster2d_producer_v);
  _output_producer_v
    = cfg.get<std::vector<std::string> >("OutputProducerList",
                                         _output_producer_v);

  if (_cluster2d_producer_v.empty()) {
    auto cluster2d_producer = cfg.get<std::string>("Cluster2DProducer", "");
    auto output_producer = cfg.get<std::string>("OutputProducer", "");
    if (!cluster2d_producer.empty()) {
      _cluster2d_producer_v.push_back(cluster2d_producer);
      if (output_producer.empty())
        output_producer = cluster2d_producer + "_threshold";
      _output_producer_v.push_back(output_producer);
    }
  }

  if (_output_producer_v.empty()) {
    _output_producer_v.resize(_cluster2d_producer_v.size(), "");
  } else if (_output_producer_v.size() != _cluster2d_producer_v.size()) {
    LARCV_CRITICAL() << "Cluster2DProducer and OutputProducer must have the "
                        "same array length!"
                     << std::endl;
    throw larbys();
  }
}

void Cluster2DThreshold::configure(const PSet& cfg) {
  configure_labels(cfg);

  _thresholds_v = cfg.get<std::vector<float> >("ThresholdList", _thresholds_v);
  if (_thresholds_v.empty()) {
    auto threshold = cfg.get<float>("Threshold");
    _thresholds_v.push_back(threshold);
  }

  if (_thresholds_v.empty()) {
    LARCV_CRITICAL() << "Must specify a threshold" << std::endl;
    throw larbys();
  }

  if (_thresholds_v.size() != _cluster2d_producer_v.size()) {
    LARCV_CRITICAL() << "Must specify thresholds for all producers"
                     << std::endl;
    throw larbys();
  }
}

void Cluster2DThreshold::initialize() {}

bool Cluster2DThreshold::process(IOManager& mgr) {

  // std::cout << "Enter Cluster2DThreshold::process " << std::endl;
  for (size_t producer_index = 0; producer_index < _cluster2d_producer_v.size();
       ++producer_index) {
    auto const& cluster2d_producer = _cluster2d_producer_v[producer_index];
    auto const& output_producer = _output_producer_v[producer_index];

    auto const& threshold = _thresholds_v[producer_index];

    auto const& ev_cluster2d =
        mgr.get_data<larcv::EventClusterPixel2D>(cluster2d_producer);
    auto& ev_cluster2d_output =
        mgr.get_data<larcv::EventClusterPixel2D>(output_producer);

    auto const& cluster2d_v = ev_cluster2d.as_vector();

    for (size_t cluster_index = 0; cluster_index < cluster2d_v.size();
         ++cluster_index) {
      // Create a voxelsetarray for each cluster

      auto const& cluster_pixel_2d = cluster2d_v.at(cluster_index);


      // Each ClusterPixel2D has a vector of VoxelSets,
      // and each VoxelSet has a vector of voxels
      //
      // Loop over the ClusterPixel2D and copy all voxels
      // above thresholds to the new producers

      ClusterPixel2D thresholded_cluster_pixel_2d;

      // Output meta is the same as new meta
      thresholded_cluster_pixel_2d.meta(cluster_pixel_2d.meta());

      for (size_t voxel_set_index = 0;
           voxel_set_index < cluster_pixel_2d.as_vector().size();
           ++voxel_set_index) {
        // Look at each voxel set, which allows iteration of voxels
        auto const& original_voxel_set =
            cluster_pixel_2d.as_vector().at(voxel_set_index);
        // Create new voxel sets:
        VoxelSet thresholded_voxels;
        thresholded_voxels.id(original_voxel_set.id());

        for (size_t voxel_index = 0;
             voxel_index < original_voxel_set.as_vector().size();
             ++voxel_index) {
          auto const& voxel = original_voxel_set.as_vector().at(voxel_index);
          // std::cout <<"---Voxel value: " << voxel.value() << ", threshold: " << threshold << std::endl;
          if (voxel.value() >= threshold) {
            // std::cout <<"----above threshold" << std::endl;
            // thresholded_voxels.emplace(std::move(voxel), true);
            thresholded_voxels.add(voxel);
          }
        }
        // There is now a VoxelSet of thresholded voxels, add it to the voxel
        // set:
        thresholded_cluster_pixel_2d.emplace(std::move(thresholded_voxels));
      }

      // Add the ClusterPixel2D to the event data:
      ev_cluster2d_output.emplace(std::move(thresholded_cluster_pixel_2d));

    }

  }
  // std::cout << "Exit Cluster2DThreshold::process " << std::endl;

  return true;
}

void Cluster2DThreshold::finalize() {}
}
#endif
