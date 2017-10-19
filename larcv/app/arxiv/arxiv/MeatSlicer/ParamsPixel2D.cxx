#ifndef __SUPERAPARAMS_PIXEL2D_CXX__
#define __SUPERAPARAMS_PIXEL2D_CXX__

#include "ParamsPixel2D.h"

namespace supera {

  void ParamsPixel2D::configure(const supera::Config_t& cfg)
  {
    _out_pixel2d_producer = cfg.get<std::string>("OutPixel2DLabel", "");
  }  
}
#endif
