#ifndef __BATCHFILLERSPARSETENSOR3D_CXX__
#define __BATCHFILLERSPARSETENSOR3D_CXX__

#include "BatchFillerSparseTensor3D.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"

#include <stdlib.h>
#include <time.h>

namespace larcv {

static BatchFillerSparseTensor3DProcessFactory
    __global_BatchFillerSparseTensor3DProcessFactory__;

BatchFillerSparseTensor3D::BatchFillerSparseTensor3D(const std::string name)
    : BatchFillerTemplate<float>(name) {}

void BatchFillerSparseTensor3D::configure(const PSet& cfg) {
  LARCV_DEBUG() << "start" << std::endl;
  _tensor3d_producer = cfg.get<std::string>("Tensor3DProducer");

  // Max voxels imposes a limit to make the memory layout regular.  Assuming average sparsity of x% , it's safe to
  // set _max_voxels to n_rows*n_cols*x*2 or so.  It's still a dramatic memory reduction.
  _max_voxels = cfg.get<int>("MaxVoxels", 0);
  _unfilled_voxel_value = cfg.get<float>("UnfilledVoxelValue", 0.0);
  _include_values = cfg.get<bool>("IncludeValues", true);
  _augment = cfg.get<bool>("Augment", true);

  if (_augment)
    srand(time(NULL));

  if (_max_voxels == 0){
    LARCV_CRITICAL() << "Maximum number of voxels must be non zero!" << std::endl;
    throw larbys();
  }

  _allow_empty = cfg.get<bool>("AllowEmpty",false);

  LARCV_DEBUG() << "done" << std::endl;
}

void BatchFillerSparseTensor3D::initialize() {}

void BatchFillerSparseTensor3D::_batch_begin_() {
  if(!batch_data().dim().empty() && (int)(batch_size()) != batch_data().dim().front()) {
    LARCV_INFO() << "Batch size changed " << batch_data().dim().front() << "=>" << batch_size() << std::endl;
    auto dim = batch_data().dim();
    dim[0] = batch_size();
    this->set_dim(dim);
  }
}

void BatchFillerSparseTensor3D::_batch_end_() {
  if (logger().level() <= msg::kINFO)
    LARCV_INFO() << "Total data size: " << batch_data().data_size()
                 << std::endl;
}

void BatchFillerSparseTensor3D::finalize() { _entry_data.clear(); }

void BatchFillerSparseTensor3D::assert_dimension(
    const EventSparseTensor3D& voxel_data) const {
  // auto const& voxel_meta = voxel_data.as_vector().front().meta();
  // if (_rows != voxel_meta.rows()) {
  //   LARCV_CRITICAL() << "# of Y-voxels (" << _rows << ") changed ... now "
  //                    << voxel_meta.rows() << std::endl;
  //   throw larbys();
  // }
  // if (_cols != voxel_meta.cols()) {
  //   LARCV_CRITICAL() << "# of X-voxels (" << _cols << ") changed ... now "
  //                    << voxel_meta.cols() << std::endl;
  //   throw larbys();
  // }
  return;
}

bool BatchFillerSparseTensor3D::process(IOManager& mgr) {
  
  /*

  Filling a dense tensor of dimensions B, H, W, C produces, in the end,
  a 4D tensor.  Here, we want to produce a list of points in sparse format.
  SO, this will be a 3 dimensional tensor of dimensions B, N_max, dims_len
  Where dims_len is the number of values that represent a point.
  In this 3D example, dims_len == 3 (H, W, Value) which are extracted from 
  the original image.

  In principle this can be bigger or smaller for different sized inputs:
  dims_len = 4 (H, W, D, Value) in 3D, for example

  The N_max value is a limit on how much memory is allocated for each event.
  The empty values are all set to 0.0, which are easily ignored in the 
  graph networks since the pooling layers are typically max reductions

  By convention, the Value component is always last in the network

  If there are more points in the input than are available in the output,
  The points are truncated.

  It's possible to do a random downsampling, but in case of segementation
  networks this needs to be coordinated across the image and label filler.

  */


  LARCV_DEBUG() << "start" << std::endl;
  auto const& voxel_data =
      mgr.get_data<larcv::EventSparseTensor3D>(_tensor3d_producer);
  if (!_allow_empty && voxel_data.as_vector().empty()) {
    LARCV_CRITICAL()
        << "Could not locate non-empty voxel data w/ producer name "
        << _tensor3d_producer << std::endl;
    throw larbys();
  }

  int point_dim = 4; // This is hardcoded to be x,y,z value, but could be extended
  if (!_include_values){
    point_dim = 3;
  }
  // one time operation: get image dimension
  if (batch_data().dim().empty()) {
    auto const& voxel_meta = voxel_data.meta();
    std::vector<int> dim;
    dim.resize(3);
    dim.at(0) = batch_size();
    dim.at(1) = _max_voxels;
    dim.at(2) = point_dim;
    this->set_dim(dim);
  } else
    this->assert_dimension(voxel_data);


  if (_entry_data.size() != batch_data().entry_data_size())
    _entry_data.resize(batch_data().entry_data_size(), 0.);


  // Reset all values to 0.0 (or whatever is specified)
  for (auto& v : _entry_data) v = _unfilled_voxel_value;


  auto & meta = voxel_data.meta();

  // Check that this projection ID is in the lists of channels:
  bool found = false;
  size_t i = 0;
  
  // Get the random x/y/z flipping
  bool flip_x = false;
  bool flip_y = false;
  bool flip_z = false;
  if (_augment){
    flip_x = bool(rand() % 2);
    flip_y = bool(rand() % 2);
    flip_z = bool(rand() % 2);
  }



  for (auto const& voxel : voxel_data.as_vector()) {
    int i_x = meta.id_to_x_index(voxel.id());
    int i_y = meta.id_to_y_index(voxel.id());
    int i_z = meta.id_to_z_index(voxel.id());
    
    if (flip_x) i_x = meta.num_voxel_x() - (i_x + 1);
    if (flip_y) i_y = meta.num_voxel_y() - (i_y + 1);
    if (flip_z) i_z = meta.num_voxel_z() - (i_z + 1);

    _entry_data.at(point_dim*i + 0) = i_x;
    _entry_data.at(point_dim*i + 1) = i_y;
    _entry_data.at(point_dim*i + 2) = i_z;
    if(_include_values)
      _entry_data.at(point_dim*i + 3) = voxel.value();
    i++;

    if (i == _max_voxels) {
      LARCV_INFO() << "Truncating the number of voxels!" << std::endl;
      break;
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
