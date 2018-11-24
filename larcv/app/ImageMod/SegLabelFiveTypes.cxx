#ifndef __SEGLABELFIVETYPES_CXX__
#define __SEGLABELFIVETYPES_CXX__

#include "SegLabelFiveTypes.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
#include "larcv/core/DataFormat/EventParticle.h"

namespace larcv {

  static SegLabelFiveTypesProcessFactory __global_SegLabelFiveTypesProcessFactory__;

  SegLabelFiveTypes::SegLabelFiveTypes(const std::string name)
    : ProcessBase(name)
  {}
    
  void SegLabelFiveTypes::configure(const PSet& cfg)
  {
    _cluster3d_producer = cfg.get<std::string>("Cluster3DProducer");
    _particle_producer  = cfg.get<std::string>("ParticleProducer");
    _output_producer    = cfg.get<std::string>("OutputProducer");
    _min_num_voxel      = cfg.get<size_t>("MinVoxelCount",0);
  }

  void SegLabelFiveTypes::initialize()
  {}

  bool SegLabelFiveTypes::process(IOManager& mgr)
  {
    auto const& event_cluster3d = mgr.get_data<larcv::EventClusterVoxel3D>(_cluster3d_producer);
    auto const& event_particle  = mgr.get_data<larcv::EventParticle>(_particle_producer);
    auto event_tensor3d = (larcv::EventSparseTensor3D*)(mgr.get_data("sparse3d",_output_producer));
    event_tensor3d->meta(event_cluster3d.meta());
    // Sanity check: length of event_particle +1 should be same as length of event_cluster3d
    auto const& cluster3d_v = event_cluster3d.as_vector();
    auto const& particle_v  = event_particle.as_vector();
    if((particle_v.size()+1) != cluster3d_v.size()) {
      LARCV_CRITICAL() << "Logic error: input EventClusterVoxel3D has " << cluster3d_v.size() << " clusters"
      << " but EventParticle has " << particle_v.size() << " particles!" << std::endl;
      //throw larbys();
      return false;
    }

    // Fill tensor3d
    for(size_t cindex=0; cindex<particle_v.size(); ++cindex) {
      auto const& particle = particle_v[cindex];
      auto const& cluster  = cluster3d_v[cindex];
      // figure out label
      auto pdg = abs(particle.pdg_code());
      auto prc = particle.creation_process();
      float class_def = -1;
      if(pdg == 2212) class_def = 0;
      else if(pdg != 22 && pdg != 11) class_def = 1;
      else if(pdg == 22) class_def = 2;
      else {
	if( prc == "primary" || prc == "nCapture" || prc == "conv") class_def = 2;
	else if( prc == "muIoni" || prc == "hIoni") class_def = 3;
	else if( prc == "muMinusCaptureAtRest" || prc == "muPlusCaptureAtRest" || prc == "Decay" )
	  class_def = 4;
	else {
	  std::cout << prc << std::endl;
	  std::cout << particle.parent_pdg_code() << " => " << particle.pdg_code() << std::endl;
	  std::cout << particle.energy_init() << std::endl;
	}
      }
      if(class_def<0) continue;
      // set the label
      for(auto const& vox : cluster.as_vector()) {
        auto const& prev_vox = ((VoxelSet*)event_tensor3d)->find(vox.id());
        if(prev_vox.id() == larcv::kINVALID_VOXELID) {
          ((VoxelSet*)event_tensor3d)->emplace(vox.id(),class_def,false);
          continue;
        }
        // prioritize higher class values
        if(prev_vox.value() >= class_def) continue;
        ((VoxelSet*)event_tensor3d)->emplace(vox.id(),class_def,false);
      }
    }

    if(_min_num_voxel<1) return true;

    if(event_tensor3d->as_vector().size() < _min_num_voxel) {
      LARCV_NORMAL() << "Skipping event " << event_tensor3d->event_key() 
		     << " due to voxel count (" << event_tensor3d->as_vector().size() 
		     << " < " << _min_num_voxel << ")" << std::endl;
      return false;
    }
    return true;
  }

  void SegLabelFiveTypes::finalize()
  {}

}
#endif
