#ifndef __RESIZEIMAGE_CXX__
#define __RESIZEIMAGE_CXX__

#include "ResizeImage.h"
#include "larcv/core/DataFormat/EventImage2D.h"
namespace larcv {

  static ResizeImageProcessFactory __global_ResizeImageProcessFactory__;

  ResizeImage::ResizeImage(const std::string name)
    : ProcessBase(name)
  {}
    
  void ResizeImage::configure(const PSet& cfg)
  {
    _input_producer  = cfg.get<std::string>("InputProducer");
    _output_producer = cfg.get<std::string>("OutputProducer");
    _origin_x_v = cfg.get<std::vector<double> >("OriginX");
    _origin_y_v = cfg.get<std::vector<double> >("OriginY");
    _width_v    = cfg.get<std::vector<double> >("Width");
    _height_v   = cfg.get<std::vector<double> >("Height");
    _rows_v     = cfg.get<std::vector<size_t> >("Rows");
    _cols_v     = cfg.get<std::vector<size_t> >("Cols");

    if(_origin_x_v.size() != _origin_y_v.size() ||
       _origin_x_v.size() != _width_v.size() ||
       _origin_x_v.size() != _height_v.size() ||
       _origin_x_v.size() != _rows_v.size() ||
       _origin_x_v.size() != _cols_v.size() ) {
      LARCV_CRITICAL() << "Input parameter array size does not match!" << std::endl;
      throw larbys();
    }
  }

  void ResizeImage::initialize()
  {}

  bool ResizeImage::process(IOManager& mgr)
  {
    auto& event_image = mgr.get_data<EventImage2D>(_output_producer);

    auto const& image_v = event_image.image2d_array();
    std::vector<larcv::Image2D> res_image_v;
    
    if(image_v.size() != _origin_x_v.size()) {
      if(image_v.empty()) {
	LARCV_INFO() << "Skipping an empty image event..." << std::endl;
	return false;
      }
      LARCV_CRITICAL() << "Image array size does not match with input parameter array size!" << std::endl;
      throw larbys();
    }

    for(size_t i=0; i<image_v.size(); ++i) {

      auto const& image = image_v[i];
      
      ImageMeta res_meta(_width_v[i], _height_v[i],
			 _rows_v[i], _cols_v[i],
			 _origin_x_v[i], _origin_y_v[i],
			 image.meta().id());
      Image2D res(res_meta);
      res.paint(0.);
      res.overlay(image);
      res_image_v.emplace_back(std::move(res));
    }

    auto& output_image = mgr.get_data<EventImage2D>(_input_producer);

    output_image.emplace(std::move(res_image_v));

    return true;
  }

  void ResizeImage::finalize()
  {}

}
#endif
