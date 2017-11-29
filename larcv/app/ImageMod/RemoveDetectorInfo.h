/**
 * \file RemoveDetectorInfo.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class RemoveDetectorInfo
 *
 * @author cadams
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __REMOVEDETECTORINFO_H__
#define __REMOVEDETECTORINFO_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class RemoveDetectorInfo ... these comments are used to generate
     doxygen documentation!
  */
  class RemoveDetectorInfo : public ProcessBase {

  public:
    
    /// Default constructor
    RemoveDetectorInfo(const std::string name="RemoveDetectorInfo");
    
    /// Default destructor
    ~RemoveDetectorInfo(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::string _cluster3d_producer, _particle_producer;
    
  };

  /**
     \class larcv::RemoveDetectorInfoFactory
     \brief A concrete factory class for larcv::RemoveDetectorInfo
  */
  class RemoveDetectorInfoProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    RemoveDetectorInfoProcessFactory() { ProcessFactory::get().add_factory("RemoveDetectorInfo",this); }
    /// dtor
    ~RemoveDetectorInfoProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new RemoveDetectorInfo(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

