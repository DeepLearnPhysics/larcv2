#ifndef __DBSCAN_CXX__
#define __DBSCAN_CXX__

#include "DBSCAN.h"
#include "DataFormat/DataFormatTypes.h"
#include "DataFormat/EventImage2D.h"
#include "DataFormat/Image2D.h"
#include "DataFormat/ImageMeta.h"
#include "DBSCANAlgo.h"

namespace larcv {

  static DBSCANProcessFactory __global_DBSCANProcessFactory__;

  DBSCAN::DBSCAN(const std::string name)
    : ProcessBase(name)
  {
  }
    
  void DBSCAN::configure(const PSet& cfg)
  {
    ftpc_producer      = cfg.get<std::string>("TPCImageProducer");
    fauxdata_producers = cfg.get<std::vector<std::string> >("AuxDataImageProducer");
    fauxdim_scale      = cfg.get<std::vector<float> >("AuxDimScales");

    foutput_producer   = cfg.get<std::string>("OutputImageProducer");
    feps               = cfg.get< std::vector<double> >("planeNeighborRadius");
    fminpoints         = cfg.get< std::vector<int> >("planeMinPoints");
    fthreshold         = cfg.get< std::vector<double> >("adchitthreshold");
  }

  void DBSCAN::initialize()
  {}

  bool DBSCAN::process(IOManager& mgr)
  {
    larcv::EventImage2D* input_imgs  = (larcv::EventImage2D*)mgr.get_data( larcv::kProductImage2D, ftpc_producer );
    larcv::EventImage2D* output_imgs = (larcv::EventImage2D*)mgr.get_data( larcv::kProductImage2D, foutput_producer );
    int nauxproducers = (int)fauxdata_producers.size();
    larcv::EventImage2D* aux_imgs[nauxproducers];
    for (int iaux=0; iaux<nauxproducers; iaux++) {
      aux_imgs[iaux] = (larcv::EventImage2D*)mgr.get_data( larcv::kProductImage2D, fauxdata_producers.at(iaux) );
    }

    // go through each plane.
    std::vector< larcv::Image2D > output_images;
    for ( auto const& img : input_imgs->Image2DArray() ) {

      // set image meta parameters, clustering parameters
      const larcv::ImageMeta& meta = img.meta();
      int plane     = (int)meta.plane();
      double thresh = fthreshold.at(plane);
      double eps    = feps.at(plane);
      int minpoints = fminpoints.at(plane);

      LARCV_INFO() << "clustering plane=" << plane << ": thresh=" << thresh << " radius=" << eps << " minpoints=" << minpoints << std::endl;

      // find hits.
      dbscan::dbPoints pixels;
      LARCV_DEBUG() << "making points for above threshold pixels" << std::endl;
      for (size_t c=0; c<meta.cols(); c++) {
	for (size_t r=0; r<meta.rows(); r++) {
	  if ( img.pixel( r, c )>thresh ) {
	    // hit found, we assemble out point in augmented space
	    // i.e., we combine 2D position, plus values from auxillary image data
	    std::vector<double> pt(2+nauxproducers,0.0);
	    // set the 2D position
	    pt.at(0) = c;
	    pt.at(1) = r;

	    // set the aux values
	    for (int iaux=0; iaux<nauxproducers; iaux++) {
	      const std::vector<larcv::Image2D>& aux_v = aux_imgs[iaux]->Image2DArray();
	      // find the matching plane
	      for (auto &aux_img : aux_v ) {
		if ( aux_img.meta().plane()==plane ) {
		  // found the matching plane
		  pt.at(2+iaux) = aux_img.pixel(r,c)/fauxdim_scale.at(iaux); // value scaled
		}
	      }
	    }
	    pixels.emplace_back(pt);
	  }
	}
      }

      
      LARCV_DEBUG() << "run dbscan algo" << std::endl;
      dbscan::DBSCANAlgo algo;
      dbscan::dbscanOutput clusterout = algo.scan( pixels, minpoints, eps );
      LARCV_DEBUG() << "dbscan algo finished" << std::endl;

      // make output image
      larcv::Image2D imgout( meta );
      imgout.paint(0.0);

      LARCV_INFO() << "number of clusters found in plane=" << plane << ": " << clusterout.clusters.size() << std::endl;

      // nothing = 0
      // background labeled with 1
      // rest increment cluster id i+1
      LARCV_DEBUG() << "make output cluster image" << std::endl;
      for (size_t ic=0; ic<clusterout.clusters.size(); ic++) {
	const std::vector< int >& cl = clusterout.clusters.at(ic);
	for ( auto& idx : cl ) {
	  const std::vector<double>& pix = pixels.at(idx);
	  if ( cl.size()>1 )
	    imgout.set_pixel( (int)(pix.at(1)+0.1), (int)(pix.at(0)+0.1), ic+1 );
	  else
	    imgout.set_pixel( (int)(pix.at(1)+0.1), (int)(pix.at(0)+0.1), 1 );
	}
      }
      output_images.emplace_back( imgout );
    }
    
    output_imgs->Emplace( std::move(output_images) );

    return true;
  }//end of analyze
  
  void DBSCAN::finalize()
  {}

}
#endif
