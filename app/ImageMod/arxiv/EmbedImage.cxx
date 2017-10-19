#ifndef __EMBEDIMAGE_CXX__
#define __EMBEDIMAGE_CXX__

#include "EmbedImage.h"
#include "DataFormat/EventImage2D.h"

namespace larcv {

  static EmbedImageProcessFactory __global_EmbedImageProcessFactory__;

  EmbedImage::EmbedImage(const std::string name)
    : ProcessBase(name)
  {}
    
  void EmbedImage::configure(const PSet& cfg)
  {
    fInputProducer  = cfg.get<std::string>("InputProducerName");  // input producer
    fOutputProducer = cfg.get<std::string>("OutputProducerName"); // input producer    
    fOutputRows     = cfg.get<int>("OutputRows");
    fOutputCols     = cfg.get<int>("OutputCols");
    finplace = ( fInputProducer==fOutputProducer ) ? true : false;
  }

  void EmbedImage::initialize()
  {}

  bool EmbedImage::process(IOManager& mgr)
  {

    EventImage2D* event_original = (EventImage2D*)mgr.get_data(kProductImage2D, fInputProducer);
    EventImage2D* event_resize   = NULL;
    if (!finplace)
      event_resize  = (EventImage2D*)mgr.get_data(kProductImage2D, fOutputProducer);
    else
      event_resize  = event_original;

    if ( !event_original ) {
      LARCV_ERROR() << "Could not open input images." << std::endl;
      return false;
    }
    if ( !event_resize )  {
      LARCV_ERROR() << "Could not open output image holder." << std::endl;
      return false;      
    }

    std::vector< larcv::Image2D> image_v;
    event_original->Move(image_v ); // original moves ownership of its image vectors to us
 
    std::vector< larcv::Image2D > resize_image_v; // space for new images if we are not working in place


    std::vector<float> _buffer;
    for ( size_t i=0; i<image_v.size(); ++i ) {

      // get access to the data
      larcv::Image2D& original_image = image_v[i];
      
      // get the image size
      int orig_rows = original_image.meta().rows();
      int orig_cols = original_image.meta().cols();
      
      if ( orig_rows>fOutputRows ) {
	LARCV_ERROR() << "Image is taller than Embedding image (" << orig_rows << ">" << fOutputRows << ")" << std::endl;
	return false;
      }
      if ( orig_cols>fOutputCols ) {
	LARCV_ERROR() << "Image is wider than Embedding image (" << orig_cols << ">" << fOutputCols <<")" << std::endl;
	return false;
      }

      // get the origin
      float topleft_x = original_image.meta().min_x();
      float topleft_y = original_image.meta().max_y();

      // get width
      float width_per_pixel  = original_image.meta().pixel_width();
      float height_per_pixel = original_image.meta().pixel_height();
      
      // get new width, keeping same pixel scales
      float new_width  = float(fOutputCols)*width_per_pixel;
      float new_height = float(fOutputRows)*height_per_pixel;

      // new origin
      int offset_row = 0.5*(fOutputRows-orig_rows);
      int offset_col = 0.5*(fOutputCols-orig_cols);
      float new_topleft_x = topleft_x - offset_row*width_per_pixel;
      float new_topleft_y = topleft_y + offset_col*height_per_pixel;
      
      // define the new meta
      larcv::ImageMeta new_meta( new_width, new_height,
				 fOutputRows, fOutputCols,
				 new_topleft_x, new_topleft_y, 
				 original_image.meta().plane() );

      // define the new image
      larcv::Image2D new_image( new_meta );
      new_image.paint( 0.0 );
      new_image.overlay( original_image );
      
      resize_image_v.emplace_back( std::move(new_image) );
    }

    // store
    if ( finplace ) {
      // give the new images
      event_original->Emplace( std::move(resize_image_v) );
    }
    else {
      // return the original images
      event_original->Emplace( std::move( image_v ) );
      // we give the new image2d container our relabeled images
      event_resize->Emplace( std::move(resize_image_v) );
    }
    
    return true;
    
  }

  void EmbedImage::finalize()
  {}

}
#endif
