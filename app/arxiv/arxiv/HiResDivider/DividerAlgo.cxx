#include "DividerAlgo.h"
#include <cstring>
#include <cmath>

#include "Base/larbys.h"

namespace divalgo {

  DividerAlgo::DividerAlgo() {
    memset( fOffset, 0, sizeof(float)*3 );
    memset( fDetCenter, 0, sizeof(float)*3 );
  }

  DividerAlgo::~DividerAlgo() {
  }

  void DividerAlgo::getregionwires( const float z1, const float y1, const float z2, const float y2,
				    std::vector<int>& ywires, std::vector<int>& uwires, std::vector<int>& vwires ) {
    // going to be a little smahter. 
    // specific corners should represent the outermost wires. 
    // we find the first and last wires inside using maths

    // for U plane, lower bound wire is upper left corner
    // for V plane, lower bound wire is lower right

    uwires.clear();
    vwires.clear();
    ywires.clear();

    // make sense of bounding box input and define upperleft and lowerright
    float upperleft[2];
    float lowerright[2];
    float lowerleft[2];
    float upperright[2];

    if ( z1<z2 && y1<y2 ) {
      // given lower left and upper right
      upperleft[0] = z1; upperleft[1] = y2;
      lowerright[0] = z2; lowerright[1] = y1;
      
      lowerleft[0] = z1; lowerleft[1] = y1;
      upperright[0] = z2; upperright[1] = y2;

    }
    else if ( z1>z2 && y1<y2 ) {
      // given upper left and lower right
      upperleft[0] = z1; upperleft[1] = y1;
      lowerright[0] = z2; lowerright[1] = y2;

      lowerleft[0] = z2; lowerleft[1] = y2;
      upperright[0] = z1; upperleft[1] = y1;
      
    }
    else if ( z2<z1 && y2<y1 ) {
      // given upper right and lowerleft
      upperleft[0] = z2; upperleft[1] = y1;
      lowerright[0] = z1; lowerright[1] = y2;      

      lowerleft[0] = z1; lowerleft[1] = y2;
      upperright[0] = z2; upperleft[1] = y1;

    }
    else if ( z1<z2 && y1>y2 ) {
      // given lower left and upper right (but reversed)
      upperleft[0] = z1; upperleft[1] = y1;
      lowerright[0] = z2; lowerright[1] = y2;

      lowerleft[0] = z2; lowerleft[1] = y2;
      upperright[0] = z1; upperleft[1] = y1;

    }
    else {
      throw larcv::larbys("bounding box coordindates don't make sense.");
      return;
    }

    // bounds check
    float minz = uplaneStart[0][2];
    float maxz = uplaneStart[2399][2];
    if ( lowerright[0]<minz ) lowerright[0]=minz;
    if ( upperright[0]<minz ) upperright[0]=minz;
    if ( lowerleft[0]<minz )  lowerleft[0]=minz;
    if ( upperleft[0]<minz )  upperleft[0]=minz;
    if ( lowerright[0]>maxz ) lowerright[0]=maxz;
    if ( upperright[0]>maxz ) upperright[0]=maxz;
    if ( lowerleft[0]>maxz )  lowerleft[0]=maxz;
    if ( upperleft[0]>maxz )  upperleft[0]=maxz;

    float maxy = uplaneStart[0][1];
    float miny = uplaneStart[2399][1];
    if ( lowerright[1]<miny ) lowerright[1]=miny;
    if ( upperright[1]<miny ) upperright[1]=miny;
    if ( lowerleft[1]<miny )  lowerleft[1]=miny;
    if ( upperleft[1]<miny )  upperleft[1]=miny;
    if ( lowerright[1]>maxy ) lowerright[1]=maxy;
    if ( upperright[1]>maxy ) upperright[1]=maxy;
    if ( lowerleft[1]>maxy )  lowerleft[1]=maxy;
    if ( upperleft[1]>maxy )  upperleft[1]=maxy;
    
    // check for nonsense
    if ( lowerright[0]==lowerleft[0] || upperright[0]==upperleft[0] ) return;
    if ( upperright[1]==lowerright[1] || upperleft[1]==lowerleft[0] ) return;


    
    // ---------------------------------
    // u plane

    int u_lowerbound = findUwire( upperleft );
    int u_upperbound = findUwire( lowerright )+1;

    if ( u_lowerbound<0 ) u_lowerbound = 0;
    if ( u_upperbound>=2400 ) u_upperbound=2399;

    // v plane
    int v_lowerbound = findVwire( lowerleft );
    int v_upperbound = findVwire( upperright )+1;
    if ( v_lowerbound<0 ) v_lowerbound = 0;
    if ( v_upperbound>=2400 ) v_upperbound=2399;

    // y plane
    int y_lowerbound = findYwire( lowerleft );
    int y_upperbound = findYwire( lowerright ) + 1;
    if ( y_lowerbound<0 ) y_lowerbound = 0;
    if ( y_upperbound>=3456 ) y_upperbound = 3455;


    uwires.clear();
    for (int u=u_lowerbound; u<=u_upperbound; u++) {
      uwires.push_back( u );
    }

    //std::cout << "v: " << v_lowerbound << ", " << v_upperbound << std::endl;
    vwires.clear();
    for (int v=v_lowerbound; v<=v_upperbound; v++) {
      vwires.push_back( v );
    }
    //vwires.push_back( v_lowerbound );
    //vwires.push_back( v_upperbound );

    ywires.clear();
    for (int y=y_lowerbound; y<=y_upperbound; y++) {
      ywires.push_back( y );
    }
    
  }
  
  int DividerAlgo::findUwire( float zypoint[] ) {
    float dy = uplaneStart[100][1]-uplaneStart[101][1];   // gap between wires on side
    float dz = uplaneStart[1001][2]-uplaneStart[1000][2]; // gap between wires on bottom
    int firstbotu = 671;
    float side_z = uplaneStart[100][2];
    float bottom_y = uplaneStart[1000][1];
    float slopeu = tan(30.0*3.1459/180.0);

    // find intersection with bottom edge
    float s = (bottom_y-zypoint[1])/slopeu;
    float ztest1 = zypoint[0] + s;
    
    // find intersection with left edge
    s = (side_z - zypoint[0])*slopeu;
    float ytest1 = zypoint[1] + s;

    int ulowerbound;
    if ( ztest1>side_z ) {
      // on the bottom
      int du = (int) (ztest1-side_z)/dz;
      ulowerbound = firstbotu + du + 1;
    }
    else {
      int du = (int) (ytest1-bottom_y)/dy;
      ulowerbound = firstbotu - du;
    }
    return ulowerbound;
  }

  int DividerAlgo::findVwire( float zypoint[] ) {
    float dy = vplaneStart[2301][1]-vplaneStart[2300][1]; // gap between wires on side
    float dz = vplaneStart[101][2]-vplaneStart[100][2];   // gap between wires on bottom
    int lastbotv = 1727;
    float side_z = vplaneStart[2300][2];
    float bottom_y = vplaneStart[100][1];
    float slopev = tan(-30.0*3.1459/180.0);

    // find intersection with bottom edge
    float s = (bottom_y-zypoint[1])/slopev;
    float ztest1 = zypoint[0] + s;
    
    // find intersection with left edge
    s = (side_z - zypoint[0])*slopev;
    float ytest1 = zypoint[1] + s;

    int vlowerbound;
    if ( ztest1<side_z ) {
      // on the bottom
      int dv = (int) (side_z-ztest1)/dz;
      vlowerbound = lastbotv - dv;
    }
    else {
      int dv = (int) (ytest1-bottom_y)/dy;
      vlowerbound = lastbotv + dv;
    }
    return vlowerbound;
  }

  int DividerAlgo::findYwire( float zypoint[] ) {
    float dz = 0.3; // wire pitch
    float first_z = yplaneStart[0][2];
    int ylowerbound = (int)(zypoint[0]-first_z)/dz;
    return ylowerbound;
  }

}
