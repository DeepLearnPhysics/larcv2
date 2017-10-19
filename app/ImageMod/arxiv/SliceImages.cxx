#ifndef __SLICEIMAGES_CXX__
#define __SLICEIMAGES_CXX__

#include "SliceImages.h"
#include "DataFormat/EventImage2D.h"

namespace larcv {

  static SliceImagesProcessFactory __global_SliceImagesProcessFactory__;

  SliceImages::SliceImages(const std::string name)
    : ProcessBase(name)
  {}
    
  void SliceImages::configure(const PSet& cfg)
  {
    _channels = cfg.get<std::vector<size_t> >("Channels");
    _image_producer = cfg.get<std::string> ("ImageProducer");
    _output_producer = cfg.get<std::string> ("OutputProducer");
  }

  void SliceImages::initialize()
  {}

  bool SliceImages::process(IOManager& mgr)
  {
    auto ev_images = (EventImage2D*)(mgr.get_data(kProductImage2D,_image_producer));

    auto out_images = (EventImage2D*)(mgr.get_data(kProductImage2D,_output_producer));

    for(auto const& index : _channels) {

      if(index >= ev_images->Image2DArray().size()) 

	throw larbys("Input image array does not have specified channels to slice!");

      auto const& img = ev_images->Image2DArray()[index];
      
      out_images->Append(img);
      
    }
    return true;
  }

  void SliceImages::finalize()
  {}

}
#endif
