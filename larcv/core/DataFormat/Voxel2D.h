/**
 * \file Voxel2D.h
 *
 * \ingroup core_DataFormat
 *
 * \brief Class def header for 2D specific extension of voxels
 *
 * @author kazuhiro
 */

/** \addtogroup core_DataFormat

    @{*/
#ifndef LARCV_VOXEL2D_H
#define LARCV_VOXEL2D_H

#include "Voxel.h"
#include "ImageMeta.h"
namespace larcv {

  /**
     \class SparseTensor2D
     @brief Container of multiple (2D-projected) voxel set array
  */
  class SparseTensor2D : public VoxelSet {
  public:
    /// Default ctor
    SparseTensor2D() {}
    /// Default dtor
    virtual ~SparseTensor2D() {}
    /// copy ctor w/ VoxelSet
    SparseTensor2D(const larcv::VoxelSet& vs)
      : VoxelSet(vs)
    {}
    /// move ctor
    SparseTensor2D(larcv::VoxelSet&& vs, larcv::ImageMeta&& meta);

    //
    // Read-access
    //
    /// Access ImageMeta of specific projection
    inline const larcv::ImageMeta& meta() const { return _meta; }

    //
    // Write-access
    //
    /// Clear everything
    inline void clear_data() { VoxelSet::clear_data(); _meta = ImageMeta(); }
    /// Meta setter
    void meta(const larcv::ImageMeta& meta);

  private:
    larcv::ImageMeta _meta;
    
  };

  /**
     \class ClusterPixel2D
     @brief Container of multiple (2D-projected) voxel set array
  */
  class ClusterPixel2D : public VoxelSetArray {
  public:
    /// Default ctor
    ClusterPixel2D() {}
    /// Default dtor
    virtual ~ClusterPixel2D() {}
    /// Copy ctor w/ VoxelSetArray
    ClusterPixel2D(const VoxelSetArray& vsa)
      : VoxelSetArray(vsa)
    {}
    /// move ctor
    ClusterPixel2D(larcv::VoxelSetArray&& vsa, larcv::ImageMeta meta);

    //
    // Read-access
    //
    /// Access ImageMeta of specific projection
    inline const larcv::ImageMeta& meta() const { return _meta; }

    //
    // Write-access
    //
    /// Clear everything
    inline void clear_data() { VoxelSetArray::clear_data(); _meta = ImageMeta(); }
    /// Meta setter
    void meta(const larcv::ImageMeta& meta);

  private:
    larcv::ImageMeta _meta;
  };  

}

#endif
/** @} */ // end of doxygen group

