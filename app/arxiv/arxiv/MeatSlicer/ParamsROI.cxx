#ifndef __SUPERAPARMSROI_CXX__
#define __SUPERAPARMSROI_CXX__

#include "ParamsROI.h"

namespace supera {

  void ParamsROI::configure(const supera::Config_t& cfg)
  {
    _out_roi_producer = cfg.get<std::string>("OutROILabel","");
  }  
}
#endif
