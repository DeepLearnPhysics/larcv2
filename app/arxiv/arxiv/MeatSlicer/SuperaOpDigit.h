/**
 * \file SuperaOpDigit.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class SuperaOpDigit
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __SUPERAOPDIGIT_H__
#define __SUPERAOPDIGIT_H__
#include "SuperaBase.h"
#include "FMWKInterface.h"
#include "ParamsImage2D.h"
#include "ImageMetaMaker.h"
#include "DataFormat/Image2D.h"

namespace larcv {

  /**
     \class ProcessBase
     User defined class SuperaOpDigit ... these comments are used to generate
     doxygen documentation!
  */
  class SuperaOpDigit : public SuperaBase,
			public supera::ParamsImage2D,
			public supera::ImageMetaMaker {

  public:
    
    /// Default constructor
    SuperaOpDigit(const std::string name="SuperaOpDigit");
    
    /// Default destructor
    ~SuperaOpDigit(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  };

  /**
     \class larcv::SuperaOpDigitFactory
     \brief A concrete factory class for larcv::SuperaOpDigit
  */
  class SuperaOpDigitProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SuperaOpDigitProcessFactory() { ProcessFactory::get().add_factory("SuperaOpDigit",this); }
    /// dtor
    ~SuperaOpDigitProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SuperaOpDigit(instance_name); }
  };

}
#endif

/** @} */ // end of doxygen group 

