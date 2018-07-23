/**
 * \file ThresholdCluster3D.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class ThresholdCluster3D
 *
 * @author kazuhiro
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __ThresholdCluster3D_H__
#define __ThresholdCluster3D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class ThresholdCluster3D ... these comments are used to generate
     doxygen documentation!
  */
  class ThresholdCluster3D : public ProcessBase {

  public:
    
    /// Default constructor
    ThresholdCluster3D(const std::string name="ThresholdCluster3D");
    
    /// Default destructor
    ~ThresholdCluster3D(){}

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
     \class larcv::ThresholdCluster3DFactory
     \brief A concrete factory class for larcv::ThresholdCluster3D
  */
  class ThresholdCluster3DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ThresholdCluster3DProcessFactory() { ProcessFactory::get().add_factory("ThresholdCluster3D",this); }
    /// dtor
    ~ThresholdCluster3DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ThresholdCluster3D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

