#ifndef __NNCOSINE_CXX__
#define __NNCOSINE_CXX__

#include "NNCosine.h"
#include "ANN/ANNAlgo.h"
#include "DataFormat/EventImage2D.h"
#include "DataFormat/ImageMeta.h"
#include <vector>
#include <string>
#include <cmath>
#include <iostream>
#include <assert.h>

namespace larcv {

  static NNCosineProcessFactory __global_NNCosineProcessFactory__;

  NNCosine::NNCosine(const std::string name)
    : ProcessBase(name)
  {}
    
  void NNCosine::configure(const PSet& cfg)
  {
    finput_producer  = cfg.get<std::string>("InputProducer");
    foutput_producer = cfg.get<std::string>("OutputProducer","nncosine");
    fthresholds      = cfg.get<std::vector<float> >("PlaneThresholds");
    fminhits         = cfg.get<std::vector<int> >("PlaneMinHits");
    fradii           = cfg.get<std::vector<float> >("PlaneRadii");
    fmakehitmarker   = cfg.get<bool>("MakeHitMarkerImage",true);
    fhitmarkerpro    = cfg.get<std::string>("HitMarkerProducer","nncosine_hitmarker");
  }

  void NNCosine::initialize()
  {}

  bool NNCosine::process(IOManager& mgr)
  {

    larcv::EventImage2D* event_imgs = (larcv::EventImage2D*)mgr.get_data( larcv::kProductImage2D, finput_producer );
    larcv::EventImage2D* event_out  = (larcv::EventImage2D*)mgr.get_data( larcv::kProductImage2D, foutput_producer );
    larcv::EventImage2D* event_hit  = (larcv::EventImage2D*)mgr.get_data( larcv::kProductImage2D, fhitmarkerpro );


    // y-plane only for now
    std::vector< larcv::Image2D > angleimg_v;
    std::vector< larcv::Image2D > hitimg_v;

    for ( auto const& img : event_imgs->Image2DArray() ) {
      larcv::ImageMeta const& meta = img.meta();
      int plane = (int)meta.plane();

      // find hit pixels
      std::vector<std::vector<double> > hitpixels;
      for (int r=0; r<meta.rows(); r++) {
	for (int c=0; c<meta.cols(); c++) {
	  if ( img.pixel( r, c )>fthresholds.at(plane) ) {
	    std::vector<double> hit(2,0.0);
	    hit.at(0) = c;
	    hit.at(1) = r;
	    hitpixels.emplace_back( hit );
	  }
	}
      }
      
      // setup kdtree
      ann::ANNAlgo algo( hitpixels.size(), 2 );
      for (int i=0; i<(int)hitpixels.size(); i++) {
	algo.setPoint( i, hitpixels.at(i) );
      }
      algo.initialize();

      // make output images
      larcv::Image2D angle_img(meta);
      larcv::Image2D hit_px(meta);
      angle_img.paint(0.0);
      hit_px.paint(0.0);

      // perform NN cosine calculations
      for (int i=0; i<(int)hitpixels.size(); i++) {
	std::vector<int> nn = algo.regionQuery( i, fradii.at(plane), 0.0 );
	
	double ave_ang = 0.0;
	double maxnorm = 0.0;
	double totw = 0.0;
	if ( (int)nn.size()<fminhits.at(plane) )
	  continue;
	double origin[2] = { hitpixels.at(i).at(0), hitpixels.at(i).at(1) };
	hit_px.set_pixel( origin[1], origin[0], 1.0 ); // mark hits
	// 	std::cout << "-----------------"<< std::endl;
	// 	std::cout << "[hit " << i << "] " << "px=(" << origin[0] << "," << origin[1] << ") "
	// 		  << " pos=(" << meta.pos_x( origin[0] ) << ", " << meta.pos_y( origin[1] ) << ") " << std::endl;
	
	for (int inn=0; inn<(int)nn.size(); inn++) {
	  int hitidx = nn.at(inn);
	  if ( hitidx==i ) continue; // do not include self
	  double pos[2] = { hitpixels.at(hitidx).at(0), hitpixels.at(hitidx).at(1) };
	  // make direction vector
	  double diffnn[2] = { pos[0]-origin[0], pos[1]-origin[1] };
	  double normdiff = 0.;
	  for (int v=0;v<2; v++) normdiff += diffnn[v]*diffnn[v];
	  normdiff = sqrt(normdiff);
	  for (int v=0; v<2; v++) diffnn[v] /= normdiff; 
	  if ( normdiff>maxnorm ) maxnorm = normdiff;
	  
	  // for debug
	  // 	  double hitx = meta.pos_x( pos[0] );
	  // 	  double hity = meta.pos_y( pos[1] );
	  // 	  std::cout << "  nn=" << inn << " hitid=" << hitidx << ": " 
// 		    << " px=" << pos[0] << "," << pos[1]
// 		    << " pos=(" << hitx << "," << hity << ") "
// 		    << " dist=" << normdiff << ": dir=(" << diffnn[0]  << "," << diffnn[1] << ")" << std::endl;
	  
	  // adjust vector by quadrant
	  if ( diffnn[0]<0 && diffnn[1]<=0 ) {
	    diffnn[0] *=-1.0;
	    diffnn[1] *=-1.0;
	  }
	  else if ( diffnn[0]<0 && diffnn[1]>0 ) {
	    diffnn[0] *=-1.0;
	    diffnn[1] *=-1.0;
	  }
	  
	  // weight
	  double w = img.pixel( (int)(hitpixels.at(hitidx).at(1)+0.1), (int)(hitpixels.at(hitidx).at(0)+0.1) );
	  if ( w<fthresholds.at(plane) ) {
	    std::cout << "mistake" << std::endl;
	    assert(false);
	  }
	  
	  totw += fabs(w);
	  
	  double diffang = atan2( diffnn[1], diffnn[0] );
	  ave_ang += w*diffang;
	  
	}

	ave_ang /= totw;
      

	//       std::cout << i << ": nn=" << nn.size() 
	// 		<< " w=" << totw
	// 		<< " ang=" << ave_ang*180.0/3.14159
	// 		<< " maxnorm=" << maxnorm
	// 		<< std::endl;
	if ( !std::isnan(ave_ang) ) {
	  angle_img.set_pixel( (int)hitpixels.at(i).at(1), (int)hitpixels.at(i).at(0), ave_ang );
	}
      }//loop over hit pixels

      angleimg_v.emplace_back( angle_img );
      hitimg_v.emplace_back( hit_px );
    }// loop over input images
      
    event_out->Emplace( std::move(angleimg_v) );
    event_hit->Emplace( std::move(hitimg_v) );

    return true;
  }
  
  // no pass pixels into 
  
  void NNCosine::finalize()
  {}

}
#endif
