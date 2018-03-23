#ifndef __COMPRESSANDEMBED_CXX__
#define __COMPRESSANDEMBED_CXX__

#include "CompressAndEmbed.h"
#include "larcv/core/DataFormat/EventImage2D.h"

namespace larcv {

  static CompressAndEmbedProcessFactory __global_CompressAndEmbedProcessFactory__;

  CompressAndEmbed::CompressAndEmbed(const std::string name)
    : ProcessBase(name)
  {}

  void CompressAndEmbed::configure(const PSet& cfg)
  {
    _image_producer_v  = cfg.get<std::vector<std::string> >("ImageProducer");
    _row_compression_v = cfg.get<std::vector<size_t     > >("RowCompression");
    _col_compression_v = cfg.get<std::vector<size_t     > >("ColCompression");
    _output_rows_v     = cfg.get<std::vector<size_t     > >("OutputRows");
    _output_cols_v     = cfg.get<std::vector<size_t     > >("OutputCols");

    auto mode_v = cfg.get<std::vector<unsigned short> >("Mode");
    _mode_v.clear();
    for (auto const& v : mode_v) _mode_v.push_back( (Image2D::CompressionModes_t)v );

    if (_mode_v.size() != _image_producer_v.size()  ||
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

      auto const& image_producer  = _image_producer_v[i];
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
      if (image_producer.empty()) {
        LARCV_CRITICAL() << "Image producer not specified!" << std::endl;
        throw larbys();
      }
    }
  }

  bool CompressAndEmbed::process(IOManager& mgr)
  {
    // Check if compression factor works - has to fit into output image
    for (size_t i = 0; i < _mode_v.size(); ++i) {

      auto const& image_producer  = _image_producer_v[i];
      auto const& row_compression = _row_compression_v[i];
      auto const& col_compression = _col_compression_v[i];
      auto const& output_rows     = _output_rows_v[i];
      auto const& output_cols     = _output_cols_v[i];

      auto ev_image = (EventImage2D*)(mgr.get_data("image2d", image_producer));
      if (!ev_image) {
        LARCV_CRITICAL() << "Input image not found by producer name " << image_producer << std::endl;
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

    // Apply compression

    // Steps are: initialize output image size
    // Determine start indices from compressed image to output image
    // Compress image and copy to proper output index


    for (size_t i = 0; i < _mode_v.size(); ++i) {

      auto const& image_producer  = _image_producer_v[i];
      auto const& row_compression = _row_compression_v[i];
      auto const& col_compression = _col_compression_v[i];
      auto const& output_rows     = _output_rows_v[i];
      auto const& output_cols     = _output_cols_v[i];
      auto const& mode = _mode_v[i];


      auto ev_image = (EventImage2D*)(mgr.get_data("image2d", image_producer));


      std::vector<larcv::Image2D> image_v;
      // ev_image->move(image_v);

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

    return true;
  }


  void CompressAndEmbed::finalize()
  {}

}
#endif
