/**
 * \file Cluster2DThreshold.h
 *
 * \ingroup ImageMod
 *
 * \brief Class def header for a class Cluster2DThreshold
 *
 * @author cadams
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __CLUSTER2DTHRESHOLD_H__
#define __CLUSTER2DTHRESHOLD_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class Cluster2DThreshold ... these comments are used to generate
     doxygen documentation!
  */
  class Cluster2DThreshold : public ProcessBase {

  public:

    /// Default constructor
    Cluster2DThreshold(const std::string name="Cluster2DThreshold");

    /// Default destructor
    ~Cluster2DThreshold(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    void configure_labels(const PSet&);

    std::vector<std::string> _cluster2d_producer_v;
    std::vector<std::string> _output_producer_v;
    std::vector<float>       _thresholds_v;
  };

  /**
     \class larcv::Cluster2DThresholdFactory
     \brief A concrete factory class for larcv::Cluster2DThreshold
  */
  class Cluster2DThresholdProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    Cluster2DThresholdProcessFactory() { ProcessFactory::get().add_factory("Cluster2DThreshold",this); }
    /// dtor
    ~Cluster2DThresholdProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new Cluster2DThreshold(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group

