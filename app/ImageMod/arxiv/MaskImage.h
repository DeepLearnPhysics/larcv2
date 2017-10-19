/**
 * \file MaskImage.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class MaskImage
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __MASKIMAGE_H__
#define __MASKIMAGE_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class MaskImage ... these comments are used to generate
     doxygen documentation!
  */
  class MaskImage : public ProcessBase {

  public:
    
    /// Default constructor
    MaskImage(const std::string name="MaskImage");
    
    /// Default destructor
    ~MaskImage(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    float _pi_thresh_min;
    float _mask_value;
    bool  _ignore_ref_origin;
    std::string _output_producer;
    std::string _reference_image_producer;
    std::string _target_image_producer;

  };

  /**
     \class larcv::MaskImageFactory
     \brief A concrete factory class for larcv::MaskImage
  */
  class MaskImageProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    MaskImageProcessFactory() { ProcessFactory::get().add_factory("MaskImage",this); }
    /// dtor
    ~MaskImageProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new MaskImage(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

