#ifndef __SUPERAPARAMSIMAGE2D_CXX__
#define __SUPERAPARAMSIMAGE2D_CXX__

#include "ParamsImage2D.h"

namespace supera {

  void ParamsImage2D::configure(const supera::Config_t& cfg)
  {
    _out_image_producer = cfg.get<std::string>("OutImageLabel","");
  }  
}
#endif
