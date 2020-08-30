/**
 * \file SuperaBBoxInteraction.h
 *
 * \ingroup Package_Name
 *
 * \brief Class def header for a class SuperaBBoxInteraction
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __SUPERAG4Trajectory_H__
#define __SUPERAG4Trajectory_H__
//#ifndef __CINT__
//#ifndef __CLING__
#include "SuperaBase.h"
#include "EDepSim/TG4Event.h"
#include "larcv/core/DataFormat/EventParticle.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
namespace larcv {

  /**
     \class SuperaG4Trajectory
     Responsible for defining a rectangular volume boundary and voxelization
  */
  class SuperaG4Trajectory : public SuperaBase {

  public:

    /// Default constructor
    SuperaG4Trajectory(const std::string name = "SuperaG4Trajectory");

    /// Default destructor
    ~SuperaG4Trajectory() {}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    larcv::Particle MakeParticle(const TG4Trajectory&);

    larcv::VoxelSet MakeVoxelSet(const ::TG4Trajectory&, 
      const larcv::Voxel3DMeta&,
      larcv::Particle&);

  private:

    std::string _cluster3d_producer;
    std::string _particle_producer;

  };

  /**
     \class larcv::SuperaBBoxInteractionFactory
     \brief A concrete factory class for larcv::SuperaG4Trajectory
  */
  class SuperaG4TrajectoryProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SuperaG4TrajectoryProcessFactory() { ProcessFactory::get().add_factory("SuperaG4Trajectory", this); }
    /// dtor
    ~SuperaG4TrajectoryProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SuperaG4Trajectory(instance_name); }
  };

}
//#endif
//#endif
#endif
/** @} */ // end of doxygen group

