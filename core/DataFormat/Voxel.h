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
namespace larcv {

  /**
     \class Voxel
     @brief 3D voxel definition element class consisting of ID and stored value
  */
  class Voxel{
    
  public:
    
    /// Default constructor
    Voxel(VoxelID_t id=kINVALID_VOXELID, float value=kINVALID_FLOAT);
    /// Default destructor
    ~Voxel(){}
    
    /// ID getter
    inline VoxelID_t ID() const { return _id; }
    /// Value getter
    inline float  Value() const { return _value; }

    /// Value setter
    inline void Set(VoxelID_t id, float value) { _id = id; _value = value; }

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
      if( _id < rhs._id) return true;
      if( _id > rhs._id) return false;
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
    VoxelSet(){}
    /// Default dtor
    ~VoxelSet(){}
    /// getter
    inline const std::vector<larcv::Voxel>& VoxelArray() const
    { return _voxel_v; }
    /// clear
    inline void Clear() { _voxel_v.clear(); }
    /// adder
    void Add(const Voxel& vox);
    /// adder
    void Emplace(Voxel&& vox);
  private:
    /// Ordered sparse vector of voxels 
    std::vector<larcv::Voxel> _voxel_v;
  };

  
}

#endif
/** @} */ // end of doxygen group 

