/**
 * \file CombineImages.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class CombineImages
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __COMBINEIMAGES_H__
#define __COMBINEIMAGES_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class CombineImages ... these comments are used to generate
     doxygen documentation!
  */
  class CombineImages : public ProcessBase {

  public:
    
    /// Default constructor
    CombineImages(const std::string name="CombineImages");
    
    /// Default destructor
    ~CombineImages(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::vector<std::string> _producer_v;
    size_t _nplanes;
    std::string _out_producer;
  };

  /**
     \class larcv::CombineImagesFactory
     \brief A concrete factory class for larcv::CombineImages
  */
  class CombineImagesProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    CombineImagesProcessFactory() { ProcessFactory::get().add_factory("CombineImages",this); }
    /// dtor
    ~CombineImagesProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new CombineImages(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

