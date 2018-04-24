#ifndef __MCTRACKFIX_CXX__
#define __MCTRACKFIX_CXX__

#include "MCTrackFix.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
#include "larcv/core/DataFormat/EventParticle.h"
namespace larcv {

  static MCTrackFixProcessFactory __global_MCTrackFixProcessFactory__;

  MCTrackFix::MCTrackFix(const std::string name)
    : ProcessBase(name)
  {}
    
  void MCTrackFix::configure(const PSet& cfg)
  {
    _cluster3d_producer = cfg.get<std::string>("Cluster3DProducer");
    _particle_producer  = cfg.get<std::string>("ParticleProducer");
  }

  void MCTrackFix::initialize()
  {}

  bool MCTrackFix::process(IOManager& mgr)
  {
    auto const& event_cluster3d = mgr.get_data< EventClusterVoxel3D > ( _cluster3d_producer );
    auto&       event_particle  = mgr.get_data< EventParticle       > ( _particle_producer  );

    auto const& cluster3d_v = event_cluster3d.as_vector();
    std::vector<Particle> particle_v = event_particle.as_vector();

    if((particle_v.size()+1) != cluster3d_v.size()) {
      LARCV_CRITICAL() << "Size of particles " << particle_v.size() << " mismatch w/ " << cluster3d_v.size() << std::endl;
      throw larbys();
    }

    auto const& meta3d = event_cluster3d.meta();
    for(size_t i=0; i<particle_v.size(); ++i) {
      auto&       particle = particle_v  [i];
      auto const& cluster  = cluster3d_v [i];
      auto const& vox_v    = cluster.as_vector();

      if(vox_v.size()<2) continue;
      if(particle.shape()==0) continue; // if shower

      // get last point
      auto const& first_step = particle.first_step ();
      auto const& last_step  = particle.last_step  ();
      Point3D first_pt (first_step.x(), first_step.y(), first_step.z());
      Point3D last_pt  (last_step.x(),  last_step.y(),  last_step.z() );

      LARCV_INFO() << "Last step before: (" << last_pt.x << "," << last_pt.y << "," << last_pt.z << ")" << std::endl;
      if(first_pt == last_pt) continue;

      // find closest voxel to the boundary within 
      double min_dist = 1e12;
      Point3D new_last_pt(last_pt);
      for(auto const& vox : vox_v) {
	auto pos = meta3d.position(vox.id());
	if(!TouchingWall(meta3d,pos)) continue;
	double dist_to_first_pt = pos.squared_distance(first_pt);
	double dist_to_last_pt  = pos.squared_distance(last_pt);
	if(dist_to_first_pt < dist_to_last_pt)
	  continue;
	
	if(dist_to_last_pt > 25) continue;
	if(dist_to_last_pt > min_dist) continue;
	min_dist = dist_to_last_pt;
	new_last_pt = pos;
      }

      LARCV_INFO() << "Last step after : (" << new_last_pt.x << "," << new_last_pt.y << "," << new_last_pt.z << ")" << std::endl;
      particle.last_step(Vertex(new_last_pt.x, new_last_pt.y, new_last_pt.z, last_step.t()));
    }

    event_particle.emplace(std::move(particle_v));
    return true;
  }

  bool MCTrackFix::TouchingWall(const Voxel3DMeta& meta3d, 
				const Point3D& pt) const
  {
    
    double dist_x = std::min(pt.x - meta3d.min_x(), meta3d.max_x() - pt.x);
    if( dist_x < meta3d.size_voxel_x() ) return true;
    double dist_y = std::min(pt.y - meta3d.min_y(), meta3d.max_y() - pt.y);
    if( dist_y < meta3d.size_voxel_y() ) return true;
    double dist_z = std::min(pt.z - meta3d.min_z(), meta3d.max_z() - pt.z);
    if( dist_z < meta3d.size_voxel_z() ) return true;
    return false;
  }

  void MCTrackFix::finalize()
  {}

}
#endif
