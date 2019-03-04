/**
 * \file EventVoxel3D.h
 *
 * \ingroup core_DataFormat
 *
 * \brief Class def header for a class larcv::EventSparseTensor3D and larcv::EventClusterPixel3D
 *
 * @author kazuhiro
 */

/** \addtogroup core_DataFormat

    @{*/
#ifndef LARCV_EVENTVOXEL3D_H
#define LARCV_EVENTVOXEL3D_H

#include <iostream>
#include "EventBase.h"
#include "DataProductFactory.h"
#include "Voxel3D.h"
namespace larcv {

  /**
    \class EventClusterVoxel3D
    \brief Event-wise class to store a collection of VoxelSet (cluster) collection
  */
  class EventClusterVoxel3D : public EventBase,
			      public ClusterVoxel3D {

  public:

    /// Default constructor
    EventClusterVoxel3D() {}

    /// Default destructor
    virtual ~EventClusterVoxel3D() {}

    /// EventBase::clear() override
    inline void clear() {EventBase::clear(); ClusterVoxel3D::clear_data();}
    
  };

  /**
    \class EventSparseTensor3D
    \brief Event-wise class to store a collection of VoxelSet 
  */
  class EventSparseTensor3D : public EventBase,
			      public SparseTensor3D{

  public:

    /// Default constructor
    EventSparseTensor3D() {}

    /// Default destructor
    virtual ~EventSparseTensor3D() {}

    /// EventBase::clear() override
    inline void clear() {EventBase::clear(); SparseTensor3D::clear_data();}
    
  };

}

#include "IOManager.h"
namespace larcv {

  // Template instantiation for IO
  template<> inline std::string product_unique_name<larcv::EventClusterVoxel3D>() { return "cluster3d"; }
  template EventClusterVoxel3D& IOManager::get_data<larcv::EventClusterVoxel3D>(const std::string&);
  template EventClusterVoxel3D& IOManager::get_data<larcv::EventClusterVoxel3D>(const ProducerID_t);

  /**
     \class larcv::EventClusterVoxel3D
     \brief A concrete factory class for larcv::EventClusterVoxel3D
  */
  class EventClusterVoxel3DFactory : public DataProductFactoryBase {
  public:
    /// ctor
    EventClusterVoxel3DFactory()
    { DataProductFactory::get().add_factory(product_unique_name<larcv::EventClusterVoxel3D>(), this); }
    /// dtor
    ~EventClusterVoxel3DFactory() {}
    /// create method
    EventBase* create() { return new EventClusterVoxel3D; }
  };

  // Template instantiation for IO
  template<> inline std::string product_unique_name<larcv::EventSparseTensor3D>() { return "sparse3d"; }
  template EventSparseTensor3D& IOManager::get_data<larcv::EventSparseTensor3D>(const std::string&);
  template EventSparseTensor3D& IOManager::get_data<larcv::EventSparseTensor3D>(const ProducerID_t);

  /**
     \class larcv::EventSparseTensor3D
     \brief A concrete factory class for larcv::EventSparseTensor3D
  */
  class EventSparseTensor3DFactory : public DataProductFactoryBase {
  public:
    /// ctor
    EventSparseTensor3DFactory()
    { DataProductFactory::get().add_factory(product_unique_name<larcv::EventSparseTensor3D>(), this); }
    /// dtor
    ~EventSparseTensor3DFactory() {}
    /// create method
    EventBase* create() { return new EventSparseTensor3D; }
  };


}

#endif
/** @} */ // end of doxygen group

