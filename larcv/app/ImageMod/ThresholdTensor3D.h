/**
 * \file ThresholdTensor3D.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class ThresholdTensor3D
 *
 * @author kazuhiro
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __ThresholdTensor3D_H__
#define __ThresholdTensor3D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class ThresholdTensor3D ... these comments are used to generate
     doxygen documentation!
  */
  class ThresholdTensor3D : public ProcessBase {

  public:
    
    /// Default constructor
    ThresholdTensor3D(const std::string name="ThresholdTensor3D");
    
    /// Default destructor
    ~ThresholdTensor3D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    void configure_labels(const PSet& cfg);

    std::vector<std::string> _target_producer_v;
    std::vector<std::string> _output_producer_v;

    std::vector<float> _voxel_value_min_v;
    std::vector<float> _voxel_value_max_v;

  };

  /**
     \class larcv::ThresholdTensor3DFactory
     \brief A concrete factory class for larcv::ThresholdTensor3D
  */
  class ThresholdTensor3DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ThresholdTensor3DProcessFactory() { ProcessFactory::get().add_factory("ThresholdTensor3D",this); }
    /// dtor
    ~ThresholdTensor3DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ThresholdTensor3D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

