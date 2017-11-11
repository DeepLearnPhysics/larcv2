/**
 * \file ImageFromCluster2D.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class ImageFromCluster2D
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __IMAGEFROMCluster2D_H__
#define __IMAGEFROMCluster2D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class ImageFromCluster2D ... these comments are used to generate
     doxygen documentation!
  */
  class ImageFromCluster2D : public ProcessBase {

  public:
    
    /// Default constructor
    ImageFromCluster2D(const std::string name="ImageFromCluster2D");
    
    /// Default destructor
    ~ImageFromCluster2D(){}

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
     \class larcv::ImageFromCluster2DFactory
     \brief A concrete factory class for larcv::ImageFromCluster2D
  */
  class ImageFromCluster2DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ImageFromCluster2DProcessFactory() { ProcessFactory::get().add_factory("ImageFromCluster2D",this); }
    /// dtor
    ~ImageFromCluster2DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ImageFromCluster2D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

