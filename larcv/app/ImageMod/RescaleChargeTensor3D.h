/**
 * \file RescaleChargeTensor3D.h
 *
 * \ingroup ImageMod
 *
 * \brief Class def header for a class RescaleChargeTensor3D
 *
 * @author kazuhiro
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __RESCALECHARGETensor3D_H__
#define __RESCALECHARGETensor3D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class RescaleChargeTensor3D ... these comments are used to generate
     doxygen documentation!
  */
  class RescaleChargeTensor3D : public ProcessBase {

  public:

    /// Default constructor
    RescaleChargeTensor3D(const std::string name="RescaleChargeTensor3D");

    /// Default destructor
    ~RescaleChargeTensor3D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:
    void configure_labels(const PSet& cfg);
    std::vector<std::string> _hit_charge_producer_v;
    std::vector<std::string> _hit_key_producer_v;
    std::string _reference_producer;
    std::string _output_producer;
  };

  /**
     \class larcv::RescaleChargeTensor3DFactory
     \brief A concrete factory class for larcv::RescaleChargeTensor3D
  */
  class RescaleChargeTensor3DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    RescaleChargeTensor3DProcessFactory() { ProcessFactory::get().add_factory("RescaleChargeTensor3D",this); }
    /// dtor
    ~RescaleChargeTensor3DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new RescaleChargeTensor3D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group
