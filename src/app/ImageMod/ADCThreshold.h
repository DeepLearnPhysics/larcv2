/**
 * \file ADCThreshold.h
 *
 * \ingroup Package_Name
 *
 * \brief Class def header for a class ADCThreshold
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __ADCTHRESHOLD_H__
#define __ADCTHRESHOLD_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class ADCThreshold ... these comments are used to generate
     doxygen documentation!
  */
  class ADCThreshold : public ProcessBase {

  public:

    /// Default constructor
    ADCThreshold(const std::string name = "ADCThreshold");

    /// Default destructor
    ~ADCThreshold() {}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::string _image_producer;
    std::vector<float> _min_adc_v;
    std::vector<float> _max_adc_v;
    std::vector<float> _buffer;
    ProducerID_t _product_id;
  };

  /**
     \class larcv::ADCThresholdFactory
     \brief A concrete factory class for larcv::ADCThreshold
  */
  class ADCThresholdProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ADCThresholdProcessFactory() { ProcessFactory::get().add_factory("ADCThreshold", this); }
    /// dtor
    ~ADCThresholdProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ADCThreshold(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group

