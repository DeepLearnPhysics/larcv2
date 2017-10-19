#ifndef __BATCHFILLERTEMPLATE_CXX__
#define __BATCHFILLERTEMPLATE_CXX__

#include "BatchFillerTemplate.h"
#include <sstream>

namespace larcv {

  template<> BatchDataType_t BatchFillerTemplate< char   >::data_type() const     { return BatchDataType_t::kBatchDataChar;   }
  template<> BatchDataType_t BatchFillerTemplate< short  >::data_type() const     { return BatchDataType_t::kBatchDataShort;  }
  template<> BatchDataType_t BatchFillerTemplate< int    >::data_type() const     { return BatchDataType_t::kBatchDataInt;    }
  template<> BatchDataType_t BatchFillerTemplate< float  >::data_type() const     { return BatchDataType_t::kBatchDataFloat;  }
  template<> BatchDataType_t BatchFillerTemplate< double >::data_type() const     { return BatchDataType_t::kBatchDataDouble; }
  template<> BatchDataType_t BatchFillerTemplate<std::string>::data_type() const  { return BatchDataType_t::kBatchDataString; }

  template class BatchFillerTemplate<char>;
  template class BatchFillerTemplate<short>;
  template class BatchFillerTemplate<int>;
  template class BatchFillerTemplate<float>;
  template class BatchFillerTemplate<double>;
  template class BatchFillerTemplate<std::string>;
}
#endif
