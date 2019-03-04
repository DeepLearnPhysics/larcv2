#ifndef __SUPERA_MCROIMAKER_H__
#define __SUPERA_MCROIMAKER_H__
//#ifndef __CINT__
//#ifndef __CLING__
#include <vector>

// LArSoft
//#include "MCBase/MCTrack.h"
//#include "MCBase/MCShower.h"
//#include "Simulation/SimChannel.h"
//#include "SimulationBase/MCParticle.h"

// LArCV
#include "Base/larcv_base.h"
#include "Base/Range.h"
#include "DataFormat/ROI.h"
#include "DataFormat/DataFormatUtil.h"
#include "FMWKInterface.h"

namespace supera {

  typedef ::larcv::Range<unsigned int> WTRange_t;
  typedef std::vector<supera::WTRange_t> WTRangeArray_t;
  
  class MCROIMaker : public ::larcv::larcv_base {
    
  public:
    
    MCROIMaker() : larcv::larcv_base("MCROIMaker")
		 , _max_time_tick(9600)
		 , _time_padding(10)
		 , _wire_padding(10)
		 , _min_width(10)
		 , _min_height(14)
    {}
    
    virtual ~MCROIMaker() {}
    
    void configure(const supera::Config_t& cfg);
    /**
       Given single MCTrack, returns length 4 range array (3 planes + time) \n
       which contains all trajectory points of input MCTrack.
    */
    WTRangeArray_t WireTimeBoundary( const supera::LArMCTrack_t& mct ) const;

    WTRangeArray_t WireTimeBoundary( const supera::LArMCTrack_t& mct,
				     const std::vector<supera::LArSimCh_t>& sch_v ) const;

    WTRangeArray_t WireTimeBoundary( const supera::LArMCShower_t& mcs ) const;

    WTRangeArray_t WireTimeBoundary( const supera::LArMCShower_t& mcs,
				     const std::vector<supera::LArSimCh_t>& sch_v ) const;
    
    ::larcv::ROI ParticleROI( const supera::LArMCTrack_t& mct,
			      const int time_offset ) const;

    ::larcv::ROI ParticleROI( const supera::LArMCTrack_t& mct,
			      const std::vector<supera::LArSimCh_t>& sch_v,
			      const int time_offset ) const;

    ::larcv::ROI ParticleROI( const supera::LArMCShower_t& mcs,
			      const int time_offset ) const;

    ::larcv::ROI ParticleROI( const supera::LArMCShower_t& mcs,
			      const std::vector<supera::LArSimCh_t>& sch_v,
			      const int time_offset ) const;
    
  private:
    
    std::vector<larcv::ImageMeta> WTRange2BB(const WTRangeArray_t&) const;
    
    unsigned int _max_time_tick; ///< Maximum tick number in time
    unsigned int _time_padding;  ///< Padding in time axis (height) for MCROIMaker::Format function
    unsigned int _wire_padding;  ///< Padding in wire axis (width) for MCROIMaker::Format function
    unsigned int _min_width;
    unsigned int _min_height;
    bool _apply_sce; 
  };
}

#endif
//#endif
//#endif
