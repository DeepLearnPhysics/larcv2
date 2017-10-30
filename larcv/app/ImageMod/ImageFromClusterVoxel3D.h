/**
 * \file ImageFromClusterVoxel3D.h
 *
 * \ingroup ImageMod
 * 
 * \brief Class def header for a class ImageFromClusterVoxel3D
 *
 * @author kazuhiro
 */

/** \addtogroup ImageMod

    @{*/
#ifndef __IMAGEFROMCLUSTERVOXEL3D_H__
#define __IMAGEFROMCLUSTERVOXEL3D_H__

#include "larcv/core/Processor/ProcessBase.h"
#include "larcv/core/Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class ImageFromClusterVoxel3D ... these comments are used to generate
     doxygen documentation!
  */
  class ImageFromClusterVoxel3D : public ProcessBase {

  public:
    
    /// Default constructor
    ImageFromClusterVoxel3D(const std::string name="ImageFromClusterVoxel3D");
    
    /// Default destructor
    ~ImageFromClusterVoxel3D(){}

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
     \class larcv::ImageFromClusterVoxel3DFactory
     \brief A concrete factory class for larcv::ImageFromClusterVoxel3D
  */
  class ImageFromClusterVoxel3DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    ImageFromClusterVoxel3DProcessFactory() { ProcessFactory::get().add_factory("ImageFromClusterVoxel3D",this); }
    /// dtor
    ~ImageFromClusterVoxel3DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new ImageFromClusterVoxel3D(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

