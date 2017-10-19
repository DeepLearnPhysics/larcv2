/**
 * \file DoNothing.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class DoNothing
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __DONOTHING_H__
#define __DONOTHING_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class DoNothing ... these comments are used to generate
     doxygen documentation!
  */
  class DoNothing : public ProcessBase {

  public:
    
    /// Default constructor
    DoNothing(const std::string name="DoNothing");
    
    /// Default destructor
    ~DoNothing(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  };

  /**
     \class larcv::DoNothingFactory
     \brief A concrete factory class for larcv::DoNothing
  */
  class DoNothingProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    DoNothingProcessFactory() { ProcessFactory::get().add_factory("DoNothing",this); }
    /// dtor
    ~DoNothingProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new DoNothing(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

