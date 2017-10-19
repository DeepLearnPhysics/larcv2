#include "PMTWireWeights.h"
#include <iostream>
#include <cmath>

#include "UBWireTool/UBWireTool.h"

namespace larcv {
  namespace pmtweights {
    
    PMTWireWeights::PMTWireWeights( std::string geofile, int wire_rows ) {
      fNWires = wire_rows;

      //fGeoInfoFile = "configfiles/geoinfo.root";
      fGeoInfoFile = geofile;
      //std::cout << "Filling Weights using " << fGeoInfoFile << std::endl;
      fGeoFile = new TFile( Form("%s/app/PMTWeights/dat/%s",getenv("LARCV_BASEDIR"),fGeoInfoFile.c_str()), "OPEN" );

      // Get the PMT Info
      fNPMTs = 32;
      fPMTTree  = (TTree*)fGeoFile->Get( "imagedivider/pmtInfo" );
      int femch;
      float pos[3];
      fPMTTree->SetBranchAddress( "femch", &femch );
      fPMTTree->SetBranchAddress( "pos", pos );
      for (int n=0; n<fNPMTs; n++) {
	fPMTTree->GetEntry(n);
	//std::cout << "[pmt " << femch << "] ";
	for (int i=0; i<3; i++) {
	  pmtpos[femch][i] = pos[i];
	  //std::cout << pmtpos[femch][i] << " ";
	}
	//std::cout << std::endl;
      }

      // Configure
      configure();

      fGeoFile->Close();
    }
    
    PMTWireWeights::~PMTWireWeights() {
    }
    
    void PMTWireWeights::configure() {
      // here build the set of weights
      // each PMT gets assigned a weight that is: w = d/(D)
      // d is the shortest distance between the wire and pmt center
      // D is the sum of all such distances such that sum(w,NPMTS=32) = 1.0
      // we make a weight matrix W = [ N, M ] where N is the number of wires and M is the number of PMTs
      // the way we will use this is to assign each PMT a value: z = q/Q so that z is the fraction of charge seen in the trigger window
      // the weight assigned to each wire will be W*Z, where Z is the vector of z values for all M PMTs.
      
      for (int plane=0; plane<3; plane++) {
	const larcv::WireData& data = larcv::UBWireTool::getWireData(plane);  
	int nwires = data.nwires();
	if ( fNWires>0 ) 
	  nwires = fNWires;

	//cv::Mat mat( nwires, fNPMTs, CV_32F );
	larcv::Image2D mat( nwires, fNPMTs );
	//larcv::Image2D mat( fNPMTs, nwires );

	int iwires = 0;
	for ( std::set< int >::iterator it_wire=data.wireIDs.begin(); it_wire!=data.wireIDs.end(); it_wire++ ) {
	  int wireid = (*it_wire);
	  // we first need to project the data into 2D: z,y -> (x,y)
	  std::vector< float > const& start = (*(data.wireStart.find(wireid))).second;
	  std::vector< float > const& end   = (*(data.wireEnd.find(wireid))).second;
	  float s2[2] = { start.at(2), start.at(1) };
	  float e2[2] = {   end.at(2),   end.at(1) };
	  float l2 = (*(data.wireL2.find(wireid))).second;

	  std::vector<float> dists(fNPMTs,0.0);
	  float denom = 0;
	  //if ( wireid==1000 )
	  //std::cout << "[plane " << plane << ", wire " << wireid << "] ";
	  //std::cout << "start=(" << s2[0] << "," << s2[1] << ") ";
	  //std::cout << "end=(" << e2[0] << "," << e2[1] << ")" ;
	  for (int ipmt=0; ipmt<fNPMTs; ipmt++) {
	    float p2[2] = { pmtpos[ipmt][2], pmtpos[ipmt][1] };
	    
	    float d = getDistance2D( s2, e2, p2, l2 );
// 	    if (d!=d)
// 	      std::cout << " err w/ pmt=" << ipmt << ": (" << p2[0] << ", " << p2[1] << ")" << " " << d << std::endl;  
// 	    if ( wireid==958 )
// 	      std::cout << "pmt pos: " << ipmt << ": (" << p2[0] << ", " << p2[1] << ") d=" << d << std::endl;

	    dists[ipmt] = d;
	    if ( d!=d || std::isinf(d) || std::isnan(d) || d<=0.0 ) {
	      std::cout << " nan: [plane " << plane << ", wire " << wireid << "] pmt=" << ipmt << " d=" << d << std::endl;
	      std::cout << "start=(" << s2[0] << "," << s2[1] << ") "; 
	      std::cout << "end=(" << e2[0] << "," << e2[1] << ")" ; 
	      std::cout << "pmt pos: " << ipmt << ": (" << p2[0] << ", " << p2[1] << ") d=" << d << std::endl;
	    }
	    //std::cout << (1.0/d) << " ";
	    if ( (1.0/d)>denom )
	      denom = (1.0/d);

	  }
	  //if ( wireid==1000 )
	  //std::cout << std::endl;
	  
	  // populate the matrix
// 	  if ( wireid==1000)
// 	    std::cout << "[plane " << plane << ", wire " << wireid << "] ";
	  for (int ipmt=0; ipmt<fNPMTs; ipmt++) {
	    //mat.at<float>( wireid, ipmt ) = dists.at(ipmt)/denom;
	    //mat.set_pixel( ipmt, wireid, (1.0/dists.at(ipmt))/denom );
	    if ( denom>0 )
	      mat.set_pixel( wireid, ipmt, (1.0/dists.at(ipmt))/denom );
	    else
	      mat.set_pixel( wireid, ipmt, 0.0 );
// 	  if ( wireid==1000 )
// 	    std::cout << (1.0/dists.at(ipmt))/denom << std::endl;
 	  }
// 	  if ( wireid==1000 )
// 	    std::cout << std::endl;
	  iwires++;
	}//end of wire loop
	planeWeights[plane] = mat;
      }//end of plane loop
    }//end of configure()
    
    float PMTWireWeights::getDistance2D( float s[], float e[], float p[], float l2 ) {
      // we assume that the user has projected the information into 2D
      // we calculate the projection of p (the pmt pos) onto the line segment formed by (e-s), the end point of the wires
      // where s is the origin of the coorindate system

      // since we want distance from wire to pmt, perpendicular to the wire, we can form
      // a right triangle. the distance is l = sqrt(P^2 - a^2), where a is the projection vector

      float ps[2]; // vector from wire start to PMT pos
      float es[2]; // vector from wire start to wire end
      float dot = 0.0;  // dot product of the above
      float psnorm = 0.0; // distane from wire start to pmt pos
      float esnorm = 0.0;
      for (int i=0; i<2; i++) {
	ps[i] = p[i]-s[i];
	es[i] = e[i]-s[i];
	dot += ps[i]*es[i];
	esnorm += es[i]*es[i];
	psnorm += ps[i]*ps[i];
      }
      esnorm = sqrt(esnorm);
      //std::cout << "esnorm=" << esnorm << std::endl;
      if ( esnorm<=0 )
	return 0.1;

      float dist_on_wire = fabs(dot/esnorm);
      float dist = 0.1;
      if ( dist_on_wire<sqrt(psnorm ) ) {
	dist = psnorm - dist_on_wire*dist_on_wire;
      }
      if ( fabs(dist)<=0.1 ) {
	//std::cout << "dist<0: " << dist << " ps=" << psnorm << " dist_on_wire=" << dist_on_wire << " esnorm=" << esnorm << std::endl;
	return 0.1;
      }
      dist = sqrt( dist );

      return dist;
    }

  }
}
