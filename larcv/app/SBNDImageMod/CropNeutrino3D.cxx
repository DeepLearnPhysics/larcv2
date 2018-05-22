#ifndef __CROPNEUTRINO3D_CXX__
#define __CROPNEUTRINO3D_CXX__

#include "CropNeutrino3D.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"

namespace larcv {

  static CropNeutrino3DProcessFactory __global_CropNeutrino3DProcessFactory__;

  CropNeutrino3D::CropNeutrino3D(const std::string name)
    : ProcessBase(name)
  {}

  void CropNeutrino3D::configure(const PSet& cfg)
  {

    _vertex_cluster3d_producer = cfg.get<std::string>("VertexProducer");
    _sparse3d_producer = cfg.get<std::string>("Sparse3DProducer");
    _output_producer   = cfg.get<std::string>("OutputProducer");
    _output_n_x        = cfg.get<int>("OutputNX");
    _output_n_y        = cfg.get<int>("OutputNY");
    _output_n_z        = cfg.get<int>("OutputNZ");
    _rand_shift_x      = cfg.get<int>("RandomShiftX", 0);
    _rand_shift_y      = cfg.get<int>("RandomShiftY", 0);
    _rand_shift_z      = cfg.get<int>("RandomShiftZ", 0);


  }

  void CropNeutrino3D::initialize()
  {
      if (!_output_n_x) {
        LARCV_CRITICAL() << "Output Row Size is 0 (undefined)!" << std::endl;
        throw larbys();
      }
      if (!_output_n_y) {
        LARCV_CRITICAL() << "Output Col Size is 0 (undefined)!" << std::endl;
        throw larbys();
      }
      if (!_output_n_z) {
        LARCV_CRITICAL() << "Output Depth Size is 0 (undefined)!" << std::endl;
        throw larbys();
      }

  }

  bool CropNeutrino3D::process(IOManager& mgr)
  {

    // First, get the input image producer and cluster producer, validate sizes match
    auto & ev_sparse3d = mgr.get_data<larcv::EventSparseTensor3D>(_sparse3d_producer);
    if (ev_sparse3d.as_vector().size() == 0) {
      LARCV_CRITICAL() << "Input image not found by producer name " << _sparse3d_producer << std::endl;
      throw larbys();
    }

    auto & ev_cluster3d = mgr.get_data<larcv::EventClusterVoxel3D>(_vertex_cluster3d_producer);
    if (ev_cluster3d.as_vector().size() == 0) {
      LARCV_CRITICAL() << "Input cluster not found by producer name " << _vertex_cluster3d_producer << std::endl;
      throw larbys();
    }

    larcv::VoxelSet _output_voxel_set;
    auto & clust = ev_cluster3d.as_vector().front();
    auto & cluster_meta  = ev_cluster3d.meta();


    // Take the average point of the clusters, weighted by value:
    float mean_x(0.0), mean_y(0.0), mean_z(0.0);
    float weight = 0;
    for (auto & voxel : clust.as_vector()){
      mean_x += voxel.value() * cluster_meta.pos_x(voxel.id());
      mean_y += voxel.value() * cluster_meta.pos_y(voxel.id());
      mean_z += voxel.value() * cluster_meta.pos_z(voxel.id());
      weight += voxel.value();
    }
    mean_x /= weight;
    mean_y /= weight;
    mean_z /= weight;

    // Now we have the mean x/y/z, figure out what pixels this is in the image:
    auto & sparse_meta = ev_sparse3d.meta();
    auto id = sparse_meta.id(mean_x, mean_y, mean_z);
    int x_ind = sparse_meta.id_to_x_index(id);
    int y_ind = sparse_meta.id_to_y_index(id);
    int z_ind = sparse_meta.id_to_z_index(id);

    // If desired, apply a random shift:
    if (_rand_shift_x != 0){
      x_ind += rand() % (2*_rand_shift_x + 1) - _rand_shift_x;
    }
    if (_rand_shift_y != 0){
      y_ind += rand() % (2*_rand_shift_y + 1) - _rand_shift_y;
    }
    if (_rand_shift_z != 0){
      z_ind += rand() % (2*_rand_shift_z + 1) - _rand_shift_z;
    }


    // Create a new image meta that contains the cropped pixels
    // By design, the vertex location is placed at 25% of the way across the colums
    // and 50% of the way across the rows;
    int min_x_ind = x_ind - 0.5  * _output_n_x;
    int max_x_ind = x_ind + 0.5  * _output_n_x;

    int min_y_ind = y_ind - 0.5  * _output_n_y;
    int max_y_ind = y_ind + 0.5  * _output_n_y;

    int min_z_ind = z_ind - 0.5  * _output_n_z;
    int max_z_ind = z_ind + 0.5  * _output_n_z;


    // get the x/y locations of the min/max row/col in the old meta:
    float min_x = sparse_meta.min_x() + sparse_meta.size_voxel_x() * min_x_ind;
    float max_x = sparse_meta.min_x() + sparse_meta.size_voxel_x() * max_x_ind;

    float min_y = sparse_meta.min_y() + sparse_meta.size_voxel_y() * min_y_ind;
    float max_y = sparse_meta.min_y() + sparse_meta.size_voxel_y() * max_y_ind;

    float min_z = sparse_meta.min_z() + sparse_meta.size_voxel_z() * min_z_ind;
    float max_z = sparse_meta.min_z() + sparse_meta.size_voxel_z() * max_z_ind;

    // Create a new meta object:
    larcv::Voxel3DMeta new_meta;
    new_meta.set(min_x, min_y, min_z,
                 max_x, max_y, max_z,
                 _output_n_x, _output_n_y, _output_n_z,
                 sparse_meta.unit());


    // Now, copy all sparse voxels from old to new

    for (auto & voxel : ev_sparse3d.as_vector() ){

      // Get the old id and i_x, i_y, i_z of this voxel:
      int old_i_x = sparse_meta.id_to_x_index(voxel.id());
      int old_i_y = sparse_meta.id_to_y_index(voxel.id());
      int old_i_z = sparse_meta.id_to_z_index(voxel.id());

      // Find the new values:
      int new_i_x = old_i_x - min_x_ind;
      int new_i_y = old_i_y - min_y_ind;
      int new_i_z = old_i_z - min_z_ind;

      // Skip invalid voxels:
      if (new_i_x < 0 || new_i_x >= _output_n_x) continue;
      if (new_i_y < 0 || new_i_y >= _output_n_y) continue;
      if (new_i_z < 0 || new_i_z >= _output_n_z) continue;

      // Ok, find the new ID:
      auto index = new_meta.index(new_i_x, new_i_y, new_i_z);
      _output_voxel_set.insert(larcv::Voxel(index, voxel.value()));
    }

    // // Make an output sparse3d producer
    auto & ev_sparse3d_out = mgr.get_data<larcv::EventSparseTensor3D>(_sparse3d_producer);
    ev_sparse3d_out.emplace(std::move(_output_voxel_set), new_meta);
    // ev_image2d_out.emplace(std::move(new_image_vector));

    return true;
  }


  void CropNeutrino3D::finalize()
  {}

}
#endif
