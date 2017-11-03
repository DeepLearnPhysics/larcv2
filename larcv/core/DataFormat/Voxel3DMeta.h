/**
 * \file Voxel3DMeta.h
 *
 * \ingroup core_DataFormat
 * 
 * \brief Class def header for a class larcv::Voxel3DMeta
 *
 * @author kazuhiro
 */

/** \addtogroup core_DataFormat

    @{*/
#ifndef VOXEL3DMETA_H
#define VOXEL3DMETA_H

#include "DataFormatTypes.h"
#include "BBox.h"
#include <array>

namespace larcv {

  /**
     \class Voxel3DMeta
     @brief Meta data for defining voxels (ID, size, position) and voxelized volume (coordinate, size)
  */
  class Voxel3DMeta : public BBox3D{
  public:
    /// Default ctor
    Voxel3DMeta();
    /// Default dtor
    ~Voxel3DMeta(){}
    /// Update voxel count
    void update(size_t xnum,size_t ynum,size_t znum);
    /// Define dimensions
    inline void set(double xmin, double ymin, double zmin,
		    double xmax, double ymax, double zmax,
		    size_t xnum,size_t ynum,size_t znum,
		    DistanceUnit_t unit=kUnitCM)
    { BBox3D::update(xmin,ymin,zmin,xmax,ymax,zmax);
      Voxel3DMeta::update(xnum,ynum,znum);
      _unit = unit;
    }
    /// Clear method
    void clear();
    /// Checker if the meta parameters are set properly or not
    inline bool valid() const { return _valid; }
    /// Returns size
    inline VoxelID_t size() const { return _num_element; }
    inline VoxelID_t id(const Point3D& pt) const
    { return id(pt.x, pt.y, pt.z); }
    /// Given a position, returns voxel ID
    VoxelID_t id(double x, double y, double z) const;
    /// Return value of invalid voxel ID:
    VoxelID_t invalid_voxel_id() const{return kINVALID_VOXELID;}
    /// Given a valid voxel ID, returns a position array
    Point3D position(VoxelID_t id) const;
    /// Given a valid voxel ID, returns X position
    double pos_x(VoxelID_t id) const;
    /// Given a valid voxel ID, returns Y position
    double pos_y(VoxelID_t id) const;
    /// Given a valid voxel ID, returns Z position
    double pos_z(VoxelID_t id) const;
    /// Returns voxel count along x-axis
    inline size_t num_voxel_x()  const { return _xnum; }
    /// Returns voxel count along y-axis
    inline size_t num_voxel_y()  const { return _ynum; }
    /// Returns voxel count along z-axis
    inline size_t num_voxel_z()  const { return _znum; }
    /// Returns voxel size along x-axis;
    inline double size_voxel_x() const { return _xlen; }
    /// Returns voxel size along y-axis;
    inline double size_voxel_y() const { return _ylen; }
    /// Returns voxel size along z-axis;
    inline double size_voxel_z() const { return _zlen; }
    /// Distance unit
    inline DistanceUnit_t unit() const { return _unit; }
    /// text dumper
    std::string  dump() const;
    
  private:

    bool   _valid; ///< Boolean set to true only if voxel parameters are properly set
    size_t _num_element; ///< Total number of voxel elements
    
    double _xlen; ///< X voxel size in [cm]
    double _ylen; ///< Y voxel size in [cm]
    double _zlen; ///< Z voxel size in [cm]
    
    size_t _xnum; ///< Number of voxels along X
    size_t _ynum; ///< Number of voxels along Y
    size_t _znum; ///< Number of voxels along Z

    DistanceUnit_t _unit; ///< length unit
  };  
}

#endif
/** @} */ // end of doxygen group 

