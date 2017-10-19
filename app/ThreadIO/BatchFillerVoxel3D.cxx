#ifndef __BatchFillerVoxel3D_CXX__
#define __BatchFillerVoxel3D_CXX__

#include "BatchFillerVoxel3D.h"
#include "DataFormat/UtilFunc.h"

#include <random>

namespace larcv {

  static BatchFillerVoxel3DProcessFactory __global_BatchFillerVoxel3DProcessFactory__;

  BatchFillerVoxel3D::BatchFillerVoxel3D(const std::string name)
    : BatchFillerTemplate<float>(name)
  {}

  void BatchFillerVoxel3D::configure(const PSet& cfg)
  {
    LARCV_DEBUG() << "start" << std::endl;
    _voxel_producer = cfg.get<std::string>("VoxelProducer");

    LARCV_DEBUG() << "done" << std::endl;
  }

  void BatchFillerVoxel3D::initialize()
  {}

  void BatchFillerVoxel3D::_batch_begin_()
  {}

  void BatchFillerVoxel3D::_batch_end_()
  {
    if(logger().level() <= msg::kINFO)
      LARCV_INFO() << "Total data size: " << batch_data().data_size() << std::endl;
  }

  void BatchFillerVoxel3D::finalize()
  { _entry_data.clear(); }

  void BatchFillerVoxel3D::assert_dimension(const EventVoxel3D* voxel_data) const
  {
    auto const& voxel_meta = voxel_data->GetVoxelMeta();
    if(_nx != voxel_meta.NumVoxelX()) {
      LARCV_CRITICAL() << "# of X-voxels (" << _nx << ") changed ... now " << voxel_meta.NumVoxelX() << std::endl;
      throw larbys();
    }
    if(_ny != voxel_meta.NumVoxelY()) {
      LARCV_CRITICAL() << "# of Y-voxels (" << _ny << ") changed ... now " << voxel_meta.NumVoxelY() << std::endl;
      throw larbys();
    }
    if(_nz != voxel_meta.NumVoxelZ()) {
      LARCV_CRITICAL() << "# of Z-voxels (" << _nz << ") changed ... now " << voxel_meta.NumVoxelZ() << std::endl;
      throw larbys();
    }
    return;
  }

  bool BatchFillerVoxel3D::process(IOManager& mgr) 
  {
    LARCV_DEBUG() <<"start"<<std::endl;
    auto voxel_data = (EventVoxel3D*)(mgr.get_data(kProductVoxel3D,_voxel_producer));
    if(!voxel_data || voxel_data->GetVoxelSet().empty()) {
      LARCV_CRITICAL() << "Could not locate non-empty voxel data w/ producer name " << _voxel_producer << std::endl;
      throw larbys();
    }
    
    // one time operation: get image dimension
    if(batch_data().dim().empty()) {
      auto const& voxel_meta = voxel_data->GetVoxelMeta();
      std::vector<int> dim;
      dim.resize(5);
      dim[0] = batch_size();
      dim[1] = _nz = voxel_meta.NumVoxelZ();
      dim[2] = _ny = voxel_meta.NumVoxelY();
      dim[3] = _nx = voxel_meta.NumVoxelX();
      dim[4] = 1;
      this->set_dim(dim);
    }
    else
      this->assert_dimension(voxel_data);

    if(_entry_data.size() != batch_data().entry_data_size())
      _entry_data.resize(batch_data().entry_data_size(),0.);

    for(auto& v : _entry_data) v = 0.;

    for(auto const& voxel : voxel_data->GetVoxelSet())
      _entry_data[voxel.ID()] += voxel.Value();

    // record the entry data
    LARCV_INFO() <<"Inserting entry data of size " << _entry_data.size() << std::endl;
    set_entry_data(_entry_data);

    return true;
  }
  
}
#endif
