#ifndef __SUPERA_TYPES_CXX__
#define __SUPERA_TYPES_CXX__

#include "SuperaTypes.h"
namespace supera {
  template<> LArDataType_t LArDataType<supera::LArHit_t>()      { return LArDataType_t::kLArHit_t;      }
  template<> LArDataType_t LArDataType<supera::LArWire_t>()     { return LArDataType_t::kLArWire_t;     }
  template<> LArDataType_t LArDataType<supera::LArOpDigit_t>()  { return LArDataType_t::kLArOpDigit_t;  }
  template<> LArDataType_t LArDataType<supera::LArMCTruth_t>()  { return LArDataType_t::kLArMCTruth_t;  }
  template<> LArDataType_t LArDataType<supera::LArMCTrack_t>()  { return LArDataType_t::kLArMCTrack_t;  }
  template<> LArDataType_t LArDataType<supera::LArMCShower_t>() { return LArDataType_t::kLArMCShower_t; }
  template<> LArDataType_t LArDataType<supera::LArSimCh_t>()    { return LArDataType_t::kLArSimCh_t;    }
}
#endif
