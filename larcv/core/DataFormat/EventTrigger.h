/**
 * \file EventTrigger.h
 *
 * \ingroup DataFormat
 *
 * \brief Wrapper for making Trigger persistable
 *
 * @author J. Wolcott <jwolcott@fnal.gov
 */

/** \addtogroup DataFormat

    @{*/
#ifndef __LARCV_EVENTTRIGGER_H__
#define __LARCV_EVENTTRIGGER_H__

#include "EventBase.h"
#include "Trigger.h"
#include "DataProductFactory.h"

namespace larcv {
  /**
    \class EventTrigger
    Persistent storage for trigger-related data
  */
  class EventTrigger : public EventBase, public larcv::Trigger
  {
    public:

      /// Data clear method
      inline void clear()
      { EventBase::clear(); Trigger::clear(); }

  };
}

#include "IOManager.h"
namespace larcv {

  // Template instantiation for IO
  template<> inline std::string product_unique_name<larcv::EventTrigger>() { return "trigger"; }
  template EventTrigger& IOManager::get_data<larcv::EventTrigger>(const std::string&);
  template EventTrigger& IOManager::get_data<larcv::EventTrigger>(const ProducerID_t);

  /**
     \class larcv::EventTriggerFactory
     \brief A concrete factory class for larcv::EventTrigger
  */
  class EventTriggerFactory : public DataProductFactoryBase {
  public:
    /// ctor
    EventTriggerFactory()
    { DataProductFactory::get().add_factory(product_unique_name<larcv::EventTrigger>(),this); }
    /// dtor
    ~EventTriggerFactory() {}
    /// create method
    EventBase* create() { return new EventTrigger; }
  };


}

#endif
/** @} */ // end of doxygen group
