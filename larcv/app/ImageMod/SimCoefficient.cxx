#ifndef __SIMCOEFFICIENT_CXX__
#define __SIMCOEFFICIENT_CXX__

#include "SimCoefficient.h"
#include "larcv/core/DataFormat/EventParticle.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"

namespace larcv {

  static SimCoefficientProcessFactory __global_SimCoefficientProcessFactory__;

  SimCoefficient::SimCoefficient(const std::string name)
    : ProcessBase(name)
  {}
    
  void SimCoefficient::configure(const PSet& cfg)
  {
    _output_producer   = cfg.get<std::string>("OutputProducer");
    _cluster_producer  = cfg.get<std::string>("Cluster3DProducer");
    _particle_producer = cfg.get<std::string>("ParticleProducer");
    _store_pdg = true;
    _store_pdg = cfg.get<bool>("StorePDG",_store_pdg);
  }

  void SimCoefficient::initialize()
  {}

  bool SimCoefficient::process(IOManager& mgr)
  {
    auto const& event_particle = mgr.get_data<larcv::EventParticle>(_particle_producer);
    auto const& event_cluster  = mgr.get_data<larcv::EventClusterVoxel3D>(_cluster_producer);
    auto const& part_v    = event_particle.as_vector();
    auto const& cluster_v = event_cluster.as_vector();

    if((part_v.size()+1) != cluster_v.size()) {
      LARCV_CRITICAL() << "Particle count: " << part_v.size()
		       << " != Cluster count: " << cluster_v.size()
		       << std::endl;
      throw larbys();
    }

    VoxelSet vs_pdg;
    VoxelSet vs_ctr;
    VoxelSet vs_cid;
    for(size_t i=0; i<event_particle.as_vector().size(); ++i) {
      
      int pdg = abs(part_v[i].pdg_code());
      for(auto const& vox : cluster_v[i].as_vector()) {
	if(_store_pdg) vs_pdg.emplace(vox.id(),(float)pdg,false);
	vs_ctr.emplace(vox.id(),1.,true);
	vs_cid.emplace(vox.id(),(float)i,false);
      }
    }
    /*
    for(auto const& vox : vs_ctr.as_vector()) {
      if(vox.value() < 1.5) continue;
      vs_pdg.emplace(vox.id(),0.,false);
      vs_cid.emplace(vox.id(),0.,false);
    }
    */
    if(_store_pdg) {
      auto& out_pdg = mgr.get_data<larcv::EventSparseTensor3D>(_output_producer + "_pdg");
      auto& out_cid = mgr.get_data<larcv::EventSparseTensor3D>(_output_producer + "_group");
      out_pdg.emplace(std::move(vs_pdg),event_cluster.meta());
      out_cid.emplace(std::move(vs_cid),event_cluster.meta());
    }else{
      auto& out_cid = mgr.get_data<larcv::EventSparseTensor3D>(_output_producer);
      out_cid.emplace(std::move(vs_cid),event_cluster.meta());
    }
    return true;
  }

  void SimCoefficient::finalize()
  {}

}
#endif
