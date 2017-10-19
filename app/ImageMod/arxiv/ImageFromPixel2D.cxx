#ifndef __IMAGEFROMPIXEL2D_CXX__
#define __IMAGEFROMPIXEL2D_CXX__

#include "ImageFromPixel2D.h"
#include "DataFormat/EventPixel2D.h"
#include "DataFormat/EventImage2D.h"
namespace larcv {

  static ImageFromPixel2DProcessFactory __global_ImageFromPixel2DProcessFactory__;

  ImageFromPixel2D::ImageFromPixel2D(const std::string name)
    : ProcessBase(name)
  {}
    
  void ImageFromPixel2D::configure(const PSet& cfg)
  {
    _pixel_producer  = cfg.get<std::string>("PixelProducer");
    _image_producer  = cfg.get<std::string>("ImageProducer");
    _output_producer = cfg.get<std::string>("OutputProducer");
    _type_pi  = (PIType_t)(cfg.get<unsigned short>("PIType",(unsigned short)(PIType_t::kPITypeFixedPI)));
    _fixed_pi = cfg.get<float>("FixedPI",100);
  }

  void ImageFromPixel2D::initialize()
  {}

  bool ImageFromPixel2D::process(IOManager& mgr)
  {
    auto ev_pixel     = (EventPixel2D*)(mgr.get_data(kProductPixel2D,_pixel_producer));
    auto ev_in_image  = (EventImage2D*)(mgr.get_data(kProductImage2D,_image_producer));
    auto ev_out_image = (EventImage2D*)(mgr.get_data(kProductImage2D,_output_producer));

    if(!ev_pixel) {
      LARCV_CRITICAL() << "EventPixel2D not found by name " << _pixel_producer << std::endl;
      throw larbys();
    }
    if(!ev_in_image) {
      LARCV_CRITICAL() << "EventImage2D not found by name " << _image_producer << std::endl;
      throw larbys();
    }
    if(!ev_out_image) {
      LARCV_CRITICAL() << "EventImage2D could not be craeted by name " << _output_producer << std::endl;
      throw larbys();
    }
    if(ev_out_image->Image2DArray().size()) {
      LARCV_CRITICAL() << "EventImage2D by name " << _output_producer << " (output) is not empty!" << std::endl;
      throw larbys();
    }

    auto const& pcluster_m = ev_pixel->Pixel2DArray();
    auto const& meta_m     = ev_pixel->MetaArray();
    auto const& image_v    = ev_in_image->Image2DArray();

    for(size_t plane=0; plane<image_v.size(); plane++) {

      auto const& ref_meta = image_v.at(plane).meta();
      auto const& ref_data = image_v.at(plane).as_vector();
      Image2D out_image(ref_meta);
      out_image.paint(0.);

      auto const& pcluster_iter = pcluster_m.find(plane);
      if(pcluster_iter == pcluster_m.end()) {
	ev_out_image->Emplace(std::move(out_image));
	continue;
      }

      auto meta_iter = meta_m.find(plane);
      if(meta_iter != meta_m.end() && ref_meta != (*meta_iter).second) {
	LARCV_CRITICAL() << "Found non-compatible image meta!" << std::endl
			 << "      Cluster: " << (*meta_iter).second.dump()
			 << "      Ref    : " << ref_meta.dump();
	throw larbys();
      }

      auto const& pcluster = (*pcluster_iter).second;
      //std::cout<<ref_meta.dump()<<std::endl;
      for(auto const& pixel2d : pcluster) {
	//std::cout<<pixel2d.X() << " " << pixel2d.Y() << " ... " << pixel2d.Intensity() << std::endl;
	//out_image.set_pixel(pixel2d.Y(), pixel2d.X(),100);
	float val=_fixed_pi;
	size_t index = (pixel2d.X() * ref_meta.rows() + pixel2d.Y());
	switch(_type_pi) {
	case PIType_t::kPITypeFixedPI:
	  break;
	case PIType_t::kPITypeInputImage:
	  val = ref_data[index];
	  break;
	case PIType_t::kPITypeClusterIndex:
	  LARCV_CRITICAL() << "kPITypeClusterIndex not unsupported!" <<std::endl;
	  throw larbys();
	  break;
	case PIType_t::kPITypeUndefined:
	  val = pixel2d.Intensity();
	  break;
	}
	//std::cout<<"Filling " << pixel2d.X() << "  " << pixel2d.Y() << std::endl;
	out_image.set_pixel( (pixel2d.X() * ref_meta.rows() + pixel2d.Y()),val);
      }
      ev_out_image->Emplace(std::move(out_image));
    }
    return true;
  }

  void ImageFromPixel2D::finalize()
  {}

}
#endif
