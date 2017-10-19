#ifndef __LARCV_VERTEX_H__
#define __LARCV_VERTEX_H__

namespace larcv {

  class Vertex {
  public:
    /// Particle ID default constructor
    Vertex();
    Vertex(double x, double y, double z, double t);
    
    /// Reset function 
    void Reset();

    /// Reset function for x, y, z, t
    void Reset(double x, double y, double z, double t);
    
    inline double X() const { return _x; }
    inline double Y() const { return _y; }
    inline double Z() const { return _z; }
    inline double T() const { return _t; }
    
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
  private:
    
    double _x, _y, _z, _t;
    
    void Approx();
  };
}

#endif
