#ifndef __SIMIONIZATION_CXX__
#define __SIMIONIZATION_CXX__

#include "SimIonization.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"

namespace larcv {

  static SimIonizationProcessFactory __global_SimIonizationProcessFactory__;

  SimIonization::SimIonization(const std::string name)
    : ProcessBase(name)
  {}
    
  void SimIonization::configure(const PSet& cfg)
  {
    _cluster3d_producer = cfg.get<std::string>("Cluster3DProducer");
    _output_producer    = cfg.get<std::string>("OutputProducer");
    _density = cfg.get<double>("LArDensity",1.38);     // g/cm^3
    _alpha   = cfg.get<double>("ModifiedBoxA",0.847);  // 0.930 in ArgoNEUT
    _beta    = cfg.get<double>("ModifiedBoxB",0.2061); // 0.212 in ArgoNEUT
    _efield  = cfg.get<double>("EField",0.273);        // kV/cm

    _elifetime = cfg.get<double>("ElectronLifetime",10000.); //us
    _vdrift    = cfg.get<double>("1.114"); // cm/us
  }

  double SimIonization::Recombination(double dedx) const {
    double alpha = _alpha;
    double beta = _beta / (_density * _efield);
    return log(alpha + beta * dedx) / beta;
  }

  double SimIonization::Lifetime(double x) const {
    return exp( - x/_vdrift/_elifetime);
  }

  void SimIonization::initialize()
  {}

  bool SimIonization::process(IOManager& mgr)
  {
    auto const& event_cluster3d = mgr.get_data<larcv::EventClusterVoxel3D>(_cluster3d_producer);
    auto const& event_dedx = mgr.get_data<larcv::EventClusterVoxel3D>(_cluster3d_producer + "_dedx");
    auto output_cluster3d = (larcv::EventClusterVoxel3D*)(mgr.get_data("cluster3d",_output_producer));
    auto const meta = event_cluster3d.meta();

    std::vector<larcv::VoxelSet> vs_v;
    vs_v.resize(event_cluster3d.as_vector().size());
    for(size_t index=0; index<event_cluster3d.as_vector().size(); ++index){
      auto const& vs_in = event_cluster3d.as_vector()[index].as_vector();
      auto const& vs_dedx = event_dedx.as_vector().at(index).as_vector();
      if(vs_in.size() != vs_dedx.size()) {
        LARCV_CRITICAL() << "cluster voxel count != dedx voxel count..." << std::endl;
        throw larbys();
      }
      auto& vs_out = vs_v[index];
      for(size_t vindex=0; vindex<vs_in.size(); ++vindex) {
        auto const& vox_in = vs_in[vindex];
        auto recomb_factor = this->Recombination(vs_dedx[vindex].value());
        auto lifetime_factor = this->Lifetime(meta.pos_x(vox_in.id()));
        vs_out.emplace(vox_in.id(), vox_in.value() * recomb_factor, true);
      }
    }

    larcv::VoxelSetArray vsa;
    vsa.emplace(std::move(vs_v));
    output_cluster3d->emplace(std::move(vsa),meta);
    return true;
  }

  void SimIonization::finalize()
  {}

}
#endif
