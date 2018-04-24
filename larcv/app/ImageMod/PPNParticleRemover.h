/**
 * \file PPNParticleRemover.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class PPNParticleRemover
 *
 * @author drinkingkazu
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __PPNPARTICLEREMOVER_H__
#define __PPNPARTICLEREMOVER_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class PPNParticleRemover ... these comments are used to generate
     doxygen documentation!
  */
  class PPNParticleRemover : public ProcessBase {

  public:
    
    /// Default constructor
    PPNParticleRemover(const std::string name="PPNParticleRemover");
    
    /// Default destructor
    ~PPNParticleRemover(){}

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
     \class larcv::PPNParticleRemoverFactory
     \brief A concrete factory class for larcv::PPNParticleRemover
  */
  class PPNParticleRemoverProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    PPNParticleRemoverProcessFactory() { ProcessFactory::get().add_factory("PPNParticleRemover",this); }
    /// dtor
    ~PPNParticleRemoverProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new PPNParticleRemover(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

