/**
 * \file MaskTensor3D.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class MaskTensor3D
 *
 * @author kazuhiro
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __MASKTensor3D_H__
#define __MASKTensor3D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class MaskTensor3D ... these comments are used to generate
     doxygen documentation!
  */
  class MaskTensor3D : public ProcessBase {

  public:
    
    /// Default constructor
    MaskTensor3D(const std::string name="MaskTensor3D");
    
    /// Default destructor
    ~MaskTensor3D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:
    void configure_labels(const PSet& cfg);
    std::vector<float> _value_min_v;
    std::vector<std::string> _reference_producer_v;
    std::vector<std::string> _target_producer_v;
    std::vector<std::string> _output_producer_v;
  };

  /**
     \class larcv::MaskTensor3DFactory
     \brief A concrete factory class for larcv::MaskTensor3D
  */
  class MaskTensor3DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    MaskTensor3DProcessFactory() { ProcessFactory::get().add_factory("MaskTensor3D",this); }
    /// dtor
    ~MaskTensor3DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new MaskTensor3D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

