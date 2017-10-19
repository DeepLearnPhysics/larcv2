#ifndef __GENDEADCHANNELMAP_CXX__
#define __GENDEADCHANNELMAP_CXX__

#include "GenDeadChannelMap.h"

namespace larcv {

  static GenDeadChannelMapProcessFactory __global_GenDeadChannelMapProcessFactory__;

  GenDeadChannelMap::GenDeadChannelMap(const std::string name)
    : ProcessBase(name)
  {}
    
  void GenDeadChannelMap::configure(const PSet& cfg)
  {
    _img_producer = cfg.get<std::string>("ImageProducer");
  }

  void GenDeadChannelMap::initialize()
  {}

  bool GenDeadChannelMap::process(IOManager& mgr)
  {

    auto ev_img2d = (EventImage2D*) mgr.get_data(kProductImage2D,_img_producer);
    
    if (_img_v.empty()) {
      _img_v.resize(3);
      _meta_v.resize(3);

      auto img2d_v = ev_img2d->Image2DArray();

      for(size_t plane=0; plane<3; ++plane) { 
	img2d_v[plane].binary_threshold(1.0,0.0,1.0);
	_img_v[plane]  = std::move(img2d_v[plane].move());
	_meta_v[plane] = ev_img2d->Image2DArray()[plane].meta();
      }

      return true;
    }
    
    for(size_t plane=0; plane<3; ++plane) {
      const auto& img = ev_img2d->Image2DArray()[plane].as_vector();
      for(size_t pid=0; pid < _img_v[plane].size(); ++pid) {
	if (img[pid]>=1.0) _img_v[plane][pid] += 1.0;
      }
    }

    return true;
  }

  void GenDeadChannelMap::finalize()
  {
       
    IOManager out_iom(IOManager::kWRITE);
    out_iom.set_verbosity((larcv::msg::Level_t)0);
    out_iom.set_out_file("test.root");
    out_iom.initialize();
    
    auto ev_img2d = (EventImage2D*)out_iom.get_data(kProductImage2D,"test");

    for(size_t plane=0; plane<3; ++plane) {
      Image2D img(_meta_v[plane]);
      img.move(std::move(_img_v[plane]));
      ev_img2d->Emplace(std::move(img));
    }

    out_iom.set_id(1,1,1);
    out_iom.save_entry();
    out_iom.finalize();
  }
  
}
#endif
