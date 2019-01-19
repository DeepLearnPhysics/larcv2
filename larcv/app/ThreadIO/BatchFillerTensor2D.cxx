#ifndef __BATCHFILLERTENSOR2D_CXX__
#define __BATCHFILLERTENSOR2D_CXX__

#include "BatchFillerTensor2D.h"
#include "larcv/core/DataFormat/EventVoxel2D.h"

#include <random>

namespace larcv {

static BatchFillerTensor2DProcessFactory
    __global_BatchFillerTensor2DProcessFactory__;

BatchFillerTensor2D::BatchFillerTensor2D(const std::string name)
    : BatchFillerTemplate<float>(name) {}

void BatchFillerTensor2D::configure(const PSet& cfg) {
  LARCV_DEBUG() << "start" << std::endl;
  _tensor2d_producer = cfg.get<std::string>("Tensor2DProducer");

  _slice_v.clear();
  _slice_v.resize(1,0);
  _slice_v = cfg.get<std::vector<size_t> >("Channels", _slice_v);

  _voxel_base_value = cfg.get<float>("EmptyVoxelValue",0.);
  _allow_empty = cfg.get<bool>("AllowEmpty",false);

  LARCV_DEBUG() << "done" << std::endl;
}

void BatchFillerTensor2D::initialize() {}

void BatchFillerTensor2D::_batch_begin_() {
  if(!batch_data().dim().empty() && (int)(batch_size()) != batch_data().dim().front()) {
    LARCV_INFO() << "Batch size changed " << batch_data().dim().front() << "=>" << batch_size() << std::endl;
    auto dim = batch_data().dim();
    dim[0] = batch_size();
    this->set_dim(dim);
  }
}

void BatchFillerTensor2D::_batch_end_() {
  if (logger().level() <= msg::kINFO)
    LARCV_INFO() << "Total data size: " << batch_data().data_size()
                 << std::endl;
}

void BatchFillerTensor2D::finalize() { _entry_data.clear(); }

void BatchFillerTensor2D::assert_dimension(
    const EventSparseTensor2D& voxel_data) const {
  auto const& voxel_meta = voxel_data.as_vector().front().meta();
  if (_rows != voxel_meta.rows()) {
    LARCV_CRITICAL() << "# of Y-voxels (" << _rows << ") changed ... now "
                     << voxel_meta.rows() << std::endl;
    throw larbys();
  }
  if (_cols != voxel_meta.cols()) {
    LARCV_CRITICAL() << "# of X-voxels (" << _cols << ") changed ... now "
                     << voxel_meta.cols() << std::endl;
    throw larbys();
  }
  return;
}

bool BatchFillerTensor2D::process(IOManager& mgr) {
  LARCV_DEBUG() << "start" << std::endl;
  auto const& voxel_data =
      mgr.get_data<larcv::EventSparseTensor2D>(_tensor2d_producer);
  if (!_allow_empty && voxel_data.as_vector().empty()) {
    LARCV_CRITICAL()
        << "Could not locate non-empty voxel data w/ producer name "
        << _tensor2d_producer << std::endl;
    throw larbys();
  }

  _num_channels = _slice_v.size();

  // if (_num_channels != voxel_data.as_vector().size()){
  //   LARCV_CRITICAL() << "Number of requested channels does not match number of available channels." << std::endl;
  //   throw larbys();
  // }


  // one time operation: get image dimension
  if (batch_data().dim().empty()) {
    auto const& voxel_meta = voxel_data.as_vector().front().meta();
    _rows = voxel_meta.rows();
    _cols = voxel_meta.cols();
    std::vector<int> dim;
    dim.resize(4);
    dim[0] = batch_size();
    dim[1] = _rows;
    dim[2] = _cols;
    dim[3] = _num_channels;
    this->set_dim(dim);
  } else
    this->assert_dimension(voxel_data);



  if (_entry_data.size() != batch_data().entry_data_size())
    _entry_data.resize(batch_data().entry_data_size(), 0.);


  for (auto& v : _entry_data) v = _voxel_base_value;


  for ( auto const& voxel_set : voxel_data.as_vector()){
    auto & meta = voxel_set.meta();
    auto projection_id = meta.id();

    // Check that this projection ID is in the lists of channels:
    bool found = false;
    int count = 0;
    for (auto & channel : _slice_v){
      if (channel == projection_id){
        found = true;
        break;
      }
      count ++;
    }
    if (!found) continue;

    for (auto const& voxel : voxel_set.as_vector())
      _entry_data[voxel.id()]=voxel.value();
  }

  // record the entry data
  LARCV_INFO() << "Inserting entry data of size " << _entry_data.size()
               << std::endl;
  set_entry_data(_entry_data);

  return true;
}
}
#endif
