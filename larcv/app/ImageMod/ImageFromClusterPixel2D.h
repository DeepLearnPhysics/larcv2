/**
 * \file ImageFromClusterPixel2D.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class ImageFromClusterPixel2D
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __IMAGEFROMCLUSTERPIXEL2D_H__
#define __IMAGEFROMCLUSTERPIXEL2D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class ImageFromClusterPixel2D ... these comments are used to generate
     doxygen documentation!
  */
  class ImageFromClusterPixel2D : public ProcessBase {

  public:
    
    /// Default constructor
    ImageFromClusterPixel2D(const std::string name="ImageFromClusterPixel2D");
    
    /// Default destructor
    ~ImageFromClusterPixel2D(){}

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
    std::string _pixel2d_producer;
    std::string _image_producer;
    std::string _output_producer;

  };

  /**
     \class larcv::ImageFromClusterPixel2DFactory
     \brief A concrete factory class for larcv::ImageFromClusterPixel2D
  */
  class ImageFromClusterPixel2DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ImageFromClusterPixel2DProcessFactory() { ProcessFactory::get().add_factory("ImageFromClusterPixel2D",this); }
    /// dtor
    ~ImageFromClusterPixel2DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ImageFromClusterPixel2D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

