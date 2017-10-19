/**
 * \file WireRange3D.h
 *
 * \ingroup MeatSlicer
 * 
 * \brief Class def header for a class WireRange3D
 *
 * @author kazuhiro
 */

/** \addtogroup MeatSlicer

    @{*/
#ifndef WIRERANGE3D_H
#define WIRERANGE3D_H

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <limits>

namespace supera {

  class GridPoint3D {
  public:
    GridPoint3D(int xval = std::numeric_limits<int>::max(),
		int yval = std::numeric_limits<int>::max(),
		int zval = std::numeric_limits<int>::max())
      : x(xval)
      , y(yval)
      , z(zval)
    {}
    ~GridPoint3D(){}

    GridPoint3D(const GridPoint3D& rhs)
      : x(rhs.x)
      , y(rhs.y)
      , z(rhs.z)
    {}

    inline bool operator < (const GridPoint3D& pt) const {
      if( x < pt.x ) return true;
      if( x > pt.x ) return false;
      if( y < pt.y ) return true;
      if( y > pt.y ) return false;
      if( z < pt.z ) return true;
      if( z > pt.z ) return false;
      return false;
    }
    inline bool operator == (const GridPoint3D& pt) const
    { return ( x == pt.x && y == pt.y && z == pt.z ); }
    inline bool operator != (const GridPoint3D& pt) const
    { return !( (*this) == pt ); }
    
    inline bool operator > (const GridPoint3D& pt) const
    {
      if( (*this) == pt ) return false;
      return !( (*this) < pt);
    }

    inline bool Valid() const
    {
      return (x != std::numeric_limits<int>::max() &&
	      y != std::numeric_limits<int>::max() &&
	      z != std::numeric_limits<int>::max());
    }
      
    int x, y, z;
  };
  
  /**
     \class WireRange3D
     User defined class WireRange3D ... these comments are used to generate
     doxygen documentation!
  */
  class WireRange3D{
    
  public:
    
    /// Default constructor
    WireRange3D()
      : _verbosity(2)
      , _width()
      , _min_bound()
      , _max_bound()
      , _min_fiducial()
      , _max_fiducial()
      , _padding(0,0,0)
      , _grid_size_x(0)
      , _grid_size_y(0)
      , _grid_size_z(0)
    {}
    
    /// Default destructor
    ~WireRange3D(){}

    inline void Clear() { _constraint_v.clear(); }

    void AddConstraint(double x, double y, double z);
    
    void SetWidth(double x, double y, double z);

    void SetMin(double x, double y, double z);

    void SetMax(double x, double y, double z);

    void SetPadding(double x, double y, double z);

    inline void Verbosity(unsigned int v=2) { _verbosity = v; }

    inline void SetGridSize(double x, double y, double z)
    { _grid_size_x = x; _grid_size_y = y; _grid_size_z = z; }

    inline const std::vector<supera::GridPoint3D> GetConstraints() const
    { return _constraint_v; }

    supera::GridPoint3D GridPoint3D(double x, double y, double z) const;
    inline double GridSizeX() const { return _grid_size_x; }
    inline double GridSizeY() const { return _grid_size_y; }
    inline double GridSizeZ() const { return _grid_size_z; }

    bool Valid() const;

    void DeriveRange(const std::vector<supera::GridPoint3D>& points,
		     supera::GridPoint3D& min_pt,
		     supera::GridPoint3D& max_pt) const;

    inline const supera::GridPoint3D& EffectiveMin() const
    { return _min_bound; }

    inline const supera::GridPoint3D& EffectiveMax() const
    { return _max_bound; }

    inline unsigned int Verbosity() const { return _verbosity; }

    std::string PrintConfig() const;

  private:
    unsigned int _verbosity;
    supera::GridPoint3D _width;
    supera::GridPoint3D _min_bound, _max_bound;
    supera::GridPoint3D _min_fiducial, _max_fiducial;
    supera::GridPoint3D _padding;
    double _grid_size_x, _grid_size_y, _grid_size_z;
    std::vector<supera::GridPoint3D> _constraint_v; /// A list of 3D grid point that must be included

    void RederiveBounds();
    void Randomize(int& min_val, int& max_val,
		   const int min_bound, const int max_bound,
		   const int width, const int padding) const;
    
  };

}

#endif
/** @} */ // end of doxygen group 

