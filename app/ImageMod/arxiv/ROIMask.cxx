#ifndef __ROIMASK_CXX__
#define __ROIMASK_CXX__

#include "ROIMask.h"
#include "DataFormat/EventImage2D.h"
#include "DataFormat/EventROI.h"

namespace larcv {

  static ROIMaskProcessFactory __global_ROIMaskProcessFactory__;

  ROIMask::ROIMask(const std::string name)
    : ProcessBase(name)
  {}
    
  void ROIMask::configure(const PSet& cfg)
  {
    fInputImageProducer  = cfg.get<std::string>( "InputImageProducer" );
    fOutputImageProducer = cfg.get<std::string>( "OutputImageProducer" );
    fInputROIProducer    = cfg.get<std::string>( "InputROIProducer" );
    fOutputROIProducer   = cfg.get<std::string>( "OutputROIProducer" );
    fMaskOutsideROI      = cfg.get<bool>( "MaskOutsideROI" );
    fCutOutOfBounds      = cfg.get<bool>( "CutOutOfBounds", true );
    fROIid               = cfg.get<int>("ROIid",0);
    fROILabel            = (larcv::ROIType_t)cfg.get<int>("ROILabel", 2 ); // larcv::kROIBNB

    if (fInputImageProducer==fOutputImageProducer) finplace = true;
    else finplace = false;
      
  }

  void ROIMask::initialize()
  {}

  bool ROIMask::process(IOManager& mgr)
  {

    // Get images (and setup output)
    EventImage2D* event_original = (EventImage2D*)mgr.get_data(kProductImage2D, fInputImageProducer);
    EventImage2D* event_masked   = NULL;
    if (!finplace)
      event_masked  = (EventImage2D*)mgr.get_data(kProductImage2D, fOutputImageProducer);
    else
      event_masked  = event_original;

    if ( !event_original ) {
      LARCV_ERROR() << "Could not open input images." << std::endl;
      return false;
    }
    if ( !event_masked )  {
      LARCV_ERROR() << "Could not open output image holder." << std::endl;
      return false;      
    }

    // Get ROI
    EventROI* event_roi = (EventROI*)mgr.get_data(kProductROI, fInputROIProducer);
    auto rois = event_roi->ROIArray();
    EventROI* output_rois = (EventROI*)mgr.get_data(kProductROI, fOutputROIProducer);

    // original moves ownership of its image vectors to us
    std::vector< larcv::Image2D> image_v;
    event_original->Move(image_v ); 
 
    // container for new images if we are not working in place
    std::vector< larcv::Image2D > masked_image_v; 

    std::vector<float> _buffer;
    bool outofbounds = false;

    larcv::ROI output_roi( fROILabel );

    for ( size_t i=0; i<image_v.size(); ++i ) {

      // get access to the data
      larcv::Image2D& original_image = image_v[i];

      auto meta = original_image.meta();
      if ( rois.size()<=0 ) {
	outofbounds = true;
	break;
      }

      auto roi = rois.at(fROIid); // fixme: smarter choice of ROI?
      auto bb  = roi.BB( meta.plane() );

      // make roi meta with same scale as input image
      double origin_x = bb.min_x();
      double origin_y = bb.max_y();
      double width    = bb.width();
      double height   = bb.height();

      // get row and col size based on scale of original image
      int cols = (int)width/meta.pixel_width();
      int rows = (int)height/meta.pixel_height();
      
      if ( rows==0 || height<meta.pixel_height() ) {
	// 0 thick roi...
	height=meta.pixel_height();
	rows = 1;
      }
      if ( cols==0 || width<meta.pixel_width() ) {
	width = meta.pixel_width();
	cols = 1;
      }

      ImageMeta maskedmeta( width, height, rows, cols, origin_x, origin_y, meta.plane() );

      LARCV_DEBUG() << "original meta: " << std::endl;
      LARCV_DEBUG() << "  " << meta.dump() << std::endl;
      LARCV_DEBUG() << "roi meta: " << std::endl;
      LARCV_DEBUG() << "  " << bb.dump() << std::endl;


      // we need to check that the roi is within image range
      if ( bb.min_x()<meta.min_x() || bb.max_x()>meta.max_x() 
	   || bb.min_y()<meta.min_y() || bb.max_y()>meta.max_y() ) {
	LARCV_INFO() << "ROI extends out of bounds" << std::endl;
	outofbounds = true;
	break;
      }

      if ( fMaskOutsideROI ) {
	// we want everything but the ROI to be zero

	// we crop out the region we're interested in
	larcv::Image2D cropped_image = original_image.crop( maskedmeta );
      
	// define the new image, and blank it out
	larcv::Image2D new_image( meta );
	new_image.paint( 0.0 );
	// overlay cropped image
	new_image.overlay( cropped_image );
	masked_image_v.emplace_back( std::move(new_image) );

      }
      else {
	// we want to zero the region inside the ROI

	// make blank with size of ROi
	larcv::Image2D roi_blank( maskedmeta );
	roi_blank.paint(0.0);

	// make copy of original
	larcv::Image2D new_image( meta, original_image.as_vector() );
	new_image.overlay( roi_blank );
	masked_image_v.emplace_back( std::move(new_image) );
      }
      
      // store the roi (maskedmeta)
      output_roi.AppendBB( maskedmeta );
      
    }

    if (outofbounds) {
      // out bound ROI found. skip or make blank
      if ( fCutOutOfBounds )
	return false;
      // make blanks
      masked_image_v.clear();
      for ( size_t i=0; i<image_v.size(); ++i ) {      
	larcv::Image2D& original_image = image_v[i];
	auto meta = original_image.meta();
	larcv::Image2D new_image( meta );
        new_image.paint( 0.0 );
        masked_image_v.emplace_back( std::move(new_image) );
      }
    }

    // store
    if ( finplace ) {
      // give the new images
      event_original->Emplace( std::move(masked_image_v) );
    }
    else {
      // return the original images
      event_original->Emplace( std::move( image_v ) );
      // we give the new image2d container our relabeled images
      event_masked->Emplace( std::move(masked_image_v) );
    }
    output_rois->Emplace( std::move(output_roi) );

    return true;

  }

  void ROIMask::finalize()
  {}

}
#endif
