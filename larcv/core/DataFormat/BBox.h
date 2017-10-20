/**
 * \file BBox.h
 *
 * \ingroup core_DataFormat
 * 
 * \brief Class def header for a class larcv::BBox2D and larcv::BBox3D
 *
 * @author kazuhiro
 */

/** \addtogroup core_DataFormat

    @{*/
#ifndef __BBOX_H__
#define __BBOX_H__

#include <iostream>

namespace larcv {

  /**
     \class BBox2D
     \brief Bounding box in 2D
  */
  class BBox2D{
    
  public:
    
    /// Default constructor
    BBox2D(double xmin=-1, double ymin=-1, double xmax=-1, double ymax=-1)
      : x1(xmin), y1(ymin), x2(xmax), y2(ymax)
    {}
    
    /// Default destructor
    ~BBox2D(){}

    std::string dump() const;
    
    double x1; ///< minimum x in absolute coordinate [cm]
    double y1; ///< minimum y in absolute coordinate [cm]
    double x2; ///< maximum x in absolute coordinate [cm]
    double y2; ///< maximum y in absolute coordinate [cm]
    
  };

  /**
     \class BBox3D
     \brief Bounding box in 3D
  */
  class BBox3D{
    
  public:
    
    /// Default constructor
    BBox3D(double xmin=-1, double ymin=-1, double zmin=-1,
	   double xmax=-1, double ymax=-1, double zmax=-1)
      : x1(xmin), y1(ymin), z1(zmin), x2(xmax), y2(ymax), z2(zmax)
    {}
    
    /// Default destructor
    ~BBox3D(){}

    std::string dump() const;
    
    double x1; ///< minimum x in absolute coordinate [cm]
    double y1; ///< minimum y in absolute coordinate [cm]
    double z1; ///< minimum z in absolute coordinate [cm]
    double x2; ///< maximum x in absolute coordinate [cm]
    double y2; ///< maximum y in absolute coordinate [cm]
    double z2; ///< maximum z in absolute coordinate [cm]
  };
}
#endif
/** @} */ // end of doxygen group 

