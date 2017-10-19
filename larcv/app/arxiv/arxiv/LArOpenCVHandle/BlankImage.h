/**
 * \file BlankImage.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class BlankImage
 *
 * @author vgenty
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __BLANKIMAGE_H__
#define __BLANKIMAGE_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class BlankImage ... these comments are used to generate
     doxygen documentation!
  */
  class BlankImage : public ProcessBase {

  public:
    
    /// Default constructor
    BlankImage(const std::string name="BlankImage");
    
    /// Default destructor
    ~BlankImage(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();


  private:
    std::string _tpc_image_producer;
    std::string _out_image_producer;
    float _pix_value;
    
  };

  /**
     \class larcv::BlankImageFactory
     \brief A concrete factory class for larcv::BlankImage
  */
  class BlankImageProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    BlankImageProcessFactory() { ProcessFactory::get().add_factory("BlankImage",this); }
    /// dtor
    ~BlankImageProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new BlankImage(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

