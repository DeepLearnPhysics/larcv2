/**
 * \file Voxel.h
 *
 * \ingroup core_DataFormat
 *
 * \brief Class def header for a class larcv::Voxel
 *
 * @author kazuhiro
 */

/** \addtogroup core_DataFormat

    @{*/
#ifndef VOXEL3D_H
#define VOXEL3D_H

#include "DataFormatTypes.h"
#include "Image2D.h"
namespace larcv {

  /**
     \class Voxel
     @brief 3D voxel definition element class consisting of ID and stored value
  */
  class Voxel {

  public:

    /// Default constructor
    Voxel(VoxelID_t id = kINVALID_VOXELID, float value = kINVALID_FLOAT);
    /// Default destructor
    ~Voxel() {}

    /// ID getter
    inline VoxelID_t id() const { return _id; }
    /// Value getter
    inline float  value() const { return _value; }

    /// Value setter
    inline void set(VoxelID_t id, float value) { _id = id; _value = value; }

    //
    // uniry operators
    //
    inline Voxel& operator += (float value)
    { _value += value; return (*this); }
    inline Voxel& operator -= (float value)
    { _value -= value; return (*this); }
    inline Voxel& operator *= (float factor)
    { _value *= factor; return (*this); }
    inline Voxel& operator /= (float factor)
    { _value /= factor; return (*this); }

    //
    // binary operators
    //
    inline bool operator == (const Voxel& rhs) const
    { return (_id == rhs._id); }
    inline bool operator <  (const Voxel& rhs) const
    {
      if ( _id < rhs._id) return true;
      if ( _id > rhs._id) return false;
      return false;
    }
    inline bool operator <= (const Voxel& rhs) const
    { return  ((*this) == rhs || (*this) < rhs); }
    inline bool operator >  (const Voxel& rhs) const
    { return !((*this) <= rhs); }
    inline bool operator >= (const Voxel& rhs) const
    { return !((*this) <  rhs); }

    inline bool operator == (const float& rhs) const
    { return _value == rhs; }
    inline bool operator <  (const float& rhs) const
    { return _value <  rhs; }
    inline bool operator <= (const float& rhs) const
    { return _value <= rhs; }
    inline bool operator >  (const float& rhs) const
    { return _value >  rhs; }
    inline bool operator >= (const float& rhs) const
    { return _value >= rhs; }

  private:
    VoxelID_t _id; ///< voxel id
    float  _value; ///< Pixel Value
  };

  /**
     \class VoxelSet
     @brief Container of multiple voxels consisting of ordered sparse vector and meta data
   */
  class VoxelSet {
  public:
    /// Default ctor
    VoxelSet() {}
    /// Default dtor
    ~VoxelSet() {}
    /// getter
    inline const std::vector<larcv::Voxel>& voxel_array() const
    { return _voxel_v; }
    /// clear
    inline void clear() { _voxel_v.clear(); }
    /// adder
    void add(const Voxel& vox);
    /// adder
    void emplace(Voxel&& vox);
    /// id setter
    inline void id(const InstanceID_t id) { _id = id; }
    /// id getter
    inline InstanceID_t id() const { return _id; }

    inline VoxelSet& operator += (float value)
    { for(auto& vox : _voxel_v) vox += value; return (*this); }
    inline VoxelSet& operator -= (float value)
    { for(auto& vox : _voxel_v) vox -= value; return (*this); }
    inline VoxelSet& operator *= (float factor)
    { for(auto& vox : _voxel_v) vox *= factor; return (*this); }
    inline VoxelSet& operator /= (float factor)
    { for(auto& vox : _voxel_v) vox /= factor; return (*this); }

  private:
    /// Instance ID
    InstanceID_t _id;
    /// Ordered sparse vector of voxels
    std::vector<larcv::Voxel> _voxel_v;
  };

  /**
     \class VoxelSetArray
     @brief Container of multiple VoxelSet (i.e. container w/ InstanceID_t & VoxelSet pairs)
  */
  class VoxelSetArray {
  public:
    /// Default ctor
    VoxelSetArray() {}
    /// Default dtor
    ~VoxelSetArray() {}

    inline void clear() { _voxel_vv.clear(); }

    inline size_t size() const { return _voxel_vv.size(); }

    const larcv::VoxelSet& get_voxel_set(InstanceID_t id) const;

    inline const std::vector<larcv::VoxelSet>& get_voxel_set_array() const
    { return _voxel_vv; }

    inline void resize(const size_t num)
    { _voxel_vv.resize(num); for(size_t i=0; i<num; ++i) _voxel_vv[i].id(i); }

    larcv::VoxelSet& writeable_voxel_set(const InstanceID_t id);

    inline void emplace(std::vector<larcv::VoxelSet>&& voxel_vv)
    { _voxel_vv = std::move(voxel_vv); }

    inline void set(const std::vector<larcv::VoxelSet>& voxel_vv)
    { _voxel_vv = voxel_vv; }

    void emplace(larcv::VoxelSet&& voxel_v, InstanceID_t id);

    void set(const larcv::VoxelSet& voxel_v, InstanceID_t id);

  private:
    std::vector<larcv::VoxelSet> _voxel_vv;
  };

  /**
     \class VoxelSetArray2D
     @brief Container of multiple (2D-projected) voxel set array
  */
  class VoxelSetArray2D {
  public:
    /// Default ctor
    VoxelSetArray2D() {}
    /// Default dtor
    ~VoxelSetArray2D() {}

    inline void clear() { _voxel_vvv.clear(); }

    const larcv::VoxelSet& get_voxel_set(ProjectionID_t p_id, InstanceID_t i_id) const;

    const larcv::ImageMeta& get_meta(ProjectionID_t p_id) const;

    inline const std::vector<larcv::VoxelSetArray>& get_voxel_set_array2d() const
    { return _voxel_vvv; }

    void emplace(std::vector<larcv::VoxelSetArray>&& voxel_vvv,
                 std::vector<larcv::ImageMeta>&& meta_v);

    void set(const std::vector<larcv::VoxelSetArray>& voxel_vvv,
             const std::vector<larcv::ImageMeta>& meta_v);

    void emplace(larcv::VoxelSetArray&& voxel_vv, larcv::ImageMeta&& meta);

    void set(const larcv::VoxelSetArray& voxel_vv, const larcv::ImageMeta& meta);

    void emplace(larcv::VoxelSetArray2D&& target)
    { _voxel_vvv = std::move(target._voxel_vvv); _meta_v = std::move(target._meta_v); }

  private:
    std::vector<larcv::VoxelSetArray> _voxel_vvv;
    std::vector<larcv::ImageMeta> _meta_v;
  };  

}

#endif
/** @} */ // end of doxygen group

