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
#include "utils.h"
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
    /// Merge another SparseTensor3D
    inline void merge(const SparseTensor3D& vs, bool add=true, bool check_meta_strict=true)
    { 
      if(!(this->meta().valid()))
        this->meta(vs.meta());
      else if(check_meta_strict) {
        if(vs.meta() != this->meta()) {
          std::cerr << "Meta mismatched (strict check)!" << std::endl
                    << this->meta().dump() << std::endl
                    << vs.meta().dump() << std::endl;
          throw std::exception();
        }
      }
      else if(this->meta().num_voxel_x() != vs.meta().num_voxel_x() || 
        this->meta().num_voxel_y() != vs.meta().num_voxel_y() || 
        this->meta().num_voxel_z() != vs.meta().num_voxel_z() ) {
          std::cerr << "Meta mismatched (loose check)!" << std::endl
                    << this->meta().dump() << std::endl
                    << vs.meta().dump() << std::endl;
          throw std::exception(); 
      }
      for(auto const& v : vs.as_vector()) this->emplace(v.id(),v.value(),add);
    }
    /// Emplace a new voxel from id & value
    inline void emplace(VoxelID_t id, float value, const bool add)
    { VoxelSet::emplace(id, value, add); }
    /// Set the whole voxel set w/ meta
    inline void set(const VoxelSet& vs, const Voxel3DMeta& meta)
    {*((VoxelSet*)this) = vs; this->meta(meta);}
    /// Clear everything
    inline void clear_data() { VoxelSet::clear_data(); _meta = Voxel3DMeta(); }
    /// Meta setter
    void meta(const larcv::Voxel3DMeta& meta);
    /// Returns a const reference to voxel closest to a voxel with specified id. if no such voxel within distance, return invalid voxel.
    const Voxel& close(VoxelID_t id, double distance) const;
    /// Whether a voxel belongs to this VoxelSet or not, within some distance threshold
    bool within(VoxelID_t id, double distance) const;
    /// Compute PCA of this set of voxels
  	Point3D pca() const;
    PCA_3D fit_pca(bool store_spread=true, bool use_true_coord=false);

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
    inline void set(VoxelSetArray& vsa, const Voxel3DMeta& meta)
    { *((VoxelSetArray*)this) = vsa; this->meta(meta); }
    /// emplace VoxelSetArray
    inline void emplace(VoxelSetArray&& vsa, const Voxel3DMeta& meta)
    { *((VoxelSetArray*)this) = std::move(vsa); this->meta(meta); }
    /// Merge another ClusterVoxel3D
    inline void merge(const ClusterVoxel3D& vsa, bool check_meta_strict=true)
    { 
      if(!(this->meta().valid()))
        this->meta(vsa.meta());
      else if(check_meta_strict) {
        if(vsa.meta() != this->meta()) {
          std::cerr << "Meta mismatched (strict check)!" << std::endl
                    << this->meta().dump() << std::endl
                    << vsa.meta().dump() << std::endl;
          throw std::exception();
        }
      }
      else if(this->meta().num_voxel_x() != vsa.meta().num_voxel_x() || 
        this->meta().num_voxel_y() != vsa.meta().num_voxel_y() || 
        this->meta().num_voxel_z() != vsa.meta().num_voxel_z() ) {
        if(vsa.meta() != this->meta()) {
          std::cerr << "Meta mismatched (loose check)!" << std::endl
                    << this->meta().dump() << std::endl
                    << vsa.meta().dump() << std::endl;
          throw std::exception(); 
        }
      }
      for(auto vs : vsa.as_vector()) {
        vs.id(larcv::kINVALID_INSTANCEID);
        ((VoxelSetArray*)this)->emplace(std::move(vs));
      } 
    }
    /// Meta setter
    void meta(const larcv::Voxel3DMeta& meta);

  private:
    larcv::Voxel3DMeta _meta;
  };

}

#endif
/** @} */ // end of doxygen group
