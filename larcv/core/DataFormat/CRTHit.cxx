#ifndef __LARCV_CRTHIT_CXX__
#define __LARCV_CRTHIT_CXX__

#include "CRTHit.h"
#include "larcv/core/Base/larbys.h"
#include <numeric> // std::accumulate()

namespace larcv {

    void CRTHitSet::set(const std::vector<larcv::CRTHit>& crthit_v)
    {
      _crthit_v = crthit_v;
      for(size_t i=0; i<_crthit_v.size(); ++i)
        if(_crthit_v.back().id() == kINVALID_INSTANCEID)
          _crthit_v[i].id(i);
    }

    void CRTHitSet::append(const larcv::CRTHit& crthit)
    {
      _crthit_v.push_back(crthit);
      if(_crthit_v.back().id() == kINVALID_INSTANCEID)
        _crthit_v.back().id(_crthit_v.size()-1);
    }

    void CRTHitSet::emplace_back(larcv::CRTHit&& crthit)
    {
      _crthit_v.emplace_back(std::move(crthit));
      if(_crthit_v.back().id() == kINVALID_INSTANCEID)
        _crthit_v.back().id(_crthit_v.size()-1);
    }

    void CRTHitSet::emplace(std::vector<larcv::CRTHit>&& crthit_v)
    {
      _crthit_v = std::move(crthit_v);
      for(size_t i=0; i<_crthit_v.size(); ++i) {
        if(_crthit_v[i].id() == kINVALID_INSTANCEID)
          _crthit_v[i].id(i);
      }
    }
}

#endif
