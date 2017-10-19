/**
 * \file ParamsChStatus.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class ParamsChStatus
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __SUPERAPARAMSCHSTATUS_H__
#define __SUPERAPARAMSCHSTATUS_H__

#include "FMWKInterface.h"

namespace supera {

  /**
     \class ParamsChStatus
     User defined class ParamsChStatus ... these comments are used to generate
     doxygen documentation!
  */
  class ParamsChStatus {

  public:
    
    /// Default constructor
    ParamsChStatus() {}
    
    /// Default destructor
    ~ParamsChStatus(){}

    void configure(const supera::Config_t&);

    //
    // Getter
    //
    const std::string& OutChStatusLabel() const { return _out_chstatus_producer; }

  private:

    std::string _out_chstatus_producer;
    
  };

}

#endif
/** @} */ // end of doxygen group 

