/**
 * \file Point.h
 *
 * \ingroup DataFormat
 * 
 * \brief Class def header for a class Point
 *
 * @author kazuhiro
 */

/** \addtogroup DataFormat

    @{*/
#ifndef LARCV_POINT_H
#define LARCV_POINT_H

#include <iostream>

namespace larcv {

  /**
     \class Point2D
     Simple 2D point struct (unit of "x" and "y" are not defined here and app specific)
  */
  class Point2D {
  public:
    Point2D(double xv=0, double yv=0) : x(xv), y(yv) {}
    ~Point2D() {}

    double x, y;
    
    inline bool operator== (const Point2D& rhs) const
    { return (x == rhs.x && y == rhs.y); }
    inline bool operator!= (const Point2D& rhs) const
    { return !(rhs == (*this)); }

    inline Point2D& operator*= (const double rhs)
    { x *= rhs; y *= rhs; return (*this); }
    inline Point2D& operator+= (const Point2D& rhs)
    { x += rhs.x; y += rhs.y; return (*this); }
    inline Point2D& operator-= (const Point2D& rhs)
    { x -= rhs.x; y -= rhs.y; return (*this); }

    inline Point2D operator* (const double rhs) const
    { return Point2D(x*rhs,y*rhs); }
    inline Point2D operator+ (const Point2D& rhs) const
    { return Point2D(x+rhs.x,y+rhs.y); }
    inline Point2D operator- (const Point2D& rhs) const
    { return Point2D(x-rhs.x,y-rhs.y); }
  };

  /**
     \class Point3D
     Simple 3D point struct (unit of "x", "y" and "z" are not defined here and app specific)
  */
  class Point3D {
  public:
    Point3D(double xv=0, double yv=0, double zv=0) : x(xv), y(yv), z(zv) {}
    ~Point3D() {}

    double x, y, z;
    
    inline bool operator== (const Point3D& rhs) const
    { return (x == rhs.x && y == rhs.y && z == rhs.z); }
    inline bool operator!= (const Point3D& rhs) const
    { return !(rhs == (*this)); }

    inline Point3D& operator*= (const double rhs)
    { x *= rhs; y *= rhs; z *= rhs; return (*this); }
    inline Point3D& operator+= (const Point3D& rhs)
    { x += rhs.x; y += rhs.y; z += rhs.z; return (*this); }
    inline Point3D& operator-= (const Point3D& rhs)
    { x -= rhs.x; y -= rhs.y; z -= rhs.z; return (*this); }

    inline Point3D operator* (const double rhs) const
    { return Point3D(x*rhs,y*rhs,z*rhs); }
    inline Point3D operator+ (const Point3D& rhs) const
    { return Point3D(x+rhs.x,y+rhs.y,z+rhs.z); }
    inline Point3D operator- (const Point3D& rhs) const
    { return Point3D(x-rhs.x,y-rhs.y,z-rhs.z); }
  };

}
#endif
/** @} */ // end of doxygen group 

