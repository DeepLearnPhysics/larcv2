#ifndef THREADIOTYPES_CXX
#define THREADIOTYPES_CXX

#include "ThreadIOTypes.h"

namespace larcv{

  std::string BatchDataTypeName(BatchDataType_t type) {
    switch(type) {
    case BatchDataType_t::kBatchDataChar:
      return std::string("char");
    case BatchDataType_t::kBatchDataShort:
      return std::string("short");
    case BatchDataType_t::kBatchDataInt:
      return std::string("int");
    case BatchDataType_t::kBatchDataFloat:
      return std::string("float");
    case BatchDataType_t::kBatchDataDouble:
      return std::string("double");
    case BatchDataType_t::kBatchDataString:
      return std::string("string");
    default:
      return std::string("");
    }
  }
}

#endif
