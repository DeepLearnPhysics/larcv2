#ifndef __PARTICLECORRECTOR_CXX__
#define __PARTICLECORRECTOR_CXX__

#include "ParticleCorrector.h"
#include "larcv/core/DataFormat/EventParticle.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
#include <ostream>
namespace larcv {

  static ParticleCorrectorProcessFactory __global_ParticleCorrectorProcessFactory__;

  ParticleCorrector::ParticleCorrector(const std::string name)
    : ProcessBase(name)
  {}

  void ParticleCorrector::configure(const PSet& cfg)
  {
		_correct_energy_deposit = cfg.get<bool>    ( "CorrectEnergyDeposit", true);
    _particle_producer  = cfg.get<std::string> ( "ParticleProducer"        );
    _cluster3d_producer = cfg.get<std::string> ( "Cluster3DProducer"       );
    _voxel_min_value = cfg.get<double>         ( "VoxelMinValue"           );
  }

  void ParticleCorrector::initialize()
  {}

  bool ParticleCorrector::process(IOManager& mgr)
  {
    auto const& event_cluster3d = mgr.get_data< EventClusterVoxel3D > ( _cluster3d_producer );
    auto const& event_particle  = mgr.get_data< EventParticle       > ( _particle_producer  );

    auto const& cluster3d_v = event_cluster3d.as_vector ();
    auto particle_v  = event_particle.as_vector  ();
    if((particle_v.size()+1) != cluster3d_v.size()) {
      LARCV_CRITICAL() << "Size of particles " << particle_v.size() << " mismatch w/ " << cluster3d_v.size() << std::endl;
      throw larbys();
    }

    larcv::Voxel3DMeta meta3d = event_cluster3d.meta();

    // Create vector of voxel positions for each cluster3d ~ particle
    std::vector<std::vector<larcv::Point3D> > positions_v;
    for(size_t i=0; i<particle_v.size(); ++i) {
      std::vector<larcv::Point3D> particle_positions;
      auto const& vs = cluster3d_v[i].as_vector(); // VoxelSet
      // Correct / fill informations on particle
			if (_correct_energy_deposit) particle_v[i].energy_deposit(cluster3d_v[i].sum());
      particle_v[i].num_voxels(vs.size());

      for(size_t j=0; j<vs.size(); ++j) {
        if (vs[j].value() > _voxel_min_value) particle_positions.push_back(meta3d.position(vs[j].id()));
      }
      positions_v.push_back(particle_positions);
    }

    // correct particle positions by looking at each cluster3d energy deposits
    std::vector<Particle> out_particle_v = correct_particle_positions<larcv::Voxel3DMeta>(meta3d, particle_v, positions_v);

    // create output
    auto& out_particle  = mgr.get_data< EventParticle       > (  _particle_producer  );
    out_particle.emplace  ( std::move(out_particle_v)              );

    return true;
  }

  void ParticleCorrector::finalize()
  {}

  template <class bbox3d>
  std::vector<larcv::Particle> correct_particle_positions(
    bbox3d const& meta3d,
    std::vector<larcv::Particle> particle_v,
    std::vector<std::vector<larcv::Point3D> > cluster3d_v
  ) {
    std::vector<larcv::Particle> out_particle_v;
    for(size_t i=0; i<particle_v.size(); ++i) {
      auto particle  = particle_v[i];
      auto const& vs = cluster3d_v[i];

      // LArbys => neither track nor shower
      if (particle.shape() == kShapeUnknown || particle.shape() == kShapeShower) {
        out_particle_v.push_back(particle);
        continue;
      }

      bool correctStart = !(meta3d.contains(particle.position().as_point3d()));
      bool correctEnd = !meta3d.contains(particle.end_position().as_point3d());
      LARCV_INFO() << particle.pdg_code() << " " << particle.id() << " " << correctStart << " " << correctEnd << " " << vs.size() << std::endl;

      // If the particle is a shower and start position is included
      // OR it is a track with both start/end positions included
      // then continue without any correction
      if (!correctStart && !correctEnd) {
        auto start = particle.position().as_point3d();
        auto end = particle.end_position().as_point3d();
        particle.first_step(start.x, start.y, start.z, particle.position().t());
        particle.last_step(end.x, end.y, end.z, particle.end_position().t());
        out_particle_v.push_back(particle);
        continue;
      }

      // Check for cluster vs boundaries
      int i_best_start = -1;
      int i_best_end = -1;
      double distance_best_start = std::numeric_limits<double>::max();
      double distance_best_end = std::numeric_limits<double>::max();
      for(size_t j=0; j<vs.size(); ++j) {
        auto point3d = vs[j];
        if (meta3d.contains(point3d)) {
          if (correctStart) {
            double distance_start = point3d.distance(particle.position().as_point3d());
            if (distance_start < distance_best_start) {
              i_best_start = j;
              distance_best_start = distance_start;
            }
          }
          if (correctEnd) {
            double distance_end = point3d.distance(particle.end_position().as_point3d());
            if (distance_end < distance_best_end) {
              i_best_end = j;
              distance_best_end = distance_end;
            }
          }
        }
      }

      // Correct starting point
      if (correctStart && i_best_start > -1) {
        LARCV_INFO() << particle.pdg_code() << " " << particle.id() << " " << vs[i_best_start].x << " " << vs[i_best_start].y << " " << vs[i_best_start].z << std::endl;
        particle.first_step(vs[i_best_start].x, vs[i_best_start].y, vs[i_best_start].z, particle.first_step().t());
      }
      else {
        auto start = particle.position().as_point3d();
        particle.first_step(start.x, start.y, start.z, particle.position().t());
      }

      // Correct end point
      if (correctEnd && i_best_end > -1) {
        LARCV_INFO() << particle.pdg_code() << " " << particle.id() << " " << vs[i_best_end].x << " " << vs[i_best_end].y << " " << vs[i_best_end].z << std::endl;
        particle.last_step(vs[i_best_end].x, vs[i_best_end].y, vs[i_best_end].z, particle.last_step().t());
      }
      else {
        auto end = particle.end_position().as_point3d();
        particle.last_step(end.x, end.y, end.z, particle.end_position().t());
      }

      out_particle_v.push_back(particle);
    }
    return out_particle_v;
  }
}
#endif
