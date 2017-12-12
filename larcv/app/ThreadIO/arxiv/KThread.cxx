#ifndef KTHREAD_CXX
#define KTHREAD_CXX

#include "KThread.h"

void KThread::start() {
  size_t id = _sum_v.size();
  _sum_v.resize(id+1);
  _thread_v.resize(id+1);
  _die_v.resize(id+1);
  std::cout << "Instantiating thread id " << id << std::endl;
  std::thread t(&KThread::ctr, this, id);
  _thread_v[id] = std::move(t);
}

#endif
