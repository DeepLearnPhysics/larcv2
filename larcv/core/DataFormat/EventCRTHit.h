/**
 * \file EventCRTHit.h
 *
 * \ingroup DataFormat
 *
 * \brief Class def header for a class EventCRTHit
 *
 * @author Temigo
 */

/** \addtogroup DataFormat

    @{*/
#ifndef __LARCV_EVENTCRTHIT_H__
#define __LARCV_EVENTCRTHIT_H__

#include "EventBase.h"
#include "CRTHit.h"
#include "DataProductFactory.h"

namespace larcv {
  /**
    \class EventCRTHit
    User-defined data product class (please comment!)
  */
  class EventCRTHit : public EventBase,
			public larcv::CRTHitSet {

  public:

    /// Default constructor
    EventCRTHit(){}

    /// Default destructor
    ~EventCRTHit(){}

    /// Data clear method
    inline void clear()
    { EventBase::clear(); larcv::CRTHitSet::clear(); }

  };

}

#include "IOManager.h"
namespace larcv {

  // Template instantiation for IO
  template<> inline std::string product_unique_name<larcv::EventCRTHit>() { return "crthit"; }
  template EventCRTHit& IOManager::get_data<larcv::EventCRTHit>(const std::string&);
  template EventCRTHit& IOManager::get_data<larcv::EventCRTHit>(const ProducerID_t);

  /**
     \class larcv::EventCRTHitFactory
     \brief A concrete factory class for larcv::EventCRTHit
  */
  class EventCRTHitFactory : public DataProductFactoryBase {
  public:
    /// ctor
    EventCRTHitFactory()
    { DataProductFactory::get().add_factory(product_unique_name<larcv::EventCRTHit>(),this); }
    /// dtor
    ~EventCRTHitFactory() {}
    /// create method
    EventBase* create() { return new EventCRTHit; }
  };


}
#endif
