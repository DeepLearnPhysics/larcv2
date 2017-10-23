/**
 * \file EventVoxelPartition.h
 *
 * \ingroup DataFormat
 * 
 * \brief Class def header for a class EventVoxelPartition
 *
 * @author kazuhiro
 */

/** \addtogroup DataFormat

    @{*/
#ifndef EVENTVOXELPARTITION_H
#define EVENTVOXELPARTITION_H

#include "EventBase.h"
#include "VoxelPartition.h"
#include "Voxel3DMeta.h"
#include "DataProductFactory.h"

namespace larcv {
  /**
    \class EventVoxelPartition
    User-defined data product class (please comment!)
  */
  class EventVoxelPartition : public EventBase,
			  public VoxelPartitionSet {
    
  public:
    
    /// Default constructor
    EventVoxelPartition(){}
    
    /// Default destructor
    ~EventVoxelPartition(){}

    /// Data clear method
    inline void clear()
    { EventBase::clear(); VoxelPartitionSet::clear(); _meta.clear(); }

    /// Meta getter
    inline const Voxel3DMeta& meta() const
    { return _meta; }

    /// Meta setter
    inline void meta(const Voxel3DMeta& meta)
    { VoxelPartitionSet::clear(); _meta = meta; }

  private:
    Voxel3DMeta _meta;

  };
}

#include "IOManager.h"
namespace larcv {

  // Template instantiation for IO
  template<> inline std::string product_unique_name<larcv::EventVoxelPartition>() { return "vpartition"; }
  template EventVoxelPartition& IOManager::get_data<larcv::EventVoxelPartition>(const std::string&);
  template EventVoxelPartition& IOManager::get_data<larcv::EventVoxelPartition>(const ProducerID_t);

  /**
     \class larcv::EventVoxelPartition
     \brief A concrete factory class for larcv::EventVoxelPartition
  */
  class EventVoxelPartitionFactory : public DataProductFactoryBase {
  public:
    /// ctor
    EventVoxelPartitionFactory()
    { DataProductFactory::get().add_factory(product_unique_name<larcv::EventVoxelPartition>(),this); }
    /// dtor
    ~EventVoxelPartitionFactory() {}
    /// create method
    EventBase* create() { return new EventVoxelPartition; }
  };

  
}
#endif
/** @} */ // end of doxygen group 

