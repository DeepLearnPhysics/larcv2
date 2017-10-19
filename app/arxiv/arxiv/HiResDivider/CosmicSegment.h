/**
 * \file CosmicSegment.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class CosmicSegment
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __COSMICSEGMENT_H__
#define __COSMICSEGMENT_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class CosmicSegment ... these comments are used to generate
     doxygen documentation!
  */
  class CosmicSegment : public ProcessBase {

  public:
    
    /// Default constructor
    CosmicSegment(const std::string name="CosmicSegment");
    
    /// Default destructor
    ~CosmicSegment(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::string _input_producer;
    std::string _output_producer;
    ProducerID_t _input_id;
    ProducerID_t _output_id;
    std::vector<float> _adc_threshold_v;
  };

  /**
     \class larcv::CosmicSegmentFactory
     \brief A concrete factory class for larcv::CosmicSegment
  */
  class CosmicSegmentProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    CosmicSegmentProcessFactory() { ProcessFactory::get().add_factory("CosmicSegment",this); }
    /// dtor
    ~CosmicSegmentProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new CosmicSegment(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

