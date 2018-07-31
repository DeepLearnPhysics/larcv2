#ifndef __CROPPER_CXX__
#define __CROPPER_CXX__

#include "Cropper.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"

namespace larcv {

  static CropperProcessFactory __global_CropperProcessFactory__;

  Cropper::Cropper(const std::string name)
    : ProcessBase(name)
  {}
    
  void Cropper::configure(const PSet& cfg)
  {

    _pmaps_producer             = cfg.get<std::string>("PMAPSProducer");
    _producer_names_v           = cfg.get<std::vector<std::string>>("Producers");
    _product_types_v            = cfg.get<std::vector<std::string>>("ProductTypes");
    _output_producers_v         = cfg.get<std::vector<std::string>>("OutputProducers");
    _output_n_x                 = cfg.get<int>("OutputNX");
    _output_n_y                 = cfg.get<int>("OutputNY");
    _output_n_z                 = cfg.get<int>("OutputNZ");
    _scale_mc_x                   = cfg.get<int>("ScaleMCX");
    _scale_mc_y                   = cfg.get<int>("ScaleMCY");
    _scale_mc_z                   = cfg.get<int>("ScaleMCZ");


    if (_producer_names_v.size() != _product_types_v.size() ||
      _producer_names_v.size() != _output_producers_v.size()) {
      LARCV_CRITICAL() << "Number of product names, product tyes, and output "
                          "producers does not match"
                       << std::endl;
      throw larbys();
    }
  }

  void Cropper::initialize()
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

    LARCV_INFO() << "Cropping images to (" << _output_n_x << ", " << _output_n_y << ", " << _output_n_z << ")." << std::endl;
  }

  bool Cropper::process(IOManager& mgr)
  {

    auto ev_sparse3d_pmaps = mgr.get_data<larcv::EventSparseTensor3D>(_pmaps_producer);
    if (ev_sparse3d_pmaps.as_vector().size() == 0) {
      LARCV_CRITICAL() << "Input cluster not found by producer name "
                       << _pmaps_producer << std::endl;
      throw larbys();
    }

    auto & reference_pmap = ev_sparse3d_pmaps.as_vector().front();
    auto & original_meta  = ev_sparse3d_pmaps.meta();



    // Make sure all metas match against the reference meta:
    for (size_t i = 0; i < _product_types_v.size(); i++) {
      // Get the data product, compare it's meta against the reference.
      if (_product_types_v.at(i) == "sparse3d") {
        auto const& ev_sparse3d = mgr.get_data<larcv::EventSparseTensor3D>(_producer_names_v.at(i));

        if (ev_sparse3d.as_vector().size() == 0) {
          LARCV_CRITICAL() << "Input sparse3d not found by producer name "
                           << _producer_names_v.at(i) << std::endl;
          throw larbys();
        }
       
      } else if (_product_types_v.at(i) == "cluster3d") {
        auto const& ev_cluster3d = mgr.get_data<larcv::EventClusterVoxel3D>(_producer_names_v.at(i));

        if (ev_cluster3d.as_vector().size() == 0) {
          LARCV_CRITICAL() << "Input cluster3d not found by producer name "
                           << _producer_names_v.at(i) << std::endl;
          throw larbys();
        }

      } else {
        LARCV_CRITICAL() << "Product type " << _product_types_v.at(i)
                         << " not supported for cropping." << std::endl;
        throw larbys();
      }
    }





    // Take the average point of the pmaps, weighted by value:
    float mean_x(0.0), mean_y(0.0), mean_z(0.0);
    float weight = 0;
    for (auto & voxel : ev_sparse3d_pmaps.as_vector()){
      // std::cout << "Voxel id: " << voxel.id() << std::endl;
      if (voxel.id() > original_meta.size())
        continue;
      mean_x += voxel.value() * original_meta.pos_x(voxel.id());
      mean_y += voxel.value() * original_meta.pos_y(voxel.id());
      mean_z += voxel.value() * original_meta.pos_z(voxel.id());
      weight += voxel.value();
    }
    mean_x /= weight;
    mean_y /= weight;
    mean_z /= weight;

    LARCV_INFO() << "PMAPS Center: mean_x = " << mean_x << "; mean_y = " << mean_y << "; mean_z = " << mean_z << std::endl;

    // Now we have the mean x/y/z, figure out what pixels this is in the image:
    auto id = original_meta.id(mean_x, mean_y, mean_z);
    int x_ind = original_meta.id_to_x_index(id);
    int y_ind = original_meta.id_to_y_index(id);
    int z_ind = original_meta.id_to_z_index(id); 


    // Create a new image meta that contains the cropped pixels
    // By design, the vertex location is placed at 50% of the way across the colums
    // and 50% of the way across the rows;
    int min_x_ind = x_ind - 0.5  * _output_n_x;
    int max_x_ind = x_ind + 0.5  * _output_n_x;  

    int min_y_ind = y_ind - 0.5  * _output_n_y;
    int max_y_ind = y_ind + 0.5  * _output_n_y;  

    int min_z_ind = z_ind - 0.5  * _output_n_z;
    int max_z_ind = z_ind + 0.5  * _output_n_z;


    // get the x/y locations of the min/max row/col in the old meta:
    float min_x = original_meta.min_x() + original_meta.size_voxel_x() * min_x_ind;
    float max_x = original_meta.min_x() + original_meta.size_voxel_x() * max_x_ind;  

    float min_y = original_meta.min_y() + original_meta.size_voxel_y() * min_y_ind;
    float max_y = original_meta.min_y() + original_meta.size_voxel_y() * max_y_ind;  

    float min_z = original_meta.min_z() + original_meta.size_voxel_z() * min_z_ind;
    float max_z = original_meta.min_z() + original_meta.size_voxel_z() * max_z_ind;


    // Create a new meta object for pmaps
    larcv::Voxel3DMeta new_meta_pmaps;
    new_meta_pmaps.set(min_x, min_y, min_z,
                       max_x, max_y, max_z,
                       _output_n_x, _output_n_y, _output_n_z,
                       original_meta.unit());

    // Create a new meta object for mc
    larcv::Voxel3DMeta new_meta_mc;
    new_meta_mc.set(min_x, min_y, min_z,
                    max_x, max_y, max_z,
                    _output_n_x * _scale_mc_x, _output_n_y * _scale_mc_y, _output_n_z * _scale_mc_z,
                    original_meta.unit());




    // Loop over all producers specified and replace by cropped items:

    for (size_t i = 0; i < _product_types_v.size(); i++) {
      if (_product_types_v.at(i) == "sparse3d"){
        // std::cout << "Making a sparse3d crop" << std::endl;
        auto const& ev_sparse3d = mgr.get_data<larcv::EventSparseTensor3D>(_producer_names_v.at(i));  

        bool is_mc = _producer_names_v.at(i).find("mc") != std::string::npos;

        larcv::VoxelSet _output_voxel_set;  

        for (auto & voxel : ev_sparse3d.as_vector() ){
          if (voxel.id() > original_meta.size() )
            continue;
          // Get the old id and i_x, i_y, i_z of this voxel:
          int old_i_x = original_meta.id_to_x_index(voxel.id());
          int old_i_y = original_meta.id_to_y_index(voxel.id());
          int old_i_z = original_meta.id_to_z_index(voxel.id());  

          // Find the new values:
          int new_i_x = old_i_x - min_x_ind;
          int new_i_y = old_i_y - min_y_ind;
          int new_i_z = old_i_z - min_z_ind;  

          // Skip invalid voxels:
          if (new_i_x < 0 || new_i_x >= _output_n_x) continue;
          if (new_i_y < 0 || new_i_y >= _output_n_y) continue;
          if (new_i_z < 0 || new_i_z >= _output_n_z) continue;  

          // Ok, find the new ID:
          VoxelID_t index; 
          if (is_mc) 
            index = new_meta_mc.index(new_i_x, new_i_y, new_i_z);
          else 
            index = new_meta_pmaps.index(new_i_x, new_i_y, new_i_z);

          _output_voxel_set.insert(larcv::Voxel(index, voxel.value()));
        }  

        // Make an output sparse3d producer
        auto & ev_sparse3d_out = mgr.get_data<larcv::EventSparseTensor3D>(_output_producers_v.at(i));
        if (is_mc) 
          ev_sparse3d_out.emplace(std::move(_output_voxel_set), new_meta_mc);
        else
          ev_sparse3d_out.emplace(std::move(_output_voxel_set), new_meta_pmaps);

        LARCV_DEBUG() << "Finished a sparse3d crop with output label " << _output_producers_v.at(i)
                      << ". Treated as " << (is_mc ? "MC." : "PMAPS.") << std::endl;  

      }
      else if (_product_types_v.at(i) == "cluster3d"){
        // std::cout << "Making a cluster3d crop" << std::endl;
        auto const& ev_cluster3d = mgr.get_data<larcv::EventClusterVoxel3D>(_producer_names_v.at(i));  

        bool is_mc = _producer_names_v.at(i).find("mc") != std::string::npos;
  
        larcv::ClusterVoxel3D _output_cluster_set;

        for (auto const & cluster : ev_cluster3d.as_vector()){  
          larcv::VoxelSet _output_voxel_set;
          _output_voxel_set.id(cluster.id());  

          for (auto & voxel : cluster.as_vector() ){
            if (voxel.id() > original_meta.size() )
              continue;
            // Get the old id and i_x, i_y, i_z of this voxel:
            int old_i_x = original_meta.id_to_x_index(voxel.id());
            int old_i_y = original_meta.id_to_y_index(voxel.id());
            int old_i_z = original_meta.id_to_z_index(voxel.id());  

            // Find the new values:
            int new_i_x = old_i_x - min_x_ind;
            int new_i_y = old_i_y - min_y_ind;
            int new_i_z = old_i_z - min_z_ind;  

            // Skip invalid voxels:
            if (new_i_x < 0 || new_i_x >= _output_n_x) continue;
            if (new_i_y < 0 || new_i_y >= _output_n_y) continue;
            if (new_i_z < 0 || new_i_z >= _output_n_z) continue;  

            // Ok, find the new ID:
            VoxelID_t index; 
            if (is_mc) 
              index = new_meta_mc.index(new_i_x, new_i_y, new_i_z);
            else 
              index = new_meta_pmaps.index(new_i_x, new_i_y, new_i_z);

            _output_voxel_set.insert(larcv::Voxel(index, voxel.value()));
          }  

          _output_cluster_set.insert(_output_voxel_set);  

        }  
  
        // Make an output sparse3d producer
        auto & ev_cluster3d_out = mgr.get_data<larcv::EventClusterVoxel3D>(_output_producers_v.at(i));

        if (is_mc)
          ev_cluster3d_out.emplace(std::move(_output_cluster_set), new_meta_mc);
        else 
          ev_cluster3d_out.emplace(std::move(_output_cluster_set), new_meta_pmaps);

        LARCV_DEBUG() << "Finished a cluster3d crop with output label " << _output_producers_v.at(i)
                      << ". Treated as " << (is_mc ? "MC." : "PMAPS.") << std::endl;   

      }  

    }



    return true;
  }

  void Cropper::finalize()
  {}

}
#endif
