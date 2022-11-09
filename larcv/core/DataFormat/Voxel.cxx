#ifndef __LARCV_VOXEL_CXX__
#define __LARCV_VOXEL_CXX__

#include "Voxel.h"
#include <iostream>
#include <cmath>
#include "larcv/core/Base/larbys.h"

namespace larcv {

  Voxel::Voxel(VoxelID_t id, float value)
  { _id = id; _value = value; }

  VoxelSet::VoxelSet(const std::vector<larcv::VoxelID_t>& id_v,
    const std::vector<float>& val_v)
  {
    if(id_v.size() != val_v.size())
      throw larbys("ID and VALUE array size mismatch. Can't construct VoxelSet!");

    if(id_v.empty())
      return;

    _voxel_v.reserve(id_v.size());
    Voxel vox(id_v[0],val_v[0]);
    _voxel_v.push_back(vox);

    for(size_t i=1; i<val_v.size(); ++i) {

      vox.set(id_v[i],val_v[i]);

      if(vox.id() == larcv::kINVALID_VOXELID)
        throw larbys("Invalid voxel ID included. Can't construct VoxelSet!");

      if(vox.id() <= id_v[i-1])
        throw larbys("Found a larger voxel id BEFORE a smaller voxel ID!");
      
      _voxel_v.push_back(vox);
    }

    return;
  }

  float VoxelSet::max() const
  {
    float val = std::numeric_limits<float>::min();
    for(auto const& vox : _voxel_v) {
      if(vox.value() > val) val = vox.value();
    }
    return val;
  }

  void VoxelSet::clear_invalid(bool clear_invalid_float, bool clear_nan, bool clear_inf)
  {
    if(!clear_invalid_float && !clear_nan && !clear_inf) return;
    std::vector<larcv::Voxel> vox_v;
    vox_v.reserve(_voxel_v.size());
    for(auto const& vox : _voxel_v) {
      if (clear_inf && std::isinf(vox.value())) continue;
      if (clear_nan && std::isnan(vox.value())) continue;
      if (clear_invalid_float && vox.value() == larcv::kINVALID_FLOAT) continue;
      vox_v.push_back(vox);
    }
    _voxel_v = std::move(vox_v);
  }


  float VoxelSet::min() const
  {
    float val = std::numeric_limits<float>::max();
    for(auto const& vox : _voxel_v) {
      if(vox.value() < val) val = vox.value();
    }
    return val;
  }

  void VoxelSet::threshold(float min, float max)
  {
    std::vector<larcv::Voxel> vox_v;
    vox_v.reserve(_voxel_v.size());
    for(auto const& vox : _voxel_v) {
      if(vox.value() < min || vox.value() > max) continue;
      vox_v.push_back(vox);
    }
    _voxel_v = std::move(vox_v);
  }

  void VoxelSet::threshold_min(float min)
  {
    std::vector<larcv::Voxel> vox_v;
    vox_v.reserve(_voxel_v.size());
    for(auto const& vox : _voxel_v) {
      if(vox.value() < min) continue;
      vox_v.push_back(vox);
    }
    _voxel_v = std::move(vox_v);
  }

  void VoxelSet::threshold_max(float max)
  {
    std::vector<larcv::Voxel> vox_v;
    vox_v.reserve(_voxel_v.size());
    for(auto const& vox : _voxel_v) {
      if(vox.value() > max) continue;
      vox_v.push_back(vox);
    }
    _voxel_v = std::move(vox_v);
  }

  void VoxelSet::add(const Voxel& vox)
  {
    Voxel copy(vox);
    emplace(std::move(copy),true);
  }

  void VoxelSet::insert(const Voxel& vox)
  {
    Voxel copy(vox);
    emplace(std::move(copy),false);
  }

  const Voxel& VoxelSet::find(VoxelID_t id) const
  {
    if(_voxel_v.empty() ||
       id < _voxel_v.front().id() ||
       id > _voxel_v.back().id())
      return kINVALID_VOXEL;

    Voxel vox(id,0.);
    // Else do log(N) search
    auto iter = std::lower_bound(_voxel_v.begin(), _voxel_v.end(), vox);
    if( (*iter).id() == id ) return (*iter);
    else {
      //std::cout << "Returning invalid voxel since lower_bound had an id " << (*iter).id() << std::endl;
      return kINVALID_VOXEL;
    }
  }

  size_t VoxelSet::index(VoxelID_t id) const
  {
    if(_voxel_v.empty() ||
       id < _voxel_v.front().id() ||
       id > _voxel_v.back().id())
      return kINVALID_SIZE;

    Voxel vox(id,0.);
    // Else do log(N) search
    auto iter = std::lower_bound(_voxel_v.begin(), _voxel_v.end(), vox);
    if( (*iter).id() == id ) return (iter - _voxel_v.begin());
    else {
      //std::cout << "Returning invalid voxel since lower_bound had an id " << (*iter).id() << std::endl;
      return kINVALID_SIZE;
    }
  }

  void VoxelSet::emplace(Voxel&& vox, const bool add)
  {
    // In case it's empty or greater than the last one
    if (_voxel_v.empty() || _voxel_v.back() < vox) {
      _voxel_v.emplace_back(std::move(vox));
      return;
    }
    // In case it's smaller than the first one
    if (_voxel_v.front() > vox) {
      _voxel_v.emplace_back(std::move(vox));
      for (size_t idx = 0; (idx + 1) < _voxel_v.size(); ++idx) {
        auto& element1 = _voxel_v[ _voxel_v.size() - (idx + 1) ];
        auto& element2 = _voxel_v[ _voxel_v.size() - (idx + 2) ];
        std::swap( element1, element2 );
      }
      return;
    }

    // Else do log(N) search
    auto iter = std::lower_bound(_voxel_v.begin(), _voxel_v.end(), vox);

    // Cannot be the end
    if ( iter == _voxel_v.end() ) {
      std::cerr << "VoxelSet sorting logic error!" << std::endl;
      throw std::exception();
    }

    // If found, merge
    if ( vox.id() == (*iter).id() ) {
      if(add) (*iter) += vox.value();
      else (*iter).set(vox.id(),vox.value());
      return;
    }
    // Else insert @ appropriate place
    else {
      size_t target_ctr = _voxel_v.size() - (iter - _voxel_v.begin());
      _voxel_v.emplace_back(std::move(vox));
      for (size_t idx = 0; idx < target_ctr; ++idx) {
        auto& element1 = _voxel_v[ _voxel_v.size() - (idx + 1) ];
        auto& element2 = _voxel_v[ _voxel_v.size() - (idx + 2) ];
        std::swap( element1, element2 );
      }
    }
    return;
  }

  //
  // VoxelSetArray
  //
  float VoxelSetArray::sum() const
  { float res=0.; for(auto const& vox_v : _voxel_vv) res+=vox_v.sum(); return res;}

  float VoxelSetArray::mean() const
  {
    size_t vox_ctr = 0;
    for(auto const& vox_v : _voxel_vv) vox_ctr += vox_v.size();
    return (vox_ctr<1 ? 0. : this->sum() / (float)vox_ctr);
  }

  float VoxelSetArray::max() const
  {
    float val = std::numeric_limits<float>::min();
    float ival = 0.;
    for(auto const& vox_v : _voxel_vv) {
      ival = vox_v.max();
      val = (val < ival ? ival : val);
    }
    return val;
  }

  float VoxelSetArray::min() const
  {
    float val = std::numeric_limits<float>::max();
    float ival = 0.;
    for(auto const& vox_v : _voxel_vv) {
      ival = vox_v.min();
      val = (val > ival ? ival : val);
    }
    return val;
  }

  const larcv::VoxelSet& VoxelSetArray::voxel_set(InstanceID_t id) const
  {
    if(id >= _voxel_vv.size()) {
      std::cerr << "VoxelSetArray has no VoxelSet with InstanceID_t " << id << std::endl;
      throw std::exception();
    }
    return _voxel_vv[id];
  }

  void VoxelSetArray::emplace(std::vector<larcv::VoxelSet>&& voxel_vv)
  {
    _voxel_vv = std::move(voxel_vv);
    for(size_t id=0; id<_voxel_vv.size(); ++id)
      _voxel_vv[id].id(id);
  }

  inline void VoxelSetArray::insert(const std::vector<larcv::VoxelSet>& voxel_vv)
  {
    _voxel_vv = voxel_vv;
    for(size_t id=0; id<_voxel_vv.size(); ++id)
      _voxel_vv[id].id(id);
  }

  void VoxelSetArray::emplace(larcv::VoxelSet&& voxel_v)
  {
    if(voxel_v.id() == larcv::kINVALID_INSTANCEID) {
      _voxel_vv.emplace_back(std::move(voxel_v));
      _voxel_vv.back().id(_voxel_vv.size()-1);
      return;
    }
    if(voxel_v.id() >= _voxel_vv.size()) {
      size_t orig_size = _voxel_vv.size();
      _voxel_vv.resize(voxel_v.id()+1);
      for(size_t id=orig_size; id<_voxel_vv.size(); ++id)
        _voxel_vv[id].id(id);
    }
    _voxel_vv[voxel_v.id()] = std::move(voxel_v);
  }

  void VoxelSetArray::insert(const larcv::VoxelSet& voxel_v)
  {
    if(voxel_v.id() == larcv::kINVALID_INSTANCEID) {
      _voxel_vv.push_back(voxel_v);
      _voxel_vv.back().id(_voxel_vv.size()-1);
      return;
    }
    if(voxel_v.id() >= _voxel_vv.size()) {
      size_t orig_size = _voxel_vv.size();
      _voxel_vv.resize(voxel_v.id()+1);
      for(size_t id=orig_size; id<_voxel_vv.size(); ++id)
        _voxel_vv.at(id).id(id);
    }
    _voxel_vv.at(voxel_v.id()) = voxel_v;
  }

  larcv::VoxelSet& VoxelSetArray::writeable_voxel_set(const InstanceID_t id)
  {
    if(id >= _voxel_vv.size()) {
      std::cerr << "VoxelSetArray has no VoxelSet with InstanceID_t " << id << std::endl;
      throw std::exception();
    }
    return _voxel_vv[id];
  }

};

#endif
