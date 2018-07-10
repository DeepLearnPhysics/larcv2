#ifndef __LEPTONSEGLABEL_CXX__
#define __LEPTONSEGLABEL_CXX__

#include "LeptonSegLabel.h"
#include "larcv/core/DataFormat/EventImage2D.h"
#include "larcv/core/DataFormat/EventParticle.h"
#include "larcv/core/DataFormat/EventVoxel2D.h"

namespace larcv {

static LeptonSegLabelProcessFactory
    __global_LeptonSegLabelProcessFactory__;

LeptonSegLabel::LeptonSegLabel(
    const std::string name)
    : ProcessBase(name) {}

void LeptonSegLabel::configure(const PSet& cfg) {
  _cluster2d_producer = cfg.get<std::string>("Cluster2dProducer");
  _output_producer    = cfg.get<std::string>("OutputProducer");
  _particle_producer  = cfg.get<std::string>("ParticleProducer");
  // _cosmic_label       = cfg.get<int>("CosmicLabel");
  // _neutrino_label     = cfg.get<int>("NeutrinoLabel");

}

void LeptonSegLabel::initialize() {}

bool LeptonSegLabel::process(IOManager& mgr) {

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

Image2D LeptonSegLabel::seg_image_creator(
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
    if (abs(particle.pdg_code()) == 13 ||
        abs(particle.pdg_code()) == 11) {
      pixel_label = 1;
    }
    else {
      pixel_label = 2;
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

void LeptonSegLabel::finalize() {}
}
#endif
