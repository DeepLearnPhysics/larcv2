/**
 * \file MaskTensor2D.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class MaskTensor2D
 *
 * @author kazuhiro
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __MASKTensor2D_H__
#define __MASKTensor2D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class MaskTensor2D ... these comments are used to generate
     doxygen documentation!
  */
  class MaskTensor2D : public ProcessBase {

  public:
    
    /// Default constructor
    MaskTensor2D(const std::string name="MaskTensor2D");
    
    /// Default destructor
    ~MaskTensor2D(){}

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
     \class larcv::MaskTensor2DFactory
     \brief A concrete factory class for larcv::MaskTensor2D
  */
  class MaskTensor2DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    MaskTensor2DProcessFactory() { ProcessFactory::get().add_factory("MaskTensor2D",this); }
    /// dtor
    ~MaskTensor2DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new MaskTensor2D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

