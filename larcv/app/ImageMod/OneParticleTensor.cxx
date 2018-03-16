#ifndef __ONEPARTICLETENSOR_CXX__
#define __ONEPARTICLETENSOR_CXX__

#include "OneParticleTensor.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
#include "larcv/core/DataFormat/EventParticle.h"
namespace larcv {

  static OneParticleTensorProcessFactory __global_OneParticleTensorProcessFactory__;

  OneParticleTensor::OneParticleTensor(const std::string name)
    : ProcessBase(name)
  {}
    
  void OneParticleTensor::configure(const PSet& cfg)
  {
    _criteria = (SelectType_t)(cfg.get<size_t>("SelectType"));
    auto const pdg_v = cfg.get<std::vector<int> >("PDGList");
    for(auto const& pdg : pdg_v) _pdg_s.insert(pdg);
    if(_pdg_s.empty()) {
      LARCV_CRITICAL() << "PDGList is empty!" << std::endl;
      throw larbys();
    }
    _particle_producer  = cfg.get<std::string>("ParticleProducer");
    _cluster3d_producer = cfg.get<std::string>("Cluster3DProducer");
    _output_producer    = cfg.get<std::string>("OutputProducer");
  }

  void OneParticleTensor::initialize()
  {}

  bool OneParticleTensor::process(IOManager& mgr)
  {
    auto const& ev_cluster3d = mgr.get_data<larcv::EventClusterVoxel3D>(_cluster3d_producer);
    if (!ev_cluster3d.meta().valid()) {
      LARCV_CRITICAL() << "Can not find cluster3d from producer "
                       << _cluster3d_producer << std::endl;
      throw larbys();
    }

    auto const& ev_particle = mgr.get_data<larcv::EventParticle>(_particle_producer);
    auto const& part_v = ev_particle.as_vector();

    auto& output_tensor   = mgr.get_data<larcv::EventSparseTensor3D>(_output_producer);
    auto& output_particle = mgr.get_data<larcv::EventParticle>(_output_producer);

    std::vector<size_t> candidates;
    double max_energy_init = -1;
    double max_energy_deposit = -1;
    for(size_t i = 0; i<part_v.size(); ++i) {
      auto const& particle = part_v[i];
      if(_pdg_s.find(particle.pdg_code()) == _pdg_s.end())
        continue;

      if(_criteria == kVoxelCount || candidates.empty()) {
        candidates.push_back(i);
        continue;
      }

      switch(_criteria) {
        case kEnergyInit:
          if(max_energy_init < particle.energy_init()) {
            candidates[0] = i;
            max_energy_init = particle.energy_init();
          }
          break;

        case kEnergyDeposit:
          if(max_energy_deposit < particle.energy_deposit()) {
            candidates[0] = i;
            max_energy_deposit = particle.energy_deposit();
          }
          break;

        default:
          break;
      }
    }

    if(candidates.empty()) return false;

    size_t candidate = candidates.front();
    if(_criteria == kVoxelCount) {
      size_t count=0;
      for(auto const& index : candidates) {
        auto const& vs = ev_cluster3d.as_vector().at(index);
        if(vs.as_vector().size() <= count) continue;
        candidate = index;
        count = vs.as_vector().size();
      }
    }

    auto const& vs = ev_cluster3d.as_vector().at(candidate);
    output_tensor.set(vs,ev_cluster3d.meta());
    output_particle.clear();
    output_particle.append(ev_particle.as_vector().at(candidate));
    return true;
  }

  void OneParticleTensor::finalize()
  {}

}
#endif
