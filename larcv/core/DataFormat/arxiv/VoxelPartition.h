/**
 * \file VoxelPartition.h
 *
 * \ingroup core_DataFormat
 *
 * \brief Class def header for a class VoxelPartition
 *
 * @author kazuhiro
 */

/** \addtogroup core_DataFormat

    @{*/
#ifndef VOXELPARTITION_H
#define VOXELPARTITION_H

#include <iostream>
//#include <array>
#include "DataFormatTypes.h"
namespace larcv {

  /**
     \class VoxelPartition
     User defined class VoxelPartition ... these comments are used to generate
     doxygen documentation!
  */
  class VoxelPartition {
  public:
    static const size_t PARTITION_SIZE = 6;

  public:

    /// Default constructor
    VoxelPartition(const VoxelID_t=kINVALID_VOXELID);

    /// Default destructor
    ~VoxelPartition() {}
    /// ID setter
    inline void set(VoxelID_t id) { _id = id; }
    /// Value setter: returns true if registered, false otherwise
    bool set(const InstanceID_t id, float value);
    /// VoxelID_t getter
    inline VoxelID_t id() const { return _id; }
    /// InstanceID_t getter
    inline const InstanceID_t* id_array() const
    { return _id_array; }
    /// Partition values getter
    inline const float* value_array() const
    { return _value_array; }
    /// A partition value for a specific instance getter
    float value(const InstanceID_t id) const;
    /// A fractional value for a specific instance getter
    float fraction(const InstanceID_t id) const;

    inline bool operator == (const VoxelPartition& rhs) const
    { return (_id == rhs._id); }
    inline bool operator <  (const VoxelPartition& rhs) const
    {
      if ( _id < rhs._id) return true;
      if ( _id > rhs._id) return false;
      return false;
    }
    inline bool operator <= (const VoxelPartition& rhs) const
    { return  ((*this) == rhs || (*this) < rhs); }
    inline bool operator >  (const VoxelPartition& rhs) const
    { return !((*this) <= rhs); }
    inline bool operator >= (const VoxelPartition& rhs) const
    { return !((*this) <  rhs); }

  private:
    VoxelID_t _id;
    InstanceID_t _id_array[PARTITION_SIZE];
    float _value_array[PARTITION_SIZE];
  };

  /**
     \class VoxelPartitionSet
     @brief Container of multiple voxels consisting of ordered sparse vector and meta data
   */
  class VoxelPartitionSet {
  public:
    /// Default ctor
    VoxelPartitionSet() {}
    /// Default dtor
    ~VoxelPartitionSet() {}
    /// getter
    inline const std::vector<larcv::VoxelPartition>& VoxelArray() const
    { return _voxel_v; }
    /// clear
    inline void clear() { _voxel_v.clear(); }
    /// adder
    void add(const VoxelPartition& vox);
    /// adder
    void emplace(VoxelPartition&& vox);
  private:
    /// Ordered sparse vector of voxels
    std::vector<larcv::VoxelPartition> _voxel_v;
  };

}

#endif
/** @} */ // end of doxygen group

