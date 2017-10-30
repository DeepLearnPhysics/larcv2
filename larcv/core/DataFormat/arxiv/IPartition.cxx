#ifndef VOXELPARTITION_CXX
#define VOXELPARTITION_CXX

#include "VoxelPartition.h"

namespace larcv {

	VoxelPartition::VoxelPartition(const VoxelID_t id)
		: _id(id)
	{
      for (size_t i = 0; i < PARTITION_SIZE; ++i)
      {_id_array[i] = kINVALID_INSTANCEID; _value_array[i] = 0.;}
    }

	float VoxelPartition::value(const InstanceID_t id) const
	{
		static size_t i = 0;
		for (i = 0; i < PARTITION_SIZE; ++i) {
			if (_id_array[i] == kINVALID_INSTANCEID) break;
			if (_id_array[i] == id) return _value_array[i];
		}
		return 0;
	}

	float VoxelPartition::fraction(const InstanceID_t id) const
	{
		static size_t i = 0;
		float numerator = 0;
		float denominator = 0;
		for (i = 0; i < PARTITION_SIZE; ++i) {
			if (_id_array[i] == kINVALID_INSTANCEID) break;
			if (_id_array[i] == id) numerator = _value_array[i];
			denominator += _value_array[i];
		}

		return numerator / denominator;
	}

    bool VoxelPartition::set(const InstanceID_t id, const float value)
    {
    	static size_t i, j;
    	for (i=0; i < PARTITION_SIZE; ++i) {
    		if( _id_array[i] == kINVALID_INSTANCEID || _id_array[i] == id ) {
    			_id_array[i] = id;
    			_value_array[i] = value;
    			return true;
    		}else if(_value_array[i] < value) {
    			for(j=i+1; j<PARTITION_SIZE; ++i) {
    				_id_array[j]    = _id_array[j-1];
    				_value_array[j] = _value_array[j-1];
    			}
    			_id_array[i] = id;
    			_value_array[i] = value;
    			return true;
    		}
    	}
    	return false;
    }

  void VoxelPartitionSet::add(const VoxelPartition& vox)
  {
    VoxelPartition copy(vox);
    emplace(std::move(copy));
  }

  void VoxelPartitionSet::emplace(VoxelPartition&& vox)
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
      std::cerr << "VoxelPartitionSet sorting logic error!" << std::endl;
      throw std::exception();
    }

    // If found, merge
    if ( !(vox < (*iter)) ) {
      (*iter) += vox.value();
      // FIXME
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

}

#endif
