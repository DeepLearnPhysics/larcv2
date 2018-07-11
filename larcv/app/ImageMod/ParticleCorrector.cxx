#ifndef __PARTICLECORRECTOR_CXX__
#define __PARTICLECORRECTOR_CXX__

#include "ParticleCorrector.h"
#include "larcv/core/DataFormat/EventParticle.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"

namespace larcv {

  static ParticleCorrectorProcessFactory __global_ParticleCorrectorProcessFactory__;

  ParticleCorrector::ParticleCorrector(const std::string name)
    : ProcessBase(name)
  {}

  void ParticleCorrector::configure(const PSet& cfg)
  {
    _particle_producer  = cfg.get<std::string> ( "ParticleProducer"        );
    _cluster3d_producer = cfg.get<std::string> ( "Cluster3DProducer"       );
  }

  void ParticleCorrector::initialize()
  {}

  bool ParticleCorrector::process(IOManager& mgr)
  {
    auto const& event_cluster3d = mgr.get_data< EventClusterVoxel3D > ( _cluster3d_producer );
    auto const& event_particle  = mgr.get_data< EventParticle       > ( _particle_producer  );

    auto const& cluster3d_v = event_cluster3d.as_vector ();
    auto const& particle_v  = event_particle.as_vector  ();
    if((particle_v.size()+1) != cluster3d_v.size()) {
      LARCV_CRITICAL() << "Size of particles " << particle_v.size() << " mismatch w/ " << cluster3d_v.size() << std::endl;
      throw larbys();
    }
    LARCV_WARNING() << "Size of particles " << particle_v.size() << " w/ " << cluster3d_v.size() << std::endl;
    larcv::Voxel3DMeta meta3d = event_cluster3d.meta();
    LARCV_WARNING() << "Meta3D : " << meta3d.min_x() << " < x < " << meta3d.max_x() << std::endl;
    LARCV_WARNING() << "Meta3D : " << meta3d.min_y() << " < y < " << meta3d.max_y() << std::endl;
    LARCV_WARNING() << "Meta3D : " << meta3d.min_z() << " < z < " << meta3d.max_z() << std::endl;
    LARCV_WARNING() << "Size voxel : " << meta3d.size_voxel_x() << " " << meta3d.size_voxel_y() << " " << meta3d.size_voxel_z() << std::endl;
    std::vector<larcv::Particle> out_particle_v;

    for(size_t i=0; i<particle_v.size(); ++i) {
      auto particle  = particle_v[i];
      auto const& vs = cluster3d_v[i].as_vector(); // VoxelSet
      LARCV_WARNING() << "Position before " << particle.position().dump() << std::endl;
      LARCV_WARNING() << "First step before " << particle.first_step().dump() << std::endl;
      LARCV_WARNING() << "Last step before " << particle.last_step().dump() << std::endl;
      LARCV_WARNING() << "End position before " << particle.end_position().dump() << std::endl;
      // Check for cluster vs boundaries
      for(size_t j=0; j<vs.size(); ++j) {
        auto point3d = meta3d.position(vs[j].id());
        if (meta3d.hasBoundaryCloseTo(point3d) && meta3d.contains(point3d)) {
          //LARCV_WARNING() << "boundary close to " << point3d.x << " " << point3d.y << " " << point3d.z << std::endl;
          // Correct positions of corresponding particle

          double a = point3d.distance(particle.position().as_point3d());
          double b = point3d.distance(particle.first_step().as_point3d());
          double c = point3d.distance(particle.last_step().as_point3d());
          double d = point3d.distance(particle.end_position().as_point3d());
          std::vector<double> distances{a, b, c, d};

          switch(std::distance(distances.begin(), std::min_element(distances.begin(), distances.end()))) {
            case 0: // Position
              //LARCV_WARNING() << "Position before = " << particle.position().dump() << std::endl;
              //particle.position(point3d.x, point3d.y, point3d.z, particle.position().t());
              //LARCV_WARNING() << "Position after = " << particle.position().dump() << std::endl;
              //break;
            case 1: // first step
              //LARCV_WARNING() << "first step before = " << particle.first_step().dump() << std::endl;
              particle.first_step(point3d.x, point3d.y, point3d.z, particle.first_step().t());
              //LARCV_WARNING() << "first step after = " << particle.first_step().dump() << std::endl;
              break;
            case 3: // end position
              //LARCV_WARNING() << "end position before = " << particle.end_position().dump() << std::endl;
              //particle.end_position(point3d.x, point3d.y, point3d.z, particle.end_position().t());
              //LARCV_WARNING() << "end position after = " << particle.end_position().dump() << std::endl;
              //break;
            case 2: // last step
              //LARCV_WARNING() << "last step before = " << particle.last_step().dump() << std::endl;
              particle.last_step(point3d.x, point3d.y, point3d.z, particle.last_step().t());
              //LARCV_WARNING() << "last step after = " << particle.last_step().dump() << std::endl;
              break;

            default:
              LARCV_CRITICAL() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
          }
        }
      }

      LARCV_WARNING() << "Position after " << particle.position().dump() << std::endl;
      LARCV_WARNING() << "First step after " << particle.first_step().dump() << std::endl;
      LARCV_WARNING() << "Last step after " << particle.last_step().dump() << std::endl;
      LARCV_WARNING() << "End position after " << particle.end_position().dump() << std::endl;

      out_particle_v.push_back(particle);
    }

    // create output
    auto& out_particle  = mgr.get_data< EventParticle       > ( _particle_producer  );
    out_particle.emplace  ( std::move(out_particle_v)              );

    return true;
  }

  void ParticleCorrector::finalize()
  {}

}
#endif
