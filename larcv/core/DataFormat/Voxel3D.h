/**
 * \file Voxel3D.h
 *
 * \ingroup core_DataFormat
 *
 * \brief Class def header for 3D specific extension of voxels
 *
 * @author kazuhiro
 */

/** \addtogroup core_DataFormat

    @{*/
#ifndef LARCV_VOXEL3D_H
#define LARCV_VOXEL3D_H

#include "Voxel.h"
#include "Voxel3DMeta.h"
namespace larcv {

  /**
     \class SparseTensor3D
     @brief Container of multiple (3D-projected) voxel set array
  */
  class SparseTensor3D : public VoxelSet {
  public:
    /// Default ctor
    SparseTensor3D() {}
    SparseTensor3D(VoxelSet&& vs, Voxel3DMeta meta)
      : VoxelSet(std::move(vs))
      , _meta(meta)
    {}
    /// Default dtor
    virtual ~SparseTensor3D() {}

    //
    // Read-access
    //
    /// Access Voxel3DMeta of specific projection
    inline const larcv::Voxel3DMeta& meta() const { return _meta; }

    //
    // Write-access
    //
    void emplace(const double x, const double y, const double z, const float val, const bool add=true);
    /// Clear everything
    inline void clear() { VoxelSet::clear(); _meta = Voxel3DMeta(); }
    /// Meta setter
    inline void meta(const larcv::Voxel3DMeta& meta) { _meta = meta; }

  private:
    larcv::Voxel3DMeta _meta;
    
  };

  /**
     \class ClusterVoxel3D
     @brief Container of multiple (3D-projected) voxel set array
  */
  class ClusterVoxel3D : public VoxelSetArray {
  public:
    /// Default ctor
    ClusterVoxel3D() {}
    /// Default dtor
    virtual ~ClusterVoxel3D() {}

    //
    // Read-access
    //
    /// Access Voxel3DMeta of specific projection
    inline const larcv::Voxel3DMeta& meta() const { return _meta; }

    //
    // Write-access
    //
    /// Clear everything
    inline void clear() { VoxelSetArray::clear(); _meta = Voxel3DMeta(); }
    /// Meta setter
    inline void meta(const larcv::Voxel3DMeta& meta) { _meta = meta; }

  private:
    larcv::Voxel3DMeta _meta;
  };  

}

#endif
/** @} */ // end of doxygen group

