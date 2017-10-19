#include "WireData.h"
#include <cmath>
#include <vector>

namespace larcv {

  void WireData::addWire( int id, float start[], float end[] )  {
    
    wireIDs.insert(id);
    std::vector<float> vstart(3,0.0);
    std::vector<float> vend(3,0.0);
    std::vector<float> vdir(3, 0.0 );
    std::vector<float> pdir(3, 0.0 );
      
    // store start, end, calculate dir
    float norm = 0;
    for (int i=0; i<3; i++) {
      vstart[i] = start[i];
      vend[i] = end[i];
      float dx = vend[i]-vstart[i];
      norm += dx*dx;
      vdir[i] = dx;
    }
    float l2 = norm;
    norm = sqrt(norm);
    for (int i=0; i<3; i++)
      vdir[i] /= norm;
      
    // take cross product of x+ and dir
    pdir[0] = 0.0;
    pdir[1] = -vdir[2];
    pdir[2] = vdir[1];
    norm = 0.0;
    for (int i=0; i<3; i++) {
      norm += pdir[i]*pdir[i];
    }
    norm = sqrt(norm);
    for (int i=0; i<3; i++)
      pdir[i] /= norm;
      
    wireStart[id] = vstart;
    wireEnd[id]    = vend;
    wireDir[id]    = vdir;
    wirePDir[id]   = pdir;
    wireL2[id]     = l2;
      
  }

}
