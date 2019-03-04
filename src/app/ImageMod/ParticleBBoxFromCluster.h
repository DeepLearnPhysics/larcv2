/**
 * \file ParticleBBoxFromCluster.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class ParticleBBoxFromCluster
 *
 * @author kazuhiro
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __PARTICLEBBoxFROMCLUSTER_H__
#define __PARTICLEBBoxFROMCLUSTER_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class ParticleBBoxFromCluster ... these comments are used to generate
     doxygen documentation!
  */
  class ParticleBBoxFromCluster : public ProcessBase {

  public:
    
    /// Default constructor
    ParticleBBoxFromCluster(const std::string name="ParticleBBoxFromCluster");
    
    /// Default destructor
    ~ParticleBBoxFromCluster(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  public:
    std::string _particle_producer;
    std::string _output_producer;
    std::string _cluster2d_producer;
    std::string _cluster3d_producer;
    float _voxel3d_min_thresh;
    float _pixel2d_min_thresh;

  };

  /**
     \class larcv::ParticleBBoxFromClusterFactory
     \brief A concrete factory class for larcv::ParticleBBoxFromCluster
  */
  class ParticleBBoxFromClusterProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ParticleBBoxFromClusterProcessFactory() { ProcessFactory::get().add_factory("ParticleBBoxFromCluster",this); }
    /// dtor
    ~ParticleBBoxFromClusterProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ParticleBBoxFromCluster(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

