#ifndef __IMAGEHOLDER_CXX__
#define __IMAGEHOLDER_CXX__

#include "ImageHolder.h"

namespace larcv {

  ImageHolder::ImageHolder(const std::string name)
    : ProcessBase(name)
  { _run = _subrun = _event = 0; }

  void ImageHolder::move_pmt_image(Image2D& dest)
  { dest = std::move(_pmt_image); }

  void ImageHolder::move_tpc_image(std::vector<larcv::Image2D>& dest)
  { dest = std::move(_tpc_image_v); }

  void ImageHolder::move_tpc_segment(std::vector<larcv::Image2D>& dest)
  { dest = std::move(_tpc_segment_v); }

  void ImageHolder::move_ch_status(std::map<larcv::PlaneID_t,larcv::ChStatus>& dest)
  { dest = std::move(_ch_status_m); }

  void ImageHolder::move_roi(std::vector<larcv::ROI>& dest)
  { dest = std::move(_roi_v); }

}
#endif
