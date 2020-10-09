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

// forward declarations
class TVector3;

namespace larcv {

  template <typename T>
  class AABBox;

  template <typename T>
  class Vec3;
  typedef Vec3<float> Vec3f;
  typedef Vec3<double> Vec3d;

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

      /// Where (if anywhere) does a line segment intersect a given bounding box?
      /// (If the entire line segment is contained, the entry and exit points
      ///  will be set to the start and stop points provided.)
      ///
      /// \param bbox        Bounding box in question
      /// \param startPoint  Start point of the line segment in question
      /// \param stopPoint   Stop point of the line segment in question
      /// \param entryPoint  Computed entry point of the line segment into the box, if any
      /// \param exitPoint   Computed exit point of the line segment from the box, if any
      /// \return            Number of intersections (will be 0, 1, or 2)
      template <typename T>
      char Intersections(const larcv::AABBox<T> & bbox,
                         const TVector3 & startPoint,
                         const TVector3 & stopPoint,
                         larcv::Vec3<T> & entryPoint,
                         larcv::Vec3<T> & exitPoint) const;

      larcv::Particle MakeParticle(const TG4Trajectory& traj,
                                   const larcv::AABBox<double>& bbox);

      std::vector<larcv::Voxel>
      MakeVoxels(const ::TG4HitSegment &hitSegment,
                 const larcv::Voxel3DMeta &meta,
                 std::vector<larcv::Particle> &particles);

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

