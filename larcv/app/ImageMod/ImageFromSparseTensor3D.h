/**
 * \file ImageFromSparseTensor3D.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class ImageFromSparseTensor3D
 *
 * @author kazuhiro
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __IMAGEFROMSPARSETENSOR3D_H__
#define __IMAGEFROMSPARSETENSOR3D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class ImageFromSparseTensor3D ... these comments are used to generate
     doxygen documentation!
  */
  class ImageFromSparseTensor3D : public ProcessBase {

  public:
    
    /// Default constructor
    ImageFromSparseTensor3D(const std::string name="ImageFromSparseTensor3D");
    
    /// Default destructor
    ~ImageFromSparseTensor3D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::string _voxel3d_producer;
    std::string _output_producer;
    bool _xy, _yz, _zx;

  };

  /**
     \class larcv::ImageFromSparseTensor3DFactory
     \brief A concrete factory class for larcv::ImageFromSparseTensor3D
  */
  class ImageFromSparseTensor3DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ImageFromSparseTensor3DProcessFactory() { ProcessFactory::get().add_factory("ImageFromSparseTensor3D",this); }
    /// dtor
    ~ImageFromSparseTensor3DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ImageFromSparseTensor3D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

