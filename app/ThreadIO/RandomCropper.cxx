#ifndef RANDOMCROPPER_CXX
#define RANDOMCROPPER_CXX

#include <random>
#include "RandomCropper.h"

namespace larcv {

  RandomCropper::RandomCropper(const std::string name)
    : larcv_base(name)
    , _target_rows(0)
    , _target_cols(0)
    , _crop_rows(0)
    , _crop_cols(0)
    , _randomize_crop(false)
  {
    _crop_cols = _crop_rows = 0;
    _randomize_crop = false;
  }
  
  void RandomCropper::configure(const PSet& cfg)
  {
    _randomize_crop = cfg.get<bool>("RandomizeCrop",false);
    _crop_cols      = cfg.get<int>("CroppedCols");
    _crop_rows      = cfg.get<int>("CroppedRows");
  }

  void RandomCropper::set_crop_region(const size_t rows, const size_t cols)
  {
    _target_rows = rows;
    _target_cols = cols;
    _col_offset = _row_offset = 0;
    int coldiff = std::max(0,(int)_target_cols - (int)_crop_cols);
    int rowdiff = std::max(0,(int)_target_rows - (int)_crop_rows);

    if ( _randomize_crop ) {

      std::random_device rd;
      std::mt19937 gen(rd());
      if ( coldiff>0 ) {
        std::uniform_int_distribution<> irand_col(0,coldiff);
        _col_offset = irand_col(gen);
      }
      
      if ( rowdiff>0 ) {
        std::uniform_int_distribution<> irand_row(0,rowdiff);
        _row_offset = irand_row(gen);
      }
    }
    else {
      if ( coldiff>0 ) _col_offset = (int)coldiff/2;
      if ( rowdiff>0 ) _row_offset = (int)rowdiff/2;
    }

  }

  const std::vector<float> RandomCropper::crop(const larcv::Image2D& image)
  {
    // Check input image size
    if(image.meta().rows() != _target_rows ||
       image.meta().cols() != _target_cols) { 

      LARCV_CRITICAL() << "Image dimension (" << image.meta().rows() 
      << "," << image.meta().cols() << ") does not match with what is specified ("
      << _target_rows << "," << _target_cols << ")!" << std::endl;

      throw larbys();
    }

    // Set _cropped_image size
    size_t cropped_rows = std::min(_target_rows, _crop_rows);
    size_t cropped_cols = std::min(_target_cols, _crop_cols);

    _cropped_image.resize( cropped_rows * cropped_cols );

    auto const& data_v = image.as_vector();

    for(size_t col = 0; col < cropped_cols; ++col) {
      for(size_t row = 0; row < cropped_rows; ++row) {
        _cropped_image[cropped_rows * col + row] = data_v[(col + _col_offset) * _target_rows + row + _row_offset];
      }
    }

    return _cropped_image;
  }


}
#endif
