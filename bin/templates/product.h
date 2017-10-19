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
#ifndef EMPTY_CLASS_NAME_H
#define EMPTY_CLASS_NAME_H

#include "larcv/core/DataFormat/EventBase.h"
#include "larcv/core/DataFormat/DataProductFactory.h"

namespace larcv {
  /**
    \class Empty_Class_Name
    User-defined data product class (please comment!)
  */
  class Empty_Class_Name : public EventBase {
    
  public:
    
    /// Default constructor
    Empty_Class_Name(){}
    
    /// Default destructor
    ~Empty_Class_Name(){}

    /// Data clear method
    void clear();

  };
}

#include "IOManager.h"
namespace larcv {

  // Template instantiation for IO
  template<> inline std::string product_unique_name<larcv::Empty_Class_Name>() { return ""; }
  template Empty_Class_Name& IOManager::get_data<larcv::Empty_Class_Name>(const std::string&);
  template Empty_Class_Name& IOManager::get_data<larcv::Empty_Class_Name>(const ProducerID_t);

  /**
     \class larcv::Empty_Class_Name
     \brief A concrete factory class for larcv::Empty_Class_Name
  */
  class Empty_Class_NameFactory : public DataProductFactoryBase {
  public:
    /// ctor
    Empty_Class_NameFactory()
    { DataProductFactory::get().add_factory(product_unique_name<larcv::Empty_Class_Name>(),this); }
    /// dtor
    ~Empty_Class_NameFactory() {}
    /// create method
    EventBase* create() { return new Empty_Class_Name; }
  };

  
}
#endif
/** @} */ // end of doxygen group 

