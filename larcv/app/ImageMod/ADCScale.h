/**
 * \file ADCScale.h
 *
 * \ingroup Package_Name
 *
 * \brief Class def header for a class ADCScale
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __ADCSCALE_H__
#define __ADCSCALE_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
#include "larcv/core/CPPUtil/RandomGaus.h"

namespace larcv {

  /**
     \class ProcessBase
     User defined class ADCScale ... these comments are used to generate
     doxygen documentation!
  */
  class ADCScale : public ProcessBase {

  public:

    /// Default constructor
    ADCScale(const std::string name = "ADCScale");

    /// Default destructor
    ~ADCScale() {}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::string _image_producer;
    ProducerID_t _image_id;
    std::vector<double> _gaus_mean_v;
    std::vector<double> _gaus_sigma_v;
    std::vector<size_t> _gaus_pool_size_v;
    bool _per_pixel;
#ifndef __CINT__
#ifndef __CLING__
    std::vector<larcv::RandomGaus > _randg_v;
#endif
#endif
  };

  /**
     \class larcv::ADCScaleFactory
     \brief A concrete factory class for larcv::ADCScale
  */
  class ADCScaleProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ADCScaleProcessFactory() { ProcessFactory::get().add_factory("ADCScale", this); }
    /// dtor
    ~ADCScaleProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ADCScale(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group

