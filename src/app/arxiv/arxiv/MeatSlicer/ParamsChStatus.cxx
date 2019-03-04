#ifndef __SUPERAPARAMSCHSTATUS_CXX__
#define __SUPERAPARAMSCHSTATUS_CXX__

#include "ParamsChStatus.h"

namespace supera {

  void ParamsChStatus::configure(const supera::Config_t& cfg)
  {
    _out_chstatus_producer = cfg.get<std::string>("OutChStatusLabel", "");
  }  
}
#endif
