/**
 * \file SuperaG4HitSegment.h
 *
 * \ingroup Package_Name
 *
 * \brief Extract truth info & voxelize true hit segments from edepsim
 *
 * @author J. Wolcott <jwolcott@fnal.gov>
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __SUPERAG4HITSEGMENT_H__
#define __SUPERAG4HITSEGMENT_H__
//#ifndef __CINT__
//#ifndef __CLING__
#include "SuperaBase.h"
#include "EDepSim/TG4Event.h"
#include "larcv/core/DataFormat/EventParticle.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
namespace larcv {

  /**
     \class SuperaG4HitSegment
     Responsible for defining a rectangular volume boundary and voxelization
  */
  class SuperaG4HitSegment : public SuperaBase {

  public:

    /// Default constructor
    SuperaG4HitSegment(const std::string name = "SuperaG4HitSegment");

    /// Default destructor
    ~SuperaG4HitSegment() {}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:
      /// Apply Landau fluctuations to the set of energies in these voxels
      void FluctuateEnergy(std::vector<Voxel> &voxels);

      larcv::Particle MakeParticle(const TG4Trajectory&);

      std::vector<larcv::Voxel>
      MakeVoxels(const ::TG4HitSegment &hitSegment,
                 const larcv::Voxel3DMeta &meta,
                 const std::vector<larcv::Particle> &particles);

    std::string _sparsetensor3d_producer;
    std::string _particle_producer;

  };

  /**
     \class larcv::SuperaBBoxInteractionFactory
     \brief A concrete factory class for larcv::SuperaG4HitSegment
  */
  class SuperaG4HitSegmentProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SuperaG4HitSegmentProcessFactory() { ProcessFactory::get().add_factory("SuperaG4HitSegment", this); }
    /// dtor
    ~SuperaG4HitSegmentProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SuperaG4HitSegment(instance_name); }
  };

}
//#endif
//#endif
#endif
/** @} */ // end of doxygen group

