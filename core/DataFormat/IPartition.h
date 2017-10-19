/**
 * \file IPartition.h
 *
 * \ingroup core_DataFormat
 * 
 * \brief Class def header for a class IPartition
 *
 * @author kazuhiro
 */

/** \addtogroup core_DataFormat

    @{*/
#ifndef INSTANCEPARTITION_H
#define INSTANCEPARTITION_H

#include <iostream>
//#include <array>
#include "DataFormatTypes.h"
namespace larcv {

  /**
     \class IPartition
     User defined class IPartition ... these comments are used to generate
     doxygen documentation!
  */
  class IPartition{
  public:
    static const size_t PARTITION_SIZE = 6;
    
  public:
    
    /// Default constructor
    IPartition()
    {
      for(size_t i=0; i<PARTITION_SIZE; ++i)
	{_id_array[i] = kINVALID_INSTANCEID; _fraction_array[i]=0.;}
    }
    
    /// Default destructor
    ~IPartition(){}

    //inline const std::array<InstanceID_t,PARTITION_SIZE>& IDs() const
    inline const InstanceID_t* IDs() const
    { return _id_array; }

    //inline const std::array<float,PARTITION_SIZE>& Fractions() const
    inline const float* Fractions() const
    { return _fraction_array; }

    inline float Fraction(const InstanceID_t id) const
    {
      static size_t i=0;
      for(i=0; i<PARTITION_SIZE; ++i)
	if(_id_array[i]==id) return _fraction_array[i];
      return 0;
    }

  private:
    
    //std::array<InstanceID_t,larcv::IPartition::PARTITION_SIZE> _id_array;
    //std::array<float,larcv::IPartition::PARTITION_SIZE> _fraction_array;
    InstanceID_t _id_array[PARTITION_SIZE];
    float _fraction_array[PARTITION_SIZE];
  };
}

#endif
/** @} */ // end of doxygen group 

