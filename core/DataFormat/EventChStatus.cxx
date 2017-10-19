#ifndef EVENTCHSTATUS_CXX
#define EVENTCHSTATUS_CXX

#include "EventChStatus.h"
#include "Base/larbys.h"

namespace larcv {

  /// Global larcv::SBClusterFactory to register ClusterAlgoFactory
  static EventChStatusFactory __global_EventChStatusFactory__;

  void EventChStatus::clear()
  {
    EventBase::clear();
    _status_m.clear();
  }

  const ChStatus& EventChStatus::Status(PlaneID_t id) const
  {
    auto iter = _status_m.find(id);
    if( iter == _status_m.end() ) throw larbys("Invalid request (PlaneID_t not found)!");
    return (*iter).second;
  }

  void EventChStatus::Insert(const ChStatus& status)
  { _status_m[status.Plane()] = status; }

  void EventChStatus::Emplace(ChStatus&& status)
  { _status_m.emplace(status.Plane(),std::move(status)); }

  std::string EventChStatus::dump() const
  {
    std::string res;
    for(auto const& plane_status : _status_m) res += plane_status.second.dump() + "\n";
    return res;
  }

}

#endif
