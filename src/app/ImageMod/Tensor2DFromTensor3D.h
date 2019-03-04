/**
 * \file Tensor2DFromTensor3D.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class Tensor2DFromTensor3D
 *
 * @author kazuhiro
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __TENSOR2DFROMTENSOR3D_H__
#define __TENSOR2DFROMTENSOR3D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class Tensor2DFromTensor3D ... these comments are used to generate
     doxygen documentation!
  */
  class Tensor2DFromTensor3D : public ProcessBase {

  public:
    
    /// Default constructor
    Tensor2DFromTensor3D(const std::string name="Tensor2DFromTensor3D");
    
    /// Default destructor
    ~Tensor2DFromTensor3D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:
    void configure_labels(const PSet& cfg);
    enum PoolType_t {kMaxPool,kSumPool};
    std::vector<std::string> _tensor3d_producer_v;
    std::vector<std::string> _output_producer_v;
    std::vector<bool> _xy_v, _yz_v, _zx_v;
    std::vector<unsigned short> _pool_type_v;
  };

  /**
     \class larcv::Tensor2DFromTensor3DFactory
     \brief A concrete factory class for larcv::Tensor2DFromTensor3D
  */
  class Tensor2DFromTensor3DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    Tensor2DFromTensor3DProcessFactory() { ProcessFactory::get().add_factory("Tensor2DFromTensor3D",this); }
    /// dtor
    ~Tensor2DFromTensor3DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new Tensor2DFromTensor3D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

