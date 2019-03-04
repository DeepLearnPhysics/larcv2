/**
 * \file ReSample.h
 *
 * \ingroup NextImageMod
 * 
 * \brief Class def header for a class ReSample
 *
 * @author deltutto
 */

/** \addtogroup NextImageMod

    @{*/
#ifndef __RESAMPLE_H__
#define __RESAMPLE_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class ReSample ... these comments are used to generate
     doxygen documentation!
  */
  class ReSample : public ProcessBase {

  public:
    
    /// Default constructor
    ReSample(const std::string name="ReSample");
    
    /// Default destructor
    ~ReSample(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:
 
    std::vector<std::string> _pmaps_producers;
    std::vector<std::string> _output_labels;
    double _scale_x = 5;
    double _scale_y = 5;
    double _scale_z = 1/2.;


  };

  /**
     \class larcv::ReSampleFactory
     \brief A concrete factory class for larcv::ReSample
  */
  class ReSampleProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ReSampleProcessFactory() { ProcessFactory::get().add_factory("ReSample",this); }
    /// dtor
    ~ReSampleProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ReSample(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

