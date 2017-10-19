/**
 * \file SegmentAna.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class SegmentAna
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __SEGMENTANA_H__
#define __SEGMENTANA_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class SegmentAna ... these comments are used to generate
     doxygen documentation!
  */
  class SegmentAna : public ProcessBase {

  public:
    
    /// Default constructor
    SegmentAna(const std::string name="SegmentAna");
    
    /// Default destructor
    ~SegmentAna(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::string _image_producer;
    std::string _segment_producer;

  };

  /**
     \class larcv::SegmentAnaFactory
     \brief A concrete factory class for larcv::SegmentAna
  */
  class SegmentAnaProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SegmentAnaProcessFactory() { ProcessFactory::get().add_factory("SegmentAna",this); }
    /// dtor
    ~SegmentAnaProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SegmentAna(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

