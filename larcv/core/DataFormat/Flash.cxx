#ifndef __LARCV_FLASH_CXX__
#define __LARCV_FLASH_CXX__

#include "Flash.h"
#include "larcv/core/Base/larbys.h"
#include <numeric> // std::accumulate()

namespace larcv {

    Flash::Flash(double time, double timeWidth, double absTime, unsigned int frame,
        std::vector<double> PEPerOpDet,
        bool inBeamFrame, int onBeamTime, double fastToTotal,
        double xCenter, double xWidth,
        double yCenter, double yWidth,
        double zCenter, double zWidth,
        std::vector<double> wireCenters,
        std::vector<double> wireWidths,
        InstanceID_t id)
        : _id(id)
        , _time(time)
        , _timeWidth(timeWidth)
        , _absTime  (absTime)
        , _frame    (frame)
        , _PEPerOpDet (PEPerOpDet)
        , _wireCenters (wireCenters)
        , _wireWidths  (wireWidths)
        , _xCenter     (xCenter)
        , _xWidth      (xWidth)
        , _yCenter     (yCenter)
        , _yWidth      (yWidth)
        , _zCenter     (zCenter)
        , _zWidth      (zWidth)
        , _fastToTotal (fastToTotal)
        , _inBeamFrame (inBeamFrame)
        , _onBeamTime  (onBeamTime)
    {}

    double Flash::TotalPE() const {
        return std::accumulate(_PEPerOpDet.begin(), _PEPerOpDet.end(), 0.0);
    }

    void FlashSet::set(const std::vector<larcv::Flash>& flash_v)
    {
      _flash_v = flash_v;
      for(size_t i=0; i<_flash_v.size(); ++i)
        if(_flash_v.back().id() == kINVALID_INSTANCEID)
          _flash_v[i].id(i);
    }

    void FlashSet::append(const larcv::Flash& flash)
    {
      _flash_v.push_back(flash);
      if(_flash_v.back().id() == kINVALID_INSTANCEID)
        _flash_v.back().id(_flash_v.size()-1);
    }

    void FlashSet::emplace_back(larcv::Flash&& flash)
    {
      _flash_v.emplace_back(std::move(flash));
      if(_flash_v.back().id() == kINVALID_INSTANCEID)
        _flash_v.back().id(_flash_v.size()-1);
    }

    void FlashSet::emplace(std::vector<larcv::Flash>&& flash_v)
    {
      _flash_v = std::move(flash_v);
      for(size_t i=0; i<_flash_v.size(); ++i) {
        if(_flash_v[i].id() == kINVALID_INSTANCEID)
          _flash_v[i].id(i);
      }
    }
}
#endif
