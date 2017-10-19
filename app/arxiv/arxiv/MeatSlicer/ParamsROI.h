/**
 * \file ParamsROI.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class ParamsROI
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __SUPERAPARAMSROI_H__
#define __SUPERAPARAMSROI_H__

#include "FMWKInterface.h"

namespace supera {

  /**
     \class ParamsROI
     User defined class ParamsROI ... these comments are used to generate
     doxygen documentation!
  */
  class ParamsROI {

  public:
    
    /// Default constructor
    ParamsROI() {}
    
    /// Default destructor
    ~ParamsROI(){}

    void configure(const supera::Config_t&);

    //
    // Getter
    //
    const std::string& OutROILabel()      const { return _out_roi_producer;      }

  private:

    std::string _out_roi_producer;

  };

}

#endif
/** @} */ // end of doxygen group 

