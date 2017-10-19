/**
 * \file larcv_base.h
 *
 * \ingroup core_Base
 * 
 * \brief Class definition file of larcv::larcv_base
 *
 * @author Kazu - Nevis 2015
 */

/** \addtogroup core_Base

    @{*/

#ifndef __LARCV_BASE_H__
#define __LARCV_BASE_H__

#include <vector>
#include "larcv_logger.h"

namespace larcv {
    
  /**
    \class larcv_base
    Framework base class equipped with a logger class
  */
  class larcv_base {
    
  public:
    
    /// Default constructor
    larcv_base(const std::string logger_name="larcv_base")
      : _logger(nullptr)
    { _logger = &(::larcv::logger::get(logger_name)); }
    
    /// Default copy constructor
    larcv_base(const larcv_base &original) : _logger(original._logger) {}
    
    /// Default destructor
    virtual ~larcv_base(){};
    
    /// Logger getter
    inline const larcv::logger& logger() const
    { return *_logger; }
    
    /// Verbosity level
    void set_verbosity(::larcv::msg::Level_t level)
    { _logger->set(level); }

    /// Name getter, defined in a logger instance attribute
    const std::string& name() const
    { return logger().name(); }
    
  private:
    
    larcv::logger *_logger;   ///< logger
    
  };
}
#endif

/** @} */ // end of doxygen group
