/**
 * \file larbys.h
 *
 * \ingroup core_Base
 * 
 * \brief Class def header for exception classes for larcv framework
 *
 * @author kazuhiro tmw
 */

/** \addtogroup core_Base

    @{*/
#ifndef __LARCV_LARBYS_H__
#define __LARCV_LARBYS_H__

#include <iostream>
#include <exception>

namespace larcv {

  /**
     \class larbys
     Throw insignificant larbys when you find nonesense 
  */
  class larbys : public std::exception {
    
  public:
    
    larbys(std::string msg="") : std::exception()
    {
      _msg = "\033[93m";
      _msg += msg;
      _msg += "\033[00m";
    }

    virtual ~larbys() throw() {}
    virtual const char* what() const throw()
    { return _msg.c_str(); }

  private:
    
    std::string _msg;
  };
}

#endif
/** @} */ // end of doxygen group 

