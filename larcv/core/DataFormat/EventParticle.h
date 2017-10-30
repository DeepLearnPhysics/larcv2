/**
 * \file EventParticle.h
 *
 * \ingroup DataFormat
 * 
 * \brief Class def header for a class EventParticle
 *
 * @author kazuhiro
 */

/** \addtogroup DataFormat

    @{*/
#ifndef EVENTPARTICLE_H
#define EVENTPARTICLE_H

#include "EventBase.h"
#include "Particle.h"
#include "DataProductFactory.h"

namespace larcv {
  /**
    \class EventParticle
    User-defined data product class (please comment!)
  */
  class EventParticle : public EventBase,
			public larcv::ParticleSet {
    
  public:
    
    /// Default constructor
    EventParticle(){}
    
    /// Default destructor
    ~EventParticle(){}

    /// Data clear method
    inline void clear()
    { EventBase::clear(); larcv::ParticleSet::clear(); }

  };
}

#include "IOManager.h"
namespace larcv {

  // Template instantiation for IO
  template<> inline std::string product_unique_name<larcv::EventParticle>() { return "particle"; }
  template EventParticle& IOManager::get_data<larcv::EventParticle>(const std::string&);
  template EventParticle& IOManager::get_data<larcv::EventParticle>(const ProducerID_t);

  /**
     \class larcv::EventParticle
     \brief A concrete factory class for larcv::EventParticle
  */
  class EventParticleFactory : public DataProductFactoryBase {
  public:
    /// ctor
    EventParticleFactory()
    { DataProductFactory::get().add_factory(product_unique_name<larcv::EventParticle>(),this); }
    /// dtor
    ~EventParticleFactory() {}
    /// create method
    EventBase* create() { return new EventParticle; }
  };

  
}
#endif
/** @} */ // end of doxygen group 

