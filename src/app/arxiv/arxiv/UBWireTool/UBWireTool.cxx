#include "UBWireTool.h"
#include "TFile.h"
#include "TTree.h"

#include <sstream>
#include <exception>
#include <set>

#include "LArUtil/Geometry.h"
#include "LArUtil/LArProperties.h"

namespace larcv {

  UBWireTool* UBWireTool::_g_ubwiretool = NULL;

  UBWireTool::UBWireTool() {
    loadData();
  }

  void UBWireTool::loadData() {

    TFile fGeoFile( Form("%s/app/PMTWeights/dat/geoinfo.root",getenv("LARCV_BASEDIR")), "OPEN" );
    
    // Get the Wire Info
    TTree* fWireTree = (TTree*)fGeoFile.Get( "imagedivider/wireInfo" );
    int wireID;
    int planeID;
    float start[3];
    float end[3];
    fWireTree->SetBranchAddress( "wireID", &wireID );
    fWireTree->SetBranchAddress( "plane",  &planeID );
    fWireTree->SetBranchAddress( "start", start );
    fWireTree->SetBranchAddress( "end", end );
      
    int nentries = fWireTree->GetEntries();
    for ( int ientry=0; ientry<nentries; ientry++ ) {
      fWireTree->GetEntry(ientry);
      if ( m_WireData.find( planeID )==m_WireData.end() ) {
	// cannot find instance of wire data for plane id. make one.
	m_WireData[planeID] = larcv::WireData( planeID );
      }
      // start is always the one with the lowest y-value
      if ( start[1]>end[1] ) {
	// swap start end end
	for (int i=0; i<3; i++) {
	  float temp = start[i];
	  start[i] = end[i];
	  end[i] = temp;
	}
      }
      m_WireData[planeID].addWire( wireID, start, end );
    }
    // insert empty instance
    m_WireData[-1] = larcv::WireData( -1 );
    
    fGeoFile.Close();
    
  }

  UBWireTool* UBWireTool::_get_global_instance() {
    if ( _g_ubwiretool==NULL )
      _g_ubwiretool = new UBWireTool();
    return _g_ubwiretool;
  }

  const larcv::WireData& UBWireTool::getWireData(int p) {
    UBWireTool* _g = UBWireTool::_get_global_instance();
    auto it = _g->m_WireData.find(p);
    if ( it!=_g->m_WireData.end() ) {
      return it->second;
    }
    _g->LARCV_CRITICAL() << "Asked UBWireTool for wire data from a plane that doesn't exist (p=" << p << ")" << std::endl;
    throw larbys();
    return _g->m_WireData[-1]; // never gets here
  }


  float UBWireTool::calculateIntersectionTriangle( const std::vector<float>& p0, const std::vector<float>& p1, const std::vector<float>& p2 ) {
    // returns area of intersection triangle for 3 wires
    // inputs
    // ------
    //  p0, p1, p3: (2D coordinate for wires from each plane (0,1,2))

    float d01[2] = { p1[0]-p0[0], p1[1]-p0[1] }; // line segment from p0 -> p1
    float d20[2] = { p0[0]-p2[0], p0[1]-p2[1] }; // line segment from p2 -> p0
    float base = sqrt( d01[0]*d01[0] + d01[1]*d01[1] ); // length of d01, acting as our base
    float vp[2] = { d01[1], -d01[0] }; // direction perpendicular to d01 ( d01.vp = 0.0 )
    float height = 0.0;
    // project d20 onto vp to get distance from d01 line to p2
    for (int i=0; i<2; i++) height += d20[i]*vp[i]; 
    height /= base;
    
    return 0.5*std::fabs(height*base); // return area of triangle
  }

  void UBWireTool::lineSegmentIntersection2D( const std::vector< std::vector<float> >& ls1, const std::vector< std::vector<float> >& ls2, 
					      std::vector<float>& insec, int& crosses ) {
    // find intersection for two line segments in 2D
    // inputs
    // ------
    // ls1,ls2: line segment 1 (x,y) for start and end point, ls1[ start or end ][ x or y ]
    // 
    // outputs
    // -------
    //  insec: intersection point (x,y)
    //  crosses: does the intersection occur, and if it does, is it within the bounds of the line segments

    //std::cout << "[lineSegmentIntersection2D] begin" << std::endl;
    //std::cout << "  testing: ls1=(" << ls1[0][0] << "," << ls1[0][1] << ") -> (" << ls1[1][0] << ","<< ls1[1][1] <<")" << std::endl;
    //std::cout << "  testing: ls2=(" << ls2[0][0] << "," << ls2[0][1] << ") -> (" << ls2[1][0] << ","<< ls2[1][1] <<")" << std::endl;
    insec.resize(2,0.0);
    float Y1 = ls1[1][1] - ls1[0][1];
    float X1 = ls1[0][0] - ls1[1][0];
    float C1 = Y1*ls1[0][0] + X1*ls1[0][1];

    float Y2 = ls2[1][1] - ls2[0][1];
    float X2 = ls2[0][0] - ls2[1][0];
    float C2 = Y2*ls2[0][0] + X2*ls2[0][1];

    float det = Y1*X2 - Y2*X1;
    if ( det==0 ) { 
      // parallel
      crosses = 0;
      //std::cout << "[lineSegmentIntersection2D] end.  parallel lines" << std::endl;
      return;
    }

    insec[0] = (X2*C1 - X1*C2)/det;
    insec[1] = (Y1*C2 - Y2*C1)/det;

    // check if interesction within line segments
    // padding needed for y-wire which is vertical
    crosses = 1;
    for (int i=0; i<2; i++) {
      if ( std::min( ls1[0][i]-0.15, ls1[1][i]-0.15 ) > insec[i] || std::max( ls1[0][i]+0.15, ls1[1][i]+0.15 )<insec[i] )
	crosses = 0;
      if ( std::min( ls2[0][i]-0.15, ls2[1][i]-0.15 ) > insec[i] || std::max( ls2[0][i]+0.15, ls2[1][i]+0.15 )<insec[i] )
	crosses = 0;
      if ( crosses==0 )
	break;
    }
    
    //std::cout << "  crosses=" << crosses << ": intersection=(" << insec[0] << "," << insec[1] << ")" << std::endl;
    //std::cout << "[lineSegmentIntersection2D] end." << std::endl;
    return;
  }

  void UBWireTool::findWireIntersections( const std::vector< std::vector<int> >& wirelists, 
					  const std::vector< std::vector<float> >& valid_range,
					  std::vector< std::vector<int> >& intersections3plane,
					  std::vector< std::vector<float> >& vertex3plane,
					  std::vector<float>& areas3plane,
					  std::vector< std::vector<int> >& intersections2plane, std::vector< std::vector<float> >& vertex2plane ) {
    // takes in lists of wire id numbers and returns intersection combinrations, the area of the triangle they make (if 3D), and 2 plane intersection candidats
    // we are going to go ahead and assume 3 planes for now. we can modify for generic number of planes later (never)

    // inputs
    // ------
    //  wirelists: list of wires to test, one for each plane wirelists[plane][ test wire ]
    //  valid_range: range for each plane for which intersections are valid (in real space), valid_range[ plane ][0] or valid_range[ plane ][1]
    //  
    // outputs
    // -------
    //  intersections3plane: list of intersections. each intersection is a list of 3 wires, one from each plane, intersections3plane[intersection #][ plane ]
    //  vertex3plane: list of (z,y) vertex
    //  areas3plane: list of intersection triangle area for each 3-plane intersection
    //  intersections2plane: list of intersections for 2 planes, intersections2plane[intersection #][ plane ]
    //  vertex2plane: list of 2-plane intersection (z,y) vertices

    // we're going to loop through all combination of wires. This can get bad fast. N^p problem...
    // expecting about O(10) endpoints per flash.  worst case, looking at 1000 combinations.
    // but we use the validity ranges to start removing intersections we don't care about

    const int nplanes = wirelists.size();
    std::set< std::vector<int> > checked2plane;
    std::set< std::vector<int> > checked3plane;

    UBWireTool* _g = UBWireTool::_get_global_instance();
    
    for (int p0=0;p0<nplanes;p0++) {
      // loop through first wire plane
      for (int idx0=0; idx0<(int)wirelists.at(p0).size(); idx0++) {
      
	// get the first wire
	int wid0 = wirelists.at(p0).at(idx0);
	const std::vector<float>& start0 = _g->m_WireData[p0].wireStart.find(wid0)->second;
	const std::vector<float>& end0   = _g->m_WireData[p0].wireEnd.find(wid0)->second;
	
	// go to the other planes and check the wires there
	for (int p1=p0+1; p1<nplanes; p1++) {
	  // get wire on this plane
	  for (int idx1=0; idx1<(int)wirelists.at(p1).size(); idx1++) {
	    int wid1 = wirelists.at(p1).at(idx1);

	    std::vector< int > combo2d(3,-1);
	    combo2d[p0] = wid0;
	    combo2d[p1] = wid1;
	    if ( checked2plane.find( combo2d )==checked2plane.end() )
	      checked2plane.insert( combo2d );
	    else {
	      //std::cout << "  .. already checked: (" << combo2d[0] << "," << combo2d[1] << "," << combo2d[2] << ")" << std::endl;
	      continue;
	    }

	    const std::vector<float>& start1 = _g->m_WireData[p1].wireStart.find(wid1)->second;
	    const std::vector<float>& end1   = _g->m_WireData[p1].wireEnd.find(wid1)->second;
	    
	    // change line end points from 3D to 2D (x,y,z) -> (z,y)
	    std::vector< std::vector<float> > ls0(2); // line segment 1
	    ls0[0].push_back( start0[2] );
	    ls0[0].push_back( start0[1] );
	    ls0[1].push_back( end0[2] );
	    ls0[1].push_back( end0[1] );
	    std::vector< std::vector<float> > ls1(2); // line segment 2
	    ls1[0].push_back( start1[2] );
	    ls1[0].push_back( start1[1] );
	    ls1[1].push_back( end1[2] );
	    ls1[1].push_back( end1[1] );
	    
	    // test intersection
	    std::vector<float> intersection01; 
	    int crosses01 = 0;
	    lineSegmentIntersection2D( ls0, ls1, intersection01, crosses01 );
	    if ( !crosses01 ) {
	      //std::cout << "  (" << p0 << "," << wid0 << ") and (" << p1 << "," << wid1 << ") does not cross" << std::endl;
	      continue; // move on if doesn't cross
	    }
	    bool valid = true;
	    for (int i=0; i<2; i++) {
	      if ( intersection01[i]<valid_range[i][0] || intersection01[i]>valid_range[i][1] ) {
		valid = false;
		break;
	      }
	    }
	    if ( !valid ) {
	      //std::cout << "  (" << p0 << "," << wid0 << ") and (" << p1 << "," << wid1 << ") crosses but not valid. "
	      //	<< " intersection(z,y)=(" << intersection01[0] << "," << intersection01[1] << ")"
	      //	<< std::endl;
	      continue; // not a valid intersection
	    }

	    // we got a 2plane intersection
	    std::vector<int> p2intersect(3,-1);
	    p2intersect[p0] = wid0;
	    p2intersect[p1] = wid1;
	    intersections2plane.emplace_back( p2intersect );
	    vertex2plane.push_back( intersection01 );

	    // we try for the 3plane intersection
	    int p2 = 2;
	    if ( p0==0 && p1==1 ) p2 = 2;
	    else if ( p0==0 && p1==2 ) p2 = 1;
	    else if ( p0==1 && p1==2 ) p2 = 0;
	    else
	      continue;


	    for (int idx2=0; idx2<(int)wirelists.at(p2).size(); idx2++) {
	      int wid2 = wirelists.at(p2).at(idx2);

	      std::vector< int > combo3d(3,-1);
	      combo3d[p0] = wid0;
	      combo3d[p1] = wid1;
	      combo3d[p2] = wid2;
	      if ( checked3plane.find( combo3d )==checked3plane.end() )
		checked3plane.insert( combo3d );
	      else {
		//std::cout << "    ... already checked: (" << combo3d[0] << ", " << combo3d[1] << ", " << combo3d[2] << ")" << std::endl;
		continue;
	      }

	      const std::vector<float>& start2 = _g->m_WireData[p2].wireStart.find(wid2)->second;
	      const std::vector<float>& end2   = _g->m_WireData[p2].wireEnd.find(wid2)->second;

	      std::vector< std::vector<float> > ls2(2); // line segment 2
	      ls2[0].push_back( start2[2] );
	      ls2[0].push_back( start2[1] );
	      ls2[1].push_back( end2[2] );
	      ls2[1].push_back( end2[1] );

	      std::vector<float> intersection02;
	      int crosses02 = 0;
	      lineSegmentIntersection2D( ls0, ls2, intersection02, crosses02 );

	      std::vector<float> intersection12;
	      int crosses12 = 0;
	      lineSegmentIntersection2D( ls1, ls2, intersection12, crosses12 );
	      
	      if ( !crosses02 || !crosses12 )  {
		//std::cout << "  3-plane check: one combination does not cross, crosses02=" << crosses02 << " crosses12=" << crosses12 << std::endl;
		continue;
	      }

	      bool valid2 = true;
	      for (int i=0; i<2; i++) {
		if ( intersection02[i]<valid_range[i][0] || intersection02[i]>valid_range[i][1] ) {
		  //std::cout << "  3-plane check: intersection02 not valid" << std::endl;
		  valid = false;
		  break;
		}
		if ( intersection12[i]<valid_range[i][0] || intersection12[i]>valid_range[i][1] ) {
		  //std::cout << "  3-plane check: intersection12 not valid" << std::endl;
		  valid = false;
		  break;
		}
	      }
	      if ( !valid2 )
		continue; // not a valid intersection
	      
	      // got a 3 plane intersection!
	      std::vector<int> p3intersect(3,-1);
	      p3intersect[p0] = wid0;
	      p3intersect[p1] = wid1;
	      p3intersect[p2] = wid2;
	      intersections3plane.emplace_back( p3intersect );
	      // get score for 3 plane intersections
	      float area = calculateIntersectionTriangle( intersection01, intersection02, intersection12 );
	      areas3plane.push_back(area);
	      // get the 3 plane vertex
	      std::vector<float> vert3(2,0.0);
	      for (int i=0; i<2; i++) 
		vert3[i] = (intersection01[i]+intersection02[i]+intersection12[i] )/3.0;
	      vertex3plane.emplace_back( vert3 );
	    }//end of loop over p2 wires
	  }//end of loop over p1 wires
	}//end of loop over p1 planes
      }//end of loop over p0 wires
    }//end of loop over p0 planes
  }//end of findWireIntersections(...)
  
  void UBWireTool::wireIntersection( int plane1, int wireid1, int plane2, int wireid2, std::vector<float>&intersection, int& crosses ) {
    // maybe think about caching all these values
    UBWireTool* _g = UBWireTool::_get_global_instance();
    
    auto it_start1 = _g->m_WireData[plane1].wireStart.find(wireid1);
    auto it_end1   = _g->m_WireData[plane1].wireEnd.find(wireid1);
    auto it_start2 = _g->m_WireData[plane2].wireStart.find(wireid2);
    auto it_end2   = _g->m_WireData[plane2].wireEnd.find(wireid2);

    // check if in bounds
    if ( it_start1==_g->m_WireData[plane1].wireStart.end() 
      || it_end1  ==_g->m_WireData[plane1].wireEnd.end() 
      || it_start2==_g->m_WireData[plane2].wireStart.end() 
      || it_end2  ==_g->m_WireData[plane2].wireEnd.end() ) {
      intersection.resize(2,0);
      crosses = 0;
      return;
    }

    const std::vector<float>& start1 = it_start1->second;
    const std::vector<float>& end1   = it_end1->second;
    const std::vector<float>& start2 = it_start2->second;
    const std::vector<float>& end2   = it_end2->second;
    
    std::vector< std::vector<float> > ls1(2); 
    ls1[0].resize(2,0);
    ls1[1].resize(2,0);    
    ls1[0][0] = start1[2];
    ls1[0][1] = start1[1];
    ls1[1][0] = end1[2];
    ls1[1][1] = end1[1];
    
    std::vector< std::vector<float> > ls2(2); 
    ls2[0].push_back( start2[2] );
    ls2[0].push_back( start2[1] );
    ls2[1].push_back( end2[2] );
    ls2[1].push_back( end2[1] );
    
    crosses = 0;
    UBWireTool::lineSegmentIntersection2D( ls1, ls2, intersection, crosses );
  }

  void UBWireTool::getMissingWireAndPlane( const int plane1, const int wireid1, const int plane2, const int wireid2, 
      int& otherplane, int& otherwire, std::vector<float>& intersection, int& crosses ) {
    // maybe think about caching all these values
    UBWireTool* _g = UBWireTool::_get_global_instance();

    _g->wireIntersection( plane1, wireid1, plane2, wireid2, intersection, crosses );

    if ( crosses==0 ) {
      otherplane = -1;
      otherwire = -1;
      return;
    }

    // which is the missing plane?
    for (int p=0; p<3; p++ ) {
      if ( p!=plane1 && p!=plane2) {
        otherplane = p;
        break;
      }
    }
    if ( otherplane==-1 ) {
      throw std::runtime_error("UBWireTool::getMissingWireAndPlane. Could not determine the other plane");
    }

    Double_t xyz[3] = { 0, intersection[1], intersection[0] };
    otherwire = (int)larutil::Geometry::GetME()->WireCoordinate( xyz, otherplane );;
  }


  void UBWireTool::wireIntersection( std::vector< int > wids, std::vector<float>& intersection, double& triangle_area, int& crosses ) {
    // inputs
    // ------
    // wids: vector of wire ids for each plane (going from 0,1,2 = U,V,Y)
    // outputs
    // -------
    // intersection: 3D intersection
    // triangle_area: trianlge formed by 3 wires
    // crosses: 1 if crosses, 0 if does not
    
    UBWireTool* _g = UBWireTool::_get_global_instance();
    
    // get wire data
    typedef std::vector< std::vector<float> > LineSegEnds_t;
    LineSegEnds_t segs[3];
    for (int p=0; p<3; p++) {
      int wid = wids.at(p);
      if (wid>=_g->m_WireData[p].nwires()) wid = _g->m_WireData[p].nwires()-1;
      auto it_start = _g->m_WireData[p].wireStart.find(wid);
      auto it_end   = _g->m_WireData[p].wireEnd.find(wid);
      if ( it_start==_g->m_WireData[p].wireStart.end() || it_end==_g->m_WireData[p].wireEnd.end() ) {
	std::stringstream ss;
	ss << __FILE__ << "::" << __LINE__ << " Wire ID=" << wid << " Plane=" << p << " could not find wireStart or wireEnd" << std::endl;
	throw std::runtime_error(ss.str());
      }
      const std::vector<float>& start = _g->m_WireData[p].wireStart.find(wid)->second;
      const std::vector<float>& end   = _g->m_WireData[p].wireEnd.find(wid)->second;
      std::vector<float> start_poszy(2);
      start_poszy[0] = start[2];
      start_poszy[1] = start[1];
      std::vector<float> end_poszy(2);
      end_poszy[0] = end[2];
      end_poszy[1] = end[1];

      segs[p].resize(2); // one for start and end
      segs[p][0] = start_poszy;
      segs[p][1] = end_poszy;
    }

    // now check all combos here
    std::vector<float> intersection01;
    std::vector<float> intersection02;
    std::vector<float> intersection12;
    int combo_crosses[3] = {0};
    lineSegmentIntersection2D( segs[0], segs[1], intersection01, combo_crosses[0] );
    lineSegmentIntersection2D( segs[0], segs[2], intersection02, combo_crosses[1] );
    lineSegmentIntersection2D( segs[1], segs[2], intersection12, combo_crosses[2] );

    int combo[3][2] = { {0,1}, {0,2}, {1,2} };
    for (int i=0; i<3; i++) {
      if ( combo_crosses[i]==0 ) {
	// check if miss involves wire's whose tips are very close
	int idx1 = combo[i][0];
	int idx2 = combo[i][1];
	float dxss = 0.;
	float dxee = 0.;
	float dxes = 0.;
	float dxse = 0.;
	for (int v=0; v<2; v++) {
	  dxss += (segs[idx1][0][v]-segs[idx2][0][v])*(segs[idx1][0][v]-segs[idx2][0][v]); // start to start
	  dxee += (segs[idx1][1][v]-segs[idx2][1][v])*(segs[idx1][1][v]-segs[idx2][1][v]); // end to end
	  dxse += (segs[idx1][0][v]-segs[idx2][1][v])*(segs[idx1][0][v]-segs[idx2][1][v]); // start to end
	  dxes += (segs[idx1][1][v]-segs[idx2][0][v])*(segs[idx1][1][v]-segs[idx2][0][v]); // end to start
	}
	dxss = sqrt(dxss);
	dxee = sqrt(dxee);
	dxes = sqrt(dxes);
	dxse = sqrt(dxse);
	if ( dxss<1.0 || dxee<1.0 || dxes<1.0 || dxse<1.0 ) {
	  // pretty close. let's change the crossing status
	  combo_crosses[i]=1;
	}
      }
      // U-V crossing. check if the end points are close
      // sometimes wires that meet close to one another don't register has an intersection
    }//end of loop over crosses

    crosses = 1;
    for (int i=0; i<3; i++) {
      if ( combo_crosses[i]==0 ) crosses = 0;
    }
    
    // get area of intersection triangle
    //triangle_area.push_back( calculateIntersectionTriangle( intersection01, intersection02, intersection12 ) );
    triangle_area = calculateIntersectionTriangle( intersection01, intersection02, intersection12 );
    
    // get the 3 plane (2D) vertex
    intersection.resize(2,0.0);
    for (int i=0; i<2; i++) 
      intersection[i] = (intersection01[i]+intersection02[i]+intersection12[i] )/3.0;
    
  }

  void UBWireTool::pixelsAlongLineSegment( const std::vector<float>& start, const std::vector<float>& end, const std::vector<larcv::Image2D>& img_v,
					   const std::vector<float>& thresholds, const std::vector<int>& neighborhood, const float max_stepsize, std::vector< Pixel2DCluster >& pixels ) {
    if ( pixels.size()!=img_v.size() ) {
      if ( pixels.size()==0 )
	pixels.resize(3);
      else {
	throw std::runtime_error("Number of images and number of pixel clusters does not agree."); 
      }
    }

    float dist = 0;
    float dir[3] = {0};
    for ( int i=0; i<3; i++) {
      dir[i] = end[i] - start[i];
      dist += dir[i]*dir[i];
    }
    if ( dist==0 )
      return; // nothing to do
    dist = sqrt(dist);
    for (int i=0; i<3; i++)
      dir[i] /= dist;

    int nsteps = dist/max_stepsize;
    if ( fabs(dist - nsteps*max_stepsize)>1.0e-5 )
      nsteps++;
    float step = dist/nsteps;

    const float cm_per_tick = 	larutil::LArProperties::GetME()->DriftVelocity()*0.5;

    typedef std::array<int,2> pix_t;
    std::vector< std::set< pix_t > > pixelset_v(img_v.size());
    for ( size_t p=0; p<img_v.size(); p++ ) {
      pixelset_v.reserve(1000);
    }
    
    for (int n=0; n<=nsteps; n++) {
      
      Double_t xyz[3] = {0};
      for (int i=0; i<3; i++)
	xyz[i] = start[i] + (n*step)*dir[i];
      
      // get the tick
      float tick = xyz[0]/cm_per_tick + 3200.0;
      if ( tick<=img_v.front().meta().min_y() || tick>img_v.front().meta().max_y() )
	continue;

      int row = img_v.front().meta().row( tick );
      
      // get the plane coordinats
      for (size_t p=0; p<img_v.size(); p++) {
	float wire = larutil::Geometry::GetME()->WireCoordinate( xyz, p );
	if ( wire<img_v.at(p).meta().min_x() || wire>=img_v.at(p).meta().max_x() )
	  continue;
	
	int col = img_v.at(p).meta().col( wire );

	// search for charge
	for ( int dr=-neighborhood.at(p); dr<=neighborhood.at(p); dr++) {
	  int r = row+dr;
	  if ( r<0 || r>=(int)img_v[p].meta().rows() ) continue;
	  for ( int dc=-neighborhood.at(p); dc<=neighborhood.at(p); dc++ ) {
	    int c = col+dc;
	    if ( c<0 || c>=(int)img_v[p].meta().cols() ) continue;

	    if ( img_v.at(p).pixel(r,c)>=thresholds.at(p) ) {
	      pix_t pix;
	      pix[0] = c;
	      pix[1] = r;
	      pixelset_v.at(p).insert( pix );
	    }
	  }
	}
	
      }// end of loop over planes
    }//end of loop over steps

    for ( size_t p=0; p<img_v.size(); p++) {
      for ( auto const& pix : pixelset_v.at(p) ) {
	larcv::Pixel2D pix2d( pix[0], pix[1] );
	pix2d.Intensity( img_v.at(p).pixel( pix[1], pix[0] ) );
	pixels.at(p).emplace_back( std::move(pix2d) );
      }
    }

    return;
  }
  
  std::vector<int> UBWireTool::getProjectedImagePixel( const std::vector<float>& pos3d, const larcv::ImageMeta& meta, const int nplanes, const float fracpixborder ) {
    /* -----------------------------------------------------------
     * returns (row,colp1,colp2,..,colppN) corresponding to
     *  projection of pos3d onto images
     * 
     * inputs
     * ------
     * pos3d: expect length 3, position in 3D space
     * meta: ImageMeta, defining image size and  coordinates
     * nplanes: number of planes
     * fracpixborder: 3D positions that fall outside of image but within
     *  fracpixboarder*pixel width or height return col or row of nearest
     *  valid pixel. otherwise return -1 value for outside the image
     * 
     * output: 1+numplanes vector providing row and col of images for the planes
     *  if outside the image, values of -1 are returned
     * 
     * weaknesses: uses hard-coded values for position to tick conversion, should use service
     *  
     * ----------------------------------------------------------*/
    
    std::vector<int> img_coords( nplanes+1, -1 );
    float row_border = fabs(fracpixborder)*meta.pixel_height();
    float col_border = fabs(fracpixborder)*meta.pixel_width();

    // tick/row
    float tick = pos3d[0]/(0.5*larutil::LArProperties::GetME()->DriftVelocity()) + 3200.0;
    if ( tick<meta.min_y() ) {
      if ( tick>meta.min_y()-row_border )
	// below min_y-border, out of image
	img_coords[0] = meta.rows()-1; // note that tick axis and row indicies are in inverse order
      else
	// outside of image and border
	img_coords[0] = -1;
    }
    else if ( tick>meta.max_y() ) {
      if ( tick<meta.max_y()+row_border )
	// within upper border
	img_coords[0] = 0;
      else
	// outside of image and border
	img_coords[0] = -1;
    }
    else {
      // within the image
      img_coords[0] = meta.row( tick );
    }

    // Columns
    Double_t xyz[3] = { pos3d[0], pos3d[1], pos3d[2] };
    
    for (int p=0; p<nplanes; p++) {
      float wire = larutil::Geometry::GetME()->WireCoordinate( xyz, p );
      // round wire
      if ( wire+0.5>=(int)wire+1.0 )
	wire = (int)wire+1.0;

      // get image coordinates
      if ( wire<meta.min_x() ) {
	if ( wire>meta.min_x()-col_border )
	  // within lower border
	  img_coords[p+1] = 0;
	else
	  img_coords[p+1] = -1;
      }
      else if ( wire>=meta.max_x() ) {
	if ( wire<meta.max_x()+col_border )
	  // within border
	  img_coords[p+1] = meta.cols()-1;
	else
	  // outside border
	  img_coords[p+1] = -1;
      }
      else
	// inside image
	img_coords[p+1] = meta.col( wire );
    }//end of plane loop
    
    return img_coords;
  }
  
}
