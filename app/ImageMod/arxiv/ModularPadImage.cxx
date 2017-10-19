#ifndef __MODULARPADIMAGE_CXX__
#define __MODULARPADIMAGE_CXX__

#include "ModularPadImage.h"
#include "DataFormat/EventImage2D.h"

namespace larcv {

  static ModularPadImageProcessFactory __global_ModularPadImageProcessFactory__;

  ModularPadImage::ModularPadImage(const std::string name)
    : ProcessBase(name)
  {}
    
  void ModularPadImage::configure(const PSet& cfg)
  {
    _modular_col     = cfg.get<size_t>      ("ModularCol");
    _modular_row     = cfg.get<size_t>      ("ModularRow");
    _max_col         = cfg.get<size_t>      ("MaxCol");
    _max_row         = cfg.get<size_t>      ("MaxRow");
    _input_producer  = cfg.get<std::string> ("InputProducer");
    _output_producer = cfg.get<std::string> ("OutputProducer");
    _ch_v            = cfg.get<std::vector<size_t> >("ImageChannel");
  }
  
  void ModularPadImage::initialize()
  {}

  bool ModularPadImage::process(IOManager& mgr)
  {
    auto event_image = (EventImage2D*)(mgr.get_data(kProductImage2D,_input_producer));
    if(!event_image) {
      LARCV_CRITICAL() << "Image producer not found: " << _input_producer << std::endl;
      throw larbys();
    }

    auto const& image_v = event_image->Image2DArray();
    for(auto const& ch : _ch_v) {
      if(ch >= image_v.size()) {
	LARCV_CRITICAL() << "Channel " << ch << " not valid!" << std::endl;
	throw larbys();
      }
    }

    std::vector<larcv::Image2D> out_image_v;

    for(auto const& ch : _ch_v) {

      auto const& image = image_v[ch];
      auto const& meta = image.meta();
      
      double px_width  = meta.width()  / (double)(meta.cols());
      double px_height = meta.height() / (double)(meta.rows());

      size_t rows = ((meta.rows() / _modular_row) + 1) * _modular_row;
      size_t cols = ((meta.cols() / _modular_col) + 1) * _modular_col;
      
      if(rows > _max_row) rows = _max_row;
      if(cols > _max_col) cols = _max_col;
      
      ImageMeta res_meta(px_width * cols, px_height * rows,
			 rows, cols,
			 meta.min_x(), meta.max_y(),
			 meta.plane());

      Image2D res(res_meta);
      res.paint(0.);
      res.overlay(image);
      out_image_v.emplace_back(std::move(res));
    }
    
    auto out_event_image = (EventImage2D*)(mgr.get_data(kProductImage2D,_output_producer));
    if(!out_event_image) {
      LARCV_CRITICAL() << "Failed to create an output Image2D array " << _output_producer << std::endl;
      throw larbys();
    }
    
    out_event_image->Emplace(std::move(out_image_v));
    return true;
  }
  
  void ModularPadImage::finalize()
  {}

}
#endif
