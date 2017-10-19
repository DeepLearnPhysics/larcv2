/**
 * \file SimpleDigitizer.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class SimpleDigitizer
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __SIMPLEDIGITIZER_H__
#define __SIMPLEDIGITIZER_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class SimpleDigitizer ... these comments are used to generate
     doxygen documentation!
  */
  class SimpleDigitizer : public ProcessBase {

  public:
    
    /// Default constructor
    SimpleDigitizer(const std::string name="SimpleDigitizer");
    
    /// Default destructor
    ~SimpleDigitizer(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::string _image_producer;
    std::vector<float>  _min_adc_v;
    std::vector<float>  _max_adc_v;
    std::vector<size_t> _nrange_v;
    std::vector<float>  _buffer;

  };

  /**
     \class larcv::SimpleDigitizerFactory
     \brief A concrete factory class for larcv::SimpleDigitizer
  */
  class SimpleDigitizerProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SimpleDigitizerProcessFactory() { ProcessFactory::get().add_factory("SimpleDigitizer",this); }
    /// dtor
    ~SimpleDigitizerProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SimpleDigitizer(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

