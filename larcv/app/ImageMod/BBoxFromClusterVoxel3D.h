/**
 * \file BBoxFromClusterVoxel3D.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class BBoxFromClusterVoxel3D
 *
 * @author kazuhiro
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __BBOXFROMCLUSTERVOXEL3D_H__
#define __BBOXFROMCLUSTERVOXEL3D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class BBoxFromClusterVoxel3D ... these comments are used to generate
     doxygen documentation!
  */
  class BBoxFromClusterVoxel3D : public ProcessBase {

  public:
    
    /// Default constructor
    BBoxFromClusterVoxel3D(const std::string name="BBoxFromClusterVoxel3D");
    
    /// Default destructor
    ~BBoxFromClusterVoxel3D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::string _voxel3d_producer;
    std::string _particle_producer;
    std::string _output_producer;
    bool _xy, _yz, _zx;
    float _voxel_threshold;
    
  };

  /**
     \class larcv::BBoxFromClusterVoxel3DFactory
     \brief A concrete factory class for larcv::BBoxFromClusterVoxel3D
  */
  class BBoxFromClusterVoxel3DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    BBoxFromClusterVoxel3DProcessFactory() { ProcessFactory::get().add_factory("BBoxFromClusterVoxel3D",this); }
    /// dtor
    ~BBoxFromClusterVoxel3DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new BBoxFromClusterVoxel3D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

