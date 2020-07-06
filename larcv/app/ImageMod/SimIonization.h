/**
 * \file SimIonization.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class SimIonization
 *
 * @author drinkingkazu
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __SIMIONIZATION_H__
#define __SIMIONIZATION_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class SimIonization ... these comments are used to generate
     doxygen documentation!
  */
  class SimIonization : public ProcessBase {

  public:
    
    /// Default constructor
    SimIonization(const std::string name="SimIonization");
    
    /// Default destructor
    ~SimIonization(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    double Recombination(double dedx) const;
    double Lifetime(double x) const;

  private:

    std::string _cluster3d_producer;
    std::string _output_producer;
    double _density;
    double _alpha;
    double _beta;
    double _efield;
    double _threshold;
    double _elifetime;
    double _vdrift;
    double _work_function;
  };

  /**
     \class larcv::SimIonizationFactory
     \brief A concrete factory class for larcv::SimIonization
  */
  class SimIonizationProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SimIonizationProcessFactory() { ProcessFactory::get().add_factory("SimIonization",this); }
    /// dtor
    ~SimIonizationProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SimIonization(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

