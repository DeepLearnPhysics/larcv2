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
#ifndef __LARCV_BBOX_H__
#define __LARCV_BBOX_H__

#include <iostream>
#include "Point.h"
#include "DataFormatTypes.h"
namespace larcv {

  /**
     \class BBox2D
     \brief Bounding box in 2D
  */
  class BBox2D {

  public:

    /// Default constructor
    BBox2D(double xmin = 0, double ymin = 0,
           double xmax = 0, double ymax = 0,
           ProjectionID_t id = kINVALID_PROJECTIONID);

    /// Default destructor
    virtual ~BBox2D() {}

    inline bool operator== (const BBox2D& rhs) const
    { return (_p1 == rhs._p1 && _p2 == rhs._p2); }

    void update(double xmin, double ymin,
                double xmax, double ymax,
                ProjectionID_t id = kINVALID_PROJECTIONID);
    void update(const Point2D& pt1, const Point2D& pt2,
                ProjectionID_t id = kINVALID_PROJECTIONID);
    void update(ProjectionID_t id);
    inline bool empty() const { return (_p1 == _p2); }
    inline const Point2D& origin      () const { return _p1; }
    inline const Point2D& bottom_left () const { return _p1; }
    inline const Point2D& top_right   () const { return _p2; }
    inline Point2D center  () const { return Point2D(center_x(), center_y()); }
    inline double center_x () const { return _p1.x + 0.5*(_p2.x - _p1.x); }
    inline double center_y () const { return _p1.y + 0.5*(_p2.y - _p1.y); }
    inline double min_x  () const { return _p1.x; }
    inline double min_y  () const { return _p1.y; }
    inline double max_x  () const { return _p2.x; }
    inline double max_y  () const { return _p2.y; }
    inline double width  () const { return _p2.x - _p1.x; }
    inline double height () const { return _p2.y - _p1.y; }
    inline double area   () const { return (_p2.y - _p1.y) * (_p2.x - _p1.x); }
    inline ProjectionID_t id() const { return _id; }
    std::string dump() const;

    BBox2D overlap(const BBox2D& box) const;
    BBox2D inclusive(const BBox2D& box) const;
    bool contains(const Point2D& point) const;

  private:
    ProjectionID_t _id; ///< ProjectionID_t identifies types of 2D projections to which BBox belongs
    Point2D _p1; ///< bottom-left point coordinate (x1,y1) where x1<x2 and y1<y2
    Point2D _p2; ///< top-right point coordinate (x2,y2) where x1<x2 and y1<y2
  };

  /**
     \class BBox3D
     \brief Bounding box in 3D
  */
  class BBox3D {

  public:

    /// Default constructor
    BBox3D(double xmin = 0, double ymin = 0, double zmin = 0,
           double xmax = 0, double ymax = 0, double zmax = 0);

    /// Default destructor
    virtual ~BBox3D() {}

    inline bool operator== (const BBox3D& rhs) const
    { return (_p1 == rhs._p1 && _p2 == rhs._p2); }

    void update(double xmin, double ymin, double zmin,
                double xmax, double ymax, double zmax);

    void update(const Point3D& pt1, const Point3D& pt2);
    inline bool empty() const { return (_p1 == _p2); }
    inline const Point3D& origin      () const { return _p1; }
    inline const Point3D& bottom_left () const { return _p1; }
    inline const Point3D& top_right   () const { return _p2; }
    inline Point3D center  () const { return Point3D(center_x(), center_y(), center_z()); }
    inline double center_x () const { return _p1.x + 0.5*(_p2.x - _p1.x); }
    inline double center_y () const { return _p1.y + 0.5*(_p2.y - _p1.y); }
    inline double center_z () const { return _p1.z + 0.5*(_p2.z - _p1.z); }
    inline double min_x  () const { return _p1.x; }
    inline double min_y  () const { return _p1.y; }
    inline double min_z  () const { return _p1.z; }
    inline double max_x  () const { return _p2.x; }
    inline double max_y  () const { return _p2.y; }
    inline double max_z  () const { return _p2.z; }
    inline double width  () const { return _p2.x - _p1.x; }
    inline double height () const { return _p2.y - _p1.y; }
    inline double depth  () const { return _p2.z - _p1.z; }
    inline double volume () const { return (_p2.x - _p1.x) * (_p2.y - _p1.y) * (_p2.z - _p1.z); }
    double area(int axis) const;
    BBox3D overlap(const BBox3D& box) const;
    BBox3D inclusive(const BBox3D& box) const;
    bool contains(const Point3D& point) const;

    std::string dump() const;

  private:

    Point3D _p1; ///< bottom-left point coordinate (x1,y1,z1) where x1<x2 and y1<y2 and z1<z2
    Point3D _p2; ///< top-right point coordinate (x2,y2,z2) where x1<x2 and y1<y2 and z1<z2
  };
}
#endif
/** @} */ // end of doxygen group
