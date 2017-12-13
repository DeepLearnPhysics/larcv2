/**
 * \file BBoxFromCluster3D.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class BBoxFromCluster3D
 *
 * @author kazuhiro
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __BBoxFromCluster3D_H__
#define __BBoxFromCluster3D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class BBoxFromCluster3D ... these comments are used to generate
     doxygen documentation!
  */
  class BBoxFromCluster3D : public ProcessBase {

  public:
    
    /// Default constructor
    BBoxFromCluster3D(const std::string name="BBoxFromCluster3D");
    
    /// Default destructor
    ~BBoxFromCluster3D(){}

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
     \class larcv::BBoxFromCluster3DFactory
     \brief A concrete factory class for larcv::BBoxFromCluster3D
  */
  class BBoxFromCluster3DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    BBoxFromCluster3DProcessFactory() { ProcessFactory::get().add_factory("BBoxFromCluster3D",this); }
    /// dtor
    ~BBoxFromCluster3DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new BBoxFromCluster3D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

