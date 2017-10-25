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
    emplace(std::move(copy));
  }

  void VoxelSet::emplace(Voxel&& vox)
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
    if ( !(vox < (*iter)) ) {
      (*iter) += vox.value();
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

  const larcv::VoxelSet& VoxelSetArray::get_voxel_set(InstanceID_t id) const
  {
    if(id >= _voxel_vv.size()) {
      std::cerr << "VoxelSetArray has no VoxelSet with InstanceID_t " << id << std::endl;
      throw std::exception();
    }
    return _voxel_vv[id];
  }

  void VoxelSetArray::emplace(larcv::VoxelSet&& voxel_v, InstanceID_t id)
  {
    if(id >= _voxel_vv.size()) _voxel_vv.resize(id+1);
    _voxel_vv[id] = std::move(voxel_v);
  }

  void VoxelSetArray::set(const larcv::VoxelSet& voxel_v, InstanceID_t id)
  {
    if(id >= _voxel_vv.size()) _voxel_vv.resize(id+1);
    _voxel_vv[id] = voxel_v;
  }

  const larcv::VoxelSet& 
  VoxelSetArray2D::get_voxel_set(ProjectionID_t p_id, InstanceID_t i_id) const
  {
    if(p_id >= _voxel_vvv.size()) {
      std::cerr << "VoxelSetArray has no VoxelSetArray with ProjectionID_t " << p_id << std::endl;
      throw std::exception();
    }
    return _voxel_vvv[p_id].get_voxel_set(i_id);
  }

  void VoxelSetArray2D::emplace(larcv::VoxelSetArray&& voxel_vv, ProjectionID_t id)
  {
    if(id >= _voxel_vvv.size()) _voxel_vvv.resize(id+1);
    _voxel_vvv[id] = std::move(voxel_vv);
  }

  void VoxelSetArray2D::set(const larcv::VoxelSetArray& voxel_vv, ProjectionID_t id)
  {
    if(id >= _voxel_vvv.size()) _voxel_vvv.resize(id+1);
    _voxel_vvv[id] = voxel_vv;
  }

};

#endif
