#ifndef __REMOVEDETECTORINFO_CXX__
#define __REMOVEDETECTORINFO_CXX__

#include "RemoveDetectorInfo.h"
#include "larcv/core/DataFormat/EventParticle.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
namespace larcv {

  static RemoveDetectorInfoProcessFactory
  __global_RemoveDetectorInfoProcessFactory__;

  RemoveDetectorInfo::RemoveDetectorInfo(const std::string name)
    : ProcessBase(name) {}

  void RemoveDetectorInfo::configure(const PSet& cfg) {
    _cluster3d_producer = cfg.get<std::string>("Cluster3DMetaProducer");
    _particle_producer  = cfg.get<std::string>("ParticleProducer","");
  }

  void RemoveDetectorInfo::initialize() {}

  bool RemoveDetectorInfo::process(IOManager& mgr) {
    // First, find the origin of all 3D objects in this event from the
    // Cluster3DMeta specified in the producer.  Then, remove it from all objects
    // present in the event by overwriting them with modified copies.

    auto const& ev_cluster3d =
      mgr.get_data<larcv::EventClusterVoxel3D>(_cluster3d_producer);
    auto origin = ev_cluster3d.meta().origin();

    // Second, gather energy deposition info if _particle_producer is specified.
    // This is used to correct the "energy deposit" information per-particle.
    std::vector<double> energy_deposited_v;
    if(!_particle_producer.empty()) {
      auto const& voxel_set_v = ev_cluster3d.as_vector();
      energy_deposited_v.reserve(voxel_set_v.size());
      for(auto const& vs : voxel_set_v)
        energy_deposited_v.push_back(vs.sum());
    }

    // Now remove the origin from all instances of objects.

    // Particles:
    auto particle_producers = mgr.producer_list("particle");

    // Loop over every producer:
    for (auto& part_producer : particle_producers) {
      // Get the particles:
      auto const& ev_part_input =
        mgr.get_data<larcv::EventParticle>(part_producer);

      auto particle_v = ev_part_input.as_vector();

      // Figure out whether or not to correct for the deposited energy
      bool correct_energy_deposited = (part_producer == _particle_producer);
      // Check if the # particles makes sense to match with deposited energy count
      if(correct_energy_deposited && (particle_v.size()+1) != energy_deposited_v.size()) {
        LARCV_CRITICAL() << "Specified Cluster3D producer " << _cluster3d_producer
        << " has un-expected size of clusters (" << energy_deposited_v.size() << ")"
        << " given the Particle producer " << _particle_producer
        << " which has size " << particle_v.size() << "!" << std::endl;
        throw larbys();
      }

      // Loop over particles to modify info
      for (auto& part : particle_v) {
        // Set values for the new particle:
        part.position(part.x() - origin.x, part.y() - origin.y,
                      part.z() - origin.z, part.t());
        part.end_position(part.end_position().x() - origin.x,
                          part.end_position().y() - origin.y,
                          part.end_position().z() - origin.z,
                          part.end_position().t());
        part.first_step(part.first_step().x() - origin.x,
                        part.first_step().y() - origin.y,
                        part.first_step().z() - origin.z, part.first_step().t());
        part.parent_position(part.parent_position().x() - origin.x,
                             part.parent_position().y() - origin.y,
                             part.parent_position().z() - origin.z, part.parent_position().t());

	part.last_step(part.last_step().x() - origin.x,
			     part.last_step().y() - origin.y,
			     part.last_step().z() - origin.z, part.last_step().t());

        //
        // Remove last_step and distance_travel as those are detector geometry specific
        //
        //part.last_step(kINVALID_DOUBLE,kINVALID_DOUBLE,kINVALID_DOUBLE,kINVALID_DOUBLE);
        part.distance_travel(-1.);

        if(correct_energy_deposited)
          part.energy_deposit(energy_deposited_v[part.id()]);
      }
      auto& ev_part_output = mgr.get_data<larcv::EventParticle>(part_producer);
      ev_part_output.emplace(std::move(particle_v));
    }

    // Clusters:
    auto cluster_producers = mgr.producer_list("cluster3d");

    // Loop over every producer:
    for (auto& cluster_producer : cluster_producers) {
      // Get the particles:
      auto const& ev_cluster_input =
        mgr.get_data<larcv::EventClusterVoxel3D>(cluster_producer);

      auto this_meta = ev_cluster_input.meta();
      if(!this_meta.valid()) {
        LARCV_INFO() << "Skipping invalid Voxel3DMeta by producer " 
        << cluster_producer << std::endl;
        continue;
      }
      LARCV_INFO() << "Updating from old Voxel3DMeta:" << std::endl
      << this_meta.dump() << std::endl
      << "... shifting with (x,y,z) => (" 
      << origin.x << "," << origin.y << "," << origin.z << ")" << std::endl;

      this_meta.set(this_meta.min_x() - origin.x,
                     this_meta.min_y() - origin.y, 
                     this_meta.min_z() - origin.z,
                     this_meta.max_x() - origin.x,
                     this_meta.max_y() - origin.y, 
                     this_meta.max_z() - origin.z,
                     this_meta.num_voxel_x(), 
                     this_meta.num_voxel_y(),
                     this_meta.num_voxel_z(),
                     this_meta.unit());

      auto& ev_cluster_output = mgr.get_data<larcv::EventClusterVoxel3D>(cluster_producer);
      ev_cluster_output.meta(this_meta);
    }

    // Tensors
    auto tensor3d_producers = mgr.producer_list("sparse3d");

    // Loop over every producer:
    for (auto& tensor3d_producer : tensor3d_producers) {
      // Get the particles:
      auto const& ev_tensor3d_input =
        mgr.get_data<larcv::EventSparseTensor3D>(tensor3d_producer);

      auto this_meta = ev_tensor3d_input.meta();
      if(!this_meta.valid()) {
        LARCV_INFO() << "Skipping invalid Voxel3DMeta by producer " 
        << tensor3d_producer << std::endl;
        continue;
      }
      LARCV_INFO() << "Updating from old Voxel3DMeta:" << std::endl
      << this_meta.dump() << std::endl
      << "... shifting with (x,y,z) => (" 
      << origin.x << "," << origin.y << "," << origin.z << ")" << std::endl;


      this_meta.set(this_meta.min_x() - origin.x,
                     this_meta.min_y() - origin.y, 
                     this_meta.min_z() - origin.z,
                     this_meta.max_x() - origin.x,
                     this_meta.max_y() - origin.y, 
                     this_meta.max_z() - origin.z,
                     this_meta.num_voxel_x(), 
                     this_meta.num_voxel_y(),
                     this_meta.num_voxel_z(),
                     this_meta.unit());

      auto& ev_tensor3d_output = mgr.get_data<larcv::EventSparseTensor3D>(tensor3d_producer);
      ev_tensor3d_output.meta(this_meta);
    }

    return true;
  }

  void RemoveDetectorInfo::finalize() {}
}
#endif
