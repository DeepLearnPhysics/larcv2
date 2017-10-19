/**
 * \file SegmentRelabel.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class SegmentRelabel
 *
 * The purpose of this module is to relabel semantic segmentation labels. 
 *
 * @author taritree
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __SEGMENTRELABEL_H__
#define __SEGMENTRELABEL_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"

#include <map>
#include <string>

namespace larcv {

  /**
     \class ProcessBase
     User defined class SegmentRelabel ... these comments are used to generate
     doxygen documentation!
  */
  class SegmentRelabel : public ProcessBase {

  public:
    
    /// Default constructor
    SegmentRelabel(const std::string name="SegmentRelabel");
    
    /// Default destructor
    ~SegmentRelabel(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  protected:

    std::string fInputSegmentProducer;
    std::string fOutputSegmentProducer;
    std::map<int,int> fOrigToNewMap;

  };

  /**
     \class larcv::SegmentRelabelFactory
     \brief A concrete factory class for larcv::SegmentRelabel
  */
  class SegmentRelabelProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SegmentRelabelProcessFactory() { ProcessFactory::get().add_factory("SegmentRelabel",this); }
    /// dtor
    ~SegmentRelabelProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SegmentRelabel(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

