/**
 * \file EventBBox.h
 *
 * \ingroup core_DataFormat
 * 
 * \brief Class def header for a class larcv::EventBBox2D and larcv::EventBBox3D
 *
 * @author kazuhiro
 */

/** \addtogroup core_DataFormat

    @{*/
#ifndef EVENTBBOX_H
#define EVENTBBOX_H

#include <iostream>
#include "EventBase.h"
#include "BBox.h"
#include "DataProductFactory.h"

namespace larcv {
  /**
    \class EventBBox2D
    Event-wise class to store a collection of larcv::BBox
  */
  class EventBBox2D : public EventBase,
		      public std::vector<larcv::BBox2D> {
    
  public:

    /// Default constructor
    EventBBox2D(){}
    
    /// Default destructor
    ~EventBBox2D(){}

    /// larcv::BBox2D array clearer
    inline void clear()
    { EventBase::clear(); std::vector<larcv::BBox2D>::clear(); }

    /// Units
    DistanceUnit_t unit;

    /// Description (optional)
    std::string info;
  };

  /**
    \class EventBBox3D
    Event-wise class to store a collection of larcv::BBox
  */
  class EventBBox3D : public EventBase,
		      public std::vector<larcv::BBox3D> {
    
  public:

    /// Default constructor
    EventBBox3D(){}
    
    /// Default destructor
    ~EventBBox3D(){}

    /// larcv::BBox3D array clearer
    inline void clear()
    { EventBase::clear(); std::vector<larcv::BBox3D>::clear(); }

    /// Units
    DistanceUnit_t unit;

    /// Description (optional)
    std::string info;

  };
}

#include "IOManager.h"
namespace larcv {

  // Template instantiation for IO
  template<> inline std::string product_unique_name<larcv::EventBBox2D>() { return "bbox2d"; }
  template EventBBox2D& IOManager::get_data<larcv::EventBBox2D>(const std::string&);
  template EventBBox2D& IOManager::get_data<larcv::EventBBox2D>(const ProducerID_t);

  /**
     \class larcv::EventBBox2D
     \brief A concrete factory class for larcv::EventBBox2D
  */
  class EventBBox2DFactory : public DataProductFactoryBase {
  public:
    /// ctor
    EventBBox2DFactory()
    { DataProductFactory::get().add_factory(product_unique_name<larcv::EventBBox2D>(),this); }
    /// dtor
    ~EventBBox2DFactory() {}
    /// create method
    EventBase* create() { return new EventBBox2D; }
  };

  // Template instantiation for IO
  template<> inline std::string product_unique_name<larcv::EventBBox3D>() { return "bbox3d"; }
  template EventBBox3D& IOManager::get_data<larcv::EventBBox3D>(const std::string&);
  template EventBBox3D& IOManager::get_data<larcv::EventBBox3D>(const ProducerID_t);

  /**
     \class larcv::EventBBox3D
     \brief A concrete factory class for larcv::EventBBox3D
  */
  class EventBBox3DFactory : public DataProductFactoryBase {
  public:
    /// ctor
    EventBBox3DFactory()
    { DataProductFactory::get().add_factory(product_unique_name<larcv::EventBBox3D>(),this); }
    /// dtor
    ~EventBBox3DFactory() {}
    /// create method
    EventBase* create() { return new EventBBox3D; }
  };

  
}
#endif
/** @} */ // end of doxygen group 

