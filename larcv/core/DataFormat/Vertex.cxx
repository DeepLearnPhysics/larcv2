#ifndef __LARCV_VERTEX_CXX__
#define __LARCV_VERTEX_CXX__

#include "Vertex.h"

namespace larcv {
    
  // Interaction ID default constructor
  Vertex::Vertex()
    : _x(0)
    , _y(0)
    , _z(0)
    , _t(0)
  {approx();}
  
  Vertex::Vertex(double x, double y, double z, double t)
    : _x(x), _y(y), _z(z), _t(t)
  {approx();}
  
  void Vertex::reset(){
    _x = _y = _z = _t = 0;
  }

  void Vertex::reset(double x, double y, double z, double t)
  {
    _x = x;
    _y = y;
    _z = z;
    _t = t;
    approx();
  }
  
  const larcv::Point2D Vertex::as_point2d(larcv::PointType_t point_type)
  {
    switch(point_type) {
      case kPointXY:
        return Point2D(_x, _y);
      case kPointYZ:
        return Point2D(_y, _z);
      case kPointZX:
        return Point2D(_z, _x); 
      // TODO throw an exception if kPoint3D
    }
  }

  const larcv::Point3D Vertex::as_point3d()
  {
    return Point3D(_x, _y, _z);
  }

  void Vertex::as_point(larcv::PointType_t point_type, double * x, double * y, double * z)
  {
    switch(point_type) {
      case kPointXY:
        *x = _x;
        *y = _y;
      case kPointYZ:
        *x = _y;
        *y = _z;
      case kPointZX:
        *x = _z;
        *y = _x;
      case kPoint3D:
        *x = _x;
        *y = _y;
        *z = _z;
    }
  }
  
  void Vertex::approx()
  {
    _x = (double)( ((double)((signed long long)(_x * 1.e6)) * 1.e-6 ));
    _y = (double)( ((double)((signed long long)(_y * 1.e6)) * 1.e-6 ));
    _z = (double)( ((double)((signed long long)(_z * 1.e6)) * 1.e-6 ));
    _t = (double)( ((double)((signed long long)(_t * 1.e6)) * 1.e-6 ));
  }
}

#endif  
