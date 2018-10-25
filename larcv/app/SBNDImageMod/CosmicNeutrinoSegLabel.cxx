#ifndef __COSMICNEUTRINOSEGLABEL_CXX__
#define __COSMICNEUTRINOSEGLABEL_CXX__

#include "CosmicNeutrinoSegLabel.h"
#include "larcv/core/DataFormat/EventImage2D.h"
#include "larcv/core/DataFormat/EventParticle.h"
#include "larcv/core/DataFormat/EventVoxel2D.h"

namespace larcv {

static CosmicNeutrinoSegLabelProcessFactory
    __global_CosmicNeutrinoSegLabelProcessFactory__;

CosmicNeutrinoSegLabel::CosmicNeutrinoSegLabel(
    const std::string name)
    : ProcessBase(name) {}

void CosmicNeutrinoSegLabel::configure(const PSet& cfg) {
  _cluster2d_producer = cfg.get<std::string>("Cluster2dProducer");
  _output_producer    = cfg.get<std::string>("OutputProducer");
  _particle_producer  = cfg.get<std::string>("ParticleProducer");
  _cosmic_label       = cfg.get<int>("CosmicLabel");
  _neutrino_label     = cfg.get<int>("NeutrinoLabel");

}

void CosmicNeutrinoSegLabel::initialize() {}

bool CosmicNeutrinoSegLabel::process(IOManager& mgr) {

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
       projection_index < ev_cluster2d.size(); ++projection_index) {
    // For each projection index, get the list of clusters
    auto const& clusters = ev_cluster2d.cluster_pixel_2d(projection_index);

    auto const& out_image =
        seg_image_creator(particles, clusters,
                          ev_cluster2d.cluster_pixel_2d(projection_index).meta());

    // Append the output image2d:
    ev_image2d_output.append(out_image);
  }

  return true;
}

Image2D CosmicNeutrinoSegLabel::seg_image_creator(
    const std::vector<Particle> & particles,
    const ClusterPixel2D & clusters, const ImageMeta & meta) {
  // Prepare an output image2d:
  Image2D out_image(meta);

  std::set<std::string> processes;
  std::set<int> interaction_types;

  // Loop over the particles and get the cluster that matches:
  for (size_t particle_index = 0; particle_index < particles.size();
       ++particle_index) {
    // Deterime this particles PDG code and if it's in the list:
    auto const& particle = particles.at(particle_index);
    int pixel_label = 0;
    processes.insert(particle.creation_process());
    interaction_types.insert(particle.nu_interaction_type());
    if (particle.nu_interaction_type() == _neutrino_label){
      pixel_label = 2;
    }
    else if (particle.nu_interaction_type() == _cosmic_label) {
      pixel_label = 1;
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
  // for (auto & process : processes){
  //   std::cout << "Process: " << process << std::endl;
  // }
  // for (auto & type : interaction_types){
  //   std::cout << "type: " << type << std::endl;
  // }


  return out_image;
}

void CosmicNeutrinoSegLabel::finalize() {}
}
#endif
