#ifndef __IMAGEMETAFROMCONFIG_CXX__
#define __IMAGEMETAFROMCONFIG_CXX__

#include "ImageMetaFromConfig.h"

namespace supera {

  void ImageMetaFromConfig::configure(const supera::Config_t& cfg)
  {
    ImageMetaMakerBase::configure(cfg);
    auto min_time = cfg.get<double>("MinTime");
    auto min_wire = cfg.get<double>("MinWire");
    auto image_rows = cfg.get<std::vector<size_t> >("EventImageRows");
    auto image_cols = cfg.get<std::vector<size_t> >("EventImageCols");

    auto const& comp_rows = RowCompressionFactor();
    auto const& comp_cols = ColCompressionFactor();

    if(image_rows.size() != supera::Nplanes()) {
      std::cerr << "EventImageRows size != # planes..." << std::endl;
      throw std::exception();
    }
    if(image_cols.size() != supera::Nplanes()) {
      std::cerr << "EventImageCols size != # planes..." << std::endl;
      throw std::exception();
    }
    
    // construct meta
    for(size_t plane=0; plane<image_rows.size(); ++plane) {
      
      larcv::ImageMeta meta(image_cols[plane] * comp_cols[plane], image_rows[plane] * comp_rows[plane],
			    image_rows[plane] * comp_rows[plane], image_cols[plane] * comp_cols[plane],
			    min_wire, min_time + image_rows[plane] * comp_rows[plane],
			    plane);
      
      LARCV_INFO() << "Created meta " <<  meta.dump();
      
      _meta_v.emplace_back(std::move(meta));
    }
    
  }  
}
#endif
