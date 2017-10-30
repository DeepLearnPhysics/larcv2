/**
 * \file EventMeta.h
 *
 * \ingroup DataFormat
 * 
 * \brief Class def header for a class EventMeta
 *
 * @author kazuhiro
 */

/** \addtogroup DataFormat

    @{*/
#ifndef LARCV_EVENTMETA_H
#define LARCV_EVENTMETA_H

#include "EventBase.h"
#include "DataProductFactory.h"
#include "Meta.h"

namespace larcv {
  /**
    \class EventMeta
    User-defined data product class (please comment!)
  */
  class EventMeta : public EventBase,
		    public larcv::Meta{
    
  public:
    
    /// Default constructor
    EventMeta(){}
    
    /// Default destructor
    ~EventMeta(){}

    /// Data clear method
    void clear();

  };
}

#include "IOManager.h"
namespace larcv {

  // Template instantiation for IO
  template<> inline std::string product_unique_name<larcv::EventMeta>() { return "meta"; }
  template EventMeta& IOManager::get_data<larcv::EventMeta>(const std::string&);
  template EventMeta& IOManager::get_data<larcv::EventMeta>(const ProducerID_t);

  /**
     \class larcv::EventMeta
     \brief A concrete factory class for larcv::EventMeta
  */
  class EventMetaFactory : public DataProductFactoryBase {
  public:
    /// ctor
    EventMetaFactory()
    { DataProductFactory::get().add_factory(product_unique_name<larcv::EventMeta>(),this); }
    /// dtor
    ~EventMetaFactory() {}
    /// create method
    EventBase* create() { return new EventMeta; }
  };

  
}
#endif
/** @} */ // end of doxygen group 

