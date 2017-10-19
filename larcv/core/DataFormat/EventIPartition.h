/**
 * \file EventIPartition.h
 *
 * \ingroup DataFormat
 * 
 * \brief Class def header for a class EventIPartition
 *
 * @author kazuhiro
 */

/** \addtogroup DataFormat

    @{*/
#ifndef EVENTIPARTITION_H
#define EVENTIPARTITION_H

#include "EventBase.h"
#include "IPartition.h"
#include "DataProductFactory.h"

namespace larcv {
  /**
    \class EventIPartition
    User-defined data product class (please comment!)
  */
  class EventIPartition : public EventBase,
			  public std::vector<larcv::IPartition> {
    
  public:
    
    /// Default constructor
    EventIPartition(){}
    
    /// Default destructor
    ~EventIPartition(){}

    /// Data clear method
    inline void clear()
    { EventBase::clear(); std::vector<larcv::IPartition>::clear(); }

  };
}

#include "IOManager.h"
namespace larcv {

  // Template instantiation for IO
  template<> inline std::string product_unique_name<larcv::EventIPartition>() { return "ipart"; }
  template EventIPartition& IOManager::get_data<larcv::EventIPartition>(const std::string&);
  template EventIPartition& IOManager::get_data<larcv::EventIPartition>(const ProducerID_t);

  /**
     \class larcv::EventIPartition
     \brief A concrete factory class for larcv::EventIPartition
  */
  class EventIPartitionFactory : public DataProductFactoryBase {
  public:
    /// ctor
    EventIPartitionFactory()
    { DataProductFactory::get().add_factory(product_unique_name<larcv::EventIPartition>(),this); }
    /// dtor
    ~EventIPartitionFactory() {}
    /// create method
    EventBase* create() { return new EventIPartition; }
  };

  
}
#endif
/** @} */ // end of doxygen group 

