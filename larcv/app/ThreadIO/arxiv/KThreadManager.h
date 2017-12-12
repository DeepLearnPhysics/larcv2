/**
 * \file KThreadManager.h
 *
 * \ingroup PyUtil
 * 
 * \brief Class def header for a class KThreadManager
 *
 * @author kazuhiro
 */

/** \addtogroup PyUtil

    @{*/
#ifndef KTHREADMANAGER_H
#define KTHREADMANAGER_H

#include <iostream>

/**
   \class KThreadManager
   User defined class KThreadManager ... these comments are used to generate
   doxygen documentation!
 */

class KThread;

class KThreadManager{

public:

  /// Default constructor
  KThreadManager() : _ptr(0) {}

  /// Default destructor
  ~KThreadManager(){ kill(); }

  void start();

  void kill();
private:
  KThread* _ptr;

};

#endif
/** @} */ // end of doxygen group 

