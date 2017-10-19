#ifndef __BBOX_CXX__
#define __BBOX_CXX__

#include "BBox.h"
#include <set>
#include <sstream>
namespace larcv {

  std::string BBox2D::dump() const
  {
    std::stringstream ss;
    ss << "    (" << x1 << "," << y1 << ") => (" << x2 << "," << y2 << ")" << std::endl;;
    return ss.str();
  }

  std::string BBox3D::dump() const
  {
    std::stringstream ss;
    ss << "    (" << x1 << "," << y1 << "," << z1 << ") => (" << x2 << "," << y2 << "," << z2 << ")" << std::endl;;
    return ss.str();
  }
  
}

#endif
