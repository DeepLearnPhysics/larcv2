/**
 * \file ParticleCorrector.h
 *
 * \ingroup ImageMod
 *
 * \brief Class def header for a class ParticleCorrector
 *
 * @author Temigo
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __PARTICLECORRECTOR_H__
#define __PARTICLECORRECTOR_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"

namespace larcv {

  /**
     \class ProcessBase
     User defined class ParticleCorrector ... these comments are used to generate
     doxygen documentation!
  */
  class ParticleCorrector : public ProcessBase {

  public:

    /// Default constructor
    ParticleCorrector(const std::string name="ParticleCorrector");

    /// Default destructor
    ~ParticleCorrector(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    std::string _particle_producer;
    std::string _cluster3d_producer;
    double _pxsum_threshold;
    size_t _pxctr_threshold;

  };

  /**
     \class larcv::ParticleCorrectorFactory
     \brief A concrete factory class for larcv::ParticleCorrector
  */
  class ParticleCorrectorProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ParticleCorrectorProcessFactory() { ProcessFactory::get().add_factory("ParticleCorrector",this); }
    /// dtor
    ~ParticleCorrectorProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ParticleCorrector(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group
