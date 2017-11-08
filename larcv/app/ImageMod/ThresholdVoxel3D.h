/**
 * \file ThresholdVoxel3D.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class ThresholdVoxel3D
 *
 * @author kazuhiro
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __THRESHOLDVOXEL3D_H__
#define __THRESHOLDVOXEL3D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class ThresholdVoxel3D ... these comments are used to generate
     doxygen documentation!
  */
  class ThresholdVoxel3D : public ProcessBase {

  public:
    
    /// Default constructor
    ThresholdVoxel3D(const std::string name="ThresholdVoxel3D");
    
    /// Default destructor
    ~ThresholdVoxel3D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::string _target_producer;
    std::string _output_producer;

    float _voxel_value_min;
    float _voxel_value_max;

  };

  /**
     \class larcv::ThresholdVoxel3DFactory
     \brief A concrete factory class for larcv::ThresholdVoxel3D
  */
  class ThresholdVoxel3DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ThresholdVoxel3DProcessFactory() { ProcessFactory::get().add_factory("ThresholdVoxel3D",this); }
    /// dtor
    ~ThresholdVoxel3DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ThresholdVoxel3D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

