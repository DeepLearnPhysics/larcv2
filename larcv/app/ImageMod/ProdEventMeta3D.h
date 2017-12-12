/**
 * \file ProdEventMeta3D.h
 *
 * \ingroup Package_Name
 *
 * \brief Class def header for a class ProdEventMeta3D
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __PRODEVENTMETA3D_H__
#define __PRODEVENTMETA3D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"

namespace larcv {

  /**
     \class ProcessBase
     User defined class ProdEventMeta3D ... these comments are used to generate
     doxygen documentation!
  */
  class ProdEventMeta3D : public ProcessBase {

  public:

    /// Default constructor
    ProdEventMeta3D(const std::string name = "ProdEventMeta3D");

    /// Default destructor
    ~ProdEventMeta3D() {}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::string _image_producer;
    std::string _output_producer;
    std::string _input_type;
    
  };

  /**
     \class larcv::ADCScaleFactory
     \brief A concrete factory class for larcv::ProdEventMeta3D
  */
  class ProdEventMeta3DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ProdEventMeta3DProcessFactory() { ProcessFactory::get().add_factory("ProdEventMeta3D", this); }
    /// dtor
    ~ProdEventMeta3DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ProdEventMeta3D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group

