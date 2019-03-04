/**
 * \file KThread.h
 *
 * \ingroup PyUtil
 * 
 * \brief Class def header for a class KThread
 *
 * @author kazuhiro
 */

/** \addtogroup PyUtil

    @{*/
#ifndef KTHREAD_H
#define KTHREAD_H

#include <iostream>
#include <thread>
#include <vector>
/**
   \class KThread
   User defined class KThread ... these comments are used to generate
   doxygen documentation!
 */
class KThread{

public:

  /// Default constructor
  KThread(){}

  /// Default destructor
  ~KThread(){ kill(); }
  std::vector<std::thread> _thread_v;
  std::vector<size_t> _sum_v;
  std::vector<bool> _die_v;
  inline void add(size_t id) {
    _sum_v[id] = 0;
    while(1) {
      usleep(1000000);
      _sum_v[id] += 1;
      if(_die_v[id]) break;
    }
  }
  inline size_t ctr(size_t id) const { return _sum_v.at(id); }

  void start();

  inline void kill() {
    for(size_t i=0; i<_die_v.size(); ++i) _die_v[i] = true;
    usleep(1);
  }
};

#endif
/** @} */ // end of doxygen group 

