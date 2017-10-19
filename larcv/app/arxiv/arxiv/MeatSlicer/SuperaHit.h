/**
 * \file SuperaHit.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class SuperaHit
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __SUPERAHIT_H__
#define __SUPERAHIT_H__
//#ifndef __CINT__
//#ifndef __CLING__
#include "SuperaBase.h"
#include "FMWKInterface.h"
#include "ParamsImage2D.h"
#include "ImageMetaMaker.h"
#include "DataFormat/Image2D.h"

namespace larcv {

  /**
     \class ProcessBase
     User defined class SuperaHit ... these comments are used to generate
     doxygen documentation!
  */
  class SuperaHit : public SuperaBase,
		    public supera::ParamsImage2D,
		    public supera::ImageMetaMaker {

  public:
    
    /// Default constructor
    SuperaHit(const std::string name="SuperaHit");
    
    /// Default destructor
    ~SuperaHit(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  };

  /**
     \class larcv::SuperaHitFactory
     \brief A concrete factory class for larcv::SuperaHit
  */
  class SuperaHitProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SuperaHitProcessFactory() { ProcessFactory::get().add_factory("SuperaHit",this); }
    /// dtor
    ~SuperaHitProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SuperaHit(instance_name); }
  };

}
#endif
//#endif
//#endif
/** @} */ // end of doxygen group 

