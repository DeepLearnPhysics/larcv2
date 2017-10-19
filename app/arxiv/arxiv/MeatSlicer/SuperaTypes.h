#ifndef __SUPERA_TYPES_H__
#define __SUPERA_TYPES_H__
//#ifndef __CINT__
//#ifndef __CLING__
#include "FMWKInterface.h"
namespace supera {

  /// enum to define LAr* data type 
  enum class LArDataType_t : unsigned int{
    kLArWire_t,      ///< recob::Wire 
    kLArHit_t,       ///< recob::Hit
    kLArOpDigit_t,   ///< raw::OpDetWaveform
    kLArMCTruth_t,   ///< simb::MCTruth
    kLArMCTrack_t,   ///< sim::MCTrack
    kLArMCShower_t,  ///< sim::MCShower
    kLArSimCh_t,      ///< sim::SimChannel
    kLArDataTypeMax 
  };

  template <class T>
  LArDataType_t LArDataType();

  template<> LArDataType_t LArDataType<supera::LArHit_t>();
  template<> LArDataType_t LArDataType<supera::LArWire_t>();
  template<> LArDataType_t LArDataType<supera::LArOpDigit_t>();
  template<> LArDataType_t LArDataType<supera::LArMCTruth_t>();
  template<> LArDataType_t LArDataType<supera::LArMCTrack_t>();
  template<> LArDataType_t LArDataType<supera::LArMCShower_t>();
  template<> LArDataType_t LArDataType<supera::LArSimCh_t>();
  
}
//#endif
//#endif
#endif
