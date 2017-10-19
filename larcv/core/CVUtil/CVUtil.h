#ifndef __LARCV_CVUTIL_H__
#define __LARCV_CVUTIL_H__

#ifndef __CINT__ 
#ifndef __CLING__
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#endif
#endif
#include "larcv/core/DataFormat/Image2D.h"
#include "larcv/core/DataFormat/ImageMeta.h"

namespace larcv {
#ifndef __CINT__
#ifndef __CLING__
  /// Image2D to cv::Mat converter (not supported in ROOT5 CINT)
  cv::Mat as_mat(const Image2D& larcv_img);

  cv::Mat as_gray_mat(const Image2D& larcv_img,
		      float q_min,float q_max,float q_to_gray);
    
  cv::Mat as_mat_1FC(const Image2D& larcv_img);
  
  cv::Mat as_mat_greyscale2bgr(const Image2D& larcv_img,
			       const float min, const float max);
  
  Image2D mat_to_image2d( const cv::Mat& mat,
			  const ImageMeta& src_meta=larcv::ImageMeta() );
  
  void draw_bb( cv::Mat& mat,
		const larcv::ImageMeta& img_meta, const larcv::ImageMeta& bbox,
		const int r, const int g, const int b, const int thickness );
#endif
#endif
  /// larcv::Image2D creator from an image file
  Image2D imread(const std::string file_name);
  /// Gray scale larcv::Image2D creator from an image file
  Image2D imread_gray(const std::string file_name);
}

#endif
