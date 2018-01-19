#ifndef BATCHDATASTORAGEFACTORY_CXX
#define BATCHDATASTORAGEFACTORY_CXX

#include "BatchDataStorageFactory.h"

namespace larcv {

  template <class T>
  const BatchDataStorageFactory<T>& BatchDataStorageFactory<T>::get() {
    if (!_me) _me = new BatchDataStorageFactory<T>();
    return (*_me);
  }

  template <class T>
  const BatchDataStorage<T>& BatchDataStorageFactory<T>::get_storage(std::string name) const
  {
    if (!_me) _me = new BatchDataStorageFactory<T>();
    auto iter = _storage_m.find(name);
    if (iter == _storage_m.end()) {
      LARCV_SCRITICAL() << "BatchDataStorage w/ name " << name << " not found!" << std::endl;
      throw larbys();
    }
    return iter->second;
  }

  template <class T>
  BatchDataStorage<T>& BatchDataStorageFactory<T>::get_storage_writeable(std::string name) {
    if (!_me) _me = new BatchDataStorageFactory<T>();
    auto iter = _storage_m.find(name);
    if (iter == _storage_m.end()) {
      LARCV_SCRITICAL() << "BatchDataStorage w/ name " << name << " not found!" << std::endl;
      throw larbys();
    }
    return iter->second;
  }

  template <class T>
  BatchDataStorageFactory<T>& BatchDataStorageFactory<T>::get_writeable() {
    if (!_me) _me = new BatchDataStorageFactory<T>();
    return (*_me);
  }
}
template<> larcv::BatchDataStorageFactory<char>*   larcv::BatchDataStorageFactory<char>::_me   = nullptr;
template<> larcv::BatchDataStorageFactory<short>*  larcv::BatchDataStorageFactory<short>::_me  = nullptr;
template<> larcv::BatchDataStorageFactory<int>*    larcv::BatchDataStorageFactory<int>::_me    = nullptr;
template<> larcv::BatchDataStorageFactory<float>*  larcv::BatchDataStorageFactory<float>::_me  = nullptr;
template<> larcv::BatchDataStorageFactory<double>* larcv::BatchDataStorageFactory<double>::_me = nullptr;
template<> larcv::BatchDataStorageFactory<std::string>* larcv::BatchDataStorageFactory<std::string>::_me = nullptr;

template class larcv::BatchDataStorageFactory<char>;
template class larcv::BatchDataStorageFactory<short>;
template class larcv::BatchDataStorageFactory<int>;
template class larcv::BatchDataStorageFactory<float>;
template class larcv::BatchDataStorageFactory<double>;
template class larcv::BatchDataStorageFactory<std::string>;

#endif
