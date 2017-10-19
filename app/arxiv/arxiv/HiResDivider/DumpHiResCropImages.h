/**
 * \file DumpHiResCropImages.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class DumpHiResCropImages
 *
 * @author taritree
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __DUMPHIRESCROPIMAGES_H__
#define __DUMPHIRESCROPIMAGES_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"

#include <string>

namespace larcv {

  /**
     \class ProcessBase
     User defined class DumpHiResCropImages ... these comments are used to generate
     doxygen documentation!
  */
  class DumpHiResCropImages : public ProcessBase {

  public:
    
    /// Default constructor
    DumpHiResCropImages(const std::string name="DumpHiResCropImages");
    
    /// Default destructor
    ~DumpHiResCropImages(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

    std::string fTPCImageProducer;
    std::string fSegmentationProducer;
    std::string fPMTWeightedProducer;
    std::string fPMTRawProducer;

  protected:
    void DumpBGRImage( IOManager& mgr, std::string producer, std::string imglabel,  bool ismc, bool augment );
      
  };

  /**
     \class larcv::DumpHiResCropImagesFactory
     \brief A concrete factory class for larcv::DumpHiResCropImages
  */
  class DumpHiResCropImagesProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    DumpHiResCropImagesProcessFactory() { ProcessFactory::get().add_factory("DumpHiResCropImages",this); }
    /// dtor
    ~DumpHiResCropImagesProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new DumpHiResCropImages(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

