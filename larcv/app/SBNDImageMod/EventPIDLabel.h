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

  enum interaction_type_t     {kNueCC, kNumuCC, kNC, kNEventCategories};
  enum proton_counter_t       {kZeroProtons, kOneProton, kNProtons, kNProtonCategories};
  enum charged_pion_counter_t {kZeroChargedPions, kNChargedPions, kNChargedPionCategories};
  enum neutral_pion_counter_t {kZeroNeutralPions, kNNeutralPions, kNNeutralPionCategories};

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

  std::string _output_neutrino_id_producer;
  std::string _output_proton_id_producer;
  std::string _output_chrpion_id_producer;
  std::string _output_ntrpion_id_producer;
  std::string _particle_producer;
  std::string _neutrino_producer;

  float _proton_threshold;
  float _charged_pion_threshold;
  float _neutral_pion_threshold;

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
