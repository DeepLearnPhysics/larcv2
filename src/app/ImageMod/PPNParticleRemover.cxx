#ifndef __PPNPARTICLEREMOVER_CXX__
#define __PPNPARTICLEREMOVER_CXX__

#include "PPNParticleRemover.h"
#include "larcv/core/DataFormat/EventParticle.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
namespace larcv {

  static PPNParticleRemoverProcessFactory __global_PPNParticleRemoverProcessFactory__;

  PPNParticleRemover::PPNParticleRemover(const std::string name)
    : ProcessBase(name)
  {}
    
  void PPNParticleRemover::configure(const PSet& cfg)
  {
    _particle_producer  = cfg.get<std::string> ( "ParticleProducer"        );
    _cluster3d_producer = cfg.get<std::string> ( "Cluster3DProducer"       );
    _pxsum_threshold    = cfg.get<double>      ( "PixelSumThreshold",   0. );
    _pxctr_threshold    = cfg.get<size_t>      ( "PixelCountThreshold", 0  );
  }

  void PPNParticleRemover::initialize()
  {}

  bool PPNParticleRemover::process(IOManager& mgr)
  {
    auto const& event_cluster3d = mgr.get_data< EventClusterVoxel3D > ( _cluster3d_producer );
    auto const& event_particle  = mgr.get_data< EventParticle       > ( _particle_producer  );

    auto const& cluster3d_v = event_cluster3d.as_vector ();
    auto const& particle_v  = event_particle.as_vector  ();
    if((particle_v.size()+1) != cluster3d_v.size()) {
      LARCV_CRITICAL() << "Size of particles " << particle_v.size() << " mismatch w/ " << cluster3d_v.size() << std::endl;
      throw larbys();
    }

    std::vector<larcv::Particle> out_particle_v;
    std::vector<larcv::VoxelSet> vs_v;

    larcv::VoxelSet unknown_vs;
    for(size_t i=0; i<particle_v.size(); ++i) {
      auto const& particle  = particle_v[i];
      auto const& vs = cluster3d_v[i];
      double sum = vs.sum();
      size_t ctr = vs.as_vector().size();
      if(sum < _pxsum_threshold || ctr < _pxctr_threshold) {
	for(auto const& vox : vs.as_vector()) unknown_vs.add(vox);
	continue;
      }
      vs_v.push_back(vs);
      out_particle_v.push_back(particle);
    }

    // add last cluster to unknown_vs;
    for(auto const& vox : cluster3d_v.back().as_vector())
      unknown_vs.add(vox);
    
    vs_v.emplace_back(std::move(unknown_vs));

    // create output
    auto& out_cluster3d = mgr.get_data< EventClusterVoxel3D > ( "ppn_" + _cluster3d_producer );
    auto& out_particle  = mgr.get_data< EventParticle       > ( "ppn_" + _particle_producer  );


    VoxelSetArray vsa;
    vsa.emplace(std::move(vs_v));
    out_cluster3d.emplace ( std::move(vsa), event_cluster3d.meta() );
    out_particle.emplace  ( std::move(out_particle_v)              );

    return true;
  }

  void PPNParticleRemover::finalize()
  {}

}
#endif
