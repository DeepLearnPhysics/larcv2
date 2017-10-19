#ifndef __LARCV_VOXEL_CXX__
#define __LARCV_VOXEL_CXX__

#include "Voxel.h"
#include <iostream>

namespace larcv {

  Voxel::Voxel(VoxelID_t id, float value)
  { _id = id; _value = value; }

  void VoxelSet::Add(const Voxel& vox)
  {
    Voxel copy(vox);
    Emplace(std::move(copy));
  }
  
  void VoxelSet::Emplace(Voxel&& vox)
  {
    // In case it's empty or greater than the last one
    if(_voxel_v.empty() || _voxel_v.back() < vox) {
      _voxel_v.emplace_back(std::move(vox));
      return;
    }
    // In case it's smaller than the first one
    if(_voxel_v.front() > vox) {
      _voxel_v.emplace_back(std::move(vox));
      for(size_t idx=0; (idx+1)<_voxel_v.size(); ++idx) {
	auto& element1 = _voxel_v[ _voxel_v.size() - (idx+1) ];
	auto& element2 = _voxel_v[ _voxel_v.size() - (idx+2) ];
	std::swap( element1, element2 );
      }
      return;
    }
    
    // Else do log(N) search
    auto iter = std::lower_bound(_voxel_v.begin(), _voxel_v.end(), vox);

    // Cannot be the end
    if( iter == _voxel_v.end() ) {
      std::cerr << "VoxelSet sorting logic error!" << std::endl;
      throw std::exception();
    }
    
    // If found, merge
    if( !(vox < (*iter)) ) {
      (*iter) += vox.Value();
      return;
    }

    // Else insert @ appropriate place
    else {
      size_t target_loc = iter - _voxel_v.begin();
      _voxel_v.emplace_back(std::move(vox));
      for(size_t idx=target_loc; (idx+1)<_voxel_v.size(); ++idx) {
	auto& element1 = _voxel_v[ _voxel_v.size() - (idx+1) ];
	auto& element2 = _voxel_v[ _voxel_v.size() - (idx+2) ];
	std::swap( element1, element2 );
      }
    }
    return;
  }
  


};

#endif
