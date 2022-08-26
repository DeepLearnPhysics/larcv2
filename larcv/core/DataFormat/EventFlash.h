/**
 * \file EventFlash.h
 *
 * \ingroup DataFormat
 *
 * \brief Class def header for a class EventFlash
 *
 * @author Temigo
 */

/** \addtogroup DataFormat

    @{*/
#ifndef __LARCV_EVENTFLASH_H__
#define __LARCV_EVENTFLASH_H__

#include "EventBase.h"
#include "Flash.h"
#include "DataProductFactory.h"

namespace larcv {
  /**
    \class EventFlash
    User-defined data product class (please comment!)
  */
  class EventFlash : public EventBase,
			public larcv::FlashSet {

  public:

    /// Default constructor
    EventFlash(){}

    /// Default destructor
    ~EventFlash(){}

    /// Data clear method
    inline void clear()
    { EventBase::clear(); larcv::FlashSet::clear(); }

  };
}

#include "IOManager.h"
namespace larcv {

  // Template instantiation for IO
  template<> inline std::string product_unique_name<larcv::EventFlash>() { return "opflash"; }
  template EventFlash& IOManager::get_data<larcv::EventFlash>(const std::string&);
  template EventFlash& IOManager::get_data<larcv::EventFlash>(const ProducerID_t);

  /**
     \class larcv::EventFlashFactory
     \brief A concrete factory class for larcv::EventFlash
  */
  class EventFlashFactory : public DataProductFactoryBase {
  public:
    /// ctor
    EventFlashFactory()
    { DataProductFactory::get().add_factory(product_unique_name<larcv::EventFlash>(),this); }
    /// dtor
    ~EventFlashFactory() {}
    /// create method
    EventBase* create() { return new EventFlash; }
  };


}

#endif
/** @} */ // end of doxygen group
