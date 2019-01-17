/**
 * \file SimCoefficient.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class SimCoefficient
 *
 * @author kterao
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __SIMCOEFFICIENT_H__
#define __SIMCOEFFICIENT_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class SimCoefficient ... these comments are used to generate
     doxygen documentation!
  */
  class SimCoefficient : public ProcessBase {

  public:
    
    /// Default constructor
    SimCoefficient(const std::string name="SimCoefficient");
    
    /// Default destructor
    ~SimCoefficient(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:
    std::string _output_producer;
    std::string _cluster_producer;
    std::string _particle_producer;
    bool _store_pdg;
  };

  /**
     \class larcv::SimCoefficientFactory
     \brief A concrete factory class for larcv::SimCoefficient
  */
  class SimCoefficientProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SimCoefficientProcessFactory() { ProcessFactory::get().add_factory("SimCoefficient",this); }
    /// dtor
    ~SimCoefficientProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SimCoefficient(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

