#ifndef __COMPRESSANDEMBED_CXX__
#define __COMPRESSANDEMBED_CXX__

#include "CompressAndEmbed.h"
#include "larcv/core/DataFormat/EventImage2D.h"
#include "larcv/core/DataFormat/EventVoxel2D.h"

namespace larcv {

  static CompressAndEmbedProcessFactory __global_CompressAndEmbedProcessFactory__;

  CompressAndEmbed::CompressAndEmbed(const std::string name)
    : ProcessBase(name)
  {}

  void CompressAndEmbed::configure(const PSet& cfg)
  {
    _producer_v        = cfg.get<std::vector<std::string> >("Producer");
    _data_type_v       = cfg.get<std::vector<std::string> >("DataType");
    _row_compression_v = cfg.get<std::vector<size_t     > >("RowCompression");
    _col_compression_v = cfg.get<std::vector<size_t     > >("ColCompression");
    _output_rows_v     = cfg.get<std::vector<size_t     > >("OutputRows");
    _output_cols_v     = cfg.get<std::vector<size_t     > >("OutputCols");

    auto mode_v = cfg.get<std::vector<unsigned short> >("Mode");
    _mode_v.clear();
    for (auto const& v : mode_v) _mode_v.push_back( (Image2D::CompressionModes_t)v );

    if (_mode_v.size() != _producer_v.size()        ||
        _mode_v.size() != _data_type_v.size()       ||
        _mode_v.size() != _row_compression_v.size() ||
        _mode_v.size() != _col_compression_v.size() ||
        _mode_v.size() != _output_rows_v.size()     ||
        _mode_v.size() != _output_cols_v.size()     ){
      LARCV_CRITICAL() << "Length of parameter arrays do not match!" << std::endl;
      throw larbys();
    }

  }

  void CompressAndEmbed::initialize()
  {
    for (size_t i = 0; i < _mode_v.size(); ++i) {

      auto const& producer        = _producer_v[i];
      auto const& row_compression = _row_compression_v[i];
      auto const& col_compression = _col_compression_v[i];

      if (!row_compression) {
        LARCV_CRITICAL() << "Row compression factor is 0 (undefined)!" << std::endl;
        throw larbys();
      }
      if (!col_compression) {
        LARCV_CRITICAL() << "Col compression factor is 0 (undefined)!" << std::endl;
        throw larbys();
      }
      if (producer.empty()) {
        LARCV_CRITICAL() << "Producer not specified!" << std::endl;
        throw larbys();
      }
    }
  }

  bool CompressAndEmbed::process(IOManager& mgr)
  {
    // Check if compression factor works - has to fit into output image
    for (size_t i = 0; i < _mode_v.size(); ++i) {

      auto const& producer        = _producer_v[i];
      auto const& datatype        = _data_type_v[i];
      auto const& row_compression = _row_compression_v[i];
      auto const& col_compression = _col_compression_v[i];
      auto const& output_rows     = _output_rows_v[i];
      auto const& output_cols     = _output_cols_v[i];

      if (datatype == "image2d"){
        auto ev_image = (EventImage2D*)(mgr.get_data("image2d", producer));
        if (!ev_image) {
          LARCV_CRITICAL() << "Input image not found by producer name " << producer << std::endl;
          throw larbys();
        }

        for (auto const& img : ev_image->image2d_array()) {
          auto const& meta = img.meta();
          if (meta.rows() / row_compression > output_rows) {
            LARCV_CRITICAL() << "Input image # rows (" << meta.rows()
                             << ") cannot be fit into output shape " << output_rows
                             << " with compression (" << row_compression
                             << ")" << std::endl;
            throw larbys();
          }
          if (meta.cols() / col_compression > output_cols) {
            LARCV_CRITICAL() << "Input image # cols (" << meta.cols()
                             << ") cannot be fit into output shape " << output_cols
                             << " with compression (" << col_compression
                             << ")" << std::endl;
            throw larbys();
          }
        }
      }
      else if (datatype == "cluster2d"){
        auto ev_clust = (EventClusterPixel2D*)(mgr.get_data("cluster2d", producer));
        if (!ev_clust) {
          LARCV_CRITICAL() << "Input cluster2d not found by producer name " << producer << std::endl;
          throw larbys();
        }

        for (auto const& clust : ev_clust->as_vector()) {
          auto const& meta = clust.meta();
          if (meta.rows() / row_compression > output_rows) {
            LARCV_CRITICAL() << "Input image # rows (" << meta.rows()
                             << ") cannot be fit into output shape " << output_rows
                             << " with compression (" << row_compression
                             << ")" << std::endl;
            throw larbys();
          }
          if (meta.cols() / col_compression > output_cols) {
            LARCV_CRITICAL() << "Input image # cols (" << meta.cols()
                             << ") cannot be fit into output shape " << output_cols
                             << " with compression (" << col_compression
                             << ")" << std::endl;
            throw larbys();
          }
        }
      }
      else{
        LARCV_CRITICAL() << "Unsupported datatype reached: " << datatype << std::endl;
        throw larbys();
      }


    }

    // Apply compression

    // Steps are: initialize output image size
    // Determine start indices from compressed image to output image
    // Compress image and copy to proper output index


    for (size_t i = 0; i < _mode_v.size(); ++i) {

      auto const& producer        = _producer_v[i];
      auto const& datatype        = _data_type_v[i];
      auto const& row_compression = _row_compression_v[i];
      auto const& col_compression = _col_compression_v[i];
      auto const& output_rows     = _output_rows_v[i];
      auto const& output_cols     = _output_cols_v[i];
      auto const& mode = _mode_v[i];


      if (datatype == "image2d"){
        auto ev_image = (EventImage2D*)(mgr.get_data("image2d", producer));
        std::vector<larcv::Image2D> image_v;
        for (auto& img : ev_image->as_vector()) {

          auto const& original_rows = img.meta().rows();
          auto const& original_cols = img.meta().cols();

          auto const& offset_rows = 0.5*(output_rows - (original_rows / row_compression));
          auto const& offset_cols = 0.5*(output_cols - (original_cols / col_compression));

          auto output_meta = img.meta();
          output_meta.update(output_rows, output_cols);
          image_v.push_back(Image2D(output_meta));

          for ( size_t col = offset_cols; col < output_cols - offset_cols; col ++ ){
            for (size_t row = offset_rows; row < output_rows - offset_rows; row ++ ){
              float value = 0;
              float count = 0;
              for (size_t orig_col = (col - offset_cols)*col_compression;
                   orig_col < (col - offset_cols + 1)*col_compression;
                   orig_col ++){
                if (orig_col >= img.meta().cols()) continue;
                for (size_t orig_row = (row - offset_rows)*row_compression;
                     orig_row < (row - offset_rows + 1)*row_compression;
                     orig_row ++){
                  if (orig_row >= img.meta().rows()) continue;
                  if (mode == Image2D::kMaxPool){
                    value = ( value < img.pixel(orig_row, orig_col) ) ? img.pixel(orig_row, orig_col) : value;
                  }
                  else{
                    value += img.pixel(orig_row, orig_col);
                    count ++;
                  }

                }
              }
              if (mode == Image2D::kAverage){
                value /= (float) count;
              }
              image_v.back().set_pixel(row, col, value);
            }
          }

        }
        ev_image->emplace(std::move(image_v));
      }
      else if (datatype == "cluster2d"){
        auto ev_clust = (EventClusterPixel2D*)(mgr.get_data("cluster2d", producer));
        std::vector<larcv::ClusterPixel2D> cluster_v;

        for (auto& clustPix : ev_clust->as_vector()) {

          auto const& original_rows = clustPix.meta().rows();
          auto const& original_cols = clustPix.meta().cols();

          auto const& offset_rows = 0.5*(output_rows - (original_rows / row_compression));
          auto const& offset_cols = 0.5*(output_cols - (original_cols / col_compression));

          auto const& original_meta = clustPix.meta();
          auto new_meta = clustPix.meta();
          new_meta.update(output_rows, output_cols);

          cluster_v.push_back(ClusterPixel2D());
          cluster_v.back().meta(new_meta);

          // Loop over clusters in this plane
          for (auto & cluster : clustPix.as_vector()){

            // Prepare a new voxel set:
            larcv::VoxelSet _new_cluster;

            _new_cluster.id(cluster.id());
            // Loop over voxels in this cluster
            for (auto & voxel : cluster.as_vector()){
              // Need to calculate the original row, column in order to
              // Find the new row, colum, which tells the new index
              auto original_row = original_meta.index_to_row(voxel.id());
              auto original_col = original_meta.index_to_col(voxel.id());
              auto new_row      = (original_row / row_compression) + offset_rows;
              auto new_col      = (original_col / col_compression) + offset_cols;
              auto new_index    = new_meta.index(new_row, new_col);
              float value = voxel.value();
              if (mode == Image2D::kMaxPool){
                if (_new_cluster.find(new_index).id() != kINVALID_VOXELID){
                  if (value > _new_cluster.find(new_index).value()){
                    _new_cluster.insert(Voxel(new_index, value));
                  }
                }
                else{
                    _new_cluster.insert(Voxel(new_index, value));
                }
              }
              else{
                _new_cluster.add(Voxel(new_index, value));
              }
            }
            if (mode == Image2D::kAverage){
              _new_cluster /= row_compression*col_compression;
            }
            cluster_v.back().emplace(std::move(_new_cluster));
          }

          // for ( size_t col = offset_cols; col < output_cols - offset_cols; col ++ ){
          //   for (size_t row = offset_rows; row < output_rows - offset_rows; row ++ ){
          //     float value = 0;
          //     float count = 0;
          //     for (size_t orig_col = (col - offset_cols)*col_compression;
          //          orig_col < (col - offset_cols + 1)*col_compression;
          //          orig_col ++){
          //       if (orig_col >= clustPix.meta().cols()) continue;
          //       for (size_t orig_row = (row - offset_rows)*row_compression;
          //            orig_row < (row - offset_rows + 1)*row_compression;
          //            orig_row ++){
          //         if (orig_row >= clustPix.meta().rows()) continue;
          //         if (mode == Image2D::kMaxPool){
          //           value = ( value < clustPix.pixel(orig_row, orig_col) ) ? clustPix.pixel(orig_row, orig_col) : value;
          //         }
          //         else{
          //           value += clustPix.pixel(orig_row, orig_col);
          //           count ++;
          //         }

          //       }
          //     }
          //     if (mode == Image2D::kAverage){
          //       value /= (float) count;
          //     }
          //    image_v.back().set_pixel(row, col, value);
          //  }
          // }

        }
        for (auto & clust : cluster_v)
          ev_clust->emplace(std::move(clust));
      }





    }

    return true;
  }


  void CompressAndEmbed::finalize()
  {}

}
#endif
