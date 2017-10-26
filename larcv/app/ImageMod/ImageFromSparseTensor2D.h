/**
 * \file ImageFromSparseTensor2D.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class ImageFromSparseTensor2D
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __IMAGEFROMSPARSETENSOR2D_H__
#define __IMAGEFROMSPARSETENSOR2D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class ImageFromSparseTensor2D ... these comments are used to generate
     doxygen documentation!
  */
  class ImageFromSparseTensor2D : public ProcessBase {

  public:
    
    /// Default constructor
    ImageFromSparseTensor2D(const std::string name="ImageFromSparseTensor2D");
    
    /// Default destructor
    ~ImageFromSparseTensor2D(){}

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
     \class larcv::ImageFromSparseTensor2DFactory
     \brief A concrete factory class for larcv::ImageFromSparseTensor2D
  */
  class ImageFromSparseTensor2DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ImageFromSparseTensor2DProcessFactory() { ProcessFactory::get().add_factory("ImageFromSparseTensor2D",this); }
    /// dtor
    ~ImageFromSparseTensor2DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ImageFromSparseTensor2D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

