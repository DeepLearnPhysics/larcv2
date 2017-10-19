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
#include <array>

namespace larcv {

  /**
     \class Voxel3DMeta
     @brief Meta data for defining voxels (ID, size, position) and voxelized volume (coordinate, size)
  */
  class Voxel3DMeta {
  public:
    /// Default ctor
    Voxel3DMeta();
    /// Default dtor
    ~Voxel3DMeta(){}

    /// Define dimensions
    void Set(double xmin, double xmax,
	     double ymin, double ymax,
	     double zmin, double zmax,
	     size_t xnum,
	     size_t ynum,
	     size_t znum);
    /// Clear method
    void Clear();
    /// Checker if the meta parameters are set properly or not
    inline bool Valid() const { return _valid; }
    /// Returns size
    inline VoxelID_t Size() const { return _num_element; }
    /// Given a position, returns voxel ID
    VoxelID_t ID(double x, double y, double z) const;
    /// Given a valid voxel ID, returns a position array
    const std::array<double,3> Position(VoxelID_t id) const;
    /// Given a valid voxel ID, returns X position
    double X(VoxelID_t id) const;
    /// Given a valid voxel ID, returns Y position
    double Y(VoxelID_t id) const;
    /// Given a valid voxel ID, returns Z position
    double Z(VoxelID_t id) const;
    /// Returns voxel count along x-axis
    inline size_t NumVoxelX()  const { return _xnum; }
    /// Returns voxel count along y-axis
    inline size_t NumVoxelY()  const { return _ynum; }
    /// Returns voxel count along z-axis
    inline size_t NumVoxelZ()  const { return _znum; }
    /// Returns voxel size along x-axis;
    inline double SizeVoxelX() const { return _xlen; }
    /// Returns voxel size along y-axis;
    inline double SizeVoxelY() const { return _ylen; }
    /// Returns voxel size along z-axis;
    inline double SizeVoxelZ() const { return _zlen; }
    /// Returns voxel definition maximum x value
    inline double MaxX() const { return _xmax; }
    /// Returns voxel definition maximum y value
    inline double MaxY() const { return _ymax; }
    /// Returns voxel definition maximum z value
    inline double MaxZ() const { return _zmax; }
    /// Returns voxel definition minimum x value
    inline double MinX() const { return _xmin; }
    /// Returns voxel definition minimum y value
    inline double MinY() const { return _ymin; }
    /// Returns voxel definition minimum z value
    inline double MinZ() const { return _zmin; }
    /// text dumper
    std::string  Dump() const;
    
  private:

    bool   _valid; ///< Boolean set to true only if voxel parameters are properly set
    size_t _num_element; ///< Total number of voxel elements
    
    double _xmin; ///< X min value in voxel definition in [cm]
    double _xmax; ///< X max value in voxel definition in [cm]
    double _xlen; ///< X voxel size in [cm]
    
    double _ymin; ///< Y min value in voxel definition in [cm]
    double _ymax; ///< Y min value in voxel definition in [cm]
    double _ylen; ///< Y voxel size in [cm]
    
    double _zmin; ///< Z min value in voxel definition in [cm]
    double _zmax; ///< Z min value in voxel definition in [cm]
    double _zlen; ///< Z voxel size in [cm]
    
    size_t _xnum; ///< Number of voxels along X
    size_t _ynum; ///< Number of voxels along Y
    size_t _znum; ///< Number of voxels along Z
  };  
}

#endif
/** @} */ // end of doxygen group 

