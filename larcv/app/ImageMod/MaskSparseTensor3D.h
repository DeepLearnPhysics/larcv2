/**
 * \file MaskSparseTensor3D.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class MaskSparseTensor3D
 *
 * @author kazuhiro
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __MASKSPARSETENSOR3D_H__
#define __MASKSPARSETENSOR3D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class MaskSparseTensor3D ... these comments are used to generate
     doxygen documentation!
  */
  class MaskSparseTensor3D : public ProcessBase {

  public:
    
    /// Default constructor
    MaskSparseTensor3D(const std::string name="MaskSparseTensor3D");
    
    /// Default destructor
    ~MaskSparseTensor3D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:
    float _value_min;
    std::string _reference_producer;
    std::string _target_producer;
    std::string _output_producer;
  };

  /**
     \class larcv::MaskSparseTensor3DFactory
     \brief A concrete factory class for larcv::MaskSparseTensor3D
  */
  class MaskSparseTensor3DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    MaskSparseTensor3DProcessFactory() { ProcessFactory::get().add_factory("MaskSparseTensor3D",this); }
    /// dtor
    ~MaskSparseTensor3DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new MaskSparseTensor3D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

