#ifndef __BBOX_CXX__
#define __BBOX_CXX__

#include "BBox.h"
#include <cmath>
#include <set>
#include <sstream>
#include "larcv/core/Base/larbys.h"

namespace larcv {

  BBox2D::BBox2D(double xmin, double ymin, double xmax, double ymax, ProjectionID_t id)
    : _id(id), _p1(xmin,ymin) , _p2(xmax,ymax)
  {
    if(xmin > xmax) throw larbys("xmin > xmax not allowed for BBox2D construction!");
    if(ymin > ymax) throw larbys("ymin > ymax not allowed for BBox2D construction!");
  }

  void BBox2D::update(double xmin, double ymin, double xmax, double ymax, ProjectionID_t id)
  {
    if(xmin > xmax) throw larbys("xmin > xmax not allowed for BBox2D::Update!");
    if(ymin > ymax) throw larbys("ymin > ymax not allowed for BBox2D::Update!");
    _p1.x = xmin; _p1.y = ymin;
    _p2.x = xmax, _p2.y = ymax;
    _id = id;
  }

  void BBox2D::update(const Point2D& p1, const Point2D& p2, ProjectionID_t id)
  { update(p1.x,p1.y,p2.x,p2.y); _id = id; }

  void BBox2D::update(ProjectionID_t id)
  { _id = id; }

  BBox2D BBox2D::overlap(const BBox2D& box) const
  {
    double minx = ( box.min_x() < this->min_x() ? this->min_x() : box.min_x()  ); //pick larger x min-bound
    double maxx = ( box.max_x() < this->max_x() ? box.max_x()  : this->max_x() ); //pick smaller x max-bound

    double miny = ( box.min_y() < this->min_y() ? this->min_y() : box.min_y()  ); //pick larger x min-bound
    double maxy = ( box.max_y() < this->max_y() ? box.max_y()  : this->max_y() ); //pick smaller x max-bound

    if(!(minx < maxx && miny < maxy)) {
      std::stringstream ss;
      ss << "No overlap found ... this X: " << this->min_x() << " => " << this->max_x() << " Y: " << this->min_y() << " => " << this->max_y()
	 << " ... the other X: " << box.min_x() << " => " << box.max_x() << " Y: " << box.min_y() << " => " << box.max_y() << std::endl;
      throw larbys(ss.str());
    }
    return BBox2D(minx, miny, maxx, maxy);
  }

  BBox2D BBox2D::inclusive(const BBox2D& box) const
  {
    double min_x = ( box.min_x() < this->min_x() ? box.min_x() : this->min_x() ); //pick smaller x min-boudn
    double max_x = ( box.max_x() > this->max_x() ? box.max_x() : this->max_x() ); //pick larger x max-bound

    double min_y = ( box.min_y() < this->min_y() ? box.min_y() : this->min_y() ); //pick smaller y min-boudn
    double max_y = ( box.max_y() > this->max_y() ? box.max_y() : this->max_y() ); //pick larger y max-bound

    return BBox2D(min_x, min_y, max_x, max_y);
  }

  bool BBox2D::contains(const Point2D& point) const
  {
    return point.x >= this->min_x() && point.y >= this->min_y() && point.x <= this->max_x() && point.y <= this->max_y();
  }

  std::string BBox2D::dump() const
  {
    std::stringstream ss;
    ss << "    (" << _p1.x << "," << _p1.y << ") => (" << _p2.x << "," << _p2.y << ")" << std::endl;;
    return ss.str();
  }

  //
  // BBox3D
  //

  BBox3D::BBox3D(double xmin, double ymin, double zmin,
		 double xmax, double ymax, double zmax)
    : _p1(xmin,ymin,zmin) , _p2(xmax,ymax,zmax)
  {
    if(xmin > xmax) throw larbys("xmin > xmax not allowed for BBox3D construction!");
    if(ymin > ymax) throw larbys("ymin > ymax not allowed for BBox3D construction!");
    if(zmin > zmax) throw larbys("zmin > zmax not allowed for BBox3D construction!");
  }



  void BBox3D::update(double xmin, double ymin, double zmin,
		      double xmax, double ymax, double zmax)
  {
    if(xmin > xmax) throw larbys("xmin > xmax not allowed for BBox3D::Update!");
    if(ymin > ymax) throw larbys("ymin > ymax not allowed for BBox3D::Update!");
    if(zmin > zmax) throw larbys("zmin > zmax not allowed for BBox3D::Update!");
    _p1.x = xmin; _p1.y = ymin; _p1.z = zmin;
    _p2.x = xmax, _p2.y = ymax; _p2.z = zmax;
  }

  void BBox3D::update(const Point3D& p1, const Point3D& p2)
  { update(p1.x,p1.y,p1.z,p2.x,p2.y,p2.z); }

  double BBox3D::area(int axis) const
  {
    if(axis == 0) return (_p2.x - _p1.x) * (_p2.y - _p1.y); // xy projection
    if(axis == 1) return (_p2.x - _p1.x) * (_p2.z - _p1.z); // xz projection
    if(axis == 2) return (_p2.y - _p1.y) * (_p2.z - _p1.z); // yz projection
    throw larbys("BBox3D::area can only take axis=[0,1,2]!");
  }


  BBox3D BBox3D::overlap(const BBox3D& box) const
  {
    double minx = ( box.min_x() < this->min_x() ? this->min_x() : box.min_x()  ); //pick larger x min-bound
    double maxx = ( box.max_x() < this->max_x() ? box.max_x()  : this->max_x() ); //pick smaller x max-bound

    double miny = ( box.min_y() < this->min_y() ? this->min_y() : box.min_y()  ); //pick larger x min-bound
    double maxy = ( box.max_y() < this->max_y() ? box.max_y()  : this->max_y() ); //pick smaller x max-bound

    double minz = ( box.min_z() < this->min_z() ? this->min_z() : box.min_z()  ); //pick larger x min-bound
    double maxz = ( box.max_z() < this->max_z() ? box.max_z()  : this->max_z() ); //pick smaller x max-bound

    if(!(minx < maxx && miny < maxy && minz < maxz)) {
      std::stringstream ss;
      ss << "No overlap found ... this"
	 << " X: " << this->min_x() << " => " << this->max_x()
	 << " Y: " << this->min_y() << " => " << this->max_y()
	 << " Z: " << this->min_z() << " => " << this->max_z()
	 << " ... the other"
	 << " X: " << box.min_x() << " => " << box.max_x()
	 << " Y: " << box.min_y() << " => " << box.max_y()
	 << " Z: " << box.min_z() << " => " << box.max_z()
	 << std::endl;
      throw larbys(ss.str());
    }
    return BBox3D(minx, miny, minz, maxx, maxy, maxz);
  }

  BBox3D BBox3D::inclusive(const BBox3D& box) const
  {
    double min_x = ( box.min_x() < this->min_x() ? box.min_x() : this->min_x() ); //pick smaller x min-boudn
    double max_x = ( box.max_x() > this->max_x() ? box.max_x() : this->max_x() ); //pick larger x max-bound

    double min_y = ( box.min_y() < this->min_y() ? box.min_y() : this->min_y() ); //pick smaller y min-boudn
    double max_y = ( box.max_y() > this->max_y() ? box.max_y() : this->max_y() ); //pick larger y max-bound

    double min_z = ( box.min_z() < this->min_z() ? box.min_z() : this->min_z() ); //pick smaller z min-boudn
    double max_z = ( box.max_z() > this->max_z() ? box.max_z() : this->max_z() ); //pick larger z max-bound

    return BBox3D(min_x, min_y, min_z, max_x, max_y, max_z);
  }

  bool BBox3D::contains(const Point3D& point) const
  {
    return point.x >= this->min_x() && point.y >= this->min_y() && point.z >= this->min_z() && point.x <= this->max_x() && point.y <= this->max_y() && point.z <= this->max_z();
  }

  std::string BBox3D::dump() const
  {
    std::stringstream ss;
    ss << "    (" << _p1.x << "," << _p1.y << "," << _p1.z << ") => (" << _p2.x << "," << _p2.y << "," << _p2.z << ")" << std::endl;;
    return ss.str();
  }

}

#endif
