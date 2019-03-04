/**
 * \file MCSingleParticleFilter.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class MCSingleParticleFilter
 *
 * @author drinkingkazu
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __MCSINGLEPARTICLEFILTER_H__
#define __MCSINGLEPARTICLEFILTER_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class MCSingleParticleFilter ... these comments are used to generate
     doxygen documentation!
  */
  class MCSingleParticleFilter : public ProcessBase {

  public:
    
    /// Default constructor
    MCSingleParticleFilter(const std::string name="MCSingleParticleFilter");
    
    /// Default destructor
    ~MCSingleParticleFilter(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::string _part_producer;
    double _shower_min_energy;
    double _track_min_energy;
    double _proton_min_energy;
  };

  /**
     \class larcv::MCSingleParticleFilterFactory
     \brief A concrete factory class for larcv::MCSingleParticleFilter
  */
  class MCSingleParticleFilterProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    MCSingleParticleFilterProcessFactory() { ProcessFactory::get().add_factory("MCSingleParticleFilter",this); }
    /// dtor
    ~MCSingleParticleFilterProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new MCSingleParticleFilter(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

