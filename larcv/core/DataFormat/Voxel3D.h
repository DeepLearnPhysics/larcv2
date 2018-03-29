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
    SparseTensor3D(VoxelSet&& vs, Voxel3DMeta meta);
    /// Default dtor
    virtual ~SparseTensor3D() {}
    SparseTensor3D& operator= (const VoxelSet& rhs)
    { *((VoxelSet*)this) = rhs; this->meta(this->meta()); return *this;}

    //
    // Read-access
    //
    /// Access Voxel3DMeta of specific projection
    inline const larcv::Voxel3DMeta& meta() const { return _meta; }

    //
    // Write-access
    //
    /// Create & add/set a single voxel
    void emplace(const double x, const double y, const double z, const float val, const bool add=true);
    /// Emplace the whole voxel set w/ meta
    inline void emplace(VoxelSet&& vs, const Voxel3DMeta& meta)
    {*((VoxelSet*)this) = std::move(vs); this->meta(meta);}
    /// Set the whole voxel set w/ meta
    inline void set(const VoxelSet& vs, const Voxel3DMeta& meta)
    {*((VoxelSet*)this) = vs; this->meta(meta);} 
    /// Clear everything
    inline void clear_data() { VoxelSet::clear_data(); _meta = Voxel3DMeta(); }
    /// Meta setter
    void meta(const larcv::Voxel3DMeta& meta);

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
    inline void clear_data() { VoxelSetArray::clear_data(); _meta = Voxel3DMeta(); }
    /// set VoxelSetArray
    inline void set(VoxelSetArray&& vsa, const Voxel3DMeta& meta)
    { *((VoxelSetArray*)this) = std::move(vsa); this->meta(meta); }
    /// emplace VoxelSetArray
    inline void emplace(VoxelSetArray&& vsa, const Voxel3DMeta& meta)
    { *((VoxelSetArray*)this) = vsa; this->meta(meta); }
    /// Meta setter
    void meta(const larcv::Voxel3DMeta& meta);

  private:
    larcv::Voxel3DMeta _meta;
  };  

}

#endif
/** @} */ // end of doxygen group

