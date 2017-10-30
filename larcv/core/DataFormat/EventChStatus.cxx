#ifndef EVENTCHSTATUS_CXX
#define EVENTCHSTATUS_CXX

#include "EventChStatus.h"
#include "larcv/core/Base/larbys.h"

namespace larcv {

  /// Global larcv::SBClusterFactory to register ClusterAlgoFactory
  static EventChStatusFactory __global_EventChStatusFactory__;

  void EventChStatus::clear()
  {
    EventBase::clear();
    _status_m.clear();
  }

  const ChStatus& EventChStatus::status(ProjectionID_t id) const
  {
    auto iter = _status_m.find(id);
    if( iter == _status_m.end() ) throw larbys("Invalid request (ProjectionID_t not found)!");
    return (*iter).second;
  }

  void EventChStatus::insert(const ChStatus& status)
  { _status_m[status.id()] = status; }

  void EventChStatus::emplace(ChStatus&& status)
  { _status_m.emplace(status.id(),std::move(status)); }

  std::string EventChStatus::dump() const
  {
    std::string res;
    for(auto const& id_status : _status_m) res += id_status.second.dump() + "\n";
    return res;
  }

}

#endif
