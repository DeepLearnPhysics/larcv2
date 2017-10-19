/**
 * \file ParamsVoxel3D.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class ParamsVoxel3D
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __SUPERAPARAMSVOXEL3D_H__
#define __SUPERAPARAMSVOXEL3D_H__

#include "FMWKInterface.h"

namespace supera {

  /**
     \class ParamsVoxel3D
     User defined class ParamsVoxel3D ... these comments are used to generate
     doxygen documentation!
  */
  class ParamsVoxel3D {

  public:
    
    /// Default constructor
    ParamsVoxel3D() {}
    
    /// Default destructor
    ~ParamsVoxel3D(){}

    void configure(const supera::Config_t&);

    //
    // Getter
    //
    const std::string& OutVoxel3DLabel()  const { return _out_voxel3d_producer;  }

  private:

    std::string _out_voxel3d_producer;

  };

}

#endif
/** @} */ // end of doxygen group 

