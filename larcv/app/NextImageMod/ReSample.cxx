#ifndef __RESAMPLE_CXX__
#define __RESAMPLE_CXX__

#include "ReSample.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"

namespace larcv {

  static ReSampleProcessFactory __global_ReSampleProcessFactory__;

  ReSample::ReSample(const std::string name)
    : ProcessBase(name)
  {}
    
  void ReSample::configure(const PSet& cfg)
  {
    _pmaps_producers = cfg.get<std::vector<std::string>>("PMAPSProducers");
    _output_labels = cfg.get<std::vector<std::string>>("OutputLables");

    _scale_x = cfg.get<double>("ScaleX");
    _scale_y = cfg.get<double>("ScaleY");
    _scale_z = cfg.get<double>("ScaleZ");
  }

  void ReSample::initialize()
  {}

  bool ReSample::process(IOManager& mgr)
  {

    for (size_t i_input = 0; i_input < _pmaps_producers.size(); i_input ++) {

      auto ev_sparse3d_pmaps = mgr.get_data<larcv::EventSparseTensor3D>(_pmaps_producers.at(i_input));
      if (ev_sparse3d_pmaps.as_vector().size() == 0) {
        LARCV_CRITICAL() << "Input EventSparseTensor3D not found by producer name "
        << _pmaps_producers.at(i_input) << std::endl;
        throw larbys();
      }

      auto & original_meta_pmaps  = ev_sparse3d_pmaps.meta();


      // get the x/y locations of the min/max row/col in the old meta:
      float min_x = original_meta_pmaps.min_x();
      float max_x = original_meta_pmaps.max_x();

      float min_y = original_meta_pmaps.min_y();
      float max_y = original_meta_pmaps.max_y();

      float min_z = original_meta_pmaps.min_z();
      float max_z = original_meta_pmaps.max_z();

      int n_x = original_meta_pmaps.num_voxel_x();
      int n_y = original_meta_pmaps.num_voxel_y();
      int n_z = original_meta_pmaps.num_voxel_z();

      n_x *= _scale_x;
      n_y *= _scale_y;
      n_z *= _scale_z;


      // Create a new meta object for pmaps
      larcv::Voxel3DMeta new_meta_pmaps;
      new_meta_pmaps.set(min_x, min_y, min_z,
       max_x, max_y, max_z,
       n_x, n_y, n_z,
       original_meta_pmaps.unit());




      larcv::VoxelSet _output_voxel_set;    


      for (auto & voxel : ev_sparse3d_pmaps.as_vector() ){

        if (voxel.id() > original_meta_pmaps.size() ){
          std::cout << "Skipping voxel with id " << voxel.id() << std::endl;
          continue;
        }

        // Get the old id and i_x, i_y, i_z of this voxel:
        float old_pos_x = original_meta_pmaps.pos_x(voxel.id());
        float old_pos_y = original_meta_pmaps.pos_y(voxel.id());
        float old_pos_z = original_meta_pmaps.pos_z(voxel.id());

        // LARCV_DEBUG() << "initial x " << old_pos_x << ", initial y " << old_pos_y << ", initial z " << old_pos_z << std::endl;

        int spacing = 2;

        for (int i = old_pos_x - (int)(_scale_x / 2) * spacing; i <= old_pos_x + (int)(_scale_x / 2) * spacing; i+= spacing) {

          // old_pos_x += i * spacing;

          for (int j = old_pos_y - (int)(_scale_y / 2) * spacing; j <= old_pos_y + (int)(_scale_y / 2) * spacing; j+= spacing) {

            // old_pos_y += j * spacing;

            for (int k = old_pos_z - (int)(_scale_z / 2) * spacing; k <= old_pos_z + (int)(_scale_z / 2) * spacing; k+= spacing) {

              // LARCV_DEBUG() << "i " << i << ", j " << j << ", k " << k<< std::endl;

              VoxelID_t new_index;
              new_index = new_meta_pmaps.id(i, j, k);

            _output_voxel_set.add(larcv::Voxel(new_index, voxel.value() /*/ (_scale_x * _scale_x * _scale_z)*/));

            }

          }
        }

      }

      auto & ev_sparse3d_out = mgr.get_data<larcv::EventSparseTensor3D>(_output_labels.at(i_input));
      ev_sparse3d_out.emplace(std::move(_output_voxel_set), new_meta_pmaps);
    }


    return true;
  }

  void ReSample::finalize()
  {}

}
#endif
