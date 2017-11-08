#ifndef __MaskImage2D_CXX__
#define __MaskImage2D_CXX__

#include "MaskImage2D.h"
#include "larcv/core/DataFormat/EventImage2D.h"

namespace larcv {

  static MaskImage2DProcessFactory __global_MaskImage2DProcessFactory__;

  MaskImage2D::MaskImage2D(const std::string name)
    : ProcessBase(name)
  {}

  void MaskImage2D::configure(const PSet& cfg)
  {
    _pi_thresh_min = cfg.get<float>("MinPIThreshold");
    _mask_value = cfg.get<float>("MaskValue");
    _output_producer = cfg.get<std::string>("OutputProducer", "");
    _reference_image_producer = cfg.get<std::string>("ReferenceProducer");
    _target_image_producer = cfg.get<std::string>("TargetProducer");
    _ignore_ref_origin = cfg.get<bool>("IgnoreRefOrigin", false);
  }

  void MaskImage2D::initialize()
  {}

  bool MaskImage2D::process(IOManager& mgr)
  {
    auto ref_event_image = (EventImage2D*)(mgr.get_data("image2d", _reference_image_producer));
    if (!ref_event_image) {
      LARCV_CRITICAL() << "Reference EventImage2D not found: " << _reference_image_producer << std::endl;
      throw larbys();
    }

    auto tar_event_image = (EventImage2D*)(mgr.get_data("image2d", _target_image_producer));
    if (!tar_event_image) {
      LARCV_CRITICAL() << "Target EventImage2D not found: " << _target_image_producer << std::endl;
      throw larbys();
    }

    EventImage2D* out_event_image = nullptr;
    if (_output_producer.empty()) out_event_image = tar_event_image;
    else out_event_image = (EventImage2D*)(mgr.get_data("image2d", _output_producer));

    if (!out_event_image) {
      LARCV_CRITICAL() << "Output EventImage2D could not be made " << _output_producer << std::endl;
      throw larbys();
    }

    auto const& ref_image_v = ref_event_image->image2d_array();

    std::vector<larcv::Image2D> tar_image_v;
    if (_output_producer.empty())
      tar_event_image->move(tar_image_v);
    else
      tar_image_v = tar_event_image->image2d_array();

    // Check # planes
    if (ref_image_v.size() != tar_image_v.size()) {
      LARCV_CRITICAL() << "# planes in target (" << tar_image_v.size()
                       << ") producer " << _target_image_producer << " and reference (" << ref_image_v.size()
                       << ") producer " << _reference_image_producer << " are not same!" << std::endl;
      throw larbys();
    }

    for (size_t pid = 0; pid < tar_image_v.size(); ++pid) {

      auto& tar_image = tar_image_v[pid];

      auto ref_image = Image2D(tar_image.meta());
      ref_image.paint(0);

      LARCV_DEBUG() << "Overlaying:\n" << tar_image.meta().dump() << ref_image_v[pid].meta().dump() << std::flush;

      if (_ignore_ref_origin) {
        if (tar_image.meta().cols() != ref_image_v[pid].meta().cols() ||
            tar_image.meta().rows() != ref_image_v[pid].meta().rows() ) {
          LARCV_CRITICAL() << "IgnoreRefOrigin option = true ... only allowed if ref and target image dimensions are identical!" << std::endl;
          throw larbys();
        }
        std::vector<float> ref_data = ref_image_v[pid].as_vector();
        ref_image.move(std::move(ref_data));
      }
      else { ref_image.overlay( ref_image_v[pid] ); }

      if (tar_image.as_vector().size() != ref_image.as_vector().size()) {
        LARCV_CRITICAL() << "Different size among the target (" << tar_image.as_vector().size()
                         << ") and reference (" << ref_image.as_vector().size()
                         << ")!" << std::endl;
        throw larbys();
      }

      auto const& ref_vector = ref_image.as_vector();

      for (size_t px_idx = 0; px_idx < ref_vector.size(); ++px_idx)

        if (ref_vector[px_idx] < _pi_thresh_min) tar_image.set_pixel(px_idx, _mask_value);

    }

    out_event_image->emplace(std::move(tar_image_v));
    return true;
  }

  void MaskImage2D::finalize()
  {}

}
#endif
