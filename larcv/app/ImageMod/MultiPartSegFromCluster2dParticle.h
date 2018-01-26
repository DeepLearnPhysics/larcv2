/**
 * \file MultiPartSegFromCluster2dParticle.h
 *
 * \ingroup ImageMod
 *
 * \brief Class def header for a class MultiPartSegFromCluster2dParticle
 *
 * @author cadams
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __MULTIPARTSEGFROMCLUSTER2DPARTICLE_H__
#define __MULTIPARTSEGFROMCLUSTER2DPARTICLE_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
#include "larcv/core/DataFormat/Image2D.h"
#include "larcv/core/DataFormat/Particle.h"
#include "larcv/core/DataFormat/Voxel2D.h"

namespace larcv {

/**
   \class ProcessBase
   User defined class MultiPartSegFromCluster2dParticle ... these comments are
   used to generate
   doxygen documentation!
*/
class MultiPartSegFromCluster2dParticle : public ProcessBase {
 public:
  /// Default constructor
  MultiPartSegFromCluster2dParticle(
      const std::string name = "MultiPartSegFromCluster2dParticle");

  /// Default destructor
  ~MultiPartSegFromCluster2dParticle() {}

  void configure(const PSet&);

  void initialize();

  bool process(IOManager& mgr);

  void finalize();

  Image2D seg_image_creator(const std::vector<Particle> & particles,
                            const ClusterPixel2D & clusters,
                            const ImageMeta & meta);

 private:
  void configure_labels(const PSet&);

  std::string _image2d_producer;
  std::string _cluster2d_producer;
  std::string _output_producer;
  std::string _particle_producer;

  std::vector<int> _pdg_list;
  std::vector<int> _label_list;
};

/**
   \class larcv::MultiPartSegFromCluster2dParticleFactory
   \brief A concrete factory class for larcv::MultiPartSegFromCluster2dParticle
*/
class MultiPartSegFromCluster2dParticleProcessFactory
    : public ProcessFactoryBase {
 public:
  /// ctor
  MultiPartSegFromCluster2dParticleProcessFactory() {
    ProcessFactory::get().add_factory("MultiPartSegFromCluster2dParticle",
                                      this);
  }
  /// dtor
  ~MultiPartSegFromCluster2dParticleProcessFactory() {}
  /// creation method
  ProcessBase* create(const std::string instance_name) {
    return new MultiPartSegFromCluster2dParticle(instance_name);
  }
};
}

#endif
/** @} */  // end of doxygen group
