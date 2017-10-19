#ifndef __LARCV_CHSTATUS_CXX__
#define __LARCV_CHSTATUS_CXX__

#include <sstream>
#include "ChStatus.h"
#include "Base/larbys.h"

namespace larcv {

  void  ChStatus::Initialize(size_t nwires, short init_status)
  {
    _status_v.resize(nwires);
    Reset(init_status);
  }

  void  ChStatus::Reset(short init_status)
  { for(auto& s : _status_v) s = init_status; }
  
  void  ChStatus::Status(size_t wire, short status)
  {
    if(wire >= _status_v.size()) {
      std::stringstream ss;
      ss << "<<ChStatus::" << __FUNCTION__ << ">>"
	 << " Invalid wire requested (" << wire 
	 << " while currently registered # wires: " << _status_v.size() << ")";
      throw larbys(ss.str());
    }
    _status_v[wire] = status;
  }
  
  short ChStatus::Status(size_t wire) const
  {
    if(wire >= _status_v.size()) {
      std::stringstream ss;
      ss << "<<ChStatus::" << __FUNCTION__ << ">>"
	 << " Invalid wire requested (" << wire 
	 << " while currently registered # wires: " << _status_v.size() << ")";
      throw larbys(ss.str());
    }
    return _status_v[wire];
  }

  std::string ChStatus::dump() const
  {
    std::stringstream ss;
    ss << "Plane " << _plane << " ... " << _status_v.size() << " wires:" << std::endl;
    for(size_t i=0; i<_status_v.size(); ++i) {
      if(_status_v[i] >= chstatus::kGOOD)
	ss << _status_v[i] << " ";
      else
	ss << "\033[93m" << _status_v[i] << "\033[00m ";
      if(i && (i+1)%50==0) ss << std::endl;
    }
    if(_status_v.size()%50)
      ss << std::endl;
    return ss.str();
  }

}

#endif
