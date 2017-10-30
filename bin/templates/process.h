/**
 * \file Empty_Class_Name.h
 *
 * \ingroup Working_Package
 * 
 * \brief Class def header for a class Empty_Class_Name
 *
 * @author Shell_User_Name
 */

/** \addtogroup Working_Package

    @{*/
#ifndef __EMPTY_CLASS_NAME_H__
#define __EMPTY_CLASS_NAME_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class Empty_Class_Name ... these comments are used to generate
     doxygen documentation!
  */
  class Empty_Class_Name : public ProcessBase {

  public:
    
    /// Default constructor
    Empty_Class_Name(const std::string name="Empty_Class_Name");
    
    /// Default destructor
    ~Empty_Class_Name(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  };

  /**
     \class larcv::Empty_Class_NameFactory
     \brief A concrete factory class for larcv::Empty_Class_Name
  */
  class Empty_Class_NameProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    Empty_Class_NameProcessFactory() { ProcessFactory::get().add_factory("Empty_Class_Name",this); }
    /// dtor
    ~Empty_Class_NameProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new Empty_Class_Name(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

