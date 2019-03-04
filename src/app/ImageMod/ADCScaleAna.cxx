#ifndef __ADCSCALEANA_CXX__
#define __ADCSCALEANA_CXX__

#include "ADCScaleAna.h"
#include "larcv/core/DataFormat/EventImage2D.h"

namespace larcv {

  static ADCScaleAnaProcessFactory __global_ADCScaleAnaProcessFactory__;

  ADCScaleAna::ADCScaleAna(const std::string name)
    : ProcessBase(name)
  {}

  void ADCScaleAna::configure(const PSet& cfg)
  {
    _image_producer = cfg.get<std::string>("ImageProducer");
  }

  void ADCScaleAna::initialize()
  {
    for (auto& p : _hADC_v) delete p;
    _hADC_v.clear();
  }

  bool ADCScaleAna::process(IOManager& mgr)
  {
    static ProducerID_t id = kINVALID_SIZE;
    if (id == kINVALID_SIZE) {
      ProducerName_t name_id("image2d", _image_producer);
      id = mgr.producer_id(name_id);
    }

    auto event_image = (EventImage2D*)(mgr.get_data(id));

    for (auto const& img : event_image->image2d_array()) {

      auto const plane = img.meta().id();
      if (_hADC_v.size() <= plane) _hADC_v.resize(plane + 1, nullptr);

      if (!_hADC_v[plane]) _hADC_v[plane] = new TH1D(Form("hADC_%s_Plane%02hu", _image_producer.c_str(), plane),
            Form("ADC Values for Plane %hu", plane),
            100, 0, 500);
      auto& ptr = _hADC_v[plane];

      auto const& adc_v = img.as_vector();
      for (size_t i = 1; i < adc_v.size() - 1; ++i) {
        float pre = adc_v[i - 1];
        float now = adc_v[i];
        float post = adc_v[i + 1];
        if (pre < now && now > post && now > 5)
          ptr->Fill(now);
      }
    }
    return true;
  }

  void ADCScaleAna::finalize()
  {
    if (has_ana_file())

      for (auto& p : _hADC_v) if (p) p->Write();
  }

}
#endif
