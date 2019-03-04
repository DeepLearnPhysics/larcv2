/**
 * \file DataProductFactory.h
 *
 * \ingroup core_DataFormat
 * 
 * \brief Class def header for a class larcv::DataProductFactory
 *
 * @author kazuhiro
 */

/** \addtogroup core_DataFormat

    @{*/
#ifndef __DATAPRODUCTFACTORY_H__
#define __DATAPRODUCTFACTORY_H__

#include <iostream>
#include <map>
#include "Base/larcv_base.h"
#include "Base/larbys.h"
#include "EventBase.h"
#include "DataFormatTypes.h"
#include "UtilFunc.h"
#include <sstream>

namespace larcv {

  class EventBase;
  /**
     \class ClusterAlgoFactory
     \brief Factory class for instantiating event product instance by larcv::IOManager
     This factory class can instantiate a specified product instance w/ provided producer name. \n
     The actual factory core (to which each product class must register creation factory instance) is \n
     a static std::map. Use static method to get a static instance (larcv::DataProductFactory::get) \n
     to access a factory.
  */
  class DataProductFactory : public larcv_base  {

  public:
    /// Default ctor, shouldn't be used
    DataProductFactory() : larcv_base("DataProductFactory")
    {}
    /// Default dtor
    ~DataProductFactory() {}
    /// Static sharable instance getter
    static DataProductFactory& get()
    { if(!_me) _me = new DataProductFactory; return *_me; }

    template <class T>
    EventBase* create(const std::string& producer)
    {
      auto ptr = new T;
      ((EventBase*)ptr)->_producer = producer;
      return ptr;
    }

  private:
    /// Static self
    static DataProductFactory* _me;
  };

}
#endif
/** @} */ // end of doxygen group 

