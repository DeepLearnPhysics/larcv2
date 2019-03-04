/**
 * \file EventIDFilter.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class EventIDFilter
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __EVENTIDFILTER_H__
#define __EVENTIDFILTER_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
#include <map>
namespace larcv {

  /**
     \class ProcessBase
     User defined class EventIDFilter ... these comments are used to generate
     doxygen documentation!
  */
  class EventIDFilter : public ProcessBase {

  public:
    
    /// Default constructor
    EventIDFilter(const std::string name="EventIDFilter");
    
    /// Default destructor
    ~EventIDFilter(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    TTree* _tree;

    std::map<larcv::EventBase,bool> _id_m;
    std::string _ref_producer;
    std::string _ref_type;
    bool _remove_duplicate;
    std::string _fname;
    int _run;
    int _subrun;
    int _event;
  };

  /**
     \class larcv::EventIDFilterFactory
     \brief A concrete factory class for larcv::EventIDFilter
  */
  class EventIDFilterProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    EventIDFilterProcessFactory() { ProcessFactory::get().add_factory("EventIDFilter",this); }
    /// dtor
    ~EventIDFilterProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new EventIDFilter(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

