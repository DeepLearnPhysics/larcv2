#include "DivisionDef.h"
#include "Base/larbys.h"
#include "Base/larcv_base.h"

namespace larcv {
  namespace hires {

    DivisionDef::DivisionDef( int plane0_wirebounds[], int plane1_wirebounds[], int plane2_wirebounds[], int tickbounds[],
			      float det_xbounds[], float det_ybounds[], float det_zbounds[] )
      : fID(plane0_wirebounds[0],plane0_wirebounds[1],
	    plane1_wirebounds[0],plane1_wirebounds[1],
	    plane2_wirebounds[0],plane2_wirebounds[1],
	    tickbounds[0],tickbounds[1])
    {
	// this constructor completely defines the definition (for 3 planes)
      mNPlanes = 0;
      setPlaneMeta( 0, plane0_wirebounds, tickbounds );
      setPlaneMeta( 1, plane1_wirebounds, tickbounds );
      setPlaneMeta( 2, plane2_wirebounds, tickbounds );
      for (int i=0; i<2; i++) {
	fDetBounds[0][i] = det_xbounds[i];
	fDetBounds[1][i] = det_ybounds[i];
	fDetBounds[2][i] = det_zbounds[i];
      }
    }

    DivisionDef::DivisionDef( const DivisionDef& src)
      : fID(src.fID)
    {
      for ( std::map< larcv::PlaneID_t, larcv::ImageMeta >::const_iterator it=src.m_planeMeta.begin(); it!=src.m_planeMeta.end(); it++ ) {
	m_planeMeta[ (*it).first ] = (*it).second; // implied copy?
      }
      for (int i=0; i<2; i++)
	for (int j=0; j<3; j++)
	  fDetBounds[j][i] = src.fDetBounds[j][i];
    }

    const larcv::ImageMeta& DivisionDef::getPlaneMeta( larcv::PlaneID_t plane ) const {
      std::map< larcv::PlaneID_t, larcv::ImageMeta >::const_iterator it=m_planeMeta.find( plane );
      if ( it==m_planeMeta.end() )
	throw larcv::larbys("Did not have a division Image meta for plane.");
      return (*it).second;
    }

    void DivisionDef::setPlaneMeta( larcv::PlaneID_t plane, int wirebounds[],int tickbounds[] ) {
      // we define divisions
      m_planeMeta[plane] = larcv::ImageMeta( wirebounds[1]-wirebounds[0], tickbounds[1]-tickbounds[0],
					     tickbounds[1]-tickbounds[0], wirebounds[1]-wirebounds[0],
					     wirebounds[0], tickbounds[1], plane );
      mNPlanes++;
      //LARCV_NORMAL() << m_planeMeta[plane].dump();
    }

    bool DivisionDef::isInsideDetRegion( float x, float y, float z ) const {
      float pos[3] = { x, y, z};
      for (int v=0; v<3; v++) {
	if ( pos[v] < fDetBounds[v][0] || pos[v] > fDetBounds[v][1] )
	  return false;
      }
      return true;
    }

    void DivisionDef::print() const {
      std::cout << "Division Def: x=[" << fDetBounds[0][0] << "," << fDetBounds[0][1] << "] "
		<< " y=[" << fDetBounds[1][0] << "," << fDetBounds[1][1] << "] "
		<< " z=[" << fDetBounds[2][0] << "," << fDetBounds[2][1] << "]" << std::endl;
    }
    
  }
}
