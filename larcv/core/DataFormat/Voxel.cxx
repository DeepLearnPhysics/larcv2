#ifndef __LARCV_VOXEL_CXX__
#define __LARCV_VOXEL_CXX__

#include "Voxel.h"
#include <iostream>

namespace larcv {

  Voxel::Voxel(VoxelID_t id, float value)
  { _id = id; _value = value; }

  void VoxelSet::add(const Voxel& vox)
  {
    Voxel copy(vox);
    emplace(std::move(copy),true);
  }

  void VoxelSet::set(const Voxel& vox)
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
      size_t target_loc = iter - _voxel_v.begin();
      _voxel_v.emplace_back(std::move(vox));
      for (size_t idx = target_loc; (idx + 1) < _voxel_v.size(); ++idx) {
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

  inline void VoxelSetArray::set(const std::vector<larcv::VoxelSet>& voxel_vv)
  { 
    _voxel_vv = voxel_vv;
    for(size_t id=0; id<_voxel_vv.size(); ++id)
      _voxel_vv[id].id(id);
  }

  void VoxelSetArray::emplace(larcv::VoxelSet&& voxel_v)
  {
    if(voxel_v.id() >= _voxel_vv.size()) {
      size_t orig_size = _voxel_vv.size();
      _voxel_vv.resize(voxel_v.id()+1);
      for(size_t id=orig_size; id<_voxel_vv.size(); ++id)
        _voxel_vv[id].id(id);
    }
    _voxel_vv[voxel_v.id()] = std::move(voxel_v);
  }

  void VoxelSetArray::set(const larcv::VoxelSet& voxel_v)
  {
    if(voxel_v.id() >= _voxel_vv.size()) {
      size_t orig_size = _voxel_vv.size();
      _voxel_vv.resize(voxel_v.id()+1);
      for(size_t id=orig_size; id<_voxel_vv.size(); ++id)
        _voxel_vv[id].id(id);
    }
    _voxel_vv[voxel_v.id()] = voxel_v;
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
