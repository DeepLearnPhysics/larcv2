/**
 * \file EventChStatus.h
 *
 * \ingroup core_DataFormat
 * 
 * \brief Class def header for a class larcv::EventChStatus
 *
 * @author kazuhiro
 */

/** \addtogroup core_DataFormat

    @{*/
#ifndef __LARCV_EVENTCHSTATUS_H__
#define __LARCV_EVENTCHSTATUS_H__

#include <iostream>
#include <map>
#include "EventBase.h"
#include "ChStatus.h"
#include "DataProductFactory.h"

namespace larcv {
  
  /**
    \class EventChStatus
    \brief Event-wise class to store a collection of larcv::ChStatus (one per plane)
  */
  class EventChStatus : public EventBase {
    
  public:
    
    /// Default constructor
    EventChStatus(){}
    
    /// Default destructor
    virtual ~EventChStatus(){}

    /// Clears an array of larcv::ChStatus
    void clear();

    /// Getter for a const reference of core data
    const std::map<larcv::ProjectionID_t,larcv::ChStatus>& chstatus_map() const { return _status_m; }

    /// Get larcv::ChStatus for a specified plane
    const ChStatus& status(larcv::ProjectionID_t id) const;

    /// Insert larcv::ChStatus into a collection
    void insert(const ChStatus& status);
    
#ifndef __CINT__
    /// Emplace larcv::ChStatus into a collection
    void emplace(ChStatus&& status);
#endif

    /// Dump contents in string representation
    std::string dump() const;
    
  private:

    std::map< ::larcv::ProjectionID_t, ::larcv::ChStatus > _status_m;

  };
}

#include "IOManager.h"
namespace larcv {

  // Template instantiation for IO
  template<> inline std::string product_unique_name<larcv::EventChStatus>() { return "chstatus"; }
  template EventChStatus& IOManager::get_data<larcv::EventChStatus>(const std::string&);
  template EventChStatus& IOManager::get_data<larcv::EventChStatus>(const ProducerID_t);

  /**
     \class larcv::EventChStatus
     \brief A concrete factory class for larcv::EventChStatus
  */
  class EventChStatusFactory : public DataProductFactoryBase {
  public:
    /// ctor
    EventChStatusFactory()
    { DataProductFactory::get().add_factory(product_unique_name<larcv::EventChStatus>(),this); }
    /// dtor
    ~EventChStatusFactory() {}
    /// create method
    EventBase* create() { return new EventChStatus; }
  };

}

#endif
/** @} */ // end of doxygen group 

