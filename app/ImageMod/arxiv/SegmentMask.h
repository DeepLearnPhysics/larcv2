/**
 * \file SegmentMask.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class SegmentMask
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __SEGMENTMASK_H__
#define __SEGMENTMASK_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class SegmentMask ... these comments are used to generate
     doxygen documentation!
  */
  class SegmentMask : public ProcessBase {

  public:
    
    /// Default constructor
    SegmentMask(const std::string name="SegmentMask");
    
    /// Default destructor
    ~SegmentMask(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::vector<larcv::ROIType_t> _valid_roi_v;

    ROIType_t _mask_value;

    std::string _image_producer;

    std::string _output_producer;

    std::string _roi_producer;

    std::vector<unsigned short> _plane_v;

  };

  /**
     \class larcv::SegmentMaskFactory
     \brief A concrete factory class for larcv::SegmentMask
  */
  class SegmentMaskProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SegmentMaskProcessFactory() { ProcessFactory::get().add_factory("SegmentMask",this); }
    /// dtor
    ~SegmentMaskProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SegmentMask(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

