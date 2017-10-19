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
  {Approx();}
  
  Vertex::Vertex(double x, double y, double z, double t)
    : _x(x), _y(y), _z(z), _t(t)
  {Approx();}
  
  void Vertex::Reset(){
    _x = _y = _z = _t = 0;
  }

  void Vertex::Reset(double x, double y, double z, double t)
  {
    _x = x;
    _y = y;
    _z = z;
    _t = t;
    Approx();
  }
  
  void Vertex::Approx()
  {
    _x = (double)( ((double)((signed long long)(_x * 1.e6)) * 1.e-6 ));
    _y = (double)( ((double)((signed long long)(_y * 1.e6)) * 1.e-6 ));
    _z = (double)( ((double)((signed long long)(_z * 1.e6)) * 1.e-6 ));
    _t = (double)( ((double)((signed long long)(_t * 1.e6)) * 1.e-6 ));
  }
}

#endif  
