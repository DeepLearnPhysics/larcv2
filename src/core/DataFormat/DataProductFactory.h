/**
 * \file DataProductFactory.h
 *
 * \ingroup DataFormat
 * 
 * \brief Class def header for a class DataProductFactory
 *
 * @author kazuhiro
 */

/** \addtogroup DataFormat

    @{*/
#ifndef __DATAPRODUCTFACTORY_H__
#define __DATAPRODUCTFACTORY_H__

#include <iostream>
#include <map>
#include "larcv/core/Base/larcv_base.h"
#include "larcv/core/Base/larbys.h"
#include "EventBase.h"
#include "DataFormatTypes.h"
#include <sstream>
namespace larcv {

  class EventBase;
  /**
     \class DataProductFactoryBase
     \brief Abstract base class for factory (to be implemented per data product)
  */
  class DataProductFactoryBase {
  public:
    /// Default ctor
    DataProductFactoryBase(){}
    /// Default dtor (virtual)
    virtual ~DataProductFactoryBase(){}
    /// Abstract constructor method
    virtual EventBase* create() = 0;
  };

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
    ~DataProductFactory() {_factory_map.clear();}
    /// Static sharable instance getter
    static inline DataProductFactory& get()
    { if(!_me) _me = new DataProductFactory; return *_me; }
    
    /// Factory registration method (should be called by global factory instance in algorithm header)
    void add_factory(std::string type, larcv::DataProductFactoryBase* factory);

    /// Factory creation method (should be called by clients, possibly you!)
    inline EventBase* create(const std::string& type, const std::string& producer) {
      return create(ProducerName_t(type,producer));
    }
    
    /// Factory creation method (should be called by clients, possibly you!)
    EventBase* create(const ProducerName_t& id);

    /// List registered products
    void list() const;

    inline size_t unique_product_count() const
    { return _id_to_type.size(); }

    inline const std::vector<std::string>& product_names() const
    { return _id_to_type; }

  private:
    /// Factory container
    std::map<std::string,larcv::DataProductFactoryBase*> _factory_map;
    /// Unique product type ID
    std::vector<std::string> _id_to_type;
    /// Static self
    static DataProductFactory* _me;
  };
}
#endif
/** @} */ // end of doxygen group 

