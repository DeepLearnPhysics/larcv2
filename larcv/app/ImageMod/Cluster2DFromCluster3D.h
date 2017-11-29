/**
 * \file Cluster2DFromCluster3D.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class Cluster2DFromCluster3D
 *
 * @author kazuhiro
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __CLUSTER2DFROMCLUSTER3D_H__
#define __CLUSTER2DFROMCLUSTER3D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class Cluster2DFromCluster3D ... these comments are used to generate
     doxygen documentation!
  */
  class Cluster2DFromCluster3D : public ProcessBase {

  public:
    
    /// Default constructor
    Cluster2DFromCluster3D(const std::string name="Cluster2DFromCluster3D");
    
    /// Default destructor
    ~Cluster2DFromCluster3D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:
    void configure_labels(const PSet& cfg);
    std::vector<std::string> _cluster3d_producer_v;
    std::vector<std::string> _output_producer_v;
    std::vector<bool> _xy_v, _yz_v, _zx_v;
  };

  /**
     \class larcv::Cluster2DFromCluster3DFactory
     \brief A concrete factory class for larcv::Cluster2DFromCluster3D
  */
  class Cluster2DFromCluster3DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    Cluster2DFromCluster3DProcessFactory() { ProcessFactory::get().add_factory("Cluster2DFromCluster3D",this); }
    /// dtor
    ~Cluster2DFromCluster3DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new Cluster2DFromCluster3D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

