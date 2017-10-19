#ifndef __IMAGEFROMPIXEL2DCLUSTER_CXX__
#define __IMAGEFROMPIXEL2DCLUSTER_CXX__

#include "ImageFromPixel2DCluster.h"
#include "DataFormat/EventPixel2D.h"
#include "DataFormat/EventImage2D.h"
namespace larcv {

  static ImageFromPixel2DClusterProcessFactory __global_ImageFromPixel2DClusterProcessFactory__;

  ImageFromPixel2DCluster::ImageFromPixel2DCluster(const std::string name)
    : ProcessBase(name)
  {}
    
  void ImageFromPixel2DCluster::configure(const PSet& cfg)
  {
    _pixel_producer  = cfg.get<std::string>("PixelProducer");
    _image_producer  = cfg.get<std::string>("ImageProducer");
    _output_producer = cfg.get<std::string>("OutputProducer");
    _type_pi  = (PIType_t)(cfg.get<unsigned short>("PIType",(unsigned short)(PIType_t::kPITypeFixedPI)));
    _fixed_pi = cfg.get<float>("FixedPI",100);
  }

  void ImageFromPixel2DCluster::initialize()
  {}

  bool ImageFromPixel2DCluster::process(IOManager& mgr)
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

    auto const& pcluster_m = ev_pixel->Pixel2DClusterArray();
    auto const& meta_m     = ev_pixel->ClusterMetaArray();
    auto const& image_v    = ev_in_image->Image2DArray();

    for(size_t plane=0; plane<image_v.size(); plane++) {

      auto const& ref_meta = image_v.at(plane).meta();
      auto const& ref_data = image_v.at(plane).as_vector();
      Image2D out_image(ref_meta);
      out_image.paint(0.);
      
      auto meta_iter = meta_m.find(plane);
      auto clus_iter = pcluster_m.find(plane);
      if(meta_iter == meta_m.end() || clus_iter==pcluster_m.end()) {
	LARCV_WARNING() << "Plane " << plane << " not found in ClusterMetaArray()!" << std::endl;
	ev_out_image->Emplace(std::move(out_image));
	continue;
      }
      
      auto const& pcluster_v = (*clus_iter).second;
      auto const& meta_v = (*meta_iter).second;

      // sanity check
      if(meta_v.size() != pcluster_v.size() ) {
	LARCV_CRITICAL() << "# cluster meta and # cluster does not match!" << std::endl;
	throw larbys();
      }
      for(auto const& meta : meta_v) {
	if(meta == ref_meta) continue;
	LARCV_CRITICAL() << "Found non-compatible image meta!" << std::endl
			 << "      Cluster: " << meta.dump()
			 << "      Ref    : " << ref_meta.dump();
	throw larbys();
      }
      
      for(size_t cluster_idx=0; cluster_idx<pcluster_v.size(); ++cluster_idx){
	auto const& pcluster = pcluster_v[cluster_idx];
	auto const& meta     = meta_v[cluster_idx];
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
	    val = cluster_idx+1;
	    break;
	  }
	  out_image.set_pixel( (pixel2d.X() * ref_meta.rows() + pixel2d.Y()),val);
	}
      }
      ev_out_image->Emplace(std::move(out_image));
    }
    return true;
  }

  void ImageFromPixel2DCluster::finalize()
  {}

}
#endif
