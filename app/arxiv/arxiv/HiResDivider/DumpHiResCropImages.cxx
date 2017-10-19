#ifndef __DUMPHIRESCROPIMAGES_CXX__
#define __DUMPHIRESCROPIMAGES_CXX__

#include "DumpHiResCropImages.h"
#include "DataFormat/EventImage2D.h"
#include "TFile.h"
#include "TTree.h"

#ifdef USE_OPENCV
#include "opencv/cv.h"
#include "opencv2/opencv.hpp"
#endif

namespace larcv {

  static DumpHiResCropImagesProcessFactory __global_DumpHiResCropImagesProcessFactory__;

  DumpHiResCropImages::DumpHiResCropImages(const std::string name)
    : ProcessBase(name)
  {}
    
  void DumpHiResCropImages::configure(const PSet& cfg)
  {
    fTPCImageProducer     = cfg.get<std::string>( "TPCProducer","" );
    fSegmentationProducer = cfg.get<std::string>( "SegmentationProducer", "" );
    fPMTWeightedProducer  = cfg.get<std::string>( "PMTWeightedProducer", "" );
    fPMTRawProducer       = cfg.get<std::string>( "PMTProducer", "" );
  }

  void DumpHiResCropImages::initialize()
  {}

  bool DumpHiResCropImages::process(IOManager& mgr)
  {

    bool hasmc = false;

#ifndef USE_OPENCV
    return false;
#else
    if ( fTPCImageProducer!="" )
      DumpBGRImage( mgr, fTPCImageProducer, "tpc", false, true );

    if ( fSegmentationProducer!="" )
      DumpBGRImage( mgr, fSegmentationProducer, "segment", false, false );
    
    if ( fPMTWeightedProducer!="" )
      DumpBGRImage( mgr, fPMTWeightedProducer, "pmtweighted", false, true );

    if ( fPMTRawProducer!="" ) {
      //needs custom routine
      
      auto input_pmtraw_images = (larcv::EventImage2D*)(mgr.get_data(kProductImage2D,fPMTRawProducer));
      cv::Mat pmtimg;
      larcv::Image2D const& pmtsrc = input_pmtraw_images->at(0);
      pmtimg = cv::Mat::zeros( pmtsrc.meta().rows(), pmtsrc.meta().cols(), CV_8UC3 );
      for (size_t r=0; r<pmtsrc.meta().rows(); r++) {
	for (size_t c=0; c<pmtsrc.meta().cols(); c++) {
	  float adc = pmtsrc.pixel( r, c ) - 2047.0;
	  int val = std::min( 255, (int)(adc) );
	  //std::cout << "(" << r << "," << c << ") " << pmtsrc.pixel( r, c ) << std::endl;
	  val = std::max( 0, val );
	  for (size_t i=0; i<3; i++) {
	    pmtimg.at< cv::Vec3b >(r,c)[i] = val;
	  }
	  if ( r==190 || r==310 )
	    pmtimg.at< cv::Vec3b >(r,c)[2] = 100;
	}
      }
      char testname[200];
      
      if (hasmc )
	sprintf( testname, "test_%s_pmtraw_%zu_mc.png", fPMTRawProducer.c_str(), input_pmtraw_images->event() );
      else
	sprintf( testname, "test_%s_pmtraw_%zu.png", fPMTRawProducer.c_str(), input_pmtraw_images->event() );
      
      cv::imwrite( testname, pmtimg );
      return true;
    }
#endif
    return true;
  }

  void DumpHiResCropImages::finalize()
  {}

  void DumpHiResCropImages::DumpBGRImage( IOManager& mgr, std::string producer, std::string imglabel,  bool ismc, bool augment ) {
    
#ifdef USE_OPENCV
    
    auto input_images = (larcv::EventImage2D*)(mgr.get_data(kProductImage2D,producer));
    
    cv::Mat img;
    for (size_t p=0; p<3; p++) {
      larcv::Image2D const& src = input_images->at(p);
      
      if ( p==0 )
	img = cv::Mat::zeros( src.meta().rows(), src.meta().cols(), CV_8UC3 );
      
      for (size_t r=0; r<src.meta().rows(); r++) {
	for (size_t c=0; c<src.meta().cols(); c++) {
	  unsigned short pix = src.pixel( r, c );
	  int val = std::min( 255, (int)(pix) );
	  if ( augment )
	    val = std::min( 255, (int)(val+0.4)*10 );
	  val = std::max( 0, val );
	  img.at< cv::Vec3b >(r,c)[p] = (unsigned int)val;
	}
      }
    }
    
    char testname[200];
    
    if (ismc )
      sprintf( testname, "test_%s_%s_%zu_mc.png", producer.c_str(), imglabel.c_str(), input_images->event() );
    else
      sprintf( testname, "test_%s_%s_%zu.png", producer.c_str(), imglabel.c_str(), input_images->event() );
    
    cv::imwrite( testname, img );
    
#endif
  }

}
#endif
