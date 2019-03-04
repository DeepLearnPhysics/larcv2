#ifndef __CROPBBox2D_CXX__
#define __CROPBBox2D_CXX__

#include "CropBBox2D.h"
#include "larcv/core/DataFormat/EventBBox.h"
#include "larcv/core/DataFormat/EventImage2D.h"
namespace larcv {

  static CropBBox2DProcessFactory __global_CropBBox2DProcessFactory__;

  CropBBox2D::CropBBox2D(const std::string name)
    : ProcessBase(name)
  {}

  void CropBBox2D::configure(const PSet& cfg)
  {
    _bbox_producer = cfg.get<std::string>("BBox2DProducer");
    _input_producer = cfg.get<std::string>("InputProducer");
    _output_producer = cfg.get<std::string>("OutputProducer");
    _image_idx = cfg.get<std::vector<size_t> >("ImageIndex");
    _bbox_idx = cfg.get<size_t>("BBox2DIndex", 0);
  }

  void CropBBox2D::initialize()
  {}

  bool CropBBox2D::process(IOManager& mgr)
  {
    auto input_image  = (EventImage2D*)(mgr.get_data("image2d", _input_producer));
    if (!input_image) {
      LARCV_CRITICAL() << "No Image2D found with a name: " << _input_producer << std::endl;
      throw larbys();
    }

    auto output_image = (EventImage2D*)(mgr.get_data("image2d", _output_producer));
    if (!output_image) {
      LARCV_CRITICAL() << "No Image2D found with a name: " << _output_producer << std::endl;
      throw larbys();
    }

    for (auto const& idx : _image_idx) {
      if (idx >= input_image->image2d_array().size()) {
        LARCV_CRITICAL() << "ImageIndex array contains index " << idx
                         << " not available in Image2DArray (" << input_image->image2d_array().size()
                         << ")!" << std::endl;
        throw larbys();
      }
    }

    auto const& bbox_v = mgr.get_data<larcv::EventBBox2D>(_bbox_producer);

    if (bbox_v.size() <= _bbox_idx) {
      LARCV_CRITICAL() << "BBOX index " << _bbox_idx << " not found!" << std::endl;
      throw larbys();
    }

    std::vector<larcv::Image2D> image_v;
    if (_input_producer == _output_producer) {
      std::vector<larcv::Image2D> tmp_v;
      input_image->move(tmp_v);
      for (auto const& idx : _image_idx)
        image_v.emplace_back(std::move(tmp_v[idx]));
    } else {
      auto const& tmp_v = input_image->image2d_array();
      for (auto const& idx : _image_idx)
        image_v.push_back(tmp_v[idx]);
    }

    // Now process
    for (size_t idx = 0; idx < _image_idx.size(); ++idx) {
      auto const& bbox = bbox_v[idx];
      image_v[idx] = image_v[idx].crop(bbox, bbox_v.unit);
    }
    output_image->emplace(std::move(image_v));
    return true;
  }

  void CropBBox2D::finalize()
  {}

}
#endif
