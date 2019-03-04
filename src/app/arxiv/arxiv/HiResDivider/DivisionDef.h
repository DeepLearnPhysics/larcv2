#ifndef __DIVISIONDEF__
#define __DIVISIONDEF__

//#include "Base/larcv_base.h"
#include "DataFormat/DataFormatTypes.h"
#include "DataFormat/ImageMeta.h"
#include <map>

namespace larcv {
  namespace hires {

    class DivisionID {
    public:
      DivisionID(int plane0_start, int plane0_end,
		 int plane1_start, int plane1_end,
		 int plane2_start, int plane2_end,
		 int time_start,   int time_end)
	: _plane0_start(plane0_start) , _plane0_end(plane0_end)
	, _plane1_start(plane1_start) , _plane1_end(plane1_end)
	, _plane2_start(plane2_start) , _plane2_end(plane2_end)
	, _time_start(time_start)     , _time_end(time_end)
      {}
      ~DivisionID(){}

      inline bool operator<(const DivisionID& rhs) const
      {
	if(_plane0_start < rhs._plane0_start) return true;
	if(_plane0_start > rhs._plane0_start) return false;
	if(_plane1_start < rhs._plane1_start) return true;
	if(_plane1_start > rhs._plane1_start) return false;
	if(_plane2_start < rhs._plane2_start) return true;
	if(_plane2_start > rhs._plane2_start) return false;

	if(_plane0_end < rhs._plane0_end) return true;
	if(_plane0_end > rhs._plane0_end) return false;
	if(_plane1_end < rhs._plane1_end) return true;
	if(_plane1_end > rhs._plane1_end) return false;
	if(_plane2_end < rhs._plane2_end) return true;
	if(_plane2_end > rhs._plane2_end) return false;
	
	if(_time_start < rhs._time_start) return true;
	if(_time_start > rhs._time_start) return false;

	return false;
      }
      
    private:
      int _plane0_start, _plane0_end;
      int _plane1_start, _plane1_end;
      int _plane2_start, _plane2_end;
      int _time_start,   _time_end;
    };

    //class DivisionDef : larcv::larcv_base {
    class DivisionDef {
    public:
      DivisionDef( int plane0_wirebounds[], int plane1_wirebounds[], int plane2_wirebounds[], int tickbounds[],
		   float det_xbounds[], float det_ybounds[], float det_zbounds[] );
      DivisionDef( const DivisionDef& ); // copy constructor
      DivisionDef() : fID(0,0,0,0,0,0,0,0) {};
      ~DivisionDef() {};

      const larcv::ImageMeta& getPlaneMeta( larcv::PlaneID_t plane ) const;
      bool isInsideDetRegion( float x, float y, float z ) const;

      inline bool operator<(const DivisionDef& rhs) const { return fID < rhs.fID; }

      const DivisionID& ID() const { return fID; }

      void print() const;

    protected:

      void setPlaneMeta( larcv::PlaneID_t plane, int wirebounds[],int tickbounds[] );
      
      int mNPlanes;
      std::map< larcv::PlaneID_t, larcv::ImageMeta > m_planeMeta; // key is plane id
      float fDetBounds[3][2];

      size_t fDivisionIDX;
      size_t fDivisionIDY;
      size_t fDivisionIDZ;
      DivisionID fID;
    };

  }
}


#endif
