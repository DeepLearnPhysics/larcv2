/**
 * \file CropBBox2D.h
 *
 * \ingroup Package_Name
 *
 * \brief Class def header for a class CropBBox2D
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __CROPBBox2D_H__
#define __CROPBBox2D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class CropBBox2D ... these comments are used to generate
     doxygen documentation!
  */
  class CropBBox2D : public ProcessBase {

  public:

    /// Default constructor
    CropBBox2D(const std::string name = "CropBBox2D");

    /// Default destructor
    ~CropBBox2D() {}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    size_t _bbox_idx;
    std::string _bbox_producer;
    std::string _input_producer;
    std::string _output_producer;
    std::vector<size_t> _image_idx;
  };

  /**
     \class larcv::CropBBox2DFactory
     \brief A concrete factory class for larcv::CropBBox2D
  */
  class CropBBox2DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    CropBBox2DProcessFactory() { ProcessFactory::get().add_factory("CropBBox2D", this); }
    /// dtor
    ~CropBBox2DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new CropBBox2D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group

