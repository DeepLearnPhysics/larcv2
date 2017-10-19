/**
 * \file SegmentFilter.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class SegmentFilter
 *
 * @author vgenty
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __SEGMENTFILTER_H__
#define __SEGMENTFILTER_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class SegmentFilter ... these comments are used to generate
     doxygen documentation!
  */
  class SegmentFilter : public ProcessBase {

  public:
    
    /// Default constructor
    SegmentFilter(const std::string name="SegmentFilter");
    
    /// Default destructor
    ~SegmentFilter(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::string _seg_producer;
    std::vector<bool> _allowed_segment_v;

  };

  /**
     \class larcv::SegmentFilterFactory
     \brief A concrete factory class for larcv::SegmentFilter
  */
  class SegmentFilterProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SegmentFilterProcessFactory() { ProcessFactory::get().add_factory("SegmentFilter",this); }
    /// dtor
    ~SegmentFilterProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SegmentFilter(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

