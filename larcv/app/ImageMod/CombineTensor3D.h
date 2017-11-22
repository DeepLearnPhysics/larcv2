/**
 * \file CombineTensor3D.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class CombineTensor3D
 *
 * @author kazuhiro
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __COMBINETENSOR3D_H__
#define __COMBINETENSOR3D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class CombineTensor3D ... these comments are used to generate
     doxygen documentation!
  */
  class CombineTensor3D : public ProcessBase {

  public:
    
    /// Default constructor
    CombineTensor3D(const std::string name="CombineTensor3D");
    
    /// Default destructor
    ~CombineTensor3D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    enum PoolType_t {kMaxPool,kSumPool};

  private:
    std::string _output_producer;
    std::vector<std::string> _tensor3d_producer_v;
    PoolType_t _pool_type;
  };

  /**
     \class larcv::CombineTensor3DFactory
     \brief A concrete factory class for larcv::CombineTensor3D
  */
  class CombineTensor3DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    CombineTensor3DProcessFactory() { ProcessFactory::get().add_factory("CombineTensor3D",this); }
    /// dtor
    ~CombineTensor3DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new CombineTensor3D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

