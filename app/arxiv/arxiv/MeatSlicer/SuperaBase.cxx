#ifndef __SUPERABASE_CXX__
#define __SUPERABASE_CXX__

#include "SuperaBase.h"

namespace larcv {

  static SuperaBaseProcessFactory __global_SuperaBaseProcessFactory__;

  SuperaBase::SuperaBase(const std::string name)
    : ProcessBase(name)
    , _empty_string()
  { ClearEventData(); }

  void SuperaBase::Request(supera::LArDataType_t type, std::string name)
  { _data_request_m[type]=name; }

  const std::string& SuperaBase::LArDataLabel(supera::LArDataType_t type) const
  {
    auto iter = _data_request_m.find(type);
    if(iter == _data_request_m.end()) return _empty_string;
    return (*iter).second;
  }
  
  void SuperaBase::configure(const PSet& cfg)
  {
    _time_offset  = cfg.get<int>("TimeOffset",2400);
    
    auto producer_wire     = cfg.get<std::string>("LArWireProducer",    "");
    auto producer_hit      = cfg.get<std::string>("LArHitProducer",     "");
    auto producer_opdigit  = cfg.get<std::string>("LArOpDigitProducer", "");
    auto producer_mctruth  = cfg.get<std::string>("LArMCTruthProducer", "");
    auto producer_mctrack  = cfg.get<std::string>("LArMCTrackProducer", "");
    auto producer_mcshower = cfg.get<std::string>("LArMCShowerProducer","");
    auto producer_simch    = cfg.get<std::string>("LArSimChProducer",   "");

    if(!producer_wire.empty()    ) {
      LARCV_INFO() << "Requesting Wire data product by " << producer_wire << std::endl;
      Request(supera::LArDataType_t::kLArWire_t, producer_wire);
    }

    if(!producer_hit.empty()     ) {
      LARCV_INFO() << "Requesting Hit data product by " << producer_hit << std::endl;
      Request(supera::LArDataType_t::kLArHit_t, producer_hit);
    }
    
    if(!producer_opdigit.empty() ) {
      LARCV_INFO() << "Requesting OpDigit data product by " << producer_opdigit << std::endl;
      Request(supera::LArDataType_t::kLArOpDigit_t, producer_opdigit );
    }
    
    if(!producer_mctruth.empty() ) {
      LARCV_INFO() << "Requesting MCTruth data product by " << producer_mctruth << std::endl;
      Request(supera::LArDataType_t::kLArMCTruth_t, producer_mctruth );
    }
    
    if(!producer_mctrack.empty() ) {
      LARCV_INFO() << "Requesting MCTrack data product by " << producer_mctrack << std::endl;
      Request(supera::LArDataType_t::kLArMCTrack_t, producer_mctrack );
    }
    
    if(!producer_mcshower.empty()) {
      LARCV_INFO() << "Requesting MCShower data product by " << producer_mcshower << std::endl;
      Request(supera::LArDataType_t::kLArMCShower_t, producer_mcshower);
    }
    
    if(!producer_simch.empty()   ) {
      LARCV_INFO() << "Requesting SimCh data product by " << producer_simch << std::endl;
      Request(supera::LArDataType_t::kLArSimCh_t, producer_simch);
    }

  }

  void SuperaBase::initialize()
  {ClearEventData();}

  bool SuperaBase::process(IOManager& mgr)
  { return true; }

  void SuperaBase::finalize()
  {ClearEventData();}

  bool SuperaBase::is(const std::string question) const
  {
    if(question == "Supera") return true;
    return false;
  }

  void SuperaBase::ClearEventData()
  {
    _ptr_wire_v     = nullptr;
    _ptr_hit_v      = nullptr;
    _ptr_opdigit_v  = nullptr;
    _ptr_sch_v      = nullptr;
    _ptr_mctruth_v  = nullptr;
    _ptr_mct_v      = nullptr;
    _ptr_mcs_v      = nullptr;
  }

  template <> const std::vector<supera::LArWire_t>& SuperaBase::LArData<supera::LArWire_t>() const
  { if(!_ptr_wire_v) throw larbys("Wire data pointer not available"); return *_ptr_wire_v; }

  template <> const std::vector<supera::LArHit_t>& SuperaBase::LArData<supera::LArHit_t>() const
  { if(!_ptr_hit_v) throw larbys("Hit data pointer not available"); return *_ptr_hit_v; }

  template <> const std::vector<supera::LArOpDigit_t>& SuperaBase::LArData<supera::LArOpDigit_t>() const
  { if(!_ptr_opdigit_v) throw larbys("OpDigit data pointer not available"); return *_ptr_opdigit_v; }

  template <> const std::vector<supera::LArSimCh_t>& SuperaBase::LArData<supera::LArSimCh_t>() const
  { if(!_ptr_sch_v) throw larbys("SimCh data pointer not available"); return *_ptr_sch_v; }

  template <> const std::vector<supera::LArMCTruth_t>& SuperaBase::LArData<supera::LArMCTruth_t>() const
  { if(!_ptr_mctruth_v) throw larbys("MCTruth data pointer not available"); return *_ptr_mctruth_v; }

  template <> const std::vector<supera::LArMCTrack_t>& SuperaBase::LArData<supera::LArMCTrack_t>() const
  { if(!_ptr_mct_v) throw larbys("MCTrack data pointer not available"); return *_ptr_mct_v; }

  template <> const std::vector<supera::LArMCShower_t>& SuperaBase::LArData<supera::LArMCShower_t>() const
  { if(!_ptr_mcs_v) throw larbys("MCShower data pointer not available"); return *_ptr_mcs_v; }

  template <> void SuperaBase::LArData(const std::vector<supera::LArWire_t>& data_v)
  { _ptr_wire_v = (std::vector<supera::LArWire_t>*)(&data_v); }

  template <> void SuperaBase::LArData(const std::vector<supera::LArHit_t>& data_v)
  { _ptr_hit_v = (std::vector<supera::LArHit_t>*)(&data_v); }

  template <> void SuperaBase::LArData(const std::vector<supera::LArOpDigit_t>& data_v)
  { _ptr_opdigit_v = (std::vector<supera::LArOpDigit_t>*)(&data_v); }

  template <> void SuperaBase::LArData(const std::vector<supera::LArMCTruth_t>& data_v)
  { _ptr_mctruth_v = (std::vector<supera::LArMCTruth_t>*)(&data_v); }

  template <> void SuperaBase::LArData(const std::vector<supera::LArMCTrack_t>& data_v)
  { _ptr_mct_v = (std::vector<supera::LArMCTrack_t>*)(&data_v); }

  template <> void SuperaBase::LArData(const std::vector<supera::LArMCShower_t>& data_v)
  { _ptr_mcs_v = (std::vector<supera::LArMCShower_t>*)(&data_v); }

  template <> void SuperaBase::LArData(const std::vector<supera::LArSimCh_t>& data_v)
  { _ptr_sch_v = (std::vector<supera::LArSimCh_t>*)(&data_v); }
  
}
#endif
