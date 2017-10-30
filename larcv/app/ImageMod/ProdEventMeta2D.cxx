#ifndef __PRODEVENTMETA2D_CXX__
#define __PRODEVENTMETA2D_CXX__

#include "ProdEventMeta2D.h"
#include "larcv/core/DataFormat/EventImage2D.h"
#include "larcv/core/DataFormat/EventMeta.h"

namespace larcv {

  static ProdEventMeta2DProcessFactory __global_ProdEventMeta2DProcessFactory__;

  ProdEventMeta2D::ProdEventMeta2D(const std::string name)
    : ProcessBase(name)
  {}

  void ProdEventMeta2D::configure(const PSet& cfg)
  {
    _image_producer  = cfg.get<std::string> ( "ImageProducer" );
    _output_producer = cfg.get<std::string> ( "OutputProducer" );

  }

  void ProdEventMeta2D::initialize()
  {
  }

  bool ProdEventMeta2D::process(IOManager& mgr)
  {

    auto input_image  = (EventImage2D*)(mgr.get_data("image2d", _image_producer));
    
    auto output_meta = (EventMeta*)(mgr.get_data("meta", _output_producer));


    int n_planes = input_image->image2d_array().size();
    std::vector<double> x_min(n_planes);
    std::vector<double> y_min(n_planes);
    std::vector<double> x_max(n_planes);
    std::vector<double> y_max(n_planes);

    std::vector<double> x_n_pixels(n_planes);
    std::vector<double> y_n_pixels(n_planes);

    for (auto image : input_image->image2d_array()){
      auto meta = image.meta();
      x_min[meta.id()] = meta.min_x();
      y_min[meta.id()] = meta.min_y();
      x_max[meta.id()] = meta.max_x();
      y_max[meta.id()] = meta.max_y();
      x_n_pixels[meta.id()] = meta.cols();
      y_n_pixels[meta.id()] = meta.rows();
    }

    output_meta->store("n_planes",n_planes);
    output_meta->store("x_min", x_min);
    output_meta->store("y_min", y_min);
    output_meta->store("x_max", x_max);
    output_meta->store("y_max", y_max);
    output_meta->store("x_n_pixels", x_n_pixels);
    output_meta->store("y_n_pixels", y_n_pixels);


    return true;
  }

  void ProdEventMeta2D::finalize()
  {}

}
#endif
