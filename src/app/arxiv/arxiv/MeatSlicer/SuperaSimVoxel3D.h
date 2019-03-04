/**
 * \file SuperaSimVoxel3D.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class SuperaSimVoxel3D
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __SUPERASIMVOXEL3D_H__
#define __SUPERASIMVOXEL3D_H__
#include "SuperaBase.h"
#include "FMWKInterface.h"
#include "ParamsVoxel3D.h"
#include "ImageMetaMaker.h"
#include "DataFormat/Image2D.h"

namespace larcv {

  /**
     \class ProcessBase
     User defined class SuperaSimVoxel3D ... these comments are used to generate
     doxygen documentation!
  */
  class SuperaSimVoxel3D : public SuperaBase,
			   public supera::ParamsVoxel3D,
			   public supera::ImageMetaMaker {

  public:
    
    /// Default constructor
    SuperaSimVoxel3D(const std::string name="SuperaSimVoxel3D");
    
    /// Default destructor
    ~SuperaSimVoxel3D(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    unsigned short _origin;
    double _voxel_size;
    size_t _target_plane;
    size_t _t0_tick;
  };

  /**
     \class larcv::SuperaSimVoxel3DFactory
     \brief A concrete factory class for larcv::SuperaSimVoxel3D
  */
  class SuperaSimVoxel3DProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SuperaSimVoxel3DProcessFactory() { ProcessFactory::get().add_factory("SuperaSimVoxel3D",this); }
    /// dtor
    ~SuperaSimVoxel3DProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SuperaSimVoxel3D(instance_name); }
  };

}
#endif
/** @} */ // end of doxygen group 

