/**
 * \file Tensor2DFromImage2D.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class Tensor2DFromImage2D
 *
 * @author coreyjadams and kazuhiro
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __TENSOR2DFROMIMAGE2D_H__
#define __TENSOR2DFROMIMAGE2D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class Tensor2DFromImage2D ... these comments are used to generate
     doxygen documentation!
  */
  class Tensor2DFromImage2D : public ProcessBase {

  public:
    
    /// Default constructor
    Tensor2DFromImage2D(const std::string name="Tensor2DFromImage2D");
    
    /// Default destructor
    ~Tensor2DFromImage2D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:
    void configure_labels(const PSet& cfg);
    std::vector<float> _thresholds_v;
    std::vector<int>   _projection_ids_v;
    std::string        _image2d_producer;
    std::string        _output_producer;
    std::string        _reference_tensor2d;
  };

  /**
     \class larcv::Tensor2DFromImage2DFactory
     \brief A concrete factory class for larcv::Tensor2DFromImage2D
  */
  class Tensor2DFromImage2DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    Tensor2DFromImage2DProcessFactory() { ProcessFactory::get().add_factory("Tensor2DFromImage2D",this); }
    /// dtor
    ~Tensor2DFromImage2DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new Tensor2DFromImage2D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

