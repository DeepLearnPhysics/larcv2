/**
 * \file ParentParticleSeg.h
 *
 * \ingroup ImageMod
 *
 * \brief Class def header for a class ParentParticleSeg
 *
 * Merge all cluster2d voxel sets into a single set if they have a common ancestor
 * @author cadams
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __PARENTPARTICLESEG_H__
#define __PARENTPARTICLESEG_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
#include "larcv/core/DataFormat/Image2D.h"
#include "larcv/core/DataFormat/Particle.h"
#include "larcv/core/DataFormat/Voxel3D.h"
#include "larcv/core/DataFormat/Voxel2D.h"

namespace larcv {

/**
   \class ProcessBase
   User defined class ParentParticleSeg ... these comments are
   used to generate
   doxygen documentation!
*/

struct particle_node{
  int trackID;
  int parentID;
  int ancestorID;
  bool primary;
  const Particle * reference;
  particle_node * parent;
  std::vector<const particle_node *> daughters;
};

class ParentParticleSeg : public ProcessBase {
 public:
  /// Default constructor
  ParentParticleSeg(
      const std::string name = "ParentParticleSeg");

  /// Default destructor
  ~ParentParticleSeg() {}

  void configure(const PSet&);

  void initialize();

  bool process(IOManager& mgr);

  void finalize();

  larcv::VoxelSet cluster_merger(const larcv::ClusterPixel2D & clusters,
                                 particle_node * primary_node);

  larcv::VoxelSet cluster_merger(const larcv::ClusterVoxel3D & clusters,
                                 particle_node * primary_node);
 private:

  void get_all_daughter_ids(std::vector<int> & ids, const particle_node * node);


  std::string _cluster3d_producer;
  std::string _cluster2d_producer;
  std::string _output_producer;
  std::string _particle_producer;

};

/**
   \class larcv::ParentParticleSegFactory
   \brief A concrete factory class for larcv::ParentParticleSeg
*/
class ParentParticleSegProcessFactory
    : public ProcessFactoryBase {
 public:
  /// ctor
  ParentParticleSegProcessFactory() {
    ProcessFactory::get().add_factory("ParentParticleSeg",
                                      this);
  }
  /// dtor
  ~ParentParticleSegProcessFactory() {}
  /// creation method
  ProcessBase* create(const std::string instance_name) {
    return new ParentParticleSeg(instance_name);
  }
};
}

#endif
/** @} */  // end of doxygen group
