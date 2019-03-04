/**
 * \file CosmicNeutrinoSegLabel.h
 *
 * \ingroup ImageMod
 *
 * \brief Class def header for a class CosmicNeutrinoSegLabel
 *
 * @author cadams
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __COSMICNEUTRINOSEGLABEL_H__
#define __COSMICNEUTRINOSEGLABEL_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
#include "larcv/core/DataFormat/Image2D.h"
#include "larcv/core/DataFormat/Particle.h"
#include "larcv/core/DataFormat/Voxel2D.h"

namespace larcv {

/**
   \class ProcessBase
   User defined class CosmicNeutrinoSegLabel ... these comments are
   used to generate
   doxygen documentation!
*/
class CosmicNeutrinoSegLabel : public ProcessBase {
 public:
  /// Default constructor
  CosmicNeutrinoSegLabel(
      const std::string name = "CosmicNeutrinoSegLabel");

  /// Default destructor
  ~CosmicNeutrinoSegLabel() {}

  void configure(const PSet&);

  void initialize();

  bool process(IOManager& mgr);

  void finalize();

  Image2D seg_image_creator(const std::vector<Particle> & particles,
                            const ClusterPixel2D & clusters,
                            const ImageMeta & meta);

 private:

  std::string _cluster2d_producer;
  std::string _output_producer;
  std::string _particle_producer;
  int _cosmic_label;
  int _neutrino_label;

};

/**
   \class larcv::CosmicNeutrinoSegLabelFactory
   \brief A concrete factory class for larcv::CosmicNeutrinoSegLabel
*/
class CosmicNeutrinoSegLabelProcessFactory
    : public ProcessFactoryBase {
 public:
  /// ctor
  CosmicNeutrinoSegLabelProcessFactory() {
    ProcessFactory::get().add_factory("CosmicNeutrinoSegLabel",
                                      this);
  }
  /// dtor
  ~CosmicNeutrinoSegLabelProcessFactory() {}
  /// creation method
  ProcessBase* create(const std::string instance_name) {
    return new CosmicNeutrinoSegLabel(instance_name);
  }
};
}

#endif
/** @} */  // end of doxygen group
