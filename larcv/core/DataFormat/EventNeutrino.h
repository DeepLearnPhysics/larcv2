/**
 * \file EventNeutrino.h
 *
 * \ingroup DataFormat
 *
 * \brief Class def header for a class EventNeutrino
 *
 * @author Temigo
 */

/** \addtogroup DataFormat

    @{*/
#ifndef EVENTNEUTRINO_H
#define EVENTNEUTRINO_H

#include "EventBase.h"
#include "Neutrino.h"
#include "DataProductFactory.h"

namespace larcv {
  /**
    \class EventNeutrino
    User-defined data product class (please comment!)
  */
  class EventNeutrino : public EventBase,
			public larcv::NeutrinoSet {

  public:

    /// Default constructor
    EventNeutrino(){}

    /// Default destructor
    ~EventNeutrino(){}

    /// Data clear method
    inline void clear()
    { EventBase::clear(); larcv::NeutrinoSet::clear(); }

  };
}

#include "IOManager.h"
namespace larcv {

  // Template instantiation for IO
  template<> inline std::string product_unique_name<larcv::EventNeutrino>() { return "neutrino"; }
  template EventNeutrino& IOManager::get_data<larcv::EventNeutrino>(const std::string&);
  template EventNeutrino& IOManager::get_data<larcv::EventNeutrino>(const ProducerID_t);

  /**
     \class larcv::EventNeutrino
     \brief A concrete factory class for larcv::EventNeutrino
  */
  class EventNeutrinoFactory : public DataProductFactoryBase {
  public:
    /// ctor
    EventNeutrinoFactory()
    { DataProductFactory::get().add_factory(product_unique_name<larcv::EventNeutrino>(),this); }
    /// dtor
    ~EventNeutrinoFactory() {}
    /// create method
    EventBase* create() { return new EventNeutrino; }
  };


}
#endif
/** @} */ // end of doxygen group

