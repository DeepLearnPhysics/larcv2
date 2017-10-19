/**
 * \file Pass.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class Pass
 *
 * @author taritree
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __PASS_H__
#define __PASS_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class Pass ... these comments are used to generate
     doxygen documentation!
  */
  class Pass : public ProcessBase {

  public:
    
    /// Default constructor
    Pass(const std::string name="Pass");
    
    /// Default destructor
    ~Pass(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  };

  /**
     \class larcv::PassFactory
     \brief A concrete factory class for larcv::Pass
  */
  class PassProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    PassProcessFactory() { ProcessFactory::get().add_factory("Pass",this); }
    /// dtor
    ~PassProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new Pass(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

