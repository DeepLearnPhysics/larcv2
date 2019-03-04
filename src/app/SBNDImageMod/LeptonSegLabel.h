/**
 * \file LeptonSegLabel.h
 *
 * \ingroup ImageMod
 *
 * \brief Class def header for a class LeptonSegLabel
 *
 * @author cadams
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __LEPTONSEGLABEL_H__
#define __LEPTONSEGLABEL_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
#include "larcv/core/DataFormat/Image2D.h"
#include "larcv/core/DataFormat/Particle.h"
#include "larcv/core/DataFormat/Voxel2D.h"

namespace larcv {

/**
   \class ProcessBase
   User defined class LeptonSegLabel ... these comments are
   used to generate
   doxygen documentation!
*/
class LeptonSegLabel : public ProcessBase {
 public:
  /// Default constructor
  LeptonSegLabel(
      const std::string name = "LeptonSegLabel");

  /// Default destructor
  ~LeptonSegLabel() {}

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
  int _lepton_label;
  int _other_label;

};

/**
   \class larcv::CosmicNeutrinoSegLabelFactory
   \brief A concrete factory class for larcv::LeptonSegLabel
*/
class LeptonSegLabelProcessFactory
    : public ProcessFactoryBase {
 public:
  /// ctor
  LeptonSegLabelProcessFactory() {
    ProcessFactory::get().add_factory("LeptonSegLabel",
                                      this);
  }
  /// dtor
  ~LeptonSegLabelProcessFactory() {}
  /// creation method
  ProcessBase* create(const std::string instance_name) {
    return new LeptonSegLabel(instance_name);
  }
};
}

#endif
/** @} */  // end of doxygen group
