/**
 * \file ParamsImage2D.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class ParamsImage2D
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __SUPERAPARAMSIMAGE2D_H__
#define __SUPERAPARAMSIMAGE2D_H__

#include "FMWKInterface.h"

namespace supera {

  /**
     \class ParamsImage2D
     User defined class ParamsImage2D ... these comments are used to generate
     doxygen documentation!
  */
  class ParamsImage2D {

  public:
    
    /// Default constructor
    ParamsImage2D() {}
    
    /// Default destructor
    ~ParamsImage2D(){}

    void configure(const supera::Config_t&);

    //
    // Getter
    //
    const std::string& OutImageLabel()    const { return _out_image_producer;    }

  private:

    std::string _out_image_producer;
  };

}

#endif
/** @} */ // end of doxygen group 

