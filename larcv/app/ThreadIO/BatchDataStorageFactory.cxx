#ifndef BATCHDATASTORAGEFACTORY_CXX
#define BATCHDATASTORAGEFACTORY_CXX

#include "BatchDataStorageFactory.h"

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
