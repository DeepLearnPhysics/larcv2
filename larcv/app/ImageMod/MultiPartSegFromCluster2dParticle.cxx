#ifndef __MULTIPARTSEGFROMCLUSTER2DPARTICLE_CXX__
#define __MULTIPARTSEGFROMCLUSTER2DPARTICLE_CXX__

#include "MultiPartSegFromCluster2dParticle.h"
#include "larcv/core/DataFormat/EventImage2D.h"
#include "larcv/core/DataFormat/EventParticle.h"
#include "larcv/core/DataFormat/EventVoxel2D.h"

namespace larcv {

static MultiPartSegFromCluster2dParticleProcessFactory
    __global_MultiPartSegFromCluster2dParticleProcessFactory__;

MultiPartSegFromCluster2dParticle::MultiPartSegFromCluster2dParticle(
    const std::string name)
    : ProcessBase(name) {}

void MultiPartSegFromCluster2dParticle::configure(const PSet& cfg) {
  _cluster2d_producer = cfg.get<std::string>("Cluster2dProducer");
  _output_producer = cfg.get<std::string>("OutputProducer");
  _particle_producer = cfg.get<std::string>("ParticleProducer");
  _image2d_producer = cfg.get<std::string>("Image2dProducer");

  _pdg_list = cfg.get<std::vector<int> >("PdgClassList");
  _label_list = cfg.get<std::vector<int> >("LabelList");

  if (_pdg_list.size() == 0) {
    LARCV_CRITICAL() << "Pdg list cannot be zero length." << std::endl;
    throw larbys();
  }

  if (_pdg_list.size() != _label_list.size()) {
    LARCV_CRITICAL() << "Pdg list and label list must be the same length."
                     << std::endl;
    throw larbys();
  }
}

void MultiPartSegFromCluster2dParticle::initialize() {}

bool MultiPartSegFromCluster2dParticle::process(IOManager& mgr) {
  // First, read in the image2d that we need to match the shape of
  auto const& ev_image2d = mgr.get_data<larcv::EventImage2D>(_image2d_producer);

  // Read in the original source of segmentation, the cluster indexes:
  auto const& ev_cluster2d =
      mgr.get_data<larcv::EventClusterPixel2D>(_cluster2d_producer);

  // Read in the particles that define the pdg types:
  auto const& ev_particle =
      mgr.get_data<larcv::EventParticle>(_particle_producer);

  // The output is an instance of image2D, so prepare that:
  auto& ev_image2d_output = mgr.get_data<larcv::EventImage2D>(_output_producer);
  ev_image2d_output.clear();
  // Next, loop over the particles and clusters per projection_ID
  // and set the values in the output image to the label specified by
  // the pdg

  auto const& particles = ev_particle.as_vector();
  for (size_t projection_index = 0;
       projection_index < ev_image2d.as_vector().size(); ++projection_index) {
    // For each projection index, get the list of clusters
    auto const& clusters = ev_cluster2d.cluster_pixel_2d(projection_index);

    auto const& out_image =
        seg_image_creator(particles, clusters,
                          ev_image2d.as_vector().at(projection_index).meta());

    // Append the output image2d:
    ev_image2d_output.append(out_image);
  }

  return true;
}

Image2D MultiPartSegFromCluster2dParticle::seg_image_creator(
    const std::vector<Particle> & particles,
    const ClusterPixel2D & clusters, const ImageMeta & meta) {
  // Prepare an output image2d:
  Image2D out_image(meta);

  // Loop over the particles and get the cluster that matches:
  for (size_t particle_index = 0; particle_index < particles.size();
       ++particle_index) {
    // Deterime this particles PDG code and if it's in the list:
    auto const& particle = particles.at(particle_index);
    int pixel_label = 0;
    for (size_t pdg_label_index = 0; pdg_label_index < _pdg_list.size();
         ++pdg_label_index) {
      if (particle.pdg_code() == _pdg_list.at(pdg_label_index)) {
        pixel_label = _label_list.at(pdg_label_index);
        break;
      }
    }
    // If the label is not zero, go ahead and set the pixels to this label
    // in the output image:
    if (pixel_label != 0) {
      auto const& cluster = clusters.as_vector().at(particle_index);
      // Loop over the pixels in this cluster:
      for (size_t voxel_index = 0; voxel_index < cluster.as_vector().size();
           ++voxel_index) {
        auto const& voxel = cluster.as_vector().at(voxel_index);
        out_image.set_pixel(voxel.id(), pixel_label);
      }
    }
  }

  return out_image;
}

void MultiPartSegFromCluster2dParticle::finalize() {}
}
#endif
