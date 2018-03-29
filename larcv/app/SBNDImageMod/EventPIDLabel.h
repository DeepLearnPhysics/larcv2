/**
 * \file EventPIDLabel.h
 *
 * \ingroup ImageMod
 *
 * \brief Class def header for a class EventPIDLabel
 *
 * Merge all cluster2d voxel sets into a single set if they have a common ancestor
 * @author cadams
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __EVENTPIDLABEL_H__
#define __EVENTPIDLABEL_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
#include "larcv/core/DataFormat/Image2D.h"
#include "larcv/core/DataFormat/Particle.h"
#include "larcv/core/DataFormat/Voxel2D.h"

namespace larcv {

/**
   \class ProcessBase
   User defined class EventPIDLabel ... these comments are
   used to generate
   doxygen documentation!
*/



class EventPIDLabel : public ProcessBase {
 public:
  /// Default constructor
  EventPIDLabel(
      const std::string name = "EventPIDLabel");

  /// Default destructor
  ~EventPIDLabel() {}

  void configure(const PSet&);

  void initialize();

  bool process(IOManager& mgr);

  void finalize();


 private:

  std::string _output_producer;
  std::string _particle_producer;


};

/**
   \class larcv::EventPIDLabelFactory
   \brief A concrete factory class for larcv::EventPIDLabel
*/
class EventPIDLabelProcessFactory
    : public ProcessFactoryBase {
 public:
  /// ctor
  EventPIDLabelProcessFactory() {
    ProcessFactory::get().add_factory("EventPIDLabel",
                                      this);
  }
  /// dtor
  ~EventPIDLabelProcessFactory() {}
  /// creation method
  ProcessBase* create(const std::string instance_name) {
    return new EventPIDLabel(instance_name);
  }
};
}

#endif
/** @} */  // end of doxygen group
