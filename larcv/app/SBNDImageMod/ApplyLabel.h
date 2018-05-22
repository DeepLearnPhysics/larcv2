/**
 * \file ApplyLabel.h
 *
 * \ingroup ImageMod
 *
 * \brief Class def header for a class ApplyLabel
 *
 * Merge all cluster2d voxel sets into a single set if they have a common ancestor
 * @author cadams
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __APPLYLABEL_H__
#define __APPLYLABEL_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
#include "larcv/core/DataFormat/Image2D.h"
#include "larcv/core/DataFormat/Particle.h"
#include "larcv/core/DataFormat/Voxel2D.h"

namespace larcv {

/**
   \class ProcessBase
   User defined class ApplyLabel ... these comments are
   used to generate
   doxygen documentation!
*/



class ApplyLabel : public ProcessBase {

 public:
  /// Default constructor
  ApplyLabel(
      const std::string name = "ApplyLabel");

  /// Default destructor
  ~ApplyLabel() {}

  void configure(const PSet&);

  void initialize();

  bool process(IOManager& mgr);

  void finalize();


 private:

  std::string _output_producer;
  int _output_id;

};

/**
   \class larcv::ApplyLabelFactory
   \brief A concrete factory class for larcv::ApplyLabel
*/
class ApplyLabelProcessFactory
    : public ProcessFactoryBase {
 public:
  /// ctor
  ApplyLabelProcessFactory() {
    ProcessFactory::get().add_factory("ApplyLabel",
                                      this);
  }
  /// dtor
  ~ApplyLabelProcessFactory() {}
  /// creation method
  ProcessBase* create(const std::string instance_name) {
    return new ApplyLabel(instance_name);
  }
};
}

#endif
/** @} */  // end of doxygen group
