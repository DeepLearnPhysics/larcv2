#ifndef __LARCV_VERTEX_H__
#define __LARCV_VERTEX_H__

#include "DataFormatTypes.h"
#include "Point.h"

namespace larcv {

  class Vertex {
  public:
    /// Particle ID default constructor
    Vertex();
    Vertex(double x, double y, double z, double t);

    /// Reset function
    void reset();

    /// Reset function for x, y, z, t
    void reset(double x, double y, double z, double t);

    /// Convert to point
    const larcv::Point2D as_point2d(larcv::PointType_t point_type) const;
    const larcv::Point3D as_point3d() const;
    void as_point(larcv::PointType_t point_type, double * x, double * y, double * z);

    inline double x() const { return _x; }
    inline double y() const { return _y; }
    inline double z() const { return _z; }
    inline double t() const { return _t; }

    /// Default destructor
    virtual ~Vertex(){};

    inline bool operator== (const Vertex& rhs) const
    {
      return ( _x == rhs._x && _y == rhs._y && _z == rhs._z && _t == rhs._t );
    }

    inline bool operator!= (const Vertex& rhs) const
    {
      return !((*this) == rhs);
    }

    inline bool operator< (const Vertex& rhs) const
    {
      if( _x     < rhs._x ) return true;
      if( rhs._x < _x     ) return false;
      if( _y     < rhs._y ) return true;
      if( rhs._y < _y     ) return false;
      if( _z     < rhs._z ) return true;
      if( rhs._z < _z     ) return false;
      if( _t     < rhs._t ) return true;
      if( rhs._t < _t     ) return false;

      return false;
    }

    std::string dump() const;
    
  private:

    double _x, _y, _z, _t;

    void approx();
  };
}

#endif
