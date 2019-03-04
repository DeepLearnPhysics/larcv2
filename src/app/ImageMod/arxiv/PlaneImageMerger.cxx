#ifndef __PLANEIMAGEMERGER_CXX__
#define __PLANEIMAGEMERGER_CXX__

#include "PlaneImageMerger.h"
#include "DataFormat/EventImage2D.h"
#include <sstream>

namespace larcv {

  static PlaneImageMergerProcessFactory __global_PlaneImageMergerProcessFactory__;

  PlaneImageMerger::PlaneImageMerger(const std::string name)
    : ProcessBase(name)
  {}
    
  void PlaneImageMerger::configure(const PSet& cfg)
  {
    _producer_key = cfg.get<std::string>("InputKey");
    _output_producer = cfg.get<std::string>("OutputProducer");

    auto plane_v = cfg.get<std::vector<unsigned short> >("PlaneIDs");

    for(auto const& plane : plane_v) _plane_s.insert(plane);
    
  }

  void PlaneImageMerger::initialize()
  {}

  bool PlaneImageMerger::process(IOManager& mgr)
  {
    auto output = (EventImage2D*)(mgr.get_data(kProductImage2D,_output_producer));
    if(!output) {
      LARCV_CRITICAL() << "Failed to create the output data product!" << std::endl;
      throw larbys();
    }

    std::vector<larcv::Image2D> image_v;

    for(auto const& plane : _plane_s) {

      std::stringstream ss;
      ss << _producer_key << "_plane" << plane;
      
      auto input = (EventImage2D*)(mgr.get_data(kProductImage2D,std::string(ss.str())));

      if(!input) {
	LARCV_CRITICAL() << "Could not locate an input producer " << ss.str() << std::endl;
	throw larbys();
      }
      auto const& input_image_v = input->Image2DArray();
      if(input_image_v.size() <= plane){
	LARCV_CRITICAL() << "Could not locate plane " << plane << " in the input image by " << ss.str() << std::endl;
	throw larbys();
      }

      image_v.resize(plane+1);
      image_v[plane] = input_image_v[plane];
    }

    output->Emplace(std::move(image_v));
    return true;
  }

  void PlaneImageMerger::finalize()
  {}

}
#endif
