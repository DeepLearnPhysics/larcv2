#ifndef __IMAGEMETAMAKERBASE_CXX__
#define __IMAGEMETAMAKERBASE_CXX__

#include "ImageMetaMakerBase.h"

namespace supera {

  void ImageMetaMakerBase::configure(const Config_t& cfg)
  {
    _comp_rows  = cfg.get<std::vector<size_t> >("EventCompRows");
    _comp_cols  = cfg.get<std::vector<size_t> >("EventCompCols");

    if(_comp_rows.size() != supera::Nplanes()) {
      std::cerr << "EventCompRows size != # planes..." << std::endl;
      throw std::exception();
    }
    if(_comp_cols.size() != supera::Nplanes()) {
      std::cerr << "EventCompCols size != # planes..." << std::endl;
      throw std::exception();
    }
  }  
}
#endif
