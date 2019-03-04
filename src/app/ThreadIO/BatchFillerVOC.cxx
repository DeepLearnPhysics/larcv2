// #ifndef __BatchFillerVOC_CXX__
// #define __BatchFillerVOC_CXX__

// #include "BatchFillerVOC.h"

// #include <random>

// namespace larcv {

// static BatchFillerVOCProcessFactory __global_BatchFillerVOCProcessFactory__;

// BatchFillerVOC::BatchFillerVOC(const std::string name)
//     : BatchFillerTemplate<float>(name),
//       _rows(kINVALID_SIZE),
//       _cols(kINVALID_SIZE),
//       _slice_v() {}

// void BatchFillerVOC::configure(const PSet& cfg) {
//   LARCV_DEBUG() << "start" << std::endl;

//   _image_producer = cfg.get<std::string>("ImageProducer");
//   _target_size_x = cfg.get<int>("TargetSizeX", 512);
//   _target_size_y = cfg.get<int>("TargetSizeY", 512);

//   LARCV_DEBUG() << "done" << std::endl;
// }

// void BatchFillerVOC::initialize() {}

// void BatchFillerVOC::_batch_begin_() {}

// void BatchFillerVOC::_batch_end_() {
//   if (logger().level() <= msg::kINFO) {
//     LARCV_INFO() << "Total data size: " << batch_data().data_size()
//                  << std::endl;

//     size_t mirror_ctr = 0;
//     for (auto const& v : _mirrored)
//       if (v) ++mirror_ctr;
//     LARCV_INFO() << mirror_ctr << " / " << _mirrored.size()
//                  << " images are mirrored!" << std::endl;
//   }
// }

// void BatchFillerVOC::finalize() { _entry_data.clear(); }

// void BatchFillerVOC::assert_dimension(const EventImage2D* image_data) const {

//   auto const& image_v = image_data->image2d_array();
  
//   if (_rows == kINVALID_SIZE) {
//     LARCV_WARNING()
//         << "set_dimension() must be called prior to check_dimension()"
//         << std::endl;
//     return;
//   }
//   bool valid_ch = (image_v.size() > _max_ch);
//   bool valid_rows = true;
//   for (size_t ch = 0; ch < _num_channels; ++ch) {
//     size_t input_ch = _slice_v.at(ch);
//     auto const& img = image_v.at(input_ch);

//     if (!_crop_image) valid_rows = (_rows == img.meta().rows());
//     if (!valid_rows) {
//       LARCV_ERROR() << "# of rows changed! (row,col): (" << _rows << ","
//                     << _cols << ") => (" << img.meta().rows() << ","
//                     << img.meta().cols() << ")" << std::endl;
//       break;
//     }
//   }

//   bool valid_cols = true;
//   for (size_t ch = 0; ch < _num_channels; ++ch) {
//     size_t input_ch = _slice_v.at(ch);
//     auto const& img = image_v.at(input_ch);
//     if (!_crop_image) valid_cols = (_cols == img.meta().cols());
//     if (!valid_cols) {
//       LARCV_ERROR() << "# of cols changed! (row,col): (" << _rows << ","
//                     << _cols << ") => (" << img.meta().rows() << ","
//                     << img.meta().cols() << ")" << std::endl;
//       break;
//     }
//   }
//   if (!valid_rows) {
//     LARCV_CRITICAL() << "# of rows in the input image have changed!"
//                      << std::endl;
//     throw larbys();
//   }
//   if (!valid_cols) {
//     LARCV_CRITICAL() << "# of cols in the input image have changed!"
//                      << std::endl;
//     throw larbys();
//   }
//   if (!valid_ch) {
//     LARCV_CRITICAL()
//         << "# of channels have changed in the input image! Image vs. MaxCh ("
//         << image_v.size() << " vs. " << _max_ch << ")" << std::endl;
//     throw larbys();
//   }
// }

// bool BatchFillerVOC::process(IOManager& mgr) {
//   LARCV_DEBUG() << "start" << std::endl;

//   auto const& image_data = mgr.get_data<larcv::EventImage2D>(_image_producer);

//   if (image_data.as_vector().empty()) {
//     LARCV_CRITICAL()
//         << "Could not locate non-empty image2d data w/ producer name "
//         << _image_producer << std::endl;
//     throw larbys();
//   }

//   // one time operation: get image dimension
//   if (batch_data().dim().empty()) {
//     auto const& voxel_meta = image_data.meta();
//     std::vector<int> dim;
//     dim.resize(4);
//     dim[0] = batch_size();
//     dim[1] = _target_size_x;
//     dim[2] = _target_size_y;
//     dim[3] = 3;  // RGB
//     this->set_dim(dim);
//   } else
//     this->assert_dimension(image_data);

// ///////////////

//   if (_entry_data.size() != batch_data().entry_data_size())
//     _entry_data.resize(batch_data().entry_data_size(), 0.);

//   for (auto& v : _entry_data) v = 0.;

//   if (_num_channel == 1) {
//     for (auto const& voxel : voxel_data.as_vector())
//       _entry_data[voxel.id()] += voxel.value();
//   } else {
//     int ch = 0;
//     for (auto const& voxel : voxel_data.as_vector()) {
//       ch = (int)(voxel.value());
//       if (ch < 0 || ch >= _num_channel) {
//         LARCV_CRITICAL() << "Voxel ID " << voxel.id() << " has value "
//                          << voxel.value()
//                          << " ... cannt map to hot label array of length "
//                          << _num_channel << "!" << std::endl;
//         throw larbys();
//       }
//       _entry_data[voxel.id() * _num_channel + ch] = 1.;
//     }
//   }

//   // record the entry data
//   LARCV_INFO() << "Inserting entry data of size " << _entry_data.size()
//                << std::endl;
//   set_entry_data(_entry_data);

//   return true;
// }
// }
// #endif
