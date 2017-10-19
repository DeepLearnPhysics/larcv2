#ifndef __LARCV_CVUTIL_CXX__
#define __LARCV_CVUTIL_CXX__

#include "CVUtil.h"

namespace larcv {

  cv::Mat as_mat(const Image2D& larcv_img)
  {
    auto const& meta = larcv_img.meta();
    cv::Mat img(meta.rows(),meta.cols(),CV_8UC3);
    
    unsigned char* px_ptr = (unsigned char*)img.data;
    int cn = img.channels();
    
    for(size_t i=0;i<meta.rows();i++) {
      for (size_t j=0;j<meta.cols();j++) {
	
	float q = larcv_img.pixel(i,j);
	px_ptr[i*img.cols*cn + j*cn + 0] = (unsigned char)(((int)(q+0.5)));
	px_ptr[i*img.cols*cn + j*cn + 1] = (unsigned char)(((int)(q+0.5))/256);
	px_ptr[i*img.cols*cn + j*cn + 2] = (unsigned char)(((int)(q+0.5))/256/256);
      }
    }
    return img;
  }

  cv::Mat as_gray_mat(const Image2D& larcv_img,
		      float q_min,float q_max,float q_to_gray)
  {
    auto const& meta = larcv_img.meta();
    cv::Mat img(meta.rows(),meta.cols(),CV_8UC1);
    
    unsigned char* px_ptr = (unsigned char*)img.data;
    
    for(size_t i=0;i<meta.rows();i++) {
      for (size_t j=0;j<meta.cols();j++) {
	float q = larcv_img.pixel(i,j);
	q -= q_min;
	if(q < 0) q = 0;
	if(q > q_max) q = q_max;
	q /= q_to_gray;
	px_ptr[i*img.cols + j] = (uchar)((int)q);
	//img.at<uchar>(j,meta.rows()-1-i) = (uchar)((int)q);
      }
    }
    return img;
  }
  cv::Mat as_mat_1FC(const Image2D& larcv_img)
  {

    auto const& meta = larcv_img.meta();
    cv::Mat img(meta.rows(),meta.cols(),CV_32FC1);
        
    for(size_t i=0;i<meta.rows();i++) {
      for (size_t j=0;j<meta.cols();j++) {
	float q = larcv_img.pixel(i,j);
	img.at<float>(i,j) = q;
      }
    }
    return img;
  }

  cv::Mat as_mat_greyscale2bgr(const Image2D& larcv_img, const float min, const float max)
  {
    // converts into a color scaled image
    auto const& meta = larcv_img.meta();
    cv::Mat img(meta.rows(),meta.cols(),CV_8UC3);
    
    unsigned char* px_ptr = (unsigned char*)img.data;
    int cn = img.channels();

    
    for(size_t i=0;i<meta.rows();i++) {
      for (size_t j=0;j<meta.cols();j++) {
	float q = larcv_img.pixel(i,j);
	if ( q>max ) q = max;
	if ( q<min ) q = min;
	float rgbval = ((q-min)/(max-min))*255.0;
	px_ptr[i*img.cols*cn + j*cn + 0] = (unsigned char)(((int)(rgbval)));
	px_ptr[i*img.cols*cn + j*cn + 1] = (unsigned char)(((int)(rgbval)));
	px_ptr[i*img.cols*cn + j*cn + 2] = (unsigned char)(((int)(rgbval)));
      }
    }
    return img;
  }

  larcv::Image2D mat_to_image2d( const cv::Mat& mat, const ImageMeta& src_meta ) {
    
    int cn = mat.channels();
    // more than 1 channel assumes a color scale. not well-defined how to re-constitute image
    if ( cn>1 ) {
      throw std::runtime_error("CVUtil.cxx: mat_to_image2d, number of channels != 1");
    }

    // define the meta
    ImageMeta meta( src_meta );
    if ( meta.width()==0.0 && meta.height()==0.0 ) {
      // empty meta
      meta = ImageMeta( mat.cols, mat.rows, mat.rows, mat.cols );
    }
    larcv::Image2D img( meta );
    
    for (size_t i=0; i<meta.rows(); i++) {
      for (size_t j=0; j<meta.cols(); j++) {
	if ( mat.type()==CV_32F ) {
	  float q = (float)mat.at<float>(i,j);
	  img.set_pixel( i, j, q );
	}
	else if (mat.type()==CV_8U) {
	  int q = (int)mat.at<unsigned char>(i,j);
	  img.set_pixel( i, j, (float)q );
	}
	else {
	  throw std::runtime_error("CVUtil.cxx: mat_to_image2d, data type not handled (accepts CV_8U=unsigned char or CV_64F=double)");
	}
      }
    }
    
    return img;
  }

  Image2D imread(const std::string file_name)
  {
    ::cv::Mat image;
    image = ::cv::imread(file_name.c_str(), CV_LOAD_IMAGE_COLOR);

    ImageMeta meta(image.cols,image.rows,image.cols, image.rows, 0., 0.);
    Image2D larcv_img(meta);
      
    unsigned char* px_ptr = (unsigned char*)image.data;
    int cn = image.channels();
    
    for(int i=0;i<image.rows;i++) {
      for (int j=0;j<image.cols;j++) {
	float q = 0;
	q += (float)(px_ptr[i*image.cols*cn + j*cn + 0]);               //B
	q += (float)(px_ptr[i*image.cols*cn + j*cn + 1]) * 256.;        //G
	q += (float)(px_ptr[i*image.cols*cn + j*cn + 2]) * 256. * 256.; //R
	larcv_img.set_pixel(j,i,q);
      }
    }
    return larcv_img;
  }

  
  Image2D imread_gray(const std::string file_name)
  {
    ::cv::Mat image;
    image = ::cv::imread(file_name.c_str(), CV_LOAD_IMAGE_GRAYSCALE);

    ImageMeta meta(image.cols,image.rows,image.cols, image.rows, 0., 0.);
    Image2D larcv_img(meta);
      
    unsigned char* px_ptr = (unsigned char*)image.data;

    for(int i=0;i<image.rows;i++) {
      for (int j=0;j<image.cols;j++) {
	float q = 0;
	q += (float)(px_ptr[i*image.cols + j]);
	larcv_img.set_pixel(j,i,q);
      }
    }
    return larcv_img;
  }


  void draw_bb( cv::Mat& mat, const larcv::ImageMeta& img_meta, const larcv::ImageMeta& bbox,
		const int r, const int g, const int b, const int thickness ) {
    cv::rectangle( mat,
		   cv::Point( img_meta.col( bbox.min_x() ), img_meta.row( bbox.min_y() ) ),
		   cv::Point( img_meta.col( bbox.max_x() ), img_meta.row( bbox.max_y() ) ),
		   cv::Scalar( b, g, r ),
		   thickness );
  }
  
}

#endif
