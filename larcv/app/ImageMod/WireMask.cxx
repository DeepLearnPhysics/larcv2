#ifndef __WIREMASK_CXX__
#define __WIREMASK_CXX__

#include "WireMask.h"
#include "larcv/core/DataFormat/EventImage2D.h"
#include "larcv/core/DataFormat/EventChStatus.h"
namespace larcv {

  static WireMaskProcessFactory __global_WireMaskProcessFactory__;

  WireMask::WireMask(const std::string name)
    : ProcessBase(name)
  {}
    
  void WireMask::configure(const PSet& cfg)
  {
    LARCV_DEBUG() << "start\n";
    _wire_v.clear();
    _wire_v = cfg.get<std::vector<size_t> >("WireList",_wire_v);
    _image_producer = cfg.get<std::string>("ImageProducer");

    _chstatus_producer = cfg.get<std::string>("ChStatusProducer","");
    _plane_id = cfg.get<int>("PlaneID",-1);
    _mask_val = cfg.get<float>("MaskValue",0);
    _threshold = (chstatus::ChannelStatus_t)(cfg.get<unsigned short>("ChStatusThreshold",chstatus::kGOOD));

    if(_wire_v.empty() && _chstatus_producer.empty()) {
      LARCV_CRITICAL() << "Neither wire list nor ChStatus producer name given. Nothing to mask!" << std::endl;
      throw larbys();
    }
  }

  void WireMask::initialize()
  {}

  bool WireMask::process(IOManager& mgr)
  {
    auto input_image_v = (EventImage2D*)(mgr.get_data("image2d",_image_producer));
    LARCV_DEBUG() << "input_image_v ptr : " << input_image_v << "\n";
    if(!input_image_v) {
      LARCV_CRITICAL() << "Invalid image producer name: " << _image_producer << std::endl;
      throw larbys();
    }
    EventChStatus* ev_chstatus = nullptr;
    if(!_chstatus_producer.empty()) {
      ev_chstatus = (EventChStatus*)(mgr.get_data("chstatus",_chstatus_producer));
      if(!ev_chstatus) {
	LARCV_CRITICAL() << "ChStatus by " << _chstatus_producer << " not found!" << std::endl;
	throw larbys();
      }

    }

    // For operation, move an array to this scope
    std::vector<Image2D> image_v;
    input_image_v->move(image_v);

    // make sure plane id is valid
    if(_plane_id >=0 && (int)(image_v.size()) <= _plane_id) {
      LARCV_CRITICAL() << "Could not find plane: " << _plane_id << ". image_v.size(): " << image_v.size() << std::endl;
      throw larbys();
    }

    // get a handle on modifiable reference
    for(int plane=0; plane<(int)(image_v.size()); ++plane) {
      
      if(_plane_id>=0 && _plane_id != plane) continue;

      // construct wire numbers to mask
      std::set<size_t> wire_s;
      for(auto const& w : _wire_v) wire_s.insert(w);
      if(ev_chstatus) {
	auto const& status_v = ev_chstatus->status(plane).as_vector();

	for(size_t w=0; w<status_v.size(); ++w) {
	  if(status_v[w] < _threshold) wire_s.insert(w);

	}
      }

      auto& img = image_v.at(plane);
      auto const& meta = img.meta();
      std::vector<float> empty_column(img.meta().rows(),_mask_val);

      // Loop over wires, find target column and erase
      for(auto const& ch : wire_s) {
	
	if(ch < meta.min_x() || ch >= meta.max_x()) continue;

	auto col = meta.col((double)ch);

	img.copy(0,col,empty_column);

      }
    }

    // put back an image
    input_image_v->emplace(std::move(image_v));

    return true;
  }

  void WireMask::finalize()
  {}

}
#endif
