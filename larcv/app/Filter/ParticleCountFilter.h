/**
 * \file ParticleCountFilter.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class ParticleCountFilter
 *
 * @author drinkingkazu
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __PARTICLECOUNTFILTER_H__
#define __PARTICLECOUNTFILTER_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class ParticleCountFilter ... these comments are used to generate
     doxygen documentation!
  */
  class ParticleCountFilter : public ProcessBase {

  public:
    
    /// Default constructor
    ParticleCountFilter(const std::string name="ParticleCountFilter");
    
    /// Default destructor
    ~ParticleCountFilter(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::string _part_producer;
    size_t _max_part_count;
    size_t _min_part_count;
    std::vector<size_t> _part_count_v;
  };

  /**
     \class larcv::ParticleCountFilterFactory
     \brief A concrete factory class for larcv::ParticleCountFilter
  */
  class ParticleCountFilterProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ParticleCountFilterProcessFactory() { ProcessFactory::get().add_factory("ParticleCountFilter",this); }
    /// dtor
    ~ParticleCountFilterProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ParticleCountFilter(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

