/**
 * \file DividerAlgo.h
 *
 * \ingroup HiResDivider
 * 
 * \brief Algorithm to determine which wires fall within a 3D box within the detector.
 *
 * @author kazuhiro
 */

/** \addtogroup HiResDivider

    @{*/

#ifndef __DIVIDER_ALGO_H__
#define __DIVIDER_ALGO_H__

#include "WireInfoData.h"
#include <vector>

namespace divalgo { 
  // meant to be transportable, what should the namespace be?

  class DividerAlgo {
    
  public:

    DividerAlgo(); //< default constructor
    virtual ~DividerAlgo();

    void getregionwires( const float z1, const float y1, const float z2, const float y2, 
			 std::vector<int>& ywires, std::vector<int>& uwires, std::vector<int>& vwires );
    
  protected:

    // helper functions
    int findUwire( float zypoint[] ); //< given (z,y) coordinate, find U wire
    int findVwire( float zypoint[] ); //< given (z,y) coordinate, find V wire
    int findYwire( float zypoint[] ); //< given (z,y) coordinate, find Y wire
    
    float fDetCenter[3]; //< center of detector in current coordinate frame 
    float fOffset[3];    //< applied offset in coorindates, default is (0,0,0)
    

  };

}



#endif
