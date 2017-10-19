/**
 * \file StepDigitizer.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class StepDigitizer
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __STEPDIGITIZER_H__
#define __STEPDIGITIZER_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class StepDigitizer ... these comments are used to generate
     doxygen documentation!
  */
  class StepDigitizer : public ProcessBase {

  public:
    
    /// Default constructor
    StepDigitizer(const std::string name="StepDigitizer");
    
    /// Default destructor
    ~StepDigitizer(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::string _image_producer;
    std::vector<float> _adc_boundary_v;
    std::vector<float> _adc_val_v;
    std::vector<float> _buffer;
  };

  /**
     \class larcv::StepDigitizerFactory
     \brief A concrete factory class for larcv::StepDigitizer
  */
  class StepDigitizerProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    StepDigitizerProcessFactory() { ProcessFactory::get().add_factory("StepDigitizer",this); }
    /// dtor
    ~StepDigitizerProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new StepDigitizer(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

