#ifndef __SUPERABASE_CXX__
#define __SUPERABASE_CXX__

#include "SuperaBase.h"

namespace larcv {

  static SuperaBaseProcessFactory __global_SuperaBaseProcessFactory__;

  SuperaBase::SuperaBase(const std::string name)
  : ProcessBase(name)
  , _empty_string()
  { ClearEventData(); }

  void SuperaBase::configure(const PSet& cfg)
  { }

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
  { _event = nullptr; }

  // FIXME(kvtsang) Temporary solution to access associations
  const TG4Event* SuperaBase::GetEvent()
  {
    if (!_event)
    throw larbys("art::Event not set!");
    return _event;
  }


}

#endif
