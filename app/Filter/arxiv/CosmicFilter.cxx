#ifndef __COSMICFILTER_CXX__
#define __COSMICFILTER_CXX__

#include "CosmicFilter.h"
#include "DataFormat/EventROI.h"
#include "TRandom3.h"

namespace larcv {

  static CosmicFilterProcessFactory __global_CosmicFilterProcessFactory__;

  CosmicFilter::CosmicFilter(const std::string name)
    : ProcessBase(name)
  {}
    
  void CosmicFilter::configure(const PSet& cfg)
  {
    _roi_producer = cfg.get<std::string>("ROIProducer");
    _thinning_factor = cfg.get<double>("ThinningFactor", -1.0);
    _rand = new TRandom3(123456);
  }

  void CosmicFilter::initialize()
  {}

  bool CosmicFilter::process(IOManager& mgr)
  {
    if(!_roi_producer.empty()) {
      auto ev_roi = (EventROI*)(mgr.get_data(kProductROI,_roi_producer));
      bool cosmic_only = true;
      for(auto const& roi : ev_roi->ROIArray()) {
	if(roi.Type()!=larcv::kROICosmic) {
	  cosmic_only = false;
	}
      }
      if ( _thinning_factor>0 && _thinning_factor<=1.0 ) {
	if ( cosmic_only && _rand->Uniform()<_thinning_factor ) return true;
      }
      else {
	if (cosmic_only) return true;
      }
    }
    return false;
  }
  
  void CosmicFilter::finalize()
  {}

}
#endif
