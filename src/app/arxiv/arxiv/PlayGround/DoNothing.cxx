#ifndef __DONOTHING_CXX__
#define __DONOTHING_CXX__

#include "DoNothing.h"
#include "DataFormat/EventImage2D.h"
namespace larcv {

  static DoNothingProcessFactory __global_DoNothingProcessFactory__;

  DoNothing::DoNothing(const std::string name)
    : ProcessBase(name)
  {}
    
  void DoNothing::configure(const PSet& cfg)
  {
    auto my_integer = cfg.get<int>("MyInteger",-1);

    LARCV_DEBUG() << "MyInteger @ Configuration: " << my_integer << std::endl;
    
  }

  void DoNothing::initialize()
  {}

  bool DoNothing::process(IOManager& mgr)
  {
    auto my_event_image2d = (EventImage2D*)(mgr.get_data(kProductImage2D,"imread"));
    LARCV_NORMAL() << my_event_image2d << std::endl;
    return true;
  }

  void DoNothing::finalize()
  {}

}
#endif
