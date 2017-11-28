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
}

void RemoveDetectorInfo::initialize() {}

bool RemoveDetectorInfo::process(IOManager& mgr) {
  // First, find the origin of all 3D objects in this event from the
  // Cluster3DMeta specified in the producer.  Then, remove it from all objects
  // present in the event by overwriting them with modified copies.

  auto const& ev_voxel3d =
      mgr.get_data<larcv::EventClusterVoxel3D>(_cluster3d_producer);
  auto const& meta3d = ev_voxel3d.meta();

  auto origin = meta3d.origin();

  // Now remove the origin from all instances of objects.

  // Particles:
  auto& _particle_producers = mgr.producer_list("particle");

  // Loop over every producer:
  for (auto& part_producer : _particle_producers) {
    // Get the particles:
    auto const& ev_part_input =
        mgr.get_data<larcv::EventParticle>(part_producer);

    auto particle_v = ev_part_input.as_vector();

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
      part.last_step(part.last_step().x() - origin.x,
                     part.last_step().y() - origin.y,
                     part.last_step().z() - origin.z, part.last_step().t());
    }
    auto& ev_part_output = mgr.get_data<larcv::EventParticle>(part_producer);
    ev_part_output.emplace(std::move(particle_v));
  }

  // Clusters:
  auto& _cluster_producers = mgr.producer_list("cluster3d");

  // Loop over every producer:
  for (auto& cluster_producer : _cluster_producers) {
    // Get the particles:
    auto const& ev_cluster_input =
        mgr.get_data<larcv::EventClusterVoxel3D>(cluster_producer);

    auto _this_meta = ev_cluster_input.meta();
    _this_meta.set(0.0, 0.0, 0.0, _this_meta.max_x() - origin.x(),
                   _this_meta.max_y - origin.y(), _this_meta.max_z - origin.z(),
                   _this_meta.num_voxel_x(), _this_meta.num_voxel_y(),
                   _this_meta.num_voxel_z());

    auto& ev_cluster_output = mgr.get_data<larcv::EventClusterVoxel3D>(cluster_producer);
    ev_cluster_output.meta(_this_meta);
    // ev_cluster_output.emplace(std::move(cluster_v));

    // Need a line to store ev_cluster_output
  }

  return true;
}

void RemoveDetectorInfo::finalize() {}
}
#endif
