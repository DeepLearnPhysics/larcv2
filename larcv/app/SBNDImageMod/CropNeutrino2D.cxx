#ifndef __CROPNEUTRINO2D_CXX__
#define __CROPNEUTRINO2D_CXX__

#include "CropNeutrino2D.h"
#include "larcv/core/DataFormat/EventImage2D.h"
#include "larcv/core/DataFormat/EventVoxel2D.h"

namespace larcv {

static CropNeutrino2DProcessFactory __global_CropNeutrino2DProcessFactory__;

CropNeutrino2D::CropNeutrino2D(const std::string name) : ProcessBase(name) {}

void CropNeutrino2D::configure(const PSet& cfg) {

  _vertex_cluster2d_producer  = cfg.get<std::string>("VertexProducer");
  _producer_names_v           = cfg.get<std::vector<std::string> >("Producers");
  _product_types_v            = cfg.get<std::vector<std::string> >("ProductTypes");
  _output_producers_v         = cfg.get<std::vector<std::string> >("OutputProducers");
  _output_rows                = cfg.get<int>("OutputRows");
  _output_cols                = cfg.get<int>("OutputCols");

  if (_producer_names_v.size() != _product_types_v.size() ||
      _producer_names_v.size() != _output_producers_v.size()) {
    LARCV_CRITICAL() << "Number of product names, product tyes, and output "
                        "producers does not match"
                     << std::endl;
    throw larbys();
  }
}

void CropNeutrino2D::initialize() {
  if (!_output_rows) {
    LARCV_CRITICAL() << "Output Row Size is 0 (undefined)!" << std::endl;
    throw larbys();
  }
  if (!_output_cols) {
    LARCV_CRITICAL() << "Output Col Size is 0 (undefined)!" << std::endl;
    throw larbys();
  }
}

bool CropNeutrino2D::process(IOManager& mgr) {
  // For this algorithm to succeed, all of the products to be cropped need to
  // have the same original
  // meta sizes and such.
  // std::cout << "Enter CropNeutrino2D::process " << std::endl;

  auto ev_reference_cluster =
      mgr.get_data<larcv::EventClusterPixel2D>(_vertex_cluster2d_producer);
  if (ev_reference_cluster.as_vector().size() == 0) {
    LARCV_CRITICAL() << "Input cluster not found by producer name "
                     << _vertex_cluster2d_producer << std::endl;
    throw larbys();
  }

  // Now we have the input vertex producer.  It has the meta that is the
  // reference meta for this
  // algorithm

  size_t n_projections = ev_reference_cluster.size();

  for (size_t i = 0; i < _product_types_v.size(); i++) {
    // Get the data product, compare it's meta against the reference.
    if (_product_types_v.at(i) == "image2d") {
      auto const& ev_image2d =
          mgr.get_data<larcv::EventImage2D>(_producer_names_v.at(i));

      if (ev_image2d.as_vector().size() == 0) {
        LARCV_CRITICAL() << "Input image not found by producer name "
                         << _producer_names_v.at(i) << std::endl;
        throw larbys();
      }
      // Check each plane:
      if (ev_image2d.as_vector().size() !=
          ev_reference_cluster.as_vector().size()) {
        LARCV_CRITICAL() << "Size mismatch between image2d by "
                         << _producer_names_v.at(i) << " and reference clsuter."
                         << std::endl;
        throw larbys();
      }
      for (size_t projection_id = 0;
           projection_id < ev_image2d.as_vector().size(); projection_id++) {
        auto image_meta = ev_image2d.at(projection_id).meta();
        auto ref_meta =
            ev_reference_cluster.cluster_pixel_2d(projection_id).meta();

        // Meta comparisons:
        if (image_meta != ref_meta) {
          LARCV_CRITICAL() << "Meta mismatch between image2d by "
                           << _producer_names_v.at(i)
                           << " and reference clsuter." << std::endl;
          throw larbys();
        }
      }

    } else if (_product_types_v.at(i) == "cluster2d") {
      auto const& ev_cluster2d =
          mgr.get_data<larcv::EventClusterPixel2D>(_producer_names_v.at(i));
      if (ev_cluster2d.as_vector().size() == 0) {
        LARCV_CRITICAL() << "Input clusters not found by producer name "
                         << _producer_names_v.at(i) << std::endl;
        throw larbys();
      }
      // Check each plane:
      if (ev_cluster2d.as_vector().size() !=
          ev_reference_cluster.as_vector().size()) {
        LARCV_CRITICAL() << "Size mismatch between image2d by "
                         << _producer_names_v.at(i) << " and reference clsuter."
                         << std::endl;
        throw larbys();
      }
      for (size_t projection_id = 0;
           projection_id < ev_cluster2d.as_vector().size(); projection_id++) {
        auto image_meta = ev_cluster2d.cluster_pixel_2d(projection_id).meta();
        auto ref_meta =
            ev_reference_cluster.cluster_pixel_2d(projection_id).meta();

        // Meta comparisons:
        if (image_meta != ref_meta) {
          LARCV_CRITICAL() << "Meta mismatch between image2d by "
                           << _producer_names_v.at(i)
                           << " and reference clsuter." << std::endl;
          throw larbys();
        }
      }

    } else {
      LARCV_CRITICAL() << "Product type " << _product_types_v.at(i)
                       << " not supported for cropping." << std::endl;
      throw larbys();
    }
  }

  // At reaching this point, we have validated size and meta matching across all
  // desired products.

  std::vector<larcv::ImageMeta> new_metas;
  std::vector<int> min_rows;
  std::vector<int> min_cols;
  std::vector<int> max_rows;
  std::vector<int> max_cols;

  // Loop over the projection ids to make the new meta creation easier to organize.
  for (size_t projection_id = 0; projection_id < n_projections; projection_id ++ ){


    auto& original_meta = ev_reference_cluster.cluster_pixel_2d(projection_id).meta();
    auto& clust
      = ev_reference_cluster.cluster_pixel_2d(projection_id).as_vector().front();

    // Take the average point of the clusters, weighted by value:
    float mean_x(0.0), mean_y(0.0);
    float weight = 0;
    for (auto & voxel : clust.as_vector()) {
      if (voxel.id() > original_meta.size())
        continue;
      mean_x += voxel.value() *
                original_meta.pos_x(original_meta.index_to_col(voxel.id()));
      mean_y += voxel.value() *
                original_meta.pos_y(original_meta.index_to_row(voxel.id()));
      weight += voxel.value();
    }
    mean_x /= weight;
    mean_y /= weight;

    // Now we have the mean x and mean y, figure out what pixels this is in the
    // meta:
    int col = original_meta.col(mean_x);
    int row = original_meta.row(mean_y);


    // Create a new image meta that contains the cropped pixels
    // By design, the vertex location is placed at 25% of the way across the
    // colums
    // and 50% of the way across the rows;
    int min_row = row - 0.5 * _output_rows;
    int min_col = col - 0.25 * _output_cols;
    int max_row = row + 0.5 * _output_rows;
    int max_col = col + 0.75 * _output_cols;

    min_rows.push_back(min_row);
    min_cols.push_back(min_col);
    max_rows.push_back(max_row);
    max_cols.push_back(max_col);

    // get the x/y locations of the min/max row/col in the old meta:
    float min_x = original_meta.min_x() + original_meta.pixel_width() * min_col;
    float max_x = original_meta.pos_x(max_col);
    float min_y = original_meta.min_y() + original_meta.pixel_height() * min_row;
    float max_y = original_meta.pos_y(max_row);

    LARCV_INFO() << "For (row, col) = (" << row << ", " << col << "): "
                 << "\n col range: " << min_col << " - " << max_col
                 << "\n row range: " << min_row << " - " << max_row
                 << "\n  x range: " << min_x << " - " << max_x
                 << "\n  y range: " << min_y << " - " << max_y
                 << std::endl;

    // Create a new meta object:
    larcv::ImageMeta new_meta(min_x, min_y, max_x, max_y, _output_rows,
                              _output_cols, projection_id, original_meta.unit());

    new_metas.push_back(new_meta);
  }


  // Now loop over the products and replace items by cropped items
  for (size_t i = 0; i < _product_types_v.size(); i++) {

    if (_product_types_v.at(i) == "image2d"){

      // Get the old image2d:
      auto ev_image2d = mgr.get_data<larcv::EventImage2D>(_producer_names_v.at(i));
      std::vector<larcv::Image2D> new_image_vector;

      for (size_t projection_id = 0; projection_id < n_projections; projection_id ++ ){

        // Make a new image:
        larcv::Image2D new_image(new_metas.at(projection_id));
        auto old_image = ev_image2d.at(projection_id);
        // Populate the new image 2d from the old image 2d.

        // Every index in the new meta should match 0 or 1 indexes in the old meta
        for (int i_row = 0; i_row < _output_rows; i_row++) {
          // This is looping over rows and columns of the new meta.
          // Need to match this row and column to the old meta
          int old_row = i_row + min_rows.at(projection_id);
          if (old_row < 0 || old_row >= old_image.meta().rows()) continue;
          for (int i_col = 0; i_col < _output_rows; i_col++) {
            int old_col = i_col + min_cols.at(projection_id);
            if (old_col < 0 || old_col >= old_image.meta().cols()) continue;

            // Ok, this is a valid row/col pair in each meta.
            // Copy the value from old to new.
            new_image.set_pixel(i_row, i_col, old_image.pixel(old_row, old_col));
          }
        }
        new_image_vector.push_back(new_image);

      }

      // Make an output image producer
      auto& ev_image2d_out = mgr.get_data<larcv::EventImage2D>(_output_producers_v.at(i));
      ev_image2d_out.emplace(std::move(new_image_vector));

    }
    else if (_product_types_v.at(i) == "cluster2d"){

      // Get the old cluster2d:
      auto ev_cluster2d = mgr.get_data<larcv::EventClusterPixel2D>(_producer_names_v.at(i));

      // Prepare a placeholder vector to hold the new set of clusters by plane:
      std::vector<larcv::ClusterPixel2D> new_cluster2d_vector;

      for (size_t projection_id = 0; projection_id < n_projections; projection_id ++ ){

        // std::cout << "Projection id " << projection_id << std::endl;

        // Make a new image:
        larcv::ClusterPixel2D new_cluster2ds;
        new_cluster2ds.meta(new_metas.at(projection_id));

        auto old_clusters = ev_cluster2d.cluster_pixel_2d(projection_id);
        // Populate the new cluster2d from the old cluster2d.

        // std::cout << " - Number of old clusters: " << old_clusters.as_vector().size() << std::endl;

        for (size_t cluster_index = 0; cluster_index < old_clusters.as_vector().size(); cluster_index ++){
          larcv::VoxelSet new_vs;
          auto const& cluster = old_clusters.as_vector().at(cluster_index);

          new_vs.id(cluster.id());
          // std::cout << " -- Number of voxels for cluster " << cluster_index << ": " << cluster.as_vector().size() << std::endl;
          for (auto const& voxel : cluster.as_vector()){
            if (voxel.id() > old_clusters.meta().size())
              continue;
            // The id of this voxel is in the old meta.
            // We get the row and column in the old meta coordinates,
            // then subtract off the min vals that made it into the new meta
            // To get the new row and column.
            // From there, convert to new id and store the voxel.

            int old_row = old_clusters.meta().index_to_row(voxel.id());
            int old_col = old_clusters.meta().index_to_col(voxel.id());

            int new_row = old_row - min_rows.at(projection_id);
            int new_col = old_col - min_cols.at(projection_id);


            // If this row or column is outside the boundaries, skip it:
            if (new_row < 0 || new_row >= new_metas.at(projection_id).rows()) continue;
            if (new_col < 0 || new_col >= new_metas.at(projection_id).cols()) continue;

            // Get the index of the voxel in the new meta:
            size_t new_index = new_metas.at(projection_id).index(new_row, new_col);

            //  This voxel is kept, store it:
            new_vs.add(larcv::Voxel(new_index, voxel.value()));
          }
          // std::cout << " -- Number of voxels in new cluster " << cluster_index << ": " << new_vs.as_vector().size() << std::endl;
          new_cluster2ds.emplace(std::move(new_vs));
        }

        // std::cout << " - Number of clusters in new ClusterPixel2d: " << new_cluster2ds.as_vector().size() << std::endl;

        new_cluster2d_vector.push_back(new_cluster2ds);

      }

      // Make an output image producer
      auto& ev_cluster2d_out = mgr.get_data<larcv::EventClusterPixel2D>(_producer_names_v.at(i));
      for (auto & cluster_pix2d : new_cluster2d_vector){
        ev_cluster2d_out.emplace(std::move(cluster_pix2d));
      }

    }

      //
  }

  // std::cout << "Exit CropNeutrino2D::process " << std::endl;

  return true;
}

void CropNeutrino2D::finalize() {}
}
#endif
