#ifndef __SEGLABELFROMCLUSTER3D_CXX__
#define __SEGLABELFROMCLUSTER3D_CXX__

#include "SegLabelFromCluster3D.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
#include "larcv/core/DataFormat/EventParticle.h"

namespace larcv {

  static SegLabelFromCluster3DProcessFactory __global_SegLabelFromCluster3DProcessFactory__;

  SegLabelFromCluster3D::SegLabelFromCluster3D(const std::string name)
    : ProcessBase(name)
  {}
    
  void SegLabelFromCluster3D::configure(const PSet& cfg)
  {
    _cluster3d_producer = cfg.get<std::string>("Cluster3DProducer");
    _particle_producer  = cfg.get<std::string>("ParticleProducer");
    _output_producer    = cfg.get<std::string>("OutputProducer");
    _undefined_label    = cfg.get<size_t>("UndefinedParticleLabel",0);
    _min_num_voxel      = cfg.get<size_t>("MinVoxelCount",0);
    std::vector<int> empty_list;
    std::vector<std::set<int> > pdg_list;
    while(true) {
      std::string param_name = "ParticleSet" + std::to_string(pdg_list.size());
      auto another_set = cfg.get<std::vector<int> >(param_name,empty_list);
      if(another_set.empty() && pdg_list.size()) break;
      for(size_t i=0; i<pdg_list.size(); ++i) {
        for(auto const& pdg : another_set) {
          if(pdg_list[i].find(pdg) == pdg_list[i].end()) continue;
          LARCV_CRITICAL() << param_name << " includes PDG " << pdg
          << " which was already used in ParticleSet" << i << std::endl;
          throw larbys();
        }
      }
      std::set<int> temp_pdg_list;
      for(auto const& pdg : another_set) temp_pdg_list.insert(pdg);
      pdg_list.emplace_back(std::move(temp_pdg_list));
    }

    _particle_to_class.clear();
    _anti_particle_to_class.clear();
    
    for(size_t class_index=0; class_index<pdg_list.size(); ++class_index) {
      LARCV_INFO() << "Assining to class " << class_index << " ... " << std::flush;
      for(auto pdg : pdg_list[class_index]) {
        if(pdg>0) {
          if(_particle_to_class.size() <= (size_t)pdg) _particle_to_class.resize(pdg+1,_undefined_label);
          _particle_to_class[pdg] = class_index;
        }else{
          pdg = std::abs(pdg);
          if(_anti_particle_to_class.size() <= (size_t)pdg) _anti_particle_to_class.resize(pdg+1,_undefined_label);
          _anti_particle_to_class[pdg] = class_index;
        }
        LARCV_INFO() << pdg << " ";
      }
      LARCV_INFO() << std::endl;
    }
  }

  void SegLabelFromCluster3D::initialize()
  {}

  bool SegLabelFromCluster3D::process(IOManager& mgr)
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
    for(size_t cindex=0; cindex<cluster3d_v.size(); ++cindex) {
      auto const& cluster  = cluster3d_v[cindex];
      // figure out label
      float class_def = 0;
      if(cindex < particle_v.size()) {
        auto const& particle = particle_v[cindex];
        auto pdg = particle.pdg_code();
        if(pdg > 0) {
          if((size_t)pdg >= _particle_to_class.size()) class_def = _undefined_label;
          else class_def = _particle_to_class[pdg];
        }else{
          pdg = std::abs(pdg);
          if((size_t)pdg >= _anti_particle_to_class.size()) class_def = _undefined_label;
          else class_def = _anti_particle_to_class[pdg];
        }
      }
      if(class_def<1) continue;
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

  void SegLabelFromCluster3D::finalize()
  {}

}
#endif
