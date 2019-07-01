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
    _shift_xyz.clear();
    _shift_xyz.resize(3,0.);
    _shift_xyz = cfg.get<std::vector<double> > ( "ShiftXYZ", _shift_xyz);
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

    // Loop over clusters and set corrected energy sum and voxel count info
    for(size_t i=0; i<particle_v.size(); ++i) {
      auto const& vs = cluster3d_v[i]; // VoxelSet
      // Correct / fill informations on particle
      if (_correct_energy_deposit) particle_v[i].energy_deposit(vs.sum());
      particle_v[i].num_voxels(vs.size());
    }

    // correct particle positions by looking at each cluster3d energy deposits
    std::vector<Particle> out_particle_v = correct_particle_positions(meta3d, particle_v, cluster3d_v);

    // create output
    auto& out_particle  = mgr.get_data< EventParticle       > (  _particle_producer  );
    out_particle.emplace  ( std::move(out_particle_v)              );

    return true;
  }

  void ParticleCorrector::finalize()
  {}

  std::vector<larcv::Particle>
  ParticleCorrector::correct_particle_positions(const larcv::Voxel3DMeta& meta3d,
						std::vector<larcv::Particle> particle_v,
						std::vector<larcv::VoxelSet> cluster3d_v
						)
  {
    std::vector<larcv::Particle> out_particle_v;
    float size_voxel = (meta3d.size_voxel_x() + meta3d.size_voxel_y() + meta3d.size_voxel_z())/3.;
    for(size_t i=0; i<particle_v.size(); ++i) {
      auto particle  = particle_v[i];
      auto const& vs = cluster3d_v[i].as_vector();
      //if(vs.size()<1) continue;
      
      auto first_step = particle.first_step().as_point3d();
      auto last_step  = particle.last_step().as_point3d();
      first_step.x += _shift_xyz[0];
      first_step.y += _shift_xyz[1];
      first_step.z += _shift_xyz[2];
      last_step.x += _shift_xyz[0];
      last_step.y += _shift_xyz[1];
      last_step.z += _shift_xyz[2];
      particle.first_step(first_step.x,first_step.y,first_step.z,particle.first_step().t());
      particle.last_step(last_step.x,last_step.y,last_step.z,particle.last_step().t());

      bool correctStart = true;
      bool correctEnd = true;
      
      if( abs(particle.pdg_code()) == 11 || particle.pdg_code() == 22) correctEnd = false;
      
      LARCV_INFO() << "PDG: " << particle.pdg_code()
		   << " start: (" << first_step.x << "," << first_step.y << "," << first_step.z << ") ..." 
		   << " end: (" << last_step.x << "," << last_step.y << "," << last_step.z << ") ..."
		   << std::endl;
      
      // Check for cluster vs boundaries
      int i_best_start = -1;
      int i_best_end = -1;
      double distance_best_start = std::numeric_limits<double>::max();
      double distance_best_end = std::numeric_limits<double>::max();
      //correctStart = correctEnd = true;
      for(size_t j=0; j<vs.size(); ++j) {
	if(vs[j].value() < _voxel_min_value) continue;
	auto point3d = meta3d.position(vs[j].id());
	
	//double distance_start = point3d.distance(particle.position().as_point3d());
	if(correctStart) {
	  double distance_start = point3d.distance(first_step);
	  if (distance_start < distance_best_start) {
	    i_best_start = j;
	    distance_best_start = distance_start;
	    //if(distance_start < 0.867) correctStart = false;
	    if(distance_start < size_voxel) correctStart = false;
	  }
	}
	//double distance_end = point3d.distance(particle.end_position().as_point3d());
	if(correctEnd) {
	  double distance_end = point3d.distance(last_step);
	  if (distance_end < distance_best_end) {
	    i_best_end = j;
	    distance_best_end = distance_end;
	    //if(distance_end < 0.867) correctEnd = false;
	    if(distance_end < size_voxel) correctEnd = false;
	  }
	}
	if(!correctStart && !correctEnd) break;
      }

      // Correct starting point
      if (correctStart && i_best_start > -1) {
	auto const pt = meta3d.position(vs[i_best_start].id());
        LARCV_INFO() << "Correcting PDG " << particle.pdg_code() << " START"
		     << " XYZ (" << pt.x << "," << pt.y << "," << pt.z << ")" << std::endl;
        particle.first_step(pt.x, pt.y, pt.z, particle.first_step().t());
      }
      /*
      else {
        //auto start = particle.position().as_point3d();
        particle.first_step(start.x, start.y, start.z, particle.position().t());
      }
      */
      // Correct end point
      if (correctEnd && i_best_end > -1) {
	auto const pt = meta3d.position(vs[i_best_end].id());
        LARCV_INFO() << "Correcting PDG " << particle.pdg_code() << " END"
		     << " XYZ (" << pt.x << "," << pt.y << "," << pt.z << ")" << std::endl;
        particle.last_step(pt.x, pt.y, pt.z, particle.last_step().t());
      }
      /*
      else {
        //auto end = particle.end_position().as_point3d();
        particle.last_step(end.x, end.y, end.z, particle.end_position().t());
      }
      */
      out_particle_v.push_back(particle);
    }
    return out_particle_v;
  }
}
#endif
