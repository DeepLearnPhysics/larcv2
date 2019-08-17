#ifndef __MichelFILTER_CXX__
#define __MichelFILTER_CXX__

#include "MichelFilter.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
#include "larcv/core/DataFormat/EventParticle.h"

namespace larcv {

  static MichelFilterProcessFactory __global_MichelFilterProcessFactory__;

  MichelFilter::MichelFilter(const std::string name)
    : ProcessBase(name)
  {}
    
  void MichelFilter::configure(const PSet& cfg)
  {
    _particle_label  = cfg.get<std::string>("ParticleLabel");
    _cluster_label   = cfg.get<std::string>("ClusterLabel","");
    _min_energy_init = cfg.get<double>("MinEnergyInit",-1.);
    _min_energy_dep  = cfg.get<double>("MinEnergyDeposit",-1.);
    _min_voxel_sum   = cfg.get<double>("MinVoxelSum",-1.);
    _min_voxel_count = cfg.get<size_t>("MinVoxelCount",0);
    _min_dist_x      = cfg.get<size_t>("MinDistX",0);
    _min_dist_y      = cfg.get<size_t>("MinDistY",0);
    _min_dist_z      = cfg.get<size_t>("MinDistZ",0);
  }

  void MichelFilter::initialize()
  {}

  bool MichelFilter::process(IOManager& mgr)
  {

    auto const& particle_v = mgr.get_data<larcv::EventParticle>(_particle_label).as_vector();
    std::vector<size_t> michel_idx;
    michel_idx.reserve(100);

    for(size_t idx=0; idx<particle_v.size(); ++idx){

      auto const& particle = particle_v[idx];

      if( abs(particle.pdg_code()) != 11 ) continue;
      
      if( abs(particle.parent_pdg_code()) != 13 ) continue;

      if(particle.energy_init() < _min_energy_init) continue;
      
      if(particle.energy_deposit() < _min_energy_dep) continue;

      auto prc = particle.creation_process();
      if(!(prc == "muMinusCaptureAtRest" || prc == "muPlusCaptureAtRest" || prc == "Decay"))
	continue;

      michel_idx.push_back(idx);      
	
    }

    if(_cluster_label.empty()) return (michel_idx.size() > 0);

    auto const& event_cluster = mgr.get_data<larcv::EventClusterVoxel3D>(_cluster_label);
    auto const& meta = event_cluster.meta();
    auto const& cluster_v = event_cluster.as_vector();

    for(auto const& idx : michel_idx) {

      auto const& cluster = cluster_v.at(idx);
      auto const& particle = particle_v.at(idx);

      if(cluster.size() < _min_voxel_count) continue;

      if(cluster.sum() < _min_voxel_sum) continue;

      auto id = meta.id(particle.x(),particle.y(),particle.z());
      if(id == larcv::kINVALID_VOXELID) continue;
      
      size_t id_x, id_y, id_z;
      meta.id_to_xyz_index(id,id_x,id_y,id_z);

      if( (_min_dist_x < id_x) && (id_x < (meta.num_voxel_x() - id_x)) &&
	  (_min_dist_y < id_y) && (id_y < (meta.num_voxel_y() - id_y)) &&
	  (_min_dist_z < id_z) && (id_z < (meta.num_voxel_z() - id_z)) )
	return true;
    }
    
    return false;
  }

  void MichelFilter::finalize()
  {}

}
#endif
