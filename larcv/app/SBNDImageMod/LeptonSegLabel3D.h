/**
 * \file LeptonSegLabel3D.h
 *
 * \ingroup ImageMod
 *
 * \brief Class def header for a class LeptonSegLabel3D
 *
 * @author cadams
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __LEPTONSEGLABEL3D_H__
#define __LEPTONSEGLABEL3D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
#include "larcv/core/DataFormat/Particle.h"
#include "larcv/core/DataFormat/Voxel3D.h"

namespace larcv {

/**
   \class ProcessBase
   User defined class LeptonSegLabel3D ... these comments are
   used to generate
   doxygen documentation!
*/
class LeptonSegLabel3D : public ProcessBase {
 public:
  /// Default constructor
  LeptonSegLabel3D(
      const std::string name = "LeptonSegLabel3D");

  /// Default destructor
  ~LeptonSegLabel3D() {}

  void configure(const PSet&);

  void initialize();

  bool process(IOManager& mgr);

  void finalize();

  VoxelSet seg_image_creator(const std::vector<Particle> & particles,
                             const ClusterVoxel3D & clusters);

 private:

  std::string _cluster3d_producer;
  std::string _output_producer;
  std::string _particle_producer;
  int _lepton_label;
  int _other_label;

};

/**
   \class larcv::CosmicNeutrinoSegLabelFactory
   \brief A concrete factory class for larcv::LeptonSegLabel3D
*/
class LeptonSegLabel3DProcessFactory
    : public ProcessFactoryBase {
 public:
  /// ctor
  LeptonSegLabel3DProcessFactory() {
    ProcessFactory::get().add_factory("LeptonSegLabel3D",
                                      this);
  }
  /// dtor
  ~LeptonSegLabel3DProcessFactory() {}
  /// creation method
  ProcessBase* create(const std::string instance_name) {
    return new LeptonSegLabel3D(instance_name);
  }
};
}

#endif
/** @} */  // end of doxygen group
