/**
 * \file EventVoxel3D.h
 *
 * \ingroup core_DataFormat
 * 
 * \brief Class def header for a class larcv::EventVoxel3D
 *
 * @author kazuhiro
 */

/** \addtogroup core_DataFormat

    @{*/
#ifndef EVENTVOXEL3D_H
#define EVENTVOXEL3D_H

#include <iostream>
#include "EventBase.h"
#include "Voxel.h"
#include "Voxel3DMeta.h"
#include "DataProductFactory.h"
namespace larcv {
  /**
    \class EventVoxel3D
    Event-wise class to store a collection of larcv::Voxel3D
  */
  class EventVoxel3D : public EventBase,
		       public VoxelSet {
    
  public:
    
    /// Default constructor
    EventVoxel3D(){}
    
    /// Default destructor
    ~EventVoxel3D(){}

    /// EventBase::clear() override
    void clear();

    /// Meta getter
    inline const Voxel3DMeta& Meta() const
    { return _meta; }
    
    /// Meta setter
    inline void Meta(const Voxel3DMeta& meta)
    { VoxelSet::Clear(); _meta = meta; }

  private:
    Voxel3DMeta _meta;

  };
}

#include "IOManager.h"
namespace larcv {

  // Template instantiation for IO
  template<> inline std::string product_unique_name<larcv::EventVoxel3D>() { return "voxel3d"; }
  template EventVoxel3D& IOManager::get_data<larcv::EventVoxel3D>(const std::string&);
  template EventVoxel3D& IOManager::get_data<larcv::EventVoxel3D>(const ProducerID_t);

  /**
     \class larcv::EventVoxel3D
     \brief A concrete factory class for larcv::EventVoxel3D
  */
  class EventVoxel3DFactory : public DataProductFactoryBase {
  public:
    /// ctor
    EventVoxel3DFactory()
    { DataProductFactory::get().add_factory(product_unique_name<larcv::EventVoxel3D>(),this); }
    /// dtor
    ~EventVoxel3DFactory() {}
    /// create method
    EventBase* create() { return new EventVoxel3D; }
  };

  
}
#endif
/** @} */ // end of doxygen group 

