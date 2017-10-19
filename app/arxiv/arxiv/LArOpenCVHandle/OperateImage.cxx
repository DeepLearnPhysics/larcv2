#ifndef __OPERATEIMAGE_CXX__
#define __OPERATEIMAGE_CXX__

#include "OperateImage.h"
#include "DataFormat/EventImage2D.h"
#include <assert.h>

namespace larcv {

  static OperateImageProcessFactory __global_OperateImageProcessFactory__;

  OperateImage::OperateImage(const std::string name)
    : ProcessBase(name)
  {}
    
  void OperateImage::configure(const PSet& cfg)
  {
    _output_producer = cfg.get<std::string>("OutputProducer","");
    _ref_producer_v  = cfg.get<std::vector<std::string> >("ReferenceProducers");
    _operation_v    = cfg.get<std::vector<uint> >("Operations");

    assert(_ref_producer_v.size() == _operation_v.size()+1);
  }

  bool OperateImage::process(IOManager& mgr)
  {

    //
    // Reference
    //
    std::vector<EventImage2D*> event_image_v;
    event_image_v.reserve(_ref_producer_v.size());
    
    for(auto ref_producer : _ref_producer_v) {
      auto ref_event_image = (EventImage2D*)(mgr.get_data(kProductImage2D,ref_producer));
      if(!ref_event_image) {
	LARCV_CRITICAL() << "Reference EventImage2D not found: " << ref_producer << std::endl;
	throw larbys();
      }
      event_image_v.emplace_back(ref_event_image);
    }

    //
    // Output producer
    // 
    
    auto out_event_image = (EventImage2D*)(mgr.get_data(kProductImage2D,_output_producer));

    auto img_v = event_image_v.front()->Image2DArray();

    for(size_t operation_id=0; operation_id < _operation_v.size(); ++operation_id) {
      Operation_t operation = (Operation_t) _operation_v[operation_id];

      const auto next_img_v = event_image_v.at(operation_id+1)->Image2DArray();
      
      for(size_t img_id=0;img_id<img_v.size();++img_id) {
	auto& img = img_v[img_id];
	const auto& next_img = next_img_v[img_id];
	  
	switch (operation) {
	case Operation_t::kSum:      img += next_img.as_vector(); break;
	case Operation_t::kSubtract: img -= next_img.as_vector(); break;
	  //case Operation_t::kMultiply: img *= next_img; break;
	  //case Operation_t::kDivide:   img /= next_img; break;
	default: throw larbys("Invalid operation requested");
	}
      }
    }
    out_event_image->Emplace(std::move(img_v));

    return true;
  }
}
#endif
