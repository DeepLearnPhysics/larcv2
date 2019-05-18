/**
 * \file EventGraph.h
 *
 * \ingroup DataFormat
 * 
 * \brief Class def header for a class EventGraph
 *
 * @author kazuhiro
 */

/** \addtogroup DataFormat

    @{*/
#ifndef EVENTGRAPH_H
#define EVENTGRAPH_H

#include "EventBase.h"
#include "Graph.h"
#include "DataProductFactory.h"

namespace larcv {
  /**
    \class EventGraph
    User-defined data product class (please comment!)
  */
  class EventGraph : public EventBase,
		     public larcv::Graph {
    
  public:
    
    /// Default constructor
    EventGraph(){}
    
    /// Default destructor
    ~EventGraph(){}

    /// Data clear method
    inline void clear()
    { EventBase::clear(); Graph::clear(); }

  };
}

#include "IOManager.h"
namespace larcv {

  // Template instantiation for IO
  template<> inline std::string product_unique_name<larcv::EventGraph>() { return "graph"; }
  template EventGraph& IOManager::get_data<larcv::EventGraph>(const std::string&);
  template EventGraph& IOManager::get_data<larcv::EventGraph>(const ProducerID_t);

  /**
     \class larcv::EventGraph
     \brief A concrete factory class for larcv::EventGraph
  */
  class EventGraphFactory : public DataProductFactoryBase {
  public:
    /// ctor
    EventGraphFactory()
    { DataProductFactory::get().add_factory(product_unique_name<larcv::EventGraph>(),this); }
    /// dtor
    ~EventGraphFactory() {}
    /// create method
    EventBase* create() { return new EventGraph; }
  };

  
}
#endif
/** @} */ // end of doxygen group 

