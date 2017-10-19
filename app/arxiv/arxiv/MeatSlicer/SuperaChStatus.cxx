#ifndef __SUPERACHSTATUS_CXX__
#define __SUPERACHSTATUS_CXX__

#include "SuperaChStatus.h"
#include "DataFormat/EventChStatus.h"

namespace larcv {

  static SuperaChStatusProcessFactory __global_SuperaChStatusProcessFactory__;

  SuperaChStatus::SuperaChStatus(const std::string name)
    : SuperaBase(name)
  {}
    
  void SuperaChStatus::configure(const PSet& cfg)
  {
    SuperaBase::configure(cfg);
    supera::ParamsChStatus::configure(cfg);
    _in_chstatus_producer = cfg.get<std::string>("LArChStatusProducer");
  }

  void SuperaChStatus::initialize()
  {
    SuperaBase::initialize();
    for (size_t i = 0; i < ::supera::Nplanes(); ++i) {
      ::larcv::ChStatus status;
      status.Plane(i);
      status.Initialize(::supera::Nwires(i), ::larcv::chstatus::kUNKNOWN);
      _status_m.emplace(status.Plane(), status);
    }
  }

  void SuperaChStatus::set_chstatus(::larcv::PlaneID_t plane, unsigned int wire, short status)
  {
    if (wire >= ::supera::Nwires(plane)) throw ::larcv::larbys("Invalid wire number to store status!");
    _status_m[plane].Status(wire, status);
  }

  bool SuperaChStatus::process(IOManager& mgr)
  {
    SuperaBase::process(mgr);
    
    auto ev_chstatus = (EventChStatus*)(mgr.get_data(kProductChStatus,OutChStatusLabel()));
    if(!ev_chstatus) {
      LARCV_CRITICAL() << "Output chstatus could not be created!" << std::endl;
      throw larbys();
    }

    for (auto const& id_status : _status_m)
      ev_chstatus->Insert(id_status.second);

    // Reset status
    for (auto& plane_status : _status_m) plane_status.second.Reset(::larcv::chstatus::kUNKNOWN);
    
    return true;    
  }

  void SuperaChStatus::finalize()
  { SuperaBase::finalize(); }

  bool SuperaChStatus::is(const std::string question) const
  { return (question == "SuperaChStatus"); }

}
#endif
