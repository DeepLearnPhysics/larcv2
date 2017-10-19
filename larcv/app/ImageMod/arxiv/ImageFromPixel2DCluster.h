/**
 * \file ImageFromPixel2DCluster.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class ImageFromPixel2DCluster
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __IMAGEFROMPIXEL2D_H__
#define __IMAGEFROMPIXEL2D_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class ImageFromPixel2DCluster ... these comments are used to generate
     doxygen documentation!
  */
  class ImageFromPixel2DCluster : public ProcessBase {

  public:
    
    /// Default constructor
    ImageFromPixel2DCluster(const std::string name="ImageFromPixel2DCluster");
    
    /// Default destructor
    ~ImageFromPixel2DCluster(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    enum class PIType_t {
      kPITypeFixedPI,
      kPITypeInputImage,
      kPITypeClusterIndex
    };
    float _fixed_pi;
    PIType_t _type_pi;
    std::string _pixel_producer;
    std::string _image_producer;
    std::string _output_producer;

  };

  /**
     \class larcv::ImageFromPixel2DClusterFactory
     \brief A concrete factory class for larcv::ImageFromPixel2DCluster
  */
  class ImageFromPixel2DClusterProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ImageFromPixel2DClusterProcessFactory() { ProcessFactory::get().add_factory("ImageFromPixel2DCluster",this); }
    /// dtor
    ~ImageFromPixel2DClusterProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ImageFromPixel2DCluster(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

