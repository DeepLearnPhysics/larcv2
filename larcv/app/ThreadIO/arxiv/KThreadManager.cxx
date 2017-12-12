#ifndef KTHREADMANAGER_CXX
#define KTHREADMANAGER_CXX

#include "KThreadManager.h"
#include "KThread.h"

void KThreadManager::start() {

  if(!_ptr) _ptr = new KThread;
  _ptr->start();
  
}

void KThreadManager::kill() {
  if(_ptr) _ptr->kill();
}

#endif
