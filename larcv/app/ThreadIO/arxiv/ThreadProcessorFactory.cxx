#ifndef THREADPROCESSORFACTORY_CXX
#define THREADPROCESSORFACTORY_CXX

#include "ThreadProcessorFactory.h"

namespace larcv {

  ThreadProcessorFactory::~ThreadProcessorFactory()
  { for(auto ptr : _tp_ptr_v) {
      if(ptr) ptr->reset();
      delete ptr;
    }
  }

  bool ThreadProcessorFactory::reset(ThreadProcessorID_t id)
  {
    if(id >= _tp_ptr_v.size()) {
      LARCV_ERROR() << "ThreadProcessorID_t " << id << " is invalid" << std::endl;
      return false;
    }
    auto ptr = _tp_ptr_v[id];
    if(!ptr) return false;
    if(ptr) ptr->reset();
    return true;
  }

  bool ThreadProcessorFactory::kill(ThreadProcessorID_t id)
  {
    if(!reset(id)) return false;
    auto ptr = _tp_ptr_v[id];
    if(!ptr) return false;
    delete ptr;
    return true;
  }

  

}

#endif
