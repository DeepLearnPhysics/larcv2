#ifndef __MULTIROICROPPER_CXX__
#define __MULTIROICROPPER_CXX__

#include "MultiROICropper.h"
#include "DataFormat/EventROI.h"
#include "DataFormat/EventImage2D.h"
namespace larcv {

  static MultiROICropperProcessFactory __global_MultiROICropperProcessFactory__;

  MultiROICropper::MultiROICropper(const std::string name)
    : ProcessBase(name)
  {}
    
  void MultiROICropper::configure(const PSet& cfg)
  {
    _image_producer = cfg.get<std::string>("ImageProducer");
    _roi_producer = cfg.get<std::string>("ROIProducer");
    _target_rows = cfg.get<size_t>("TargetRows");
    _target_cols = cfg.get<size_t>("TargetCols");
    _target_ch   = cfg.get<size_t>("TargetChannel");
  }

  void MultiROICropper::initialize()
  {}

  bool MultiROICropper::process(IOManager& mgr)
  {
    _cropped_v.clear();

    auto event_roi_v = (EventROI*)(mgr.get_data(kProductROI,_roi_producer));
    if(!event_roi_v){
      LARCV_CRITICAL() << "EventROI not found for label " << _roi_producer << std::endl;
      throw larbys();
    }

    auto const& roi_v = event_roi_v->ROIArray();
    if(roi_v.empty()) return false;

    // assert valid pointer
    auto event_img_v = (EventImage2D*)(mgr.get_data(kProductImage2D,_image_producer));
    if(!event_img_v){
      LARCV_CRITICAL() << "EventImage2D not found for label " << _image_producer << std::endl;
      throw larbys();
    }

    // assert target image ch
    auto const& img_v = event_img_v->Image2DArray();
    if(img_v.size() <= _target_ch) {
      LARCV_CRITICAL() << "EventImage2D size " << img_v.size() << " <= target channel " << _target_ch << std::endl;
      throw larbys();
    }

    // get image, meta
    _image = img_v[_target_ch];
    auto const& meta = _image.meta();
    LARCV_INFO() << "Image dimension: " << std::endl << meta.dump() << std::endl;
    double px_width  = meta.width()  / (double)(meta.cols());
    double px_height = meta.height() / (double)(meta.rows());
    double min_x = meta.min_x();
    double min_y = meta.min_y();
    double max_x = meta.max_x();
    double max_y = meta.max_y();
    size_t max_col = meta.cols();
    size_t max_row = meta.rows();

    // Loop over ROIs and find relevant regions
    std::vector<larcv::ImageMeta> meta_v;
    for(auto const& roi : roi_v) {
      if(roi.MCSTIndex() != kINVALID_INDEX) continue;

      auto const& bb_v = roi.BB();
      if(bb_v.size() <= _target_ch) {
	LARCV_CRITICAL() << "Target channel " << _target_ch << " too large for # BB " << bb_v.size() << std::endl;
	throw larbys();
      }
      ImageMeta bb = bb_v[_target_ch].overlap(meta);
      LARCV_INFO() << "Target ROI:" << std::endl << bb.dump() << std::endl;
      double bb_min_x = bb.min_x();
      double bb_max_x = bb.max_x();
      double bb_min_y = bb.min_y();
      double bb_max_y = bb.max_y();

      double bb_width  = bb_max_x - bb_min_x;
      double bb_height = bb_max_y - bb_min_y;

      size_t nbox_x = (size_t)(bb_width / px_width) / _target_cols;
      if(bb_width > px_width * nbox_x * _target_cols) nbox_x += 1;

      size_t nbox_y = (size_t)(bb_height / px_height) / _target_rows;
      if(bb_height > px_height * nbox_y * _target_rows) nbox_y += 1;

      for(size_t ix=0; ix < nbox_x; ++ix) {

	double crop_min_x = bb_min_x + ix * _target_cols * px_width;
	double crop_max_x = bb_min_x + (ix+1) * _target_cols * px_width;
	if(crop_max_x > max_x) 
	  crop_min_x = max_x - _target_cols * px_width;

	for(size_t iy=0; iy < nbox_y; ++iy) {
	  double crop_max_y = bb_max_y - iy * _target_rows * px_height;
	  double crop_min_y = bb_max_y - (iy+1) * _target_rows * px_height;
	  if(crop_min_y < min_y)
	    crop_max_y = min_y + _target_rows * px_height;
	  
	  meta_v.push_back(ImageMeta(px_width * _target_cols, px_height * _target_rows,
				     _target_rows, _target_cols,
				     crop_min_x, crop_max_y,
				     meta.plane()));
	  LARCV_INFO() << "Small ROI " << meta_v.size() - 1 << ": " << std::endl
		       << meta_v.back().dump() << std::endl;
	}
      }
    }

    _cropped_v.clear();
    for(auto const& meta : meta_v)
      _cropped_v.emplace_back(std::move(_image.crop(meta)));

    return true;
  }

  void MultiROICropper::finalize()
  {}

}
#endif
