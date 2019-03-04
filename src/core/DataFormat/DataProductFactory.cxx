#ifndef __DATAPRODUCTFACTORY_CXX__
#define __DATAPRODUCTFACTORY_CXX__

#include "DataProductFactory.h"

larcv::DataProductFactory* larcv::DataProductFactory::_me = nullptr;

namespace larcv {
  
  void DataProductFactory::add_factory(std::string type, larcv::DataProductFactoryBase* factory)
  {
    if(type.empty()) {
      LARCV_CRITICAL() << "Attempted to register a data product type with empty name!" << std::endl
		       << "Maybe a new data product source code was made by gen_class_product script?" << std::endl
		       << "This error happens when that + forgotten implementation of product_unique_name<T> function (see the header!)" << std::endl
		       << std::endl;
      throw larbys();
    }
    
    LARCV_INFO() << "Registering a factory " << factory << " of type " << type << std::endl;
    
    auto iter = _factory_map.find(type);
    
    if(iter != _factory_map.end()) {
      LARCV_CRITICAL() << "Attempted a duplicate registration of Data product "
		       << type << " to a factory!" << std::endl;
      throw larbys();
    }
    
    _factory_map[type] = factory;
    _id_to_type.push_back(type);
  }

  EventBase* DataProductFactory::create(const ProducerName_t& id) {
    auto iter = _factory_map.find(id.first);
    if(iter == _factory_map.end() || !((*iter).second)) {
      LARCV_ERROR() << "Found no registered class " << id.first << std::endl;
      return nullptr;
    }
    auto ptr = (*iter).second->create();
    ptr->_producer = id.second;
    return ptr;
  }
  
  void DataProductFactory::list() const {
    std::stringstream ss;
    ss << "    Listing registered products:" << std::endl;
    for(size_t id=0; id<_id_to_type.size(); ++id) {
      ss << "    Name: " << _id_to_type[id]
	 << " ... ID=" << id
	 << " ... Factory @ " << _factory_map.find(_id_to_type[id])->second
	 << std::endl;
    }
    ss << std::endl;
    LARCV_NORMAL() << ss.str() << std::endl;
  }
  
}

#endif
