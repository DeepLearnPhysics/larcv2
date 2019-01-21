/**
 * \file ProdEventMeta2D.h
 *
 * \ingroup Package_Name
 *
 * \brief Class def header for a class ProdEventMeta2D
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __PRODEVENTMETA2D_H__
#define __PRODEVENTMETA2D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"

namespace larcv {

  /**
     \class ProcessBase
     User defined class ProdEventMeta2D ... these comments are used to generate
     doxygen documentation!
  */
  class ProdEventMeta2D : public ProcessBase {

  public:

    /// Default constructor
    ProdEventMeta2D(const std::string name = "ProdEventMeta2D");

    /// Default destructor
    ~ProdEventMeta2D() {}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::string _image_producer;
    std::string _image_type;
    std::string _output_producer;
    
  };

  /**
     \class larcv::ADCScaleFactory
     \brief A concrete factory class for larcv::ProdEventMeta2D
  */
  class ProdEventMeta2DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ProdEventMeta2DProcessFactory() { ProcessFactory::get().add_factory("ProdEventMeta2D", this); }
    /// dtor
    ~ProdEventMeta2DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ProdEventMeta2D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group

