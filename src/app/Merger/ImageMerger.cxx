#ifndef __IMAGEMERGER_CXX__
#define __IMAGEMERGER_CXX__

#include "ImageMerger.h"
#include "DataFormat/EventImage2D.h"
#include "DataFormat/EventROI.h"
#include "DataFormat/EventChStatus.h"

namespace larcv {

  static ImageMergerProcessFactory __global_ImageMergerProcessFactory__;

  ImageMerger::ImageMerger(const std::string name)
    : ProcessBase(name)
    , _in1_proc  (nullptr)
    , _in2_proc  (nullptr)
  {}
    
  void ImageMerger::configure(const PSet& cfg)
  {
    _pmt_pedestal         = cfg.get<float>       ("PMTPedestal"      );
    _out_tpc_producer     = cfg.get<std::string> ("OutNameTPCImage"  );
    _out_pmt_producer     = cfg.get<std::string> ("OutNamePMTImage"  );
    _out_roi_producer     = cfg.get<std::string> ("OutNameROI"       );
    _out_status_producer  = cfg.get<std::string> ("OutNameChStatus"  );
    _out_segment_producer = cfg.get<std::string> ("OutNameSegment"   );
  }

  void ImageMerger::initialize()
  {
  }

  void ImageMerger::InputImageHolder1(ImageHolder* ptr)
  { _in1_proc = ptr; }

  void ImageMerger::InputImageHolder2(ImageHolder* ptr)
  { _in2_proc = ptr; }

  bool ImageMerger::process(IOManager& mgr)
  {
    LARCV_INFO() << "Start merging"<<std::endl;
    if(!_in1_proc) {
      LARCV_CRITICAL() << "InputImageHolder1() must be called to set ImageHolder pointer!" << std::endl;
      throw larbys();
    }

    if(!_in2_proc) {
      LARCV_CRITICAL() << "InputImageHolder2() must be called to set ImageHolder pointer!" << std::endl;
      throw larbys();
    }

    mgr.set_id(_in2_proc->run(), _in2_proc->subrun(), _in2_proc->event());

    //
    // Retrieve data from data/mc image holders
    //
    std::vector<larcv::Image2D>  in1_tpc_image_v;
    std::vector<larcv::Image2D>  in1_tpc_segment_v;
    larcv::Image2D  in1_pmt_image;
    std::map<larcv::PlaneID_t,larcv::ChStatus> in1_status_m;
    std::vector<larcv::ROI> in1_roi_v;

    std::vector<larcv::Image2D>  in2_tpc_image_v;
    std::vector<larcv::Image2D>  in2_tpc_segment_v;
    larcv::Image2D  in2_pmt_image;
    std::map<larcv::PlaneID_t,larcv::ChStatus> in2_status_m;
    std::vector<larcv::ROI> in2_roi_v;

    LARCV_INFO() << "Moving Input1 images..." << std::endl;
    _in1_proc->move_tpc_image   ( in1_tpc_image_v   );
    _in1_proc->move_tpc_segment ( in1_tpc_segment_v );
    _in1_proc->move_pmt_image   ( in1_pmt_image     );
    _in1_proc->move_ch_status   ( in1_status_m      );
    _in1_proc->move_roi         ( in1_roi_v         );

    LARCV_INFO() << "Moving Input2 images..." << std::endl;
    _in2_proc->move_tpc_image   ( in2_tpc_image_v   );
    _in2_proc->move_tpc_segment ( in2_tpc_segment_v );
    _in2_proc->move_pmt_image   ( in2_pmt_image     );
    _in2_proc->move_ch_status   ( in2_status_m      );
    _in2_proc->move_roi         ( in2_roi_v         );

    //
    // Sanity checks
    //
    // Check size
    if(in1_tpc_image_v.size() != in2_tpc_image_v.size()) {
      LARCV_ERROR() << "# of Data stream image do not match between two streams! Skipping this entry..." << std::endl;
      return false;
    }
    // Check PlaneID
    for(size_t i=0; i<in1_tpc_image_v.size(); ++i) {
      auto const& image1 = in1_tpc_image_v[i];
      auto const& image2 = in2_tpc_image_v[i];
      if(image1.meta().plane() != image2.meta().plane()) {
	LARCV_ERROR() << "Plane ID mismatch! skipping..." << std::endl;
	return false;
      }
      if( (!in2_status_m.empty() && (in2_status_m.find(image2.meta().plane()) == in2_status_m.end())) ||
	  (!in1_status_m.empty() && (in1_status_m.find(image1.meta().plane()) == in1_status_m.end())) ) {
	LARCV_ERROR() << "Plane ID " << image1.meta().plane() << " not found for ch status!" << std::endl;
	return false;
      }
    }
    // All check done

    //
    // Merge them
    //
    // Merge status
    std::set<larcv::PlaneID_t> status_plane_s;
    for( auto const& plane_status : in1_status_m ) status_plane_s.insert(plane_status.first);
    for( auto const& plane_status : in2_status_m ) status_plane_s.insert(plane_status.first);
    for( auto const& plane : status_plane_s) {

      auto in1_iter = in1_status_m.find(plane);
      auto in2_iter = in2_status_m.find(plane);

      if(in2_iter == in2_status_m.end()) continue;
      if(in1_iter == in1_status_m.end()) {
	in1_status_m[plane] = (*in2_iter).second;
	continue;
      }

      auto const& in1_status_v = (*in1_iter).second.as_vector();
      auto const& in2_status_v = (*in2_iter).second.as_vector();

      std::vector<short> status_v(std::max(in1_status_v.size(),in2_status_v.size()),0);
      const size_t min_entry = std::min(in1_status_v.size(),in2_status_v.size());

      for(size_t i=0; i<min_entry; ++i)
	status_v[i] = std::min(in1_status_v[i],in2_status_v[i]);

      if(in1_status_v.size() > min_entry)
	for(size_t i=min_entry; i<in1_status_v.size(); ++i) status_v[i] = in2_status_v[i];

      if(in2_status_v.size() > min_entry)
	for(size_t i=min_entry; i<in2_status_v.size(); ++i) status_v[i] = in2_status_v[i];

      (*in1_iter).second=std::move(ChStatus(plane,std::move(status_v)));
      
    }

    // Merge tpc image
    for(size_t i=0; i<in1_tpc_image_v.size(); ++i) {

      // retrieve & sum
      auto& in1_image = in1_tpc_image_v[i];
      auto& in2_image = in2_tpc_image_v[i];
      in1_image.overlay(in2_image,Image2D::kSum);
      
    }
    in2_tpc_image_v.clear(); // free memory, slow but keep job memory low

    // Merge tpc segment
    std::vector<larcv::Image2D> out_segment_v;
    if(in1_tpc_segment_v.empty()) {
      out_segment_v = std::move(in1_tpc_segment_v);
    }
    else if(in2_tpc_segment_v.empty()) {
      out_segment_v = std::move(in2_tpc_segment_v);
    }
    else {
      out_segment_v = std::move(in1_tpc_segment_v);
      
      for(size_t i=0; i<out_segment_v.size(); ++i) {
	
	// retrieve & sum
	auto& in1_segment = out_segment_v[i];
	
	if(in2_tpc_segment_v.size()>i) {
	  auto const& in2_segment   = in2_tpc_segment_v[i];
	  in1_segment.overlay(in2_segment,Image2D::kMaxPool);
	}
      }
    }	

    // Merge pmt image
    in1_pmt_image.overlay(in2_pmt_image);
    in1_pmt_image -= _pmt_pedestal;

    // Merge ROI
    std::vector<larcv::ROI> out_roi_v;
    out_roi_v.reserve(in1_roi_v.size() + in2_roi_v.size());
    for(auto const& roi : in1_roi_v) out_roi_v.push_back(roi);
    for(auto const& roi : in2_roi_v) out_roi_v.push_back(roi);

    // Store
    auto out_tpc_image   = (EventImage2D*)(mgr.get_data(kProductImage2D,_out_tpc_producer));
    out_tpc_image->Emplace(std::move(in1_tpc_image_v));
    
    auto out_pmt_image   = (EventImage2D*)(mgr.get_data(kProductImage2D,_out_pmt_producer));
    out_pmt_image->Emplace(std::move(in1_pmt_image));

    auto out_tpc_segment = (EventImage2D*)(mgr.get_data(kProductImage2D,_out_segment_producer));
    out_tpc_segment->Emplace(std::move(out_segment_v));

    auto out_status = (EventChStatus*)(mgr.get_data(kProductChStatus,_out_status_producer));
    for(auto& plane_status : in1_status_m)  out_status->Emplace(std::move(plane_status.second));
    
    auto out_roi = (EventROI*)(mgr.get_data(kProductROI,_out_roi_producer));
    out_roi->Emplace(std::move(out_roi_v));

    LARCV_INFO() << "End merging"<<std::endl;
    return true;
  }

  void ImageMerger::finalize()
  {}

}
#endif
