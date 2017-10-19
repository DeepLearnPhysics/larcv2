/**
 * \file SuperaMetaMaker.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class SuperaMetaMaker
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __SUPERAMETAMAKER_H__
#define __SUPERAMETAMAKER_H__
#include "SuperaBase.h"
#include "FMWKInterface.h"
#include "ImageMetaMakerBase.h"

namespace larcv {

  /**
     \class ProcessBase
     User defined class SuperaMetaMaker ... these comments are used to generate
     doxygen documentation!
  */
  class SuperaMetaMaker : public SuperaBase {

  public:
    
    /// Default constructor
    SuperaMetaMaker(const std::string name="SuperaMetaMaker");
    
    /// Default destructor
    ~SuperaMetaMaker(){ if(_meta_maker) delete _meta_maker; }

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    supera::ImageMetaMakerBase* _meta_maker;

  };

  /**
     \class larcv::SuperaMetaMakerFactory
     \brief A concrete factory class for larcv::SuperaMetaMaker
  */
  class SuperaMetaMakerProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SuperaMetaMakerProcessFactory() { ProcessFactory::get().add_factory("SuperaMetaMaker",this); }
    /// dtor
    ~SuperaMetaMakerProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SuperaMetaMaker(instance_name); }
  };

}
#endif
/** @} */ // end of doxygen group 

