#ifndef LARBYSIMAGEMAKER_CXX
#define LARBYSIMAGEMAKER_CXX

#include "LArbysImageMaker.h"
#include "DataFormat/EventPixel2D.h"

namespace larcv {

  void
  LArbysImageMaker::Configure(const PSet& pset) {
    LARCV_DEBUG() << "start" << std::endl;
    this->set_verbosity((msg::Level_t)pset.get<int>("Verbosity",2));

    _charge_to_gray_scale = pset.get<double>("Q2Gray");
    _charge_max = pset.get<double>("QMax");
    _charge_min = pset.get<double>("QMin");
    
    LARCV_DEBUG() << "end" << std::endl;
    return;
  }

  std::vector<cv::Mat>
  LArbysImageMaker::ExtractMat(const std::vector<Image2D>& image_v) {
    std::vector<cv::Mat> mat_v;
    auto img_data_v = ExtractImage(image_v);
    mat_v.reserve(img_data_v.size());
    for(auto& image_data : img_data_v)
      mat_v.emplace_back(std::move(std::get<0>(image_data)));
    return mat_v;
  }
  
  cv::Mat
  LArbysImageMaker::ExtractMat(const Image2D& image) {
    std::vector<cv::Mat> mat_v;
    std::vector<Image2D> image_v(1,image);
    auto img_data_v = ExtractImage(image_v);
    return std::get<0>(img_data_v.front());
  }
  
  std::vector<std::tuple<cv::Mat,larocv::ImageMeta> >
  LArbysImageMaker::ExtractImage(const std::vector<Image2D>& image_v) {

    std::vector<std::tuple<cv::Mat,larocv::ImageMeta> > ret_v;
    ret_v.resize(image_v.size(),std::make_tuple(cv::Mat(),larocv::ImageMeta()));
    
    for(size_t i=0; i<image_v.size(); ++i) {
      auto& ret = ret_v[i];
      
      auto const& cvmeta = image_v[i].meta();
	
      LARCV_DEBUG() << "Reading  image (rows,cols) = (" << cvmeta.rows() << "," << cvmeta.cols() << ") "
		    << " ... (height,width) = (" << cvmeta.height() << "," << cvmeta.width() << ")" << std::endl;
	
      larocv::ImageMeta meta(cvmeta.width(), cvmeta.height(),
			     cvmeta.cols(),  cvmeta.rows(),
			     cvmeta.min_y(), cvmeta.min_x(), i);
	
      LARCV_DEBUG() << "LArOpenCV meta @ plane " << i << " ... "
		    << "Reading  image (rows,cols) = (" << meta.num_pixel_row() << "," << meta.num_pixel_column() << ") "
		    << " ... (height,width) = (" << meta.height() << "," << meta.width() << ")" << std::endl;

      std::get<0>(ret) = cv::Mat(cvmeta.cols(),cvmeta.rows(),CV_8UC1,cvScalar(0.));
      std::get<1>(ret) = meta;
      auto& mat  = std::get<0>(ret);
      
      for(size_t row=0; row<cvmeta.rows(); ++row) {
	for(size_t col=0; col<cvmeta.cols(); ++col) {
	  float charge = image_v[i].pixel(row,col);
	  charge -= _charge_min;
	  if(charge < 0) charge = 0;
	  if(charge > _charge_max) charge = _charge_max;
	  charge /= _charge_to_gray_scale;
	  mat.at<uchar>(col,cvmeta.rows()-1-row) = (uchar)((int)charge);
	}
      }
    } // end collection of images

    return ret_v;
  } // end extract image
  
  
  void
  LArbysImageMaker::ConstructCosmicImage(IOManager& mgr,
					 std::string producer,
					 ProductType_t datatype, 
					 const std::vector<larcv::Image2D>& adc_image_v,
					 std::vector<larcv::Image2D>& mu_image_v) {
    if ( datatype==kProductPixel2D ) {
      LARCV_DEBUG() << "Constructing " << producer << " Pixel2D => Image2D" << std::endl;
      if(!producer.empty()) {
	auto ev_pixel2d = (EventPixel2D*)(mgr.get_data(kProductPixel2D,producer));
	if(!ev_pixel2d) {
	  LARCV_CRITICAL() << "Pixel2D by producer " << producer << " not found..." << std::endl;
	  throw larbys();
	}
	LARCV_DEBUG() << "Using Pixel2D producer " << producer << std::endl;
	const auto& pixel2d_m = ev_pixel2d->Pixel2DClusterArray();
	for(size_t img_idx=0; img_idx<adc_image_v.size(); ++img_idx) {
	  const auto& meta = adc_image_v[img_idx].meta();
	  if(mu_image_v.size() <= img_idx)
	    mu_image_v.emplace_back(larcv::Image2D(meta));
	  if(mu_image_v[img_idx].meta() != meta)
	    mu_image_v[img_idx] = larcv::Image2D(meta);
	  auto& mu_image = mu_image_v[img_idx];
	  mu_image.paint(0);
	  auto itr = pixel2d_m.find(img_idx);
	  if(itr == pixel2d_m.end()) {
	    LARCV_DEBUG() << "No Pixel2D found for plane " << img_idx << std::endl;
	    continue;
	  }
	  else{
	    size_t npx_x = meta.cols();
	    size_t npx_y = meta.rows();
	    for(const auto& pixel_cluster : (*itr).second) {
	      for(const auto& pixel : pixel_cluster) {
		if(pixel.X() >= npx_x || pixel.Y() >= npx_y) {
		  LARCV_WARNING() << "Ignoring cosmic pixel (row,col) = ("
				  << pixel.Y() << "," << pixel.X() << ")"
				  << " as it is out of bounds (ncol=" << npx_x << ", nrow=" << npx_y << ")" << std::endl;
		  continue;
		}
		mu_image.set_pixel( (pixel.X() * meta.rows() + pixel.Y()), 100 );
	      }
	    }
	  }
	}//end of image loop
      }//end of if producer not empty
    }//end of if datatype is pixel2d
    else if ( datatype==kProductImage2D ) {
      LARCV_DEBUG() << "Constructing " << producer << " Image2D => Image2D" << std::endl;
      auto ev_image2d = (EventImage2D*)(mgr.get_data(kProductImage2D,producer));
      if(!producer.empty()) {
	if(!ev_image2d) {
	  std::stringstream ss;
	  ss << __FILE__ << ":" << __LINE__ << " Image2D by producer " << producer << " not found..." << std::endl;
	  LARCV_CRITICAL() << ss.str() << std::endl;
	  throw larbys();
	}
	for(size_t img_idx=0; img_idx<adc_image_v.size(); ++img_idx) {
	  auto const& img = ev_image2d->Image2DArray().at(img_idx);

	  if(mu_image_v.size() <= img_idx)
	    mu_image_v.emplace_back(larcv::Image2D(img.meta()));

	  if(mu_image_v[img_idx].meta() != img.meta())
	    mu_image_v[img_idx] = larcv::Image2D(img.meta());
	  
	  auto& mu_img = mu_image_v[img_idx];
	  mu_img.paint(0);

	  for ( size_t r=0; r<img.meta().rows(); r++) {
	    for ( size_t c=0; c<img.meta().cols(); c++ ) {
	      if ( img.pixel(r,c)>0 ) {
		mu_img.set_pixel(r,c,100);
	      }
	    }
	  }
	  
	} // each cosmic image
      } // no empty producer
    } // is image2d
    else {
      std::stringstream ss;
      ss << __FILE__
	 << ":"
	 << __LINE__
	 << " Invalid data type for constructing cosmic tag image: "
	 << (size_t)datatype
	 << ", should be Pixel2D (3) or Image2D (0)" << std::endl;
      throw larbys(ss.str());
    }
    
  }
  
  Image2D
  LArbysImageMaker::ConstructCosmicImage(const EventPixel2D* ev_pixel2d,
					 const Image2D& adc_image,
					 const size_t plane,
					 float value) {
    
    
    auto const& pixel2d_m = ev_pixel2d->Pixel2DClusterArray();
    auto img_idx=plane;
    auto const& meta = adc_image.meta();
    Image2D mu_image(meta);
    mu_image.paint(0);
    auto itr = pixel2d_m.find(img_idx);
    if(itr == pixel2d_m.end()) {
      LARCV_DEBUG() << "No Pixel2D found for plane " << img_idx << std::endl;
      return mu_image;
    }
    size_t npx_x = meta.cols();
    size_t npx_y = meta.rows();
    for(auto const& pixel_cluster : (*itr).second) {
      for(auto const& pixel : pixel_cluster) {
	if(pixel.X() >= npx_x || pixel.Y() >= npx_y) {
	  LARCV_WARNING() << "Ignoring cosmic pixel (row,col) = ("
			  << pixel.Y() << "," << pixel.X() << ")"
			  << " as it is out of bounds (ncol=" << npx_x << ", nrow=" << npx_y << ")" << std::endl;
	  continue;
	}
	mu_image.set_pixel( (pixel.X() * meta.rows() + pixel.Y()), value );
      }
    }

    return mu_image;
  }

  Image2D
  LArbysImageMaker::ConstructCosmicImage(const EventPixel2D& ev_pixel2d,
					 const Image2D& adc_image,
					 const size_t plane,
					 float value) {
    return ConstructCosmicImage(&ev_pixel2d,adc_image,plane,value);
  }
  
}
#endif
