#ifndef EVENTPIXEL2D_CXX
#define EVENTPIXEL2D_CXX

#include "EventPixel2D.h"
#include "Base/larcv_logger.h"
#include "Base/larbys.h"

namespace larcv {

  /// Global larcv::EventPixel2DFactory to register EventPixel2D
  static EventPixel2DFactory __global_EventPixel2DFactory__;

  void EventPixel2D::clear()
  {
    EventBase::clear();
    VoxelSet::Clear();
    _meta = ImageMeta();
  }

}

#endif
