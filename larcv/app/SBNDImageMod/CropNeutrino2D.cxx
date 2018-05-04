#ifndef __CROPNEUTRINO2D_CXX__
#define __CROPNEUTRINO2D_CXX__

#include "CropNeutrino2D.h"
#include "larcv/core/DataFormat/EventImage2D.h"
#include "larcv/core/DataFormat/EventVoxel2D.h"

namespace larcv {

  static CropNeutrino2DProcessFactory __global_CropNeutrino2DProcessFactory__;

  CropNeutrino2D::CropNeutrino2D(const std::string name)
    : ProcessBase(name)
  {}

  void CropNeutrino2D::configure(const PSet& cfg)
  {

    _vertex_cluster2d_producer = cfg.get<std::string>("VertexProducer");
    _image_producer  = cfg.get<std::string>("ImageProducer");
    _output_producer = cfg.get<std::string>("OutputProducer");
    _output_rows     = cfg.get<int>("OutputRows");
    _output_cols     = cfg.get<int>("OutputCols");
    _rand_shift_row  = cfg.get<int>("RandomShiftRow",0);
    _rand_shift_col  = cfg.get<int>("RandomShiftCol",0);


  }

  void CropNeutrino2D::initialize()
  {
      if (!_output_rows) {
        LARCV_CRITICAL() << "Output Row Size is 0 (undefined)!" << std::endl;
        throw larbys();
      }
      if (!_output_cols) {
        LARCV_CRITICAL() << "Output Col Size is 0 (undefined)!" << std::endl;
        throw larbys();
      }

  }

  bool CropNeutrino2D::process(IOManager& mgr)
  {

    // First, get the input image producer and cluster producer, validate sizes match
    auto & ev_image2d = mgr.get_data<larcv::EventImage2D>(_image_producer);
    if (ev_image2d.as_vector().size() == 0) {
      LARCV_CRITICAL() << "Input image not found by producer name " << _image_producer << std::endl;
      throw larbys();
    }

    auto ev_cluster2d = mgr.get_data<larcv::EventClusterPixel2D>(_vertex_cluster2d_producer);
    if (ev_cluster2d.as_vector().size() == 0) {
      LARCV_CRITICAL() << "Input cluster not found by producer name " << _vertex_cluster2d_producer << std::endl;
      throw larbys();
    }

    if (ev_image2d.as_vector().size() != ev_cluster2d.as_vector().size()){
      LARCV_CRITICAL() << "Error in size of image2d or cluster2d products "
                       << std::endl;
      throw larbys();
    }


    // Have the images, now we need to get new image meta for cropping.


    std::vector<larcv::Image2D> new_image_vector;
    for (size_t projection_id = 0; projection_id < ev_image2d.as_vector().size(); projection_id ++){
      auto & image = ev_image2d.at(projection_id);
      // Take the clusterPixel2D for this projection ID, and the first cluster
      auto & clust = ev_cluster2d.cluster_pixel_2d(projection_id).as_vector().front();
      if (clust.as_vector().size() == 0){
        LARCV_CRITICAL() << "Can not crop using a cluster of 0 size." << std::endl;
        throw larbys();
      }

      auto & cluster_meta  = ev_cluster2d.cluster_pixel_2d(projection_id).meta();

      // Take the average point of the clusters, weighted by value:
      float mean_x(0.0), mean_y(0.0);
      float weight = 0;
      for (auto & voxel : clust.as_vector()){
        mean_x += voxel.value() * cluster_meta.pos_x(cluster_meta.index_to_col(voxel.id()));
        mean_y += voxel.value() * cluster_meta.pos_y(cluster_meta.index_to_row(voxel.id()));
        weight += voxel.value();
      }
      mean_x /= weight;
      mean_y /= weight;

      // Now we have the mean x and mean y, figure out what pixels this is in the image:
      auto & image_meta = image.meta();
      int col = image_meta.col(mean_x);
      int row = image_meta.row(mean_y);

      // If desired, apply a random shift:
      if (_rand_shift_col != 0){
        col += rand() % (2*_rand_shift_col + 1) - _rand_shift_col;
      }
      if (_rand_shift_row != 0){
        row += rand() % (2*_rand_shift_row + 1) - _rand_shift_row;
      }


      // Create a new image meta that contains the cropped pixels
      // By design, the vertex location is placed at 25% of the way across the colums
      // and 50% of the way across the rows;
      int min_row = row - 0.5*_output_rows;
      int min_col = col - 0.25*_output_cols;
      int max_row = row + 0.5*_output_rows;
      int max_col = col + 0.75*_output_cols;

      // get the x/y locations of the min/max row/col in the old meta:
      float min_x = image_meta.min_x() + image_meta.pixel_width() * min_col;
      float max_x = image_meta.pos_x(max_col);
      float min_y = image_meta.min_y() + image_meta.pixel_height()* min_row;
      float max_y = image_meta.pos_y(max_row);

      // std::cout << "For (row, col) = (" << row << ", " << col << "): "
      //           << "\n col range: " << min_col << " - " << max_col
      //           << "\n row range: " << min_row << " - " << max_row
      //           << "\n  x range: " << min_x << " - " << max_x
      //           << "\n  y range: " << min_y << " - " << max_y
      //           << std::endl;

      // Create a new meta object:
      larcv::ImageMeta new_meta(min_x, min_y, max_x, max_y,
                                _output_rows, _output_cols,
                                projection_id,
                                image_meta.unit());

      // Create a new Image2d from the new meta:
      larcv::Image2D new_image(new_meta);

      // Populate the new image 2d from the old image 2d.
      // Every index in the new meta should match 0 or 1 indexes in the old meta
      for (int i_row = 0; i_row < _output_rows; i_row ++){
        // This is looping over rows and columns of the new meta.
        // Need to match this row and column to the old meta
        int old_row = i_row + min_row;
        if (old_row < 0 || old_row >= image_meta.rows()) continue;
        for (int i_col = 0; i_col < _output_rows; i_col ++){
          int old_col = i_col + min_col;
          if (old_col < 0 || old_col >= image_meta.cols()) continue;

          // Ok, this is a valid row/col pair in each meta.
          // Copy the value from old to new.
          new_image.set_pixel(i_row, i_col, image.pixel(old_row, old_col));

        }
      }
      new_image_vector.push_back(new_image);

      // Now crop the cluster2d objects.


    }

    // If we're also cropping cluster2d, get a list of producers and
    // be ready for output


    // Get the original meta:


    // Make an output image producer
    auto & ev_image2d_out = mgr.get_data<larcv::EventImage2D>(_output_producer);
    ev_image2d_out.emplace(std::move(new_image_vector));

    return true;
  }


  void CropNeutrino2D::finalize()
  {}

}
#endif
