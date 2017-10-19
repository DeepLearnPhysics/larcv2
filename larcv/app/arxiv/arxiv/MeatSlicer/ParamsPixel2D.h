/**
 * \file ParamsPixel2D.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class ParamsPixel2D
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __SUPERAPARAMS_PIXEL2D_H__
#define __SUPERAPARAMS_PIXEL2D_H__

#include "FMWKInterface.h"

namespace supera {

  /**
     \class ParamsPixel2D
     User defined class ParamsPixel2D ... these comments are used to generate
     doxygen documentation!
  */
  class ParamsPixel2D {

  public:
    
    /// Default constructor
    ParamsPixel2D() {}
    
    /// Default destructor
    ~ParamsPixel2D(){}

    void configure(const supera::Config_t&);

    //
    // Getter
    //
    const std::string& OutPixel2DLabel()  const { return _out_pixel2d_producer;  }

  private:

    std::string _out_pixel2d_producer;

  };

}

#endif
/** @} */ // end of doxygen group 

