#ifndef __PMTWEIGHTIMAGEBUILDER_CXX__
#define __PMTWEIGHTIMAGEBUILDER_CXX__

#include "PMTWeightImageBuilder.h"
#include "DataFormat/Image2D.h"
#include "DataFormat/EventImage2D.h"

namespace larcv {
  namespace pmtweights {

    static PMTWeightImageBuilderProcessFactory __global_PMTWeightImageBuilderProcessFactory__;
    

    PMTWeightImageBuilder::~PMTWeightImageBuilder() {
      delete m_WireWeights;
      m_WireWeights = NULL;
    }

    PMTWeightImageBuilder::PMTWeightImageBuilder(const std::string name)
      : ProcessBase(name)
    {}
    
    void PMTWeightImageBuilder::configure(const PSet& cfg)
    {
      fGeoFile     = cfg.get<std::string> ( "GeoFile" );
      fNWirePixels = cfg.get<int>( "NWirePixels" );
      fPMTImageProducer = cfg.get<std::string>( "PMTImageProducer" );
      fTPCImageProducer = cfg.get<std::string>( "TPCImageProducer" );
      fStartTick = cfg.get<int>( "StartTick" );
      fEndTick = cfg.get<int>( "EndTick" );
      fCheckSat = cfg.get<bool>("CheckSaturation");
      fPMTImageIndex = cfg.get<int>("PMTImageIndex");
      fHGpedestal = cfg.get<float>("HGpedestal",2047.0);
      fOutputProducerName = cfg.get<std::string>( "OutputProducerName" );
      fSaveWeightMask = cfg.get<bool>("SaveWeightMask",false);
      fWeightMaskProducerName = cfg.get<std::string>("WeightMaskProducer","weightmask");
      fDebugSaveDistWeightMatrix = cfg.get<bool>("DebugSaveDistWeightMatrix", false );
    }
    
    void PMTWeightImageBuilder::initialize()
    {
      m_WireWeights = new PMTWireWeights( fGeoFile, fNWirePixels );
      fResizedMat = false;
    }

    
    bool PMTWeightImageBuilder::process(IOManager& mgr)
    {

      auto pmt_event_image = (EventImage2D*)(mgr.get_data(kProductImage2D,fPMTImageProducer));
      if(!pmt_event_image || pmt_event_image->Image2DArray().empty()) return false;      

      auto tpc_event_image = (EventImage2D*)(mgr.get_data(kProductImage2D,fTPCImageProducer));
      if(!tpc_event_image || tpc_event_image->Image2DArray().empty()) return false;      


      // -------------------
      // Pick out PMT Image
      // -------------------
      larcv::Image2D const& pmtimg_highgain = pmt_event_image->at( fPMTImageIndex ); // placeholder code
      //larcv::Image2D const& pmtimg_lowgain  = event_image->at( 4 ); // placeholder code
//       std::cout << "pmt image: " << pmtimg_highgain.meta().rows() << " x " << pmtimg_highgain.meta().cols() << std::endl;
//       std::cout << "tpc image: " << tpc_event_image->at(2).meta().rows() << " x " << tpc_event_image->at(2).meta().cols() << std::endl;
//       std::cout << "summing between: " << fStartTick << " and " << fEndTick << std::endl;
      // sum pmt charge in the trigger window

      larcv::Image2D pmtq( 32, 1 );
      float totalq = 0.0;
      for (int ipmt=0; ipmt<32; ipmt++) {
	float high_q = 0.0;
	float low_q  = 0.0;
	bool usehigh = true;
	for (int t=fStartTick; t<=fEndTick; t++) {
	  // sum over the trigger window
	  float hq = pmtimg_highgain.pixel( t, ipmt )-fHGpedestal; 
	  high_q += hq;
	  if ( fCheckSat && hq>1040 )
	    usehigh = false;
	}

	if ( high_q <0.0 )
	  high_q  = 0.0;
	if ( low_q < 0.0 )
	  low_q = 0.0;

	if ( high_q > 0.0 ) {
	  if ( usehigh ) {
	    pmtq.set_pixel( ipmt, 0, high_q/100.0 );
	    totalq += high_q/100.0;
	  }
	  else {
	    pmtq.set_pixel( ipmt, 0, 10.0*low_q/100.0 );
	    totalq += 10.0*low_q/100.0;
	  }
	}
      }//end of pmt loop
    
      // normalize charge
      if ( totalq > 0 ) {
	for (size_t ipmt=0; ipmt<32; ipmt++) {
	  pmtq.set_pixel( ipmt, 0, pmtq.pixel( ipmt, 0 )/totalq );
	  //std::cout << "PMT " << ipmt << ": " << pmtq.pixel( ipmt, 0 )/totalq << std::endl;
	}
      }
      LARCV_DEBUG() << "Total Q: " << totalq << std::endl;


      // need to resize weight matrices to have right number of wire pixels: wait until we get the first image so we can resize
      if ( !fResizedMat ) {
	resizedWeightMat.clear();
	for (int p=0; p<3; p++) {
	  larcv::Image2D const& tpcimg  = tpc_event_image->at( p );
	  larcv::Image2D weights( m_WireWeights->planeWeights[p] );
	  int factor = weights.meta().rows()/tpcimg.meta().cols(); // cols is the wire dimension
	  if ( weights.meta().rows()%tpcimg.meta().cols()!=0 )
	    factor++;
	  weights.resize( tpcimg.meta().cols()*factor, weights.meta().cols(), 0.0 );
	  weights.compress( tpcimg.meta().cols(), weights.meta().cols(), larcv::Image2D::kAverage );
// 	  std::cout << "(" << weights.meta().rows() << "," << weights.meta().cols() << ") "
// 		    << "(" << tpcimg.meta().rows()  << "," << tpcimg.meta().cols() << ")" << std::endl;
	  resizedWeightMat[p] = weights;
	  // check it:
	  for (int r=0; r<weights.meta().rows(); r++) {
	    for (int c=0; c<weights.meta().cols(); c++) {
	      if ( std::isnan(weights.pixel( r, c )) || weights.pixel( r, c )<0 || weights.pixel( r, c )>1.0 ) {
		std::cout << "weird weight value in (" << p << ", " << r << ", " << c << ")=" << weights.pixel( r, c ) << std::endl;
	      }
	    }
	  }
	}
	fResizedMat = true;
      }

      // make weight matrices
      std::vector<larcv::Image2D> pmtw_image_array;
      for (int p=0; p<3; p++) {
	larcv::Image2D const& planeweight = resizedWeightMat[p];
	larcv::Image2D weights = planeweight*pmtq;
	pmtw_image_array.emplace_back( weights );
// 	std::cout << "planeweights: (" << planeweight.meta().rows() << "," << planeweight.meta().cols() << ") "
// 		  << "pmt-weights: (" << weights.meta().rows()  << "," << weights.meta().cols() << ")" << std::endl;
      }

      // now apply weights
      std::vector<larcv::Image2D> weighted_images;
      std::vector<larcv::Image2D> mask_images;
      for (int p=0; p<3; p++) {
	larcv::Image2D const& tpcimg  = tpc_event_image->at( p );
	larcv::Image2D const& weights = pmtw_image_array.at( p );
	larcv::Image2D weighted( tpcimg );
	larcv::Image2D mask( tpcimg );
//  	std::cout << "tpc: (" << tpcimg.meta().rows() << "," << tpcimg.meta().cols() << ") "
// 		  << "weights: (" << resizedWeightMat[p].meta().rows()  << "," << resizedWeightMat[p].meta().cols() << ")" << std::endl;
	for (size_t c=0; c<weighted.meta().cols(); c++) {
	  float w = weights.pixel(c,0);
// 	  if ( w<0 ) w = 0.0;
// 	  if ( w>1.0 ) w = 1.0;	    
	  for (size_t r=0; r<weighted.meta().rows(); r++) {
	    weighted.set_pixel(r,c, w*tpcimg.pixel(r,c));
	    mask.set_pixel( r, c, w );
	  }
	}
	weighted_images.emplace_back( weighted );
	if (fSaveWeightMask) {
	  mask_images.emplace_back( mask );
	}
      }
      
      LARCV_DEBUG() << "Made " << weighted_images.size() << " PMT Weighted Images." << std::endl;
      
      auto out_image_v = (::larcv::EventImage2D*)(mgr.get_data(::larcv::kProductImage2D,fOutputProducerName));
      ::larcv::EventImage2D* out_mask_v = NULL;
      out_image_v->Emplace(std::move(weighted_images));
      if ( fSaveWeightMask ) {
	out_mask_v = (::larcv::EventImage2D*)(mgr.get_data(::larcv::kProductImage2D,fWeightMaskProducerName));
	out_mask_v->Emplace(std::move(mask_images));
      }

      if ( fDebugSaveDistWeightMatrix ){
	auto out_matrix_v = (::larcv::EventImage2D*)(mgr.get_data(::larcv::kProductImage2D,"DistWeightMatrix"));
	std::vector<larcv::Image2D> mat;
	for (int p=0; p<3; p++) {
	  mat.push_back( resizedWeightMat[p] );
	}
	out_matrix_v->Emplace(std::move(mat));
      }
      
      return true;
    }

//     larcv::Image2D& PMTWeightImageBuilder::getWeightedImage( int plane ) {
//       return m_weighted_images.at(plane);
//     }
    
    void PMTWeightImageBuilder::finalize()
    {}

  }
}
#endif
