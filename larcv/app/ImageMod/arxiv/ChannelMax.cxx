#ifndef __CHANNELMAX_CXX__
#define __CHANNELMAX_CXX__

#include "ChannelMax.h"
#include "DataFormat/EventImage2D.h"

namespace larcv {

  static ChannelMaxProcessFactory __global_ChannelMaxProcessFactory__;

  ChannelMax::ChannelMax(const std::string name)
    : ProcessBase(name)
  {}
    
  void ChannelMax::configure(const PSet& cfg)
  {
    _in_producer      = cfg.get<std::string>("InProducer");
    _nplanes          = cfg.get<size_t>("NPlanes");
    _out_producer     = cfg.get<std::string>("OutputProducer");

    _plane_weight_v   = cfg.get<std::vector<float> >("ChannelWeights",{});
    if (_plane_weight_v.empty()) {
      _plane_weight_v.resize(_nplanes,1.0);
    }

    auto relevant_chanid_v  = cfg.get<std::vector<size_t> >("RelevantChannels",{});
    _relevant_chan_v.resize(_nplanes);

    //set all channels false
    for(size_t plane=0;plane<_nplanes;++plane) _relevant_chan_v[plane]=false;

    //if no input, set all channels true
    if (relevant_chanid_v.empty()) {
      for(size_t plane=0;plane<_nplanes;++plane) _relevant_chan_v[plane]=true;
    }
    //else set the requested ones true
    else {
      for(auto& chanid : relevant_chanid_v) {
	_relevant_chan_v[chanid]=true;
      }
    }

    auto channel_mask_v = cfg.get<std::vector<float> >("ChannelMask",{});
    _channel_mask_v.resize(_nplanes);
    if (channel_mask_v.empty()) {
      for(size_t plane=0;plane<_nplanes;++plane) _channel_mask_v[plane] = (float)plane;
    } else {
      _channel_mask_v = channel_mask_v;
    }
  }

  void ChannelMax::initialize()
  {}

  bool ChannelMax::process(IOManager& mgr)
  {

    auto event_image = (EventImage2D*)(mgr.get_data(kProductImage2D,_in_producer));    
    if(!event_image) throw larbys("No event image found");
    auto& img_v = event_image->Image2DArray();

    if (img_v.empty()) return false;
    
    larcv::Image2D image(img_v[0].meta());
    
    for(size_t row=0;row<image.meta().rows();++row){
      for(size_t col=0;col<image.meta().cols();++col){
	float maxpx(-1),maxpl(-1);
	for(size_t plane=0;plane<_nplanes;++plane) {
	  if(!_relevant_chan_v[plane]) continue;
	  auto px=img_v[plane].pixel(row,col);
	  px*=_plane_weight_v[plane];
	  if (px>maxpx) { maxpx=px; maxpl=plane; }
	}
	if (maxpx<0 or maxpl<0) throw larbys("No max plane identified");
	image.set_pixel(row,col,_channel_mask_v[maxpl]);
      }
    }
    
    auto out_image = (EventImage2D*)(mgr.get_data(kProductImage2D,_out_producer));
    
    out_image->Emplace(std::move(image));

    return true;
  }

  void ChannelMax::finalize()
  {}

}
#endif
