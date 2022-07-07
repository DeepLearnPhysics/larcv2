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
        , _inBeamFrame (inBeamFrame)
        , _onBeamTime  (onBeamTime)
        , _fastToTotal (fastToTotal)
        , _xCenter     (xCenter)
        , _xWidth      (xWidth)
        , _yCenter     (yCenter)
        , _yWidth      (yWidth)
        , _zCenter     (zCenter)
        , _zWidth      (zWidth)
        , _wireCenters (wireCenters)
        , _wireWidths  (wireWidths)
    {}

    double Flash::TotalPE() const {
        return std::accumulate(_PEPerOpDet.begin(), _PEPerOpDet.end(), 0.0);
    }

    void FlashSet::set(const std::vector<larcv::Flash>& part_v)
    {
      _part_v = part_v;
      for(size_t i=0; i<_part_v.size(); ++i)
        if(_part_v.back().id() == kINVALID_INSTANCEID)
          _part_v[i].id(i);
    }

    void FlashSet::append(const larcv::Flash& part)
    {
      _part_v.push_back(part);
      if(_part_v.back().id() == kINVALID_INSTANCEID)
        _part_v.back().id(_part_v.size()-1);
    }

    void FlashSet::emplace_back(larcv::Flash&& part)
    {
      _part_v.emplace_back(std::move(part));
      if(_part_v.back().id() == kINVALID_INSTANCEID)
        _part_v.back().id(_part_v.size()-1);
    }

    void FlashSet::emplace(std::vector<larcv::Flash>&& part_v)
    {
      _part_v = std::move(part_v);
      for(size_t i=0; i<_part_v.size(); ++i) {
        if(_part_v[i].id() == kINVALID_INSTANCEID)
          _part_v[i].id(i);
      }
    }
}
#endif
