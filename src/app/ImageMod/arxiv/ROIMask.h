/**
 * \file ROIMask.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class ROIMask
 *
 * This class takes ROIs and masks out either inside or outside the region.
 *
 * @author tmw
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __ROIMASK_H__
#define __ROIMASK_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"

#include <string>

namespace larcv {

  /**
     \class ProcessBase
     User defined class ROIMask ... these comments are used to generate
     doxygen documentation!
  */
  class ROIMask : public ProcessBase {

  public:
    
    /// Default constructor
    ROIMask(const std::string name="ROIMask");
    
    /// Default destructor
    ~ROIMask(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  protected:
    std::string fInputImageProducer;
    std::string fOutputImageProducer;
    std::string fInputROIProducer;
    std::string fOutputROIProducer;
    bool fMaskOutsideROI;
    bool finplace;
    bool fCutOutOfBounds;
    int fROIid;
    larcv::ROIType_t fROILabel;

  };

  /**
     \class larcv::ROIMaskFactory
     \brief A concrete factory class for larcv::ROIMask
  */
  class ROIMaskProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ROIMaskProcessFactory() { ProcessFactory::get().add_factory("ROIMask",this); }
    /// dtor
    ~ROIMaskProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ROIMask(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

