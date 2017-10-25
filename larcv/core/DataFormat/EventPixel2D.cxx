#ifndef EVENTPIXEL2D_CXX
#define EVENTPIXEL2D_CXX

#include "EventPixel2D.h"

namespace larcv {

  /// Global larcv::EventPixel2DFactory to register EventPixel2D
  static EventPixel2DFactory __global_EventPixel2DFactory__;

  void EventPixel2D::clear()
  {
    EventBase::clear();
    VoxelSetArray2D::clear();
  }

}

#endif
