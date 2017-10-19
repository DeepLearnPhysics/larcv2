#ifndef __UBWIRETOOL_H__
#define __UBWIRETOOL_H__

#include <vector>
#include <array>
#include <map>

#include "Base/larcv_base.h"
#include "Base/larbys.h"
#include "WireData.h"

#include "DataFormat/Pixel2DCluster.h"
#include "DataFormat/ImageMeta.h"
#include "DataFormat/Image2D.h"

namespace larcv {

  class UBWireTool : public larcv::larcv_base {

    UBWireTool();
    virtual ~UBWireTool() {};

  public:

    static float calculateIntersectionTriangle( const std::vector<float>& p0, const std::vector<float>& p1, const std::vector<float>& p2 );
    static void findWireIntersections( const std::vector< std::vector<int> >& wirelists,
				       const std::vector< std::vector<float> >& valid_range,
				       std::vector< std::vector<int> >& intersections3plane,
				       std::vector< std::vector<float> >& vertex3plane,
				       std::vector<float>& areas3plane,
				       std::vector< std::vector<int> >& intersections2plane, std::vector< std::vector<float> >& vertex2plane );
    static void lineSegmentIntersection2D( const std::vector< std::vector<float> >& ls1, 
					   const std::vector< std::vector<float> >& ls2, 
					   std::vector<float>& insec, int& crosses );
    static void wireIntersection( int plane1, int wireid1, int plane2, int wireid2, std::vector<float>& intersection, int& crosses );
    static void wireIntersection( std::vector< int > wids, std::vector<float>& intersection, double& triangle_area, int& crosses );
    static void getMissingWireAndPlane( const int plane1, const int wireid1, const int plane2, const int wireid2, 
      int& otherplane, int& otherwire, std::vector<float>& intersection, int& crosses );
    
    static std::vector<int> getProjectedImagePixel( const std::vector<float>& pos3d, const larcv::ImageMeta& meta, const int nplanes, const float fracborderpix=0.5 );

    static const larcv::WireData& getWireData(int plane);

    static void pixelsAlongLineSegment( const std::vector<float>& start, const std::vector<float>& end, const std::vector<larcv::Image2D>& img_v,
					const std::vector<float>& thresholds, const std::vector<int>& neighborhood, const float max_stepsize,
					std::vector< Pixel2DCluster >& pixels );

  private:
    
    static UBWireTool* _g_ubwiretool;

    static UBWireTool* _get_global_instance();

    void loadData();
    std::map<int,larcv::WireData> m_WireData; // key is plane ID, value is class with wire info

  };


}

#endif
