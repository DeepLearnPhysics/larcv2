#ifndef __WireInfo__
#define __WireInfo__

// this class makes a helper image that "weights" the charge 
// on a wire based on the pmt flash location

#include <string>
#include <vector>
#include <map>
#include <set>

namespace larcv {

  // Data container class for wire info
  class WireData  {
  public:
    WireData( int pid=0 ) { planeID=pid; };
    ~WireData() {};
      
    int planeID; // plane number
    std::set< int > wireIDs;                       // set of wireIDs
    std::map< int, std::vector<float> > wireStart; // index is wireID, key is the start position of a wire
    std::map< int, std::vector<float> > wireEnd;   // index is wireID, key is the end position of a wire
    std::map< int, float >              wireL2;    // index is wireID, key is normalized wire direction
    std::map< int, std::vector<float> > wireDir;   // index is wireID, key is normalized wire direction
    std::map< int, std::vector<float> > wirePDir;  // index is wireID, key is cross product of wire dir and (1,0,0), the direction of the wire plane      
    
    int nwires() const { return (int)wireIDs.size(); };
    
    void addWire( int id, float start[], float end[] );
    
    
  };
}

#endif
