/**
 * \file ImageFromTensor2D.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class ImageFromTensor2D
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __IMAGEFROMTensor2D_H__
#define __IMAGEFROMTensor2D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class ImageFromTensor2D ... these comments are used to generate
     doxygen documentation!
  */
  class ImageFromTensor2D : public ProcessBase {

  public:
    
    /// Default constructor
    ImageFromTensor2D(const std::string name="ImageFromTensor2D");
    
    /// Default destructor
    ~ImageFromTensor2D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    enum class PIType_t {
      kPITypeFixedPI,
      kPITypeInputVoxel,
      kPITypeClusterIndex,
      kPITypeUndefined
    };
    float _fixed_pi;
    PIType_t _type_pi;
    std::string _tensor2d_producer;
    std::string _image_producer;
    std::string _output_producer;

  };

  /**
     \class larcv::ImageFromTensor2DFactory
     \brief A concrete factory class for larcv::ImageFromTensor2D
  */
  class ImageFromTensor2DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ImageFromTensor2DProcessFactory() { ProcessFactory::get().add_factory("ImageFromTensor2D",this); }
    /// dtor
    ~ImageFromTensor2DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ImageFromTensor2D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

