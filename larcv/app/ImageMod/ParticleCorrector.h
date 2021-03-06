/**
 * \file ParticleCorrector.h
 *
 * \ingroup ImageMod
 *
 * \brief Class def header for a class ParticleCorrector
 *
 * @author Temigo
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __PARTICLECORRECTOR_H__
#define __PARTICLECORRECTOR_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
#include "larcv/core/DataFormat/Particle.h"
#include "larcv/core/DataFormat/Voxel.h"
#include "larcv/core/DataFormat/Voxel3DMeta.h"
#include "larcv/core/DataFormat/DataFormatTypes.h"

namespace larcv {

  /**
     \class ProcessBase
     User defined class ParticleCorrector ... these comments are used to generate
     doxygen documentation!
  */
  class ParticleCorrector : public ProcessBase {

  public:

    /// Default constructor
    ParticleCorrector(const std::string name="ParticleCorrector");

    /// Default destructor
    ~ParticleCorrector(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    bool _correct_energy_deposit;
    std::string _particle_producer;
    std::string _output_producer;
    std::string _cluster3d_producer;
    double _voxel_min_value;
    std::vector<double> _shift_xyz;
		std::vector<int> _skip_semantic_label;
    /**
       Correct particle positions (first/last step) based on cluster3d information
       \param BBox3D to consider as frame.
       \param particle_v Vector of particles.
       \param cluster3d_v Corresponding vector of clusters (each "cluster" itself is a vector of Point3D)
       \return Vector of particles whose position has been corrected.
    */
    std::vector<larcv::Particle>
      correct_particle_positions(const larcv::Voxel3DMeta& meta3d,
				 std::vector<larcv::Particle> particle_v,
				 std::vector<larcv::VoxelSet> cluster3d_v
				 );

  };

  /**
     \class larcv::ParticleCorrectorFactory
     \brief A concrete factory class for larcv::ParticleCorrector
  */
  class ParticleCorrectorProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ParticleCorrectorProcessFactory() { ProcessFactory::get().add_factory("ParticleCorrector",this); }
    /// dtor
    ~ParticleCorrectorProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ParticleCorrector(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group
