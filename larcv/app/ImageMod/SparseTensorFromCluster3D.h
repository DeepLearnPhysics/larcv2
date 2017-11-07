/**
 * \file SparseTensorFromCluster3D.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class SparseTensorFromCluster3D
 *
 * @author kazuhiro
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __SPARSETENSORFROMCLUSTER3D_H__
#define __SPARSETENSORFROMCLUSTER3D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class SparseTensorFromCluster3D ... these comments are used to generate
     doxygen documentation!
  */
  class SparseTensorFromCluster3D : public ProcessBase {

  public:
    
    /// Default constructor
    SparseTensorFromCluster3D(const std::string name="SparseTensorFromCluster3D");
    
    /// Default destructor
    ~SparseTensorFromCluster3D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    enum class PIType_t {
      kPITypeFixedPI,
      kPITypeInputVoxel,
      kPITypeClusterIndex,
      kPITypeUndefined
    };

    std::string _cluster3d_producer;
    std::string _output_producer;
    PIType_t _pi_type;
    float _fixed_pi;
  };

  /**
     \class larcv::SparseTensorFromCluster3DFactory
     \brief A concrete factory class for larcv::SparseTensorFromCluster3D
  */
  class SparseTensorFromCluster3DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SparseTensorFromCluster3DProcessFactory() { ProcessFactory::get().add_factory("SparseTensorFromCluster3D",this); }
    /// dtor
    ~SparseTensorFromCluster3DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SparseTensorFromCluster3D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

