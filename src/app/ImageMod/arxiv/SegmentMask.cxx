#ifndef __SEGMENTMASK_CXX__
#define __SEGMENTMASK_CXX__

#include "SegmentMask.h"
#include "DataFormat/EventImage2D.h"
#include "DataFormat/EventROI.h"
#include "DataFormat/UtilFunc.h"

namespace larcv {

  static SegmentMaskProcessFactory __global_SegmentMaskProcessFactory__;

  SegmentMask::SegmentMask(const std::string name)
    : ProcessBase(name)
  {
    _output_producer = "";
  }
    
  void SegmentMask::configure(const PSet& cfg)
  {
    _roi_producer = cfg.get<std::string>("ROIProducer","");
    _output_producer = cfg.get<std::string>("OutputProducer","");
    _valid_roi_v.clear();
    auto valid_roi = cfg.get<std::vector<std::string> >("ValidROI");
    for(auto const& name : valid_roi) {
      auto const roi_type = String2ROIType(name);
      if(roi_type == kROIUnknown) {
	LARCV_CRITICAL() << "Invalid ROI type name provided: " << name << std::endl;
	throw larbys();
      }
      _valid_roi_v.push_back((ROIType_t)roi_type);
    }
    if(_valid_roi_v.empty() && _roi_producer.empty()) {
      LARCV_CRITICAL() << "No valid ROI type nor ROI producer name provided!" << std::endl;
      throw larbys();
    }
    _mask_value = (ROIType_t)(cfg.get<unsigned short>("MaskValue",(unsigned short)(kROIUnknown)));
    _image_producer = cfg.get<std::string>("ImageProducer");
    _plane_v = cfg.get<std::vector<unsigned short> >("PlaneID");
  }

  void SegmentMask::initialize()
  {}

  bool SegmentMask::process(IOManager& mgr)
  {
    auto event_image = (EventImage2D*)(mgr.get_data(kProductImage2D,_image_producer));
    if(!event_image) {
      LARCV_CRITICAL() << "EventImage2D not found: " << _image_producer << std::endl;
      throw larbys();
    }


    EventImage2D* output_event_image = nullptr;
    
    std::vector<Image2D> image_v;
    if(_output_producer.empty()) {
      event_image->Move(image_v);
      output_event_image = event_image;
    }
    else {
      output_event_image = (EventImage2D*)(mgr.get_data(kProductImage2D,_output_producer));
      if(!output_event_image) {
	LARCV_CRITICAL() << "Failed to create an output data product w/ name " <<_output_producer << std::endl;
	throw larbys();
      }
      if(output_event_image->Image2DArray().size()) {
	LARCV_CRITICAL() << "Output producer not empty! " << _output_producer << std::endl;
	throw larbys();
      }
      image_v = event_image->Image2DArray();
    }
    
    std::vector<bool> valid_roi_v(kROITypeMax,false);
    if(!_roi_producer.empty()) {
      auto event_roi = (EventROI*)(mgr.get_data(kProductROI,_roi_producer));
      if(!event_roi) {
	LARCV_CRITICAL() << "EventROI not found: " << _roi_producer << std::endl;
	throw larbys();
      }
      for(auto const& roi : event_roi->ROIArray())

	valid_roi_v[roi.Type()] = true;
    }
    
    for(auto const& roi : _valid_roi_v) valid_roi_v[roi] = true;

    // Plane ID sanity check
    for(auto const& pid : _plane_v) {

      if(pid < image_v.size()) continue;

      LARCV_CRITICAL() << "Plane ID " << pid << " does not exist!" << std::endl;
      throw larbys();
    }
    
    for(auto const& pid : _plane_v) {

      auto& image = image_v[pid];
      auto const& data = image.as_vector();
      
      for(size_t px_idx=0; px_idx < data.size(); ++px_idx) {

	if(data[px_idx]>=valid_roi_v.size()) {
	  LARCV_WARNING() << "Masking an invalid segment value " << data[px_idx] << " @ " << px_idx << std::endl;
	  image.set_pixel(px_idx,_mask_value);
	}
	else if(!valid_roi_v[(size_t)(data[px_idx])]) 
	  image.set_pixel(px_idx,_mask_value);
      }
      
      size_t pixel_val;
      for(size_t px_idx=0; px_idx < data.size(); ++px_idx) {

	if(data[px_idx]<0) image.set_pixel(px_idx,_mask_value);
	else{
	  pixel_val = (size_t)(data[px_idx]);

	  if(pixel_val >= valid_roi_v.size() || !valid_roi_v[pixel_val])
	    image.set_pixel(px_idx,_mask_value);
	}
      }
    }
      
    output_event_image->Emplace(std::move(image_v));
    
    return true;
  }

  void SegmentMask::finalize()
  {}

}
#endif
