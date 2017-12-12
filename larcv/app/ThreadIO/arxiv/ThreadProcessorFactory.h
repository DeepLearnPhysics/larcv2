/**
 * \file ThreadProcessorFactory.h
 *
 * \ingroup ThreadIO
 * 
 * \brief Class def header for a class ThreadProcessorFactory
 *
 * @author kazuhiro
 */

/** \addtogroup ThreadIO

    @{*/
#ifndef THREADPROCESSORFACTORY_H
#define THREADPROCESSORFACTORY_H

#include <iostream>

namespace larcv {
  
  class ThreadProcessor;
  
  /**
     \class ThreadProcessorFactory
     User defined class ThreadProcessorFactory ... these comments are used to generate
     doxygen documentation!
  */
  class ThreadProcessorFactory{
    
  public:
    
    /// Default constructor
    ThreadProcessorFactory(){}
    
    /// Default destructor
    ~ThreadProcessorFactory();

    /// Instance getter
    ThreadProcessorFactory& get()
    {
      if(!_me) _me = new ThreadProcessorFactory;
      return _me;
    }

    /// Create a new ThreadProcessor
    ThreadProcessorID_t construct(const larcv::PSet cfg);
    /// Reset an instance
    bool reset(ThreadProcessorID_t id);
    /// Destruct an instance
    bool kill(ThreadProcessorID_t id);
    /// 
  private:
    /// string=>id mapping
    std::map<std::string,size_t> _id_m;
    /// ThreadProcessor instance pointer
    std::vector<larcv::ThreadProcessor*> _tp_ptr_v;
  };
}

#endif
/** @} */ // end of doxygen group 

