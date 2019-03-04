#ifndef __BatchFillerTensor3D_CXX__
#define __BatchFillerTensor3D_CXX__

#include "BatchFillerTensor3D.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"

#include <random>

namespace larcv {

static BatchFillerTensor3DProcessFactory
    __global_BatchFillerTensor3DProcessFactory__;

BatchFillerTensor3D::BatchFillerTensor3D(const std::string name)
    : BatchFillerTemplate<float>(name) {}

void BatchFillerTensor3D::configure(const PSet& cfg) {
  LARCV_DEBUG() << "start" << std::endl;
  _tensor3d_producer = cfg.get<std::string>("Tensor3DProducer");
  _num_channel = cfg.get<size_t>("MakeHotLabel", 0);
  _voxel_base_value = cfg.get<float>("EmptyVoxelValue",0.);
  _allow_empty = cfg.get<bool>("AllowEmpty",false);
  if (_num_channel == 1) {
    LARCV_CRITICAL() << "Cannot make hot label of length 1!" << std::endl;
    throw larbys();
  }
  if (_num_channel < 1) _num_channel = 1;
  LARCV_DEBUG() << "done" << std::endl;
}

void BatchFillerTensor3D::initialize() {}

void BatchFillerTensor3D::_batch_begin_() {
  if(!batch_data().dim().empty() && (int)(batch_size()) != batch_data().dim().front()) {
    LARCV_INFO() << "Batch size changed " << batch_data().dim().front() << "=>" << batch_size() << std::endl;
    auto dim = batch_data().dim();
    dim[0] = batch_size();
    this->set_dim(dim);
  }
}

void BatchFillerTensor3D::_batch_end_() {
  if (logger().level() <= msg::kINFO)
    LARCV_INFO() << "Total data size: " << batch_data().data_size()
                 << std::endl;
}

void BatchFillerTensor3D::finalize() { _entry_data.clear(); }

void BatchFillerTensor3D::assert_dimension(
    const EventSparseTensor3D& voxel_data) const {
  auto const& voxel_meta = voxel_data.meta();
  if (_nx != voxel_meta.num_voxel_x()) {
    LARCV_CRITICAL() << "# of X-voxels (" << _nx << ") changed ... now "
                     << voxel_meta.num_voxel_x() << std::endl;
    throw larbys();
  }
  if (_ny != voxel_meta.num_voxel_y()) {
    LARCV_CRITICAL() << "# of Y-voxels (" << _ny << ") changed ... now "
                     << voxel_meta.num_voxel_y() << std::endl;
    throw larbys();
  }
  if (_nz != voxel_meta.num_voxel_z()) {
    LARCV_CRITICAL() << "# of Z-voxels (" << _nz << ") changed ... now "
                     << voxel_meta.num_voxel_z() << std::endl;
    throw larbys();
  }
  return;
}

bool BatchFillerTensor3D::process(IOManager& mgr) {
  LARCV_DEBUG() << "start" << std::endl;
  auto const& voxel_data =
      mgr.get_data<larcv::EventSparseTensor3D>(_tensor3d_producer);
  if (!_allow_empty && voxel_data.as_vector().empty()) {
    LARCV_CRITICAL()
        << "Could not locate non-empty voxel data w/ producer name "
        << _tensor3d_producer << std::endl;
    throw larbys();
  }

  // one time operation: get image dimension
  if (batch_data().dim().empty()) {
    auto const& voxel_meta = voxel_data.meta();
    std::vector<int> dim;
    dim.resize(5);
    dim[0] = batch_size();
    dim[1] = _nz = voxel_meta.num_voxel_z();
    dim[2] = _ny = voxel_meta.num_voxel_y();
    dim[3] = _nx = voxel_meta.num_voxel_x();
    dim[4] = _num_channel;
    this->set_dim(dim);
  } else
    this->assert_dimension(voxel_data);

  if (_entry_data.size() != batch_data().entry_data_size())
    _entry_data.resize(batch_data().entry_data_size(), 0.);

  for (auto& v : _entry_data) v = _voxel_base_value;

  if (_num_channel == 1) {
    for (auto const& voxel : voxel_data.as_vector())
      _entry_data[voxel.id()] = voxel.value();
  } else {
    int ch = 0;
    for (auto const& voxel : voxel_data.as_vector()) {
      ch = (int)(voxel.value());
      if (ch < 0 || ch >= (int)_num_channel) {
        LARCV_CRITICAL() << "Voxel ID " << voxel.id() << " has value "
                         << voxel.value()
                         << " ... cannt map to hot label array of length "
                         << _num_channel << "!" << std::endl;
        throw larbys();
      }
      _entry_data[voxel.id() * _num_channel + ch] = 1.;
    }
  }

  // record the entry data
  LARCV_INFO() << "Inserting entry data of size " << _entry_data.size()
               << std::endl;
  set_entry_data(_entry_data);

  return true;
}
}
#endif
