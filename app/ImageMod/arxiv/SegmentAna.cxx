#ifndef __SEGMENTANA_CXX__
#define __SEGMENTANA_CXX__

#include "SegmentAna.h"
#include "DataFormat/EventROI.h"
#include "DataFormat/EventImage2D.h"

namespace larcv {

  static SegmentAnaProcessFactory __global_SegmentAnaProcessFactory__;

  SegmentAna::SegmentAna(const std::string name)
    : ProcessBase(name)
  {}
    
  void SegmentAna::configure(const PSet& cfg)
  {
    _image_producer = cfg.get<std::string>("ImageProducer");
    _segment_producer = cfg.get<std::string>("SegmentProducer");
  }

  void SegmentAna::initialize()
  {}

  bool SegmentAna::process(IOManager& mgr)
  {
    auto event_adc = (EventImage2D*)(mgr.get_data(kProductImage2D,_image_producer));
    auto event_seg = (EventImage2D*)(mgr.get_data(kProductImage2D,_segment_producer));

    if(!event_adc || !event_seg) {
      LARCV_ERROR() << "Input ot found..." << std::endl;
      return false;
    }

    auto out_image1 = (EventImage2D*)(mgr.get_data(kProductImage2D,"SegmentRemanent"));
    auto out_image2 = (EventImage2D*)(mgr.get_data(kProductImage2D,"ADCRemanent"));

    auto const& adc_image_v = event_adc->Image2DArray();
    auto const& seg_image_v = event_seg->Image2DArray();

    if(adc_image_v.size() != seg_image_v.size()) {
      LARCV_ERROR() << "ADC and Segement image # disagrees..." << std::endl;
      return false;
    }	

    for(size_t i=0; i<adc_image_v.size(); ++i) {

      auto const& adc_image_orig = adc_image_v[i];
      auto adc_image = adc_image_orig;
      //adc_image.compress(adc_image.meta().rows()/6,adc_image.meta().cols(),Image2D::kSum);
      auto const& seg_image = seg_image_v[i];

      auto const& adc_vec = adc_image.as_vector();
      auto const& seg_vec = seg_image.as_vector();

      std::vector<float> out_vec1(adc_vec.size(),0.);
      std::vector<float> out_vec2(adc_vec.size(),0.);

      if(adc_vec.size() != seg_vec.size()) {
	LARCV_ERROR() << "ADC (" << adc_vec.size() << ") and Segment (" << seg_vec.size() << ") image size disagrees..." << std::endl;
	return false;
      }
      
      for(size_t j=0; j<adc_vec.size(); ++j) {

	if(adc_vec[j] == 0. && seg_vec[j]  > 0.) out_vec1[j] = 1.;
	if(adc_vec[j]  > 0. && seg_vec[j] == 0.) out_vec2[j] = 1.;

      }
      
      Image2D out1(adc_image.meta(),std::move(out_vec1));
      out_image1->Emplace(std::move(out1));

      Image2D out2(adc_image.meta(),std::move(out_vec2));
      out_image2->Emplace(std::move(out2));
      
    }
    return true;
  }

  void SegmentAna::finalize()
  {}

}
#endif
