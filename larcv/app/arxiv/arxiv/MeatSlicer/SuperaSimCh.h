/**
 * \file SuperaSimCh.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class SuperaSimCh
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __SUPERASIMCH_H__
#define __SUPERASIMCH_H__
#include "SuperaBase.h"
#include "FMWKInterface.h"
#include "ImageMetaMaker.h"
#include "ParamsImage2D.h"
#include "DataFormat/Image2D.h"

namespace larcv {

  /**
     \class ProcessBase
     User defined class SuperaSimCh ... these comments are used to generate
     doxygen documentation!
  */
  class SuperaSimCh : public SuperaBase,
		      public supera::ParamsImage2D,
		      public supera::ImageMetaMaker {

  public:
    
    /// Default constructor
    SuperaSimCh(const std::string name="SuperaSimCh");
    
    /// Default destructor
    ~SuperaSimCh(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    unsigned short _origin;

  };

  /**
     \class larcv::SuperaSimChFactory
     \brief A concrete factory class for larcv::SuperaSimCh
  */
  class SuperaSimChProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SuperaSimChProcessFactory() { ProcessFactory::get().add_factory("SuperaSimCh",this); }
    /// dtor
    ~SuperaSimChProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SuperaSimCh(instance_name); }
  };

}
#endif
/** @} */ // end of doxygen group 

