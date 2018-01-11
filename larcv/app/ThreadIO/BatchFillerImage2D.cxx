#ifndef __BatchFillerImage2D_CXX__
#define __BatchFillerImage2D_CXX__

#include "BatchFillerImage2D.h"

#include <random>

namespace larcv {

  static BatchFillerImage2DProcessFactory __global_BatchFillerImage2DProcessFactory__;

  BatchFillerImage2D::BatchFillerImage2D(const std::string name)
    : BatchFillerTemplate<float>(name)
    , _rows(kINVALID_SIZE)
    , _cols(kINVALID_SIZE)
    , _slice_v()
    , _max_ch(0)
  {}

  void BatchFillerImage2D::configure(const PSet& cfg)
  {
    LARCV_DEBUG() << "start" << std::endl;
    _caffe_mode = cfg.get<bool>("CaffeMode",false);
    _image_producer = cfg.get<std::string>("ImageProducer");

    _slice_v = cfg.get<std::vector<size_t> >("Channels", _slice_v);
    _mirror_image = cfg.get<bool>("EnableMirror", false);
    _crop_image     = cfg.get<bool>("EnableCrop", false);

    LARCV_DEBUG() << "configuring cropper" << std::endl;
    if (_crop_image)
      _cropper.configure(cfg);

    LARCV_DEBUG() << "done" << std::endl;
  }

  void BatchFillerImage2D::initialize()
  {}

  void BatchFillerImage2D::_batch_begin_()
  {
    _mirrored.clear();
    _mirrored.reserve(batch_size());

    if (!batch_data().dim().empty() && (int)(batch_size()) != batch_data().dim().front()) {
      auto dim = batch_data().dim();
      dim[0] = batch_size();
      this->set_dim(dim);
    }

  }

  void BatchFillerImage2D::_batch_end_()
  {
    if (logger().level() <= msg::kINFO) {
      LARCV_INFO() << "Total data size: " << batch_data().data_size() << std::endl;

      size_t mirror_ctr = 0;
      for (auto const& v : _mirrored) if (v) ++mirror_ctr;
      LARCV_INFO() << mirror_ctr << " / " << _mirrored.size() << " images are mirrored!" << std::endl;

    }
  }

  void BatchFillerImage2D::finalize()
  { _entry_data.clear(); }

  size_t BatchFillerImage2D::set_image_size(const EventImage2D* image_data)
  {
    auto const& image_v = image_data->image2d_array();
    if (image_v.empty()) {
      LARCV_CRITICAL() << "Input image is empty!" << std::endl;
      throw larbys();
    }
    if (_slice_v.empty()) {
      _slice_v.resize(image_v.size());
      for (size_t i = 0; i < _slice_v.size(); ++i) _slice_v.at(i) = i;
    }

    _num_channels = _slice_v.size();
    _max_ch = 0;
    for (auto const& v : _slice_v) if (_max_ch < v) _max_ch = v;

    if (image_v.size() <= _max_ch) {
      LARCV_CRITICAL() << "Requested slice max channel (" << _max_ch
                       << ") exceeds available # of channels in the input image" << std::endl;
      throw larbys();
    }
    if ( !_crop_image ) {
      // set the dimensions from the image
      _rows = image_v.at(_slice_v.front()).meta().rows();
      _cols = image_v.at(_slice_v.front()).meta().cols();
    }
    else {
      // gonna crop (if speicifed dim is smaller than image dim)
      _rows = std::min( image_v.at(_slice_v.front()).meta().rows(), _cropper.rows() );
      _cols = std::min( image_v.at(_slice_v.front()).meta().cols(), _cropper.cols() );
    }

    LARCV_INFO() << "Rows = " << _rows << " ... Cols = " << _cols << std::endl;

    // Define caffe idx to Image2D idx (assuming no crop)
    _caffe_idx_to_img_idx.resize(_rows * _cols, 0);
    _mirror_caffe_idx_to_img_idx.resize(_rows * _cols, 0);
    size_t caffe_idx = 0;
    for (size_t row = 0; row < _rows; ++row) {
      for (size_t col = 0; col < _cols; ++col) {
        _caffe_idx_to_img_idx.at(caffe_idx) = col * _rows + row;
        _mirror_caffe_idx_to_img_idx.at(caffe_idx) = (_cols - col - 1) * _rows + row;
        ++caffe_idx;
      }
    }

    return (_rows * _cols * _num_channels);
  }

  void BatchFillerImage2D::assert_dimension(const EventImage2D* image_data) const
  {
    auto const& image_v = image_data->image2d_array();
    if (_rows == kINVALID_SIZE) {
      LARCV_WARNING() << "set_dimension() must be called prior to check_dimension()" << std::endl;
      return;
    }
    bool valid_ch   = (image_v.size() > _max_ch);
    bool valid_rows = true;
    for (size_t ch = 0; ch < _num_channels; ++ch) {
      size_t input_ch = _slice_v.at(ch);
      auto const& img = image_v.at(input_ch);

      if ( !_crop_image )
        valid_rows = ( _rows == img.meta().rows() );
      if (!valid_rows) {
        LARCV_ERROR() << "# of rows changed! (row,col): (" << _rows << "," << _cols << ") => ("
                      << img.meta().rows() << "," << img.meta().cols() << ")" << std::endl;
        break;
      }
    }

    bool valid_cols = true;
    for (size_t ch = 0; ch < _num_channels; ++ch) {
      size_t input_ch = _slice_v.at(ch);
      auto const& img = image_v.at(input_ch);
      if ( !_crop_image )
        valid_cols = ( _cols == img.meta().cols() );
      if (!valid_cols) {
        LARCV_ERROR() << "# of cols changed! (row,col): (" << _rows << "," << _cols << ") => ("
                      << img.meta().rows() << "," << img.meta().cols() << ")" << std::endl;
        break;
      }
    }
    if (!valid_rows) {
      LARCV_CRITICAL() << "# of rows in the input image have changed!" << std::endl;
      throw larbys();
    }
    if (!valid_cols) {
      LARCV_CRITICAL() << "# of cols in the input image have changed!" << std::endl;
      throw larbys();
    }
    if (!valid_ch) {
      LARCV_CRITICAL() << "# of channels have changed in the input image! Image vs. MaxCh ("
                       << image_v.size() << " vs. " << _max_ch << ")" << std::endl;
      throw larbys();
    }
  }

  bool BatchFillerImage2D::process(IOManager& mgr)
  {
    LARCV_DEBUG() << "start" << std::endl;
    auto image_data = (EventImage2D*)(mgr.get_data("image2d", _image_producer));
    if (!image_data) {
      LARCV_CRITICAL() << "Could not locate image data w/ producer name " << _image_producer << std::endl;
      throw larbys();
    }
    if (image_data->image2d_array().empty()) {
      LARCV_CRITICAL() << "Image data w/ producer name " << _image_producer << " is empty!" << std::endl;
      throw larbys();
    }
    // one time operation: get image dimension
    if (batch_data().dim().empty()) {
      this->set_image_size(image_data);

      std::vector<int> dim;
      dim.resize(4);
      if (_caffe_mode) {
        dim[0] = batch_size();
        dim[1] = _num_channels;
        dim[2] = _rows;
        dim[3] = _cols;
      } else {
        dim[0] = batch_size();
        dim[1] = _rows;
        dim[2] = _cols;
        dim[3] = _num_channels;
      }
      this->set_dim(dim);
    }
    else
      this->assert_dimension(image_data);

    if (_entry_data.size() != batch_data().entry_data_size())
      _entry_data.resize(batch_data().entry_data_size(), 0.);

    for (auto& v : _entry_data) v = 0.;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> irand(0, 1);
    bool mirror_image = false;
    if (_mirror_image && irand(gen)) {
      _mirrored.push_back(true);
      mirror_image = true;
    }
    else { _mirrored.push_back(false); }

    auto const& image_v = ((EventImage2D*)image_data)->image2d_array();

    for (size_t ch = 0; ch < _num_channels; ++ch) {

      size_t input_ch = _slice_v.at(ch);

      auto const& input_img2d = image_v.at(input_ch);
      auto const& input_meta  = input_img2d.meta();

      if (_crop_image) _cropper.set_crop_region(input_meta.rows(), input_meta.cols());

      auto const& input_image = (_crop_image ? _cropper.crop(input_img2d) : input_img2d.as_vector());

      //size_t caffe_idx = 0;
      size_t target_idx = ch * _rows * _cols;

      for (size_t caffe_idx = 0; caffe_idx < (_rows * _cols); ++caffe_idx) {

        if (_caffe_mode) {

          if (mirror_image)

            _entry_data.at(target_idx) = input_image.at(_mirror_caffe_idx_to_img_idx[caffe_idx]);

          else

            _entry_data.at(target_idx) = input_image.at(_caffe_idx_to_img_idx[caffe_idx]);

        } else {

          if (mirror_image)

            _entry_data.at(caffe_idx * _num_channels + ch) = input_image.at(_mirror_caffe_idx_to_img_idx[caffe_idx]);

          else

            _entry_data.at(caffe_idx * _num_channels + ch) = input_image.at(_caffe_idx_to_img_idx[caffe_idx]);

        }
        ++target_idx;
      }
    }

    // record the entry data
    LARCV_INFO() << "Inserting entry data of size " << _entry_data.size() << std::endl;
    set_entry_data(_entry_data);

    return true;
  }

}
#endif
