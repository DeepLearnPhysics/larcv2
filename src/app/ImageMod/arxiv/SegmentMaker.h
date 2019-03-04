/**
 * \file SegmentMaker.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class SegmentMaker
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __SEGMENTMAKER_H__
#define __SEGMENTMAKER_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"

namespace larcv {

  /**
     \class ProcessBase
     User defined class SegmentMaker ... these comments are used to generate
     doxygen documentation!
  */
  class SegmentMaker : public ProcessBase {

  public:
    
    /// Default constructor
    SegmentMaker(const std::string name="SegmentMaker");
    
    /// Default destructor
    ~SegmentMaker(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();
    
  private:
    std::string _tpc_image_producer;
    std::string _seg_image_producer;
    float _adc_threshold;
  };

  /**
     \class larcv::SegmentMakerFactory
     \brief A concrete factory class for larcv::SegmentMaker
  */
  class SegmentMakerProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SegmentMakerProcessFactory() { ProcessFactory::get().add_factory("SegmentMaker",this); }
    /// dtor
    ~SegmentMakerProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SegmentMaker(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

