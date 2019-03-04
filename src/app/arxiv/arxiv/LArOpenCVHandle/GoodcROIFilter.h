/**
 * \file GoodcROIFilter.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class GoodcROIFilter
 *
 * @author vgenty
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __GOODCROIFILTER_H__
#define __GOODCROIFILTER_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"

#include "DataFormat/EventROI.h"
#include "LArUtil/SpaceChargeMicroBooNE.h"

namespace larcv {

  /**
     \class ProcessBase
     User defined class GoodcROIFilter ... these comments are used to generate
     doxygen documentation!
  */
  class GoodcROIFilter : public ProcessBase {

  public:
    
    /// Default constructor
    GoodcROIFilter(const std::string name="GoodcROIFilter");
    
    /// Default destructor
    ~GoodcROIFilter(){}

    void configure(const PSet&);
    void initialize();
    bool process(IOManager& mgr);
    void finalize();


  private:

    std::string _true_roi_producer;
    std::string _reco_roi_producer;

    larutil::SpaceChargeMicroBooNE _sce;
  };

  /**
     \class larcv::GoodcROIFilterFactory
     \brief A concrete factory class for larcv::GoodcROIFilter
  */
  class GoodcROIFilterProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    GoodcROIFilterProcessFactory() { ProcessFactory::get().add_factory("GoodcROIFilter",this); }
    /// dtor
    ~GoodcROIFilterProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new GoodcROIFilter(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

