/**
 * \file EmbedImage.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class EmbedImage
 *
 * @author taritree
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __EMBEDIMAGE_H__
#define __EMBEDIMAGE_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"

#include <string>

namespace larcv {

  /**
     \class ProcessBase
     User defined class EmbedImage ... these comments are used to generate
     doxygen documentation!
  */
  class EmbedImage : public ProcessBase {

  public:
    
    /// Default constructor
    EmbedImage(const std::string name="EmbedImage");
    
    /// Default destructor
    ~EmbedImage(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  protected:

    std::string fInputProducer;
    std::string fOutputProducer;
    int fOutputRows;
    int fOutputCols;
    bool finplace;

  };

  /**
     \class larcv::EmbedImageFactory
     \brief A concrete factory class for larcv::EmbedImage
  */
  class EmbedImageProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    EmbedImageProcessFactory() { ProcessFactory::get().add_factory("EmbedImage",this); }
    /// dtor
    ~EmbedImageProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new EmbedImage(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

