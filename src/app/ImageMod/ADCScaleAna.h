/**
 * \file ADCScaleAna.h
 *
 * \ingroup Package_Name
 *
 * \brief Class def header for a class ADCScaleAna
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __ADCSCALEANA_H__
#define __ADCSCALEANA_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
#include <TH1D.h>
namespace larcv {

  /**
     \class ProcessBase
     User defined class ADCScaleAna ... these comments are used to generate
     doxygen documentation!
  */
  class ADCScaleAna : public ProcessBase {

  public:

    /// Default constructor
    ADCScaleAna(const std::string name = "ADCScaleAna");

    /// Default destructor
    ~ADCScaleAna() {}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::vector<TH1D*> _hADC_v;
    std::string _image_producer;

  };

  /**
     \class larcv::ADCScaleAnaFactory
     \brief A concrete factory class for larcv::ADCScaleAna
  */
  class ADCScaleAnaProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ADCScaleAnaProcessFactory() { ProcessFactory::get().add_factory("ADCScaleAna", this); }
    /// dtor
    ~ADCScaleAnaProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ADCScaleAna(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group

