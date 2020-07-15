#ifndef __SIMIONIZATION_CXX__
#define __SIMIONIZATION_CXX__

#include "SimIonization.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
#include <math.h>
namespace larcv {

  static SimIonizationProcessFactory __global_SimIonizationProcessFactory__;

  SimIonization::SimIonization(const std::string name)
    : ProcessBase(name)
    , _density(1.38)
    , _alpha(0.847)
    , _beta(0.2061)
    , _efield(0.273)
    , _elifetime(10000.)
    , _vdrift(1.114)
    , _work_function(23.6e-6)
  {}

  void SimIonization::configure(const PSet& cfg)
  {
    _cluster3d_producer = cfg.get<std::string>("Cluster3DProducer");
    _output_producer    = cfg.get<std::string>("OutputProducer");
    _density = cfg.get<double>("LArDensity",_density);// g/cm^3
    _alpha   = cfg.get<double>("ModifiedBoxA",_alpha);// 0.930 in ArgoNEUT
    _beta    = cfg.get<double>("ModifiedBoxB",_beta); // 0.212 in ArgoNEUT
    _efield  = cfg.get<double>("EField",_efield);     // kV/cm
    _threshold = cfg.get<double>("MinThreshold",0.06);// MeV
    _elifetime = cfg.get<double>("ElectronLifetime",_elifetime); //us
    _vdrift    = cfg.get<double>("DriftVelocity",_vdrift); // cm/us
    _work_function = cfg.get<double>("WorkFunction", _work_function); // MeV/e-
  }

  double SimIonization::Recombination(double dedx) const {
    double alpha = _alpha;
    double beta = _beta / (_density * _efield);
    double W = _work_function;
    return log(alpha + beta * dedx) / (beta * W);
  }

  double SimIonization::Lifetime(double x) const {
    return exp( - x/_vdrift/_elifetime);
  }

  void SimIonization::initialize()
  {}

  bool SimIonization::process(IOManager& mgr)
  {
    auto const& event_cluster3d = mgr.get_data<larcv::EventClusterVoxel3D>(_cluster3d_producer);
    auto const& event_dx = mgr.get_data<larcv::EventClusterVoxel3D>(_cluster3d_producer + "_dx");
    auto const meta = event_cluster3d.meta();

    std::vector<larcv::VoxelSet> vs_v;
    vs_v.resize(event_cluster3d.as_vector().size());
    for(size_t index=0; index<event_cluster3d.as_vector().size(); ++index){
      auto const& vs_in = event_cluster3d.as_vector()[index].as_vector();
      auto const& vs_dx = event_dx.as_vector().at(index).as_vector();
      if(vs_in.size() != vs_dx.size()) {
        LARCV_ERROR() << "cluster voxel count " << vs_in.size()
			 << " != dx voxel count " << vs_dx.size() << std::endl;
	return false;
      }
      size_t invalid_ctr=0;
      auto& vs_out = vs_v[index];
      for(size_t vindex=0; vindex<vs_in.size(); ++vindex) {
        auto const& vox_in = vs_in[vindex];
	auto const& vox_dx = vs_dx[vindex];
	if(vox_in.value()<_threshold || vox_dx.value() < 0.0001) continue;
	double dedx = vox_in.value() / vox_dx.value();
	if(vox_in.id() != vox_dx.id()) {
	  LARCV_ERROR() << "Unmatched voxel id for de and dx! " << std::endl;
	  return false;
	}
	if(dedx < 1.0) {
	  LARCV_INFO() << "Ignoring voxel " << vox_in.value() << " MeV with dedx=" << dedx << std::endl;
	  ++invalid_ctr;
	  continue;
	}
        auto recomb_factor = this->Recombination(dedx);
        auto lifetime_factor = this->Lifetime( (meta.pos_x(vox_in.id()) - meta.min_x()) );
	float vox_value = vox_in.value() * recomb_factor * lifetime_factor;
	if(vox_value < 0 || std::isnan(vox_value)) {
	  LARCV_ERROR() << "Invalid voxel value ignored: de=" << vox_in.value() << " dedx=" << dedx
			<< " recomb=" << recomb_factor << " lifetime="<<lifetime_factor << std::endl;
	  ++invalid_ctr;
	  continue;
	}
        vs_out.emplace(vox_in.id(), vox_value, true);
      }
      if(invalid_ctr)
	LARCV_INFO()<<"Invalid voxel count " << invalid_ctr<< " / " << vs_in.size() << std::endl;
    }

    auto output_cluster3d = (larcv::EventClusterVoxel3D*)(mgr.get_data("cluster3d",_output_producer));
    larcv::VoxelSetArray vsa;
    vsa.emplace(std::move(vs_v));
    output_cluster3d->emplace(std::move(vsa),meta);
    return true;
  }

  void SimIonization::finalize()
  {}

}
#endif
