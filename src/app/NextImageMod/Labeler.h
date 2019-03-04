/**
 * \file Labeler.h
 *
 * \ingroup NextImageMod
 * 
 * \brief Class def header for a class Labeler
 *
 * @author deltutto
 */

/** \addtogroup NextImageMod

    @{*/
#ifndef __LABELER_H__
#define __LABELER_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class Labeler ... these comments are used to generate
     doxygen documentation!
  */
  class Labeler : public ProcessBase {

  public:
    
    /// Default constructor
    Labeler(const std::string name="Labeler");
    
    /// Default destructor
    ~Labeler(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    std::string _particle_producer = "";
    std::string _voxel3d_producer = "";
    std::string _output_producer = "";

    enum VoxelStatus {
      kBackground,
      kNormal,
      kParticleStart,
      kParticleEnd,
      kOther
    };

  };

  /**
     \class larcv::LabelerFactory
     \brief A concrete factory class for larcv::Labeler
  */
  class LabelerProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    LabelerProcessFactory() { ProcessFactory::get().add_factory("Labeler",this); }
    /// dtor
    ~LabelerProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new Labeler(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

