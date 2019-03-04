#ifndef EVENTBASE_CXX
#define EVENTBASE_CXX

#include "EventBase.h"
#include <sstream>
#include <iomanip>
namespace larcv {

  void EventBase::clear()
  { _run = _subrun = _event = kINVALID_SIZE; }
   
  
  std::string EventBase::event_key() const
  {
    std::stringstream ss;
    ss << std::setw( 7 ) << std::setfill( '0' ) << _run << "_"
       << std::setw( 5 ) << std::setfill( '0' ) << _subrun << "_"
       << std::setw( 6 ) << std::setfill( '0' ) << _event;
    return ss.str();
  }

}

#endif
