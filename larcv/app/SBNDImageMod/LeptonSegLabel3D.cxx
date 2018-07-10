#ifndef __LEPTONSEGLABEL3D_CXX__
#define __LEPTONSEGLABEL3D_CXX__

#include "LeptonSegLabel3D.h"
#include "larcv/core/DataFormat/EventParticle.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"

namespace larcv {

static LeptonSegLabel3DProcessFactory
    __global_LeptonSegLabel3DProcessFactory__;

LeptonSegLabel3D::LeptonSegLabel3D(
    const std::string name)
    : ProcessBase(name) {}

void LeptonSegLabel3D::configure(const PSet& cfg) {
  _cluster3d_producer = cfg.get<std::string>("Cluster3dProducer");
  _output_producer    = cfg.get<std::string>("OutputProducer");
  _particle_producer  = cfg.get<std::string>("ParticleProducer");

}

void LeptonSegLabel3D::initialize() {}

bool LeptonSegLabel3D::process(IOManager& mgr) {

  // Read in the original source of segmentation, the cluster indexes:
  auto const& ev_cluster3d =
      mgr.get_data<larcv::EventClusterVoxel3D>(_cluster3d_producer);

  // Read in the particles that define the pdg types:
  auto const& ev_particle =
      mgr.get_data<larcv::EventParticle>(_particle_producer);

  // The output is an instance of voxel3d, so prepare that:
  auto& ev_image3d_output = mgr.get_data<larcv::EventSparseTensor3D>(_output_producer);
  ev_image3d_output.clear();

  // Next, loop over the particles and clusters per projection_ID
  // and set the values in the output image to the label specified by
  // the pdg

  auto const& particles = ev_particle.as_vector();

  auto const& out_image =
      seg_image_creator(particles, ev_cluster3d);

  // Append the output image3d:
  ev_image3d_output.set(out_image, ev_cluster3d.meta());

  return true;
}

VoxelSet LeptonSegLabel3D::seg_image_creator(
    const std::vector<Particle> & particles,
    const ClusterVoxel3D & clusters) {
  // Prepare an output image3d:
  VoxelSet out_image;

  // Loop over the particles and get the cluster that matches:
  for (size_t particle_index = 0; particle_index < particles.size();
       ++particle_index) {
    // Deterime this particles PDG code and if it's in the list:
    auto const& particle = particles.at(particle_index);
    int pixel_label = 0;

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
        out_image.emplace(voxel.id(), pixel_label, false);
      }
    }
  }



  return out_image;
}

void LeptonSegLabel3D::finalize() {}
}
#endif
