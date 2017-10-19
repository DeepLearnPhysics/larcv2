/**
 * \file CosmicFilter.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class CosmicFilter
 *
 * @author drinkingkazu
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __COSMICFILTER_H__
#define __COSMICFILTER_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"

class TRandom3;

namespace larcv {

  /**
     \class ProcessBase
     User defined class CosmicFilter ... these comments are used to generate
     doxygen documentation!
  */
  class CosmicFilter : public ProcessBase {

  public:
    
    /// Default constructor
    CosmicFilter(const std::string name="CosmicFilter");
    
    /// Default destructor
    ~CosmicFilter(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::string _roi_producer;
    double _thinning_factor;
    TRandom3* _rand;
  };

  /**
     \class larcv::CosmicFilterFactory
     \brief A concrete factory class for larcv::CosmicFilter
  */
  class CosmicFilterProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    CosmicFilterProcessFactory() { ProcessFactory::get().add_factory("CosmicFilter",this); }
    /// dtor
    ~CosmicFilterProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new CosmicFilter(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

